#pragma once

#include <QMainWindow>
#include <QTabWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QStatusBar>
#include <QPointer>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QWidgetAction>
#include <QDialog>
#include <QTextBrowser>
#include <QCloseEvent>
#include "../core/ProjectData.hpp"
#include "TabProjectParams.hpp"
#include "TabMaterials.hpp"
#include "TabMortarContent.hpp"
#include "TabExperimentalMixes.hpp"
#include "TabFinalMix.hpp"
#include "TabCosts.hpp"
#include "TabTechnicalReport.hpp"

namespace ConcreteMix::Gui {
class SobreDialog;
class ManualDialog;
}

namespace ConcreteMix {

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void closeEvent(QCloseEvent *event) override;
    void changeEvent(QEvent *event) override;
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;
    void retranslateUi();
    void applySystemTheme();
    bool isSystemInDarkMode() const;

private slots:
    void onPresetSelected(int index);
    void setApplicationLanguage(const QString &langCode);
    void onParametersChanged();
    void onMaterialsChanged();
    void onMortarContentChanged();
    void onExperimentalMixesChanged();
    void onCostsChanged();
    void recalculateAll();
    void onTabChanged(int index);
    void showAboutDialog();
    void showManualDialog();
    void showNormsDialog();
    void showProjectSettingsDialog();

private:
    void setupUI();
    void createMenuBar();
    void updateNormsDialogContent();
    void updateStatusBar(const MixDesignResult &res);
    void syncAllTabsFromData();
    void loadProjectMetadataSettings();
    void saveProjectMetadataSettings();

    ProjectData m_projectData;

    // Menus
    QMenu *m_menuFile = nullptr;
    QMenu *m_menuView = nullptr;
    QMenu *m_menuSettings = nullptr;
    QMenu *m_menuLanguage = nullptr;
    QMenu *m_menuHelp = nullptr;
    QAction *m_actExit = nullptr;
    QAction *m_actRecalculate = nullptr;
    QAction *m_actProjectSettings = nullptr;
    QAction *m_actProjectUseScenario = nullptr;
    QAction *m_actProjectUseManual = nullptr;
    QAction *m_actNorms = nullptr;
    QAction *m_actManual = nullptr;
    QAction *m_actAbout = nullptr;
    QAction *m_actLangPt = nullptr;
    QAction *m_actLangEn = nullptr;
    QAction *m_actViewTabs[7] = {nullptr};

    // Controles de Cenário da Planilha no menu Configurações
    QWidgetAction *m_presetWidgetAction = nullptr;
    QLabel *m_lblPreset = nullptr;
    QComboBox *m_comboPresets = nullptr;

    QTabWidget *m_tabWidget = nullptr;

    TabProjectParams *m_tabParams = nullptr;
    TabMaterials *m_tabMaterials = nullptr;
    TabMortarContent *m_tabMortar = nullptr;
    TabExperimentalMixes *m_tabExpMixes = nullptr;
    TabFinalMix *m_tabFinalMix = nullptr;
    TabCosts *m_tabCosts = nullptr;
    TabTechnicalReport *m_tabReport = nullptr;

    // Janelas de Diálogo Modeless (Ordem no Eixo Z independente)
    QPointer<ConcreteMix::Gui::SobreDialog> m_sobreDialog;
    QPointer<ConcreteMix::Gui::ManualDialog> m_manualDialog;
    QPointer<QDialog> m_normsDialog;
    QPointer<QTextBrowser> m_normsBrowser;
    QPointer<QPushButton> m_normsCloseBtn;

    // Status Bar Labels
    QLabel *m_statusFcj = nullptr;
    QLabel *m_statusMix = nullptr;
    QLabel *m_statusAc = nullptr;
    QLabel *m_statusCement = nullptr;
    QLabel *m_statusCompliance = nullptr;
};

} // namespace ConcreteMix
