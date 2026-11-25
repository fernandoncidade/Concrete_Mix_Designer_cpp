#include "TestFramework.hpp"

#include "../mocks/find_VersionEditor.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTemporaryDir>

using namespace mocks::version_editor;

namespace {

bool copyDirectoryRecursively(const QString& srcPath, const QString& dstPath) {
    QDir srcDir(srcPath);
    if (!srcDir.exists()) {
        return false;
    }
    QDir().mkpath(dstPath);

    for (const QFileInfo& entry : srcDir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot)) {
        const QString srcItem = entry.absoluteFilePath();
        const QString dstItem = dstPath + QLatin1Char('/') + entry.fileName();
        if (entry.isDir()) {
            if (!copyDirectoryRecursively(srcItem, dstItem)) {
                return false;
            }
        } else {
            if (QFile::exists(dstItem)) {
                QFile::remove(dstItem);
            }
            if (!QFile::copy(srcItem, dstItem)) {
                return false;
            }
        }
    }
    return true;
}

QString readTextUtf8(const QString& path) {
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) {
        return {};
    }
    return QString::fromUtf8(f.readAll());
}

}  // namespace

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    concrete_tests::TestRunner runner("MocksAndAuditTools");

#ifdef CONCRETE_REPO_ROOT_DIR
    const QString repoRoot = QString::fromUtf8(CONCRETE_REPO_ROOT_DIR);
#else
    const QString repoRoot = detect_project_root();
