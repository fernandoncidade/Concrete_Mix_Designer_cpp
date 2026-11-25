#include "TestFramework.hpp"

#include "../source/core/NormativeDatabase.hpp"
#include "../source/core/ProjectData.hpp"
#include "../source/gui/IptUnifiedDiagramWidget.hpp"
#include "../source/gui/MainWindow.hpp"
#include "../source/gui/ManualDialog.hpp"
#include "../source/gui/QuadChartWidget.hpp"
#include "../source/gui/SobreDialog.hpp"
#include "../source/language/GerenciadorTraducao.hpp"

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QComboBox>
#include <QFileInfo>
#include <QImage>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPainter>
#include <QSignalSpy>
#include <QTabWidget>
#include <QTemporaryDir>
#include <QTextBrowser>
#include <QtTest/QTest>

using namespace ConcreteMix;

namespace {

void closeAnyPopups() {
    for (QWidget* w : QApplication::topLevelWidgets()) {
        if (qobject_cast<QMessageBox*>(w)) {
            w->close();
        }
    }
    QApplication::processEvents();
}

}  // namespace

int main(int argc, char* argv[]) {
    if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM")) {
        qputenv("QT_QPA_PLATFORM", "offscreen");
    }
    QApplication app(argc, argv);
    app.setOrganizationName(QStringLiteral("AppsCpp"));
    app.setApplicationName(QStringLiteral("ConcreteMixDesign"));

    concrete_tests::SettingsBackupGuard settingsGuard;
    ConcreteMix::Language::GerenciadorTraducao::instance().inicializar();
    NormativeDatabase::instance().initialize();

    concrete_tests::TestRunner runner("GuiWorkflowsAndCharts");

    runner.add("MainWindowTabsMenusRetranslationAndTheme", []() {
        MainWindow win;
        win.resize(1280, 800);
        win.show();
        QTest::qWait(30);

        auto* tabs = win.findChild<QTabWidget*>();
        CMD_ASSERT(tabs != nullptr);
        CMD_ASSERT(tabs->count() == 7);

        for (int i = 0; i < tabs->count(); ++i) {
            tabs->setCurrentIndex(i);
            QApplication::processEvents();
            CMD_ASSERT(tabs->currentIndex() == i);
        }

        // Alterna idioma dinamicamente e verifica atualização do título / menus
        const QString titleBefore = win.windowTitle();
        ConcreteMix::Language::GerenciadorTraducao::instance().definirIdioma(QStringLiteral("en_US"));
        QApplication::processEvents();
        const QString titleEn = win.windowTitle();

        ConcreteMix::Language::GerenciadorTraducao::instance().definirIdioma(QStringLiteral("pt_BR"));
        QApplication::processEvents();
        const QString titlePt = win.windowTitle();

        CMD_ASSERT(!titleBefore.isEmpty());
        CMD_ASSERT(!titleEn.isEmpty());
        CMD_ASSERT(!titlePt.isEmpty());
        CMD_ASSERT(titleEn != titlePt || titlePt == titleBefore);

        win.applySystemTheme();
        win.close();
        closeAnyPopups();
    });

    runner.add("QuadChartWidgetAndIptUnifiedDiagramRenderExport", []() {
        ProjectData proj;
        proj.loadPreset(5);
        const MixDesignResult res = proj.calculate();

        QuadChartWidget quad;
        quad.resize(1000, 700);
        quad.updateCharts(res, proj.experimentalMixes);
        quad.updateTheme(false);
        quad.updateTheme(true);
        quad.retranslateUi();
        quad.show();
        QTest::qWait(20);

        QImage quadImg(1000, 700, QImage::Format_ARGB32);
        quadImg.fill(Qt::white);
        {
            QPainter p(&quadImg);
            quad.render(&p);
        }
        CMD_ASSERT(!quadImg.isNull());

        IptUnifiedDiagramWidget ipt;
        ipt.resize(960, 540);
        ipt.setData(res, proj.experimentalMixes);
        ipt.setTheme(false);
        ipt.setTheme(true);
        ipt.retranslateUi();

        QTemporaryDir tempDir;
        CMD_ASSERT(tempDir.isValid());
        const QString pngPath = tempDir.filePath(QStringLiteral("ipt_unified_probe.png"));
        CMD_ASSERT(ipt.exportImage(pngPath, 1280, 720));
        closeAnyPopups();

        QFileInfo pngInfo(pngPath);
        CMD_ASSERT(pngInfo.exists());
        CMD_ASSERT(pngInfo.size() > 2048);
        QImage loadedPng(pngPath);
        CMD_ASSERT(!loadedPng.isNull());
        // IptUnifiedDiagramWidget::exportImage renderiza com fator de escala 2x sobre width() x height()
        CMD_ASSERT(loadedPng.width() == 1920 && loadedPng.height() == 1080);
    });

    runner.add("SobreDialogAndManualDialogHeapLifecycleWA_DeleteOnClose", []() {
        // SobreDialog e ManualDialog configuram Qt::WA_DeleteOnClose — devem ser alocados no heap
        auto* sobre = new ConcreteMix::Gui::SobreDialog();
        QSignalSpy destroyedSobre(sobre, &QObject::destroyed);
        sobre->resize(800, 600);
        sobre->show();
        QTest::qWait(20);

        auto* sobreTabs = sobre->findChild<QTabWidget*>();
        CMD_ASSERT(sobreTabs != nullptr);
        CMD_ASSERT(sobreTabs->count() == 9);
        for (int i = 0; i < sobreTabs->count(); ++i) {
            sobreTabs->setCurrentIndex(i);
            QApplication::processEvents();
        }

        // Valida todas as abas (incluindo CLC, Copyright e ComboBox de LICENSES)
        auto* clcBrowser = qobject_cast<QTextBrowser*>(sobreTabs->widget(3));
        auto* copyBrowser = qobject_cast<QTextBrowser*>(sobreTabs->widget(4));
        CMD_ASSERT(clcBrowser != nullptr && !clcBrowser->toPlainText().trimmed().isEmpty());
        CMD_ASSERT(copyBrowser != nullptr && !copyBrowser->toPlainText().trimmed().isEmpty());

        auto* combo = sobreTabs->widget(8)->findChild<QComboBox*>();
        CMD_ASSERT(combo != nullptr);
        CMD_ASSERT(combo->count() == 11);

        auto* licBrowser = sobreTabs->widget(8)->findChild<QTextBrowser*>();
        CMD_ASSERT(licBrowser != nullptr);

        for (int idx = 0; idx < combo->count(); ++idx) {
            combo->setCurrentIndex(idx);
            QApplication::processEvents();
            CMD_ASSERT_MSG(!licBrowser->toPlainText().trimmed().isEmpty(),
                           combo->itemText(idx).toStdString());
        }

        // Alterna para uma licença intermediária e testa tradução dinâmica em tempo de execução
        const int targetLicIdx = 4;
        combo->setCurrentIndex(targetLicIdx);
        sobreTabs->setCurrentIndex(8);
        QApplication::processEvents();

        auto hasComboLabelText = [&](const QString& expectedText) {
            for (QLabel* lbl : sobreTabs->widget(8)->findChildren<QLabel*>()) {
                if (lbl->text() == expectedText) {
                    return true;
                }
            }
            return false;
        };

        ConcreteMix::Language::GerenciadorTraducao::instance().definirIdioma(QStringLiteral("en_US"));
        QApplication::processEvents();
        CMD_ASSERT(sobreTabs->tabText(3) == QStringLiteral("CLC / Licensing"));
        CMD_ASSERT(sobreTabs->tabText(4) == QStringLiteral("Copyright"));
        CMD_ASSERT(sobreTabs->tabText(8) == QStringLiteral("Full Licenses"));
        CMD_ASSERT(hasComboLabelText(QStringLiteral("Select license:")));
        CMD_ASSERT(combo->currentIndex() == targetLicIdx);
        const QString clcEnText = clcBrowser->toPlainText();
        const QString copyEnText = copyBrowser->toPlainText();
        CMD_ASSERT(!clcEnText.isEmpty() && !copyEnText.isEmpty());

        ConcreteMix::Language::GerenciadorTraducao::instance().definirIdioma(QStringLiteral("pt_BR"));
        QApplication::processEvents();
        CMD_ASSERT(sobreTabs->tabText(3) == QStringLiteral("CLC / Licenciamento"));
        CMD_ASSERT(sobreTabs->tabText(4) == QStringLiteral("Copyright"));
        CMD_ASSERT(sobreTabs->tabText(8) == QStringLiteral("Licenças Completas"));
        CMD_ASSERT(hasComboLabelText(QStringLiteral("Selecione a licença:")));
        CMD_ASSERT(combo->currentIndex() == targetLicIdx);
        CMD_ASSERT(clcBrowser->toPlainText() != clcEnText);
        CMD_ASSERT(copyBrowser->toPlainText() != copyEnText);

        if (auto* searchEdit = sobre->findChild<QLineEdit*>()) {
            searchEdit->setText(QStringLiteral("GNU"));
            QApplication::processEvents();
        }
        sobre->close();
        QApplication::processEvents();
        QTest::qWait(20);
        CMD_ASSERT(destroyedSobre.count() == 1);

        auto* manual = new ConcreteMix::Gui::ManualDialog();
        QSignalSpy destroyedManual(manual, &QObject::destroyed);
        manual->resize(900, 650);
        manual->show();
        QTest::qWait(20);

        auto* toc = manual->findChild<QListWidget*>();
        CMD_ASSERT(toc != nullptr);
        CMD_ASSERT(toc->count() == 14);
        toc->setCurrentRow(13);

        ConcreteMix::Language::GerenciadorTraducao::instance().definirIdioma(QStringLiteral("en_US"));
        QApplication::processEvents();
        CMD_ASSERT(toc->item(13)->text() == QStringLiteral("14. About Dialog, CLC, Copyright & Full Licenses"));

        ConcreteMix::Language::GerenciadorTraducao::instance().definirIdioma(QStringLiteral("pt_BR"));
        QApplication::processEvents();
        CMD_ASSERT(toc->item(13)->text() == QStringLiteral("14. Janela Sobre, CLC, Copyright e Licenças Completas"));

        if (auto* searchEdit = manual->findChild<QLineEdit*>()) {
            searchEdit->setText(QStringLiteral("IPT"));
            QApplication::processEvents();
        }
        manual->close();
        QApplication::processEvents();
        QTest::qWait(20);
        CMD_ASSERT(destroyedManual.count() == 1);
    });

    return runner.runAll();
}
