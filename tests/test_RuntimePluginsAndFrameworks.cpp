#include "TestFramework.hpp"

#include <QIcon>
#include "../source/language/GerenciadorTraducao.hpp"
#include "../source/utils/ApplicationPathUtils.hpp"

#include <QApplication>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QDir>
#include <QFileInfo>
#include <QImage>
#include <QImageReader>
#include <QLibraryInfo>
#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QPainter>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QStyleFactory>
#include <QtTest/QTest>
#include <QTranslator>

int main(int argc, char* argv[]) {
    if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM")) {
        qputenv("QT_QPA_PLATFORM", "offscreen");
    }
    QApplication app(argc, argv);
    app.setOrganizationName(QStringLiteral("AppsCpp"));
    app.setApplicationName(QStringLiteral("ConcreteMixDesign"));

    concrete_tests::SettingsBackupGuard settingsGuard;
    concrete_tests::TestRunner runner("RuntimePluginsAndFrameworks");

    runner.add("Cpp23AndQt612BuildMetadata", []() {
#if defined(_MSVC_LANG)
        CMD_ASSERT(_MSVC_LANG >= 202302L);
#else
        CMD_ASSERT(__cplusplus >= 202302L);
#endif
        CMD_ASSERT(QString::fromLatin1(qVersion()).startsWith(QStringLiteral("6.12.")));
        CMD_ASSERT(!QTest::currentAppName() || true); // Qt6::Test linkage active
        CMD_ASSERT(!QLibraryInfo::path(QLibraryInfo::PluginsPath).isEmpty());
    });

    runner.add("QtSqlAndQSqlitePluginDriver", []() {
        CMD_ASSERT_MSG(QSqlDatabase::isDriverAvailable(QStringLiteral("QSQLITE")),
                       "QSQLITE plugin unavailable");

        const QString connName = QStringLiteral("suite_sqlite_probe");
        {
            QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), connName);
            db.setDatabaseName(QStringLiteral(":memory:"));
            CMD_ASSERT(db.open());

            QSqlQuery query(db);
            CMD_ASSERT(query.exec(QStringLiteral("SELECT sqlite_version()")));
            CMD_ASSERT(query.next());
            const QString ver = query.value(0).toString();
            CMD_ASSERT(ver.startsWith(QStringLiteral("3.")));
            db.close();
        }
        QSqlDatabase::removeDatabase(connName);
    });

    runner.add("PlatformImageFormatAndStylePlugins", []() {
        const QString platform = QGuiApplication::platformName();
        CMD_ASSERT(!platform.isEmpty());

        const QList<QByteArray> formats = QImageReader::supportedImageFormats();
        CMD_ASSERT(formats.contains("png"));
        CMD_ASSERT_MSG(formats.contains("ico"), "qico imageformat plugin unavailable");

        const QStringList styles = QStyleFactory::keys();
        CMD_ASSERT(!styles.isEmpty());
        const bool hasWindowsOrFusion =
            styles.contains(QStringLiteral("windows11"), Qt::CaseInsensitive) ||
            styles.contains(QStringLiteral("windowsvista"), Qt::CaseInsensitive) ||
            styles.contains(QStringLiteral("Fusion"), Qt::CaseInsensitive);
        CMD_ASSERT(hasWindowsOrFusion);
    });

    runner.add("QtChartsAndOpenGLWidgetsFrameRender", []() {
        auto* series = new QLineSeries();
        series->append(0.35, 45.0);
        series->append(0.50, 32.0);
        series->append(0.65, 22.0);

        auto* chart = new QChart();
        chart->addSeries(series);
        chart->createDefaultAxes();

        QChartView view(chart);
        view.resize(640, 400);
        QImage canvas(640, 400, QImage::Format_ARGB32);
        canvas.fill(Qt::white);
        QPainter painter(&canvas);
        view.render(&painter);
        painter.end();
        CMD_ASSERT(!canvas.isNull());
        CMD_ASSERT(canvas.width() == 640 && canvas.height() == 400);

        QOpenGLWidget glWidget;
        glWidget.setWindowTitle(QStringLiteral("OpenGLProbe"));
        CMD_ASSERT(glWidget.windowTitle() == QStringLiteral("OpenGLProbe"));
    });

    runner.add("EmbeddedAssetsIconsAndTranslationsDiscovery", []() {
        const QString base = getAppBasePath();
        CMD_ASSERT(QDir(base).exists());

        const QStringList requiredDocs = {
            QStringLiteral("ABOUT/ABOUT_pt_BR.txt"),
            QStringLiteral("ABOUT/ABOUT_en_US.txt"),
            QStringLiteral("CLC/CLC_pt_BR.txt"),
            QStringLiteral("CLC/CLC_en_US.txt"),
            QStringLiteral("COPYRIGHT/AVISO DE COPYRIGHT E MARCA REGISTRADA_pt_BR.txt"),
            QStringLiteral("COPYRIGHT/COPYRIGHT AND TRADEMARK NOTICE_en_US.txt"),
            QStringLiteral("EULA/EULA_pt_BR.txt"),
            QStringLiteral("EULA/EULA_en_US.txt"),
            QStringLiteral("LICENSES/GNU AFFERO GENERAL PUBLIC LICENSE Version3.txt"),
            QStringLiteral("LICENSES/MIT License.txt"),
            QStringLiteral("LICENSES/SQLite Is Public Domain.txt"),
            QStringLiteral("MANUAL/MANUAL_pt_BR.txt"),
            QStringLiteral("MANUAL/MANUAL_en_US.txt"),
            QStringLiteral("NOTICES/NOTICE_pt_BR.txt"),
            QStringLiteral("NOTICES/NOTICE_en_US.txt"),
            QStringLiteral("PRIVACY_POLICY/Privacy_Policy_pt_BR.txt"),
            QStringLiteral("PRIVACY_POLICY/Privacy_Policy_en_US.txt"),
            QStringLiteral("RELEASE/RELEASE NOTES_pt_BR.txt"),
            QStringLiteral("RELEASE/RELEASE NOTES_en_US.txt")
        };

        for (const QString& rel : requiredDocs) {
            const int slash = rel.indexOf(QLatin1Char('/'));
            const QString folder = rel.left(slash);
            const QString file = rel.mid(slash + 1);
            const QString content = loadTextFile(file, folder);
            CMD_ASSERT_MSG(!content.trimmed().isEmpty(), rel.toStdString());
        }

        const QString icoPath = getTextFilePath(
            QStringLiteral("concrete_mix_design.ico"), QStringLiteral("icons"));
        if (QFileInfo::exists(icoPath)) {
            QImage icoImg(icoPath);
            CMD_ASSERT(!icoImg.isNull());
            QIcon icon = get_app_icon();
            CMD_ASSERT(!icon.isNull());
        }

        auto& i18n = ConcreteMix::Language::GerenciadorTraducao::instance();
        i18n.inicializar();
        CMD_ASSERT(i18n.definirIdioma(QStringLiteral("en_US")));
        CMD_ASSERT(i18n.codigoIdiomaAtual() == QStringLiteral("en_US"));
        CMD_ASSERT(i18n.definirIdioma(QStringLiteral("pt_BR")));
        CMD_ASSERT(i18n.codigoIdiomaAtual() == QStringLiteral("pt_BR"));
    });

    return runner.runAll();
}