#endif

    runner.add("RepoRootVersionEditorCheckAllExpectedFiles", [&]() {
        CMD_ASSERT(QDir(repoRoot).exists());
        const QList<FileStatus> statuses = check_expected_lines(repoRoot);
        CMD_ASSERT(!statuses.isEmpty());

        for (const FileStatus& st : statuses) {
            CMD_ASSERT_MSG(st.exists, ("Missing file: " + st.relative_path).toStdString());
            CMD_ASSERT_MSG(st.missing.isEmpty(),
                           ("Missing pattern in " + st.relative_path + ": " + st.missing.join(", ")).toStdString());
        }
    });

    runner.add("IsolatedTempRepoVersionAndDateApplyAndRenameVerification", [&]() {
        QTemporaryDir tempDir;
        CMD_ASSERT(tempDir.isValid());
        const QString tempRoot = tempDir.path();

        // Copia apenas os arquivos monitorados para testar --apply sem modificar o repositório real
        const QStringList topFiles = {
            QStringLiteral("main.cpp"),
            QStringLiteral("CODE_OF_CONDUCT.md"),
            QStringLiteral("CONTRIBUTING.md"),
            QStringLiteral("MANUAL.md"),
            QStringLiteral("OBSERVACAO.md"),
            QStringLiteral("README.md"),
            QStringLiteral("SECURITY.md")
        };
        for (const QString& rel : topFiles) {
            CMD_ASSERT(QFile::copy(repoRoot + QLatin1Char('/') + rel, tempRoot + QLatin1Char('/') + rel));
        }
        QDir(tempRoot).mkpath(QStringLiteral("source/cli"));
        QDir(tempRoot).mkpath(QStringLiteral("source/gui"));
        CMD_ASSERT(QFile::copy(repoRoot + QStringLiteral("/source/cli/main_cli.cpp"),
                               tempRoot + QStringLiteral("/source/cli/main_cli.cpp")));
        CMD_ASSERT(QFile::copy(repoRoot + QStringLiteral("/source/gui/SobreDialog.cpp"),
                               tempRoot + QStringLiteral("/source/gui/SobreDialog.cpp")));

        CMD_ASSERT(copyDirectoryRecursively(repoRoot + QStringLiteral("/source/assets"),
                                            tempRoot + QStringLiteral("/source/assets")));
        CMD_ASSERT(copyDirectoryRecursively(repoRoot + QStringLiteral("/docs"),
                                            tempRoot + QStringLiteral("/docs")));
        CMD_ASSERT(copyDirectoryRecursively(repoRoot + QStringLiteral("/library"),
                                            tempRoot + QStringLiteral("/library")));

        UpdateContext ctx;
        ctx.pt_version = QStringLiteral("2027.03.15.0");
        ctx.pt_date = QStringLiteral("15 de março de 2027");
        ctx.en_version = QStringLiteral("2027.03.15.0");
        ctx.en_date = QStringLiteral("March 15, 2027");
        ctx.pt_readme_version = QStringLiteral("v2027.03.15.0");
        ctx.en_readme_version = QStringLiteral("v2027.03.15.0");

        const QList<UpdateResult> results = apply_updates(ctx, tempRoot);
        CMD_ASSERT(!results.isEmpty());
        for (const UpdateResult& res : results) {
            CMD_ASSERT_MSG(res.ok,
                           ("Update failed for " + res.relative_path + ": " + res.message).toStdString());
        }

        // 1. Verifica renomeação e título de docs/release-v2027.03.15.0.md
        const QString newRelPath = tempRoot + QStringLiteral("/docs/release-v2027.03.15.0.md");
        CMD_ASSERT(QFileInfo::exists(newRelPath));
        const QString newRelText = readTextUtf8(newRelPath);
        CMD_ASSERT_CONTAINS(newRelText.toStdString(), "2027.03.15.0");
        CMD_ASSERT_CONTAINS(newRelText.toStdString(), "15 de março de 2027");
        CMD_ASSERT_CONTAINS(newRelText.toStdString(), "March 15, 2027");

        // 2. Verifica library/AppxManifest.xml e library/Concrete_Mix_Designer_cpp.txt
        const QString appxText = readTextUtf8(tempRoot + QStringLiteral("/library/AppxManifest.xml"));
        CMD_ASSERT_CONTAINS(appxText.toStdString(), "Version=\"2027.03.15.0\"");

        const QString promptTxt = readTextUtf8(tempRoot + QStringLiteral("/library/Concrete_Mix_Designer_cpp.txt"));
        CMD_ASSERT_CONTAINS(promptTxt.toStdString(), "release-v2027.03.15.0.md");
        CMD_ASSERT_CONTAINS(promptTxt.toStdString(), "NOTAS DE LANÇAMENTO: Versão 2027.03.15.0");
        CMD_ASSERT_CONTAINS(promptTxt.toStdString(), "Data: 15 de março de 2027");
        CMD_ASSERT_CONTAINS(promptTxt.toStdString(), "RELEASE NOTES: Version 2027.03.15.0");
        CMD_ASSERT_CONTAINS(promptTxt.toStdString(), "Date: March 15, 2027");

        // 3. Verifica root markdowns (MANUAL, OBSERVACAO, README, CODE_OF_CONDUCT, CONTRIBUTING, SECURITY)
        for (const QString& rootMd : topFiles) {
            if (!rootMd.endsWith(QStringLiteral(".md"))) continue;
            const QString content = readTextUtf8(tempRoot + QLatin1Char('/') + rootMd);
            CMD_ASSERT_MSG(content.contains(QStringLiteral("2027.03.15.0")), rootMd.toStdString());
            CMD_ASSERT_MSG(content.contains(QStringLiteral("15 de março de 2027")), rootMd.toStdString());
            CMD_ASSERT_MSG(content.contains(QStringLiteral("March 15, 2027")), rootMd.toStdString());
        }

        // 4. Verifica CLI version em main_cli.cpp
        const QString cliText = readTextUtf8(tempRoot + QStringLiteral("/source/cli/main_cli.cpp"));
        CMD_ASSERT_CONTAINS(cliText.toStdString(), "kCliVersion = \"2027.03.15.0\"");

        // 5. Revalida --check e idempotência de --apply na cópia atualizada
        const QList<FileStatus> recheck = check_expected_lines(tempRoot);
        for (const FileStatus& st : recheck) {
            CMD_ASSERT_MSG(st.exists && st.missing.isEmpty(), st.relative_path.toStdString());
        }

        const QList<UpdateResult> secondPass = apply_updates(ctx, tempRoot);
        for (const UpdateResult& res : secondPass) {
            CMD_ASSERT_MSG(res.ok, ("Second pass failed: " + res.relative_path).toStdString());
        }
    });

    runner.add("WorkspaceRepoLayoutImagesAndConfigValidation", [&]() {
        // Garante remoção dos caminhos antigos e presença dos novos diretórios estruturais
        CMD_ASSERT(!QFileInfo::exists(repoRoot + QStringLiteral("/cmake/windows_app_icon.rc.in")));
        CMD_ASSERT(QFileInfo::exists(repoRoot + QStringLiteral("/source/windows_app_icon.rc.in")));
        CMD_ASSERT(!QDir(repoRoot + QStringLiteral("/repo/LISTAGEM")).exists());
        CMD_ASSERT(QDir(repoRoot + QStringLiteral("/images/interfaces/pt_BR")).exists());
        CMD_ASSERT(QDir(repoRoot + QStringLiteral("/images/interfaces/en_US")).exists());
        CMD_ASSERT(QFileInfo::exists(repoRoot + QStringLiteral("/library/AppxManifest.xml")));
        CMD_ASSERT(QFileInfo::exists(repoRoot + QStringLiteral("/library/Concrete_Mix_Designer.iss")));
        CMD_ASSERT(QFileInfo::exists(repoRoot + QStringLiteral("/library/Concrete_Mix_Designer_cpp.txt")));

        // Valida sintaxe JSON de .vscode/tasks.json e CMakePresets.json
        const QString tasksRaw = readTextUtf8(repoRoot + QStringLiteral("/.vscode/tasks.json"));
        QJsonParseError jErr{};
        const QJsonDocument tasksDoc = QJsonDocument::fromJson(tasksRaw.toUtf8(), &jErr);
        CMD_ASSERT_MSG(jErr.error == QJsonParseError::NoError, ".vscode/tasks.json invalid JSON");
        const QJsonArray tasks = tasksDoc.object().value(QStringLiteral("tasks")).toArray();
        int visibleCount = 0;
        for (const QJsonValue& val : tasks) {
            const QJsonObject t = val.toObject();
            if (!t.value(QStringLiteral("hide")).toBool(false)) {
                ++visibleCount;
            }
        }
        CMD_ASSERT(visibleCount == 6);

        const QString presetsRaw = readTextUtf8(repoRoot + QStringLiteral("/CMakePresets.json"));
        const QJsonDocument presetsDoc = QJsonDocument::fromJson(presetsRaw.toUtf8(), &jErr);
        CMD_ASSERT_MSG(jErr.error == QJsonParseError::NoError, "CMakePresets.json invalid JSON");
    });

    return runner.runAll();
}
