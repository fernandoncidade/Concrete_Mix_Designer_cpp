#include "TestFramework.hpp"

#include "../source/core/ProjectData.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QRegularExpression>
#include <QTemporaryDir>

using namespace ConcreteMix;

namespace {

QString locateCliExecutable() {
#ifdef CONCRETE_CLI_EXE_PATH
    const QString compiled = QString::fromUtf8(CONCRETE_CLI_EXE_PATH);
    if (QFileInfo::exists(compiled)) {
        return compiled;
    }
#endif
    const QString sibling = QCoreApplication::applicationDirPath() + QStringLiteral("/concrete-mix-designer-cli.exe");
    return sibling;
}

struct CliRunResult {
    int exitCode = -1;
    QString stdOut;
    QString stdErr;
};

CliRunResult runCli(const QStringList& args) {
    QProcess proc;
    proc.start(locateCliExecutable(), args);
    const bool finished = proc.waitForFinished(15000);
    CliRunResult out;
    out.exitCode = finished ? proc.exitCode() : -999;
    out.stdOut = QString::fromUtf8(proc.readAllStandardOutput());
    out.stdErr = QString::fromUtf8(proc.readAllStandardError());
    return out;
}

}  // namespace

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    concrete_tests::TestRunner runner("CliIntegration");

    runner.add("CliVersionAndHelpFlags", []() {
        CMD_ASSERT_MSG(QFileInfo::exists(locateCliExecutable()), locateCliExecutable().toStdString());

        const auto ver = runCli({QStringLiteral("--version")});
        CMD_ASSERT(ver.exitCode == 0);
        const QRegularExpression calVerRx(QStringLiteral(R"((?:\b|[vV])\d{4}\.\d{1,2}\.\d{1,2}\.\d+\b)"));
        CMD_ASSERT_MSG(calVerRx.match(ver.stdOut).hasMatch(), ver.stdOut.toStdString());

        const auto help = runCli({QStringLiteral("--help")});
        CMD_ASSERT(help.exitCode == 0);
        CMD_ASSERT_CONTAINS(help.stdOut.toStdString(), "design");
        CMD_ASSERT_CONTAINS(help.stdOut.toStdString(), "standards");
    });

    runner.add("CliDesignJsonAndStandardsQuery", []() {
        const auto design = runCli({
            QStringLiteral("design"),
            QStringLiteral("--preset"), QStringLiteral("5"),
            QStringLiteral("--fck"), QStringLiteral("35"),
            QStringLiteral("--slump"), QStringLiteral("120"),
            QStringLiteral("--format"), QStringLiteral("json")
        });
        CMD_ASSERT_MSG(design.exitCode == 0, design.stdErr.toStdString());

        QJsonParseError err{};
        const QJsonDocument doc = QJsonDocument::fromJson(design.stdOut.toUtf8(), &err);
        CMD_ASSERT(err.error == QJsonParseError::NoError);
        CMD_ASSERT(doc.isObject());
        const QJsonObject root = doc.object();
        CMD_ASSERT(root.contains(QStringLiteral("compliance")));
        CMD_ASSERT(root.value(QStringLiteral("fcj")).toDouble() > 35.0);

        const auto standards = runCli({
            QStringLiteral("standards"),
            QStringLiteral("all"),
            QStringLiteral("--format"), QStringLiteral("json")
        });
        CMD_ASSERT_MSG(standards.exitCode == 0, standards.stdErr.toStdString());
        const QJsonDocument stdDoc = QJsonDocument::fromJson(standards.stdOut.toUtf8(), &err);
        CMD_ASSERT(err.error == QJsonParseError::NoError);
        const QJsonObject stdRoot = stdDoc.object();
        CMD_ASSERT(!stdRoot.value(QStringLiteral("cements")).toArray().isEmpty());
        CMD_ASSERT(!stdRoot.value(QStringLiteral("aggregates")).toArray().isEmpty());
        CMD_ASSERT(!stdRoot.value(QStringLiteral("slumpClasses")).toArray().isEmpty());
        CMD_ASSERT(!stdRoot.value(QStringLiteral("batchingTolerances")).toArray().isEmpty());
    });

    runner.add("CliCalculateVerifyReportRoundTripAndErrorHandling", []() {
        QTemporaryDir tempDir;
        CMD_ASSERT(tempDir.isValid());

        ProjectData proj;
        proj.loadPreset(1);
        const QString projPath = tempDir.filePath(QStringLiteral("cli_study.cmdproj"));
        {
            QFile f(projPath);
            CMD_ASSERT(f.open(QIODevice::WriteOnly));
            f.write(QByteArray::fromStdString(proj.toJson()));
        }

        const auto calc = runCli({
            QStringLiteral("calculate"),
            projPath,
            QStringLiteral("--format"), QStringLiteral("text")
        });
        CMD_ASSERT(calc.exitCode == 0);
        CMD_ASSERT(!calc.stdOut.trimmed().isEmpty());

        const auto verify = runCli({
            QStringLiteral("verify"),
            projPath,
            QStringLiteral("--format"), QStringLiteral("json")
        });
        CMD_ASSERT(verify.exitCode == 0);
        const QJsonDocument verifyDoc = QJsonDocument::fromJson(verify.stdOut.toUtf8());
        CMD_ASSERT(verifyDoc.object().contains(QStringLiteral("isCompliant")));

        const QString htmlOut = tempDir.filePath(QStringLiteral("cli_report.html"));
        const auto repHtml = runCli({
            QStringLiteral("report"),
            projPath,
            QStringLiteral("--format"), QStringLiteral("html"),
            QStringLiteral("--lang"), QStringLiteral("en_US"),
            QStringLiteral("--out"), htmlOut
        });
        CMD_ASSERT(repHtml.exitCode == 0);
        CMD_ASSERT(QFileInfo(htmlOut).size() > 1024);

        const QString mdOut = tempDir.filePath(QStringLiteral("cli_report.md"));
        const auto repMd = runCli({
            QStringLiteral("report"),
            projPath,
            QStringLiteral("--format"), QStringLiteral("md"),
            QStringLiteral("--out"), mdOut
        });
        CMD_ASSERT(repMd.exitCode == 0);
        CMD_ASSERT(QFileInfo(mdOut).size() > 512);

        const auto missingFile = runCli({
            QStringLiteral("calculate"),
            tempDir.filePath(QStringLiteral("nonexistent_study.cmdproj"))
        });
        CMD_ASSERT(missingFile.exitCode != 0);
    });

    return runner.runAll();
}
