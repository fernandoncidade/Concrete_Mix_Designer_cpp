#include "TestFramework.hpp"

#include "../source/language/GerenciadorTraducao.hpp"
#include "../source/utils/ApplicationPathUtils.hpp"
#include "../source/utils/CaminhoPersistenteUtils.hpp"
#include "../source/utils/LogManager.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSignalSpy>

using namespace ConcreteMix::Language;

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    app.setOrganizationName(QStringLiteral("AppsCpp"));
    app.setApplicationName(QStringLiteral("ConcreteMixDesign"));

    concrete_tests::SettingsBackupGuard settingsGuard;
    concrete_tests::TestRunner runner("I18nUtilsAndPersistence");

    runner.add("GerenciadorTraducaoDynamicSwitchAndCurrency", []() {
        auto& mgr = GerenciadorTraducao::instance();
        mgr.inicializar();

        const auto langs = mgr.idiomasDisponiveis();
        CMD_ASSERT(langs.contains(QStringLiteral("pt_BR")));
        CMD_ASSERT(langs.contains(QStringLiteral("en_US")));

        QSignalSpy spyStr(&mgr, &GerenciadorTraducao::idiomaAlterado);
        QSignalSpy spyEnum(&mgr, &GerenciadorTraducao::idiomaAlteradoEnum);

        CMD_ASSERT(mgr.definirIdioma(QStringLiteral("en_US")));
        CMD_ASSERT(mgr.codigoIdiomaAtual() == QStringLiteral("en_US"));
        CMD_ASSERT(mgr.idiomaAtual() == Idioma::InglesEstadosUnidos);
        CMD_ASSERT(mgr.simboloMoeda().contains(QStringLiteral("$")));
        CMD_ASSERT(!mgr.simboloMoeda().contains(QStringLiteral("R$")));
        CMD_ASSERT(GerenciadorTraducao::currencySymbol() == mgr.simboloMoeda());

        mgr.mudarIdioma(Idioma::PortuguesBrasil);
        CMD_ASSERT(mgr.codigoIdiomaAtual() == QStringLiteral("pt_BR"));
        CMD_ASSERT(mgr.idiomaAtual() == Idioma::PortuguesBrasil);
        CMD_ASSERT(mgr.simboloMoeda().contains(QStringLiteral("R$")));

        CMD_ASSERT(spyStr.count() >= 2);
        CMD_ASSERT(spyEnum.count() >= 2);
    });

    runner.add("ApplicationPathAndPersistentStorageWritable", []() {
        const QString appBase = getAppBasePath();
        CMD_ASSERT(!appBase.isEmpty());
        CMD_ASSERT(QDir(appBase).exists());

        const QString persistent = source::utils::obterCaminhoPersistente();
        CMD_ASSERT(!persistent.isEmpty());
        CMD_ASSERT(QDir(persistent).exists());
        CMD_ASSERT(::obterCaminhoPersistente() == persistent);

        // Escrita temporária no diretório persistente do usuário sem privilégios administrativos
        const QString probePath = QDir(persistent).filePath(QStringLiteral("_suite_write_probe.tmp"));
        QFile probe(probePath);
        CMD_ASSERT(probe.open(QIODevice::WriteOnly));
        probe.write("ok");
        probe.close();
        CMD_ASSERT(QFile::remove(probePath));
    });

    runner.add("LogManagerInitializationAndFileOutput", []() {
        LogManager::configure_logging();
        LogManager::info(QStringLiteral("ConcreteMixTestSuite info probe"));
        LogManager::warning(QStringLiteral("ConcreteMixTestSuite warning probe"));
        LogManager::debug(QStringLiteral("ConcreteMixTestSuite debug probe"));

        const QString logPath = LogManager::getLogFile();
        CMD_ASSERT(!logPath.isEmpty());
        CMD_ASSERT(QFileInfo::exists(logPath));
        QFile f(logPath);
        CMD_ASSERT(f.open(QIODevice::ReadOnly));
        const QByteArray bytes = f.readAll();
        CMD_ASSERT(bytes.contains("ConcreteMixTestSuite info probe"));
    });

    runner.add("TranslationSourceFilesZeroUnfinishedCheck", []() {
#ifdef CONCRETE_REPO_ROOT_DIR
        const QDir localeDir(QString::fromUtf8(CONCRETE_REPO_ROOT_DIR) + QStringLiteral("/source/language/locale"));
        if (localeDir.exists()) {
            const QStringList tsFiles = localeDir.entryList({QStringLiteral("*.ts")}, QDir::Files);
            CMD_ASSERT(!tsFiles.isEmpty());
            for (const QString& tsName : tsFiles) {
                QFile ts(localeDir.filePath(tsName));
                CMD_ASSERT(ts.open(QIODevice::ReadOnly));
                const QByteArray raw = ts.readAll();
                CMD_ASSERT(!raw.isEmpty());
                CMD_ASSERT_MSG(!raw.contains("type=\"unfinished\""), tsName.toStdString());
            }
        }
#endif
    });

    return runner.runAll();
}
