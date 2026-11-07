#include "MainWindow.hpp"
#include "source/language/GerenciadorTraducao.hpp"
#include "SobreDialog.hpp"
#include "ManualDialog.hpp"

#include <QApplication>
#include <QCoreApplication>
#include <QFile>
#include <QToolBar>
#include <QMessageBox>
#include <QEvent>
#include <QIcon>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QActionGroup>
#include <QKeySequence>
#include <QStyleHints>
#include <QStyleFactory>
#include <QTimer>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QLineEdit>
#include <QSettings>

#if defined(Q_OS_WIN)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <dwmapi.h>
#endif

namespace ConcreteMix {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QIcon appIcon;
    const QStringList iconPaths = {
        QCoreApplication::applicationDirPath() + "/assets/icons/concrete_mix_design.ico",
        QCoreApplication::applicationDirPath() + "/source/assets/icons/concrete_mix_design.ico",
        "source/assets/icons/concrete_mix_design.ico",
        "assets/icons/concrete_mix_design.ico"
    };
    for (const auto& p : iconPaths) {
        if (QFile::exists(p)) {
            appIcon = QIcon(p);
            break;
        }
    }
    setWindowIcon(appIcon);


    // Carrega predefinição 0 (Referência inicial da planilha de ensaios UFPR)
    m_projectData.loadPresetReference0();
    loadProjectMetadataSettings();

    setupUI();
    createMenuBar();

    // Sincroniza dados com a UI
    syncAllTabsFromData();

    // Calcula e atualiza todas as abas
    recalculateAll();

    retranslateUi();

    // Monitora a troca dinâmica e automática de tema do Windows 11
    if (QGuiApplication::styleHints()) {
        connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged, this, [this](Qt::ColorScheme) {
            applySystemTheme();
        });
    }

    applySystemTheme();
}

MainWindow::~MainWindow()
{
    if (m_sobreDialog) {
        delete m_sobreDialog.data();
    }
    if (m_manualDialog) {
        delete m_manualDialog.data();
    }
    if (m_normsDialog) {
        delete m_normsDialog.data();
    }
}

void MainWindow::setupUI()
{
    resize(1200, 800);
    setMinimumSize(1000, 700);

    m_tabWidget = new QTabWidget(this);
    setCentralWidget(m_tabWidget);

    m_tabParams = new TabProjectParams(&m_projectData, this);
    m_tabMaterials = new TabMaterials(&m_projectData, this);
    m_tabMortar = new TabMortarContent(&m_projectData, this);
    m_tabExpMixes = new TabExperimentalMixes(&m_projectData, this);
    m_tabFinalMix = new TabFinalMix(&m_projectData, this);
    m_tabCosts = new TabCosts(&m_projectData, this);
    m_tabReport = new TabTechnicalReport(&m_projectData, this);

    m_tabWidget->addTab(m_tabParams, tr("1. Parâmetros de Projeto"));
    m_tabWidget->addTab(m_tabMaterials, tr("2. Materiais & Britas"));
    m_tabWidget->addTab(m_tabMortar, tr("3. Teor de Argamassa (α)"));
    m_tabWidget->addTab(m_tabExpMixes, tr("4. Traços & Diagrama IPT"));
    m_tabWidget->addTab(m_tabFinalMix, tr("5. Traço Final & Amassadas"));
    m_tabWidget->addTab(m_tabCosts, tr("6. Análise de Custos"));
    m_tabWidget->addTab(m_tabReport, tr("7. Relatório Técnico"));

    connect(m_tabParams, &TabProjectParams::parametersChanged, this, &MainWindow::onParametersChanged);
    connect(m_tabMaterials, &TabMaterials::materialsChanged, this, &MainWindow::onMaterialsChanged);
    connect(m_tabMortar, &TabMortarContent::mortarContentChanged, this, &MainWindow::onMortarContentChanged);
    connect(m_tabExpMixes, &TabExperimentalMixes::experimentalMixesChanged, this, &MainWindow::onExperimentalMixesChanged);
    connect(m_tabFinalMix, &TabFinalMix::finalMixChanged, this, &MainWindow::recalculateAll);
    connect(m_tabCosts, &TabCosts::costsChanged, this, &MainWindow::onCostsChanged);
    connect(m_tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);

    // Barra de status com indicadores principais
    m_statusFcj = new QLabel(this);
    m_statusAc = new QLabel(this);
    m_statusMix = new QLabel(this);
    m_statusCement = new QLabel(this);
    m_statusCompliance = new QLabel(this);

    statusBar()->addPermanentWidget(m_statusFcj);
    statusBar()->addPermanentWidget(m_statusAc);
    statusBar()->addPermanentWidget(m_statusMix);
    statusBar()->addPermanentWidget(m_statusCement);
    statusBar()->addPermanentWidget(m_statusCompliance);
}

void MainWindow::createMenuBar()
{
    // Menu Arquivo
    m_menuFile = menuBar()->addMenu(tr("&Arquivo"));
    m_actRecalculate = m_menuFile->addAction(tr("🔄 Recalcular Tudo"), this, &MainWindow::recalculateAll);
    m_actRecalculate->setShortcut(QKeySequence::Refresh);
    m_menuFile->addSeparator();
    m_actExit = m_menuFile->addAction(tr("&Sair"), this, &QWidget::close);
    m_actExit->setShortcut(QKeySequence::Quit);

    // Menu Exibir
    m_menuView = menuBar()->addMenu(tr("&Exibir"));
    m_actViewTabs[0] = m_menuView->addAction(tr("1. Parâmetros de Projeto"), [this]() { m_tabWidget->setCurrentIndex(0); });
    m_actViewTabs[1] = m_menuView->addAction(tr("2. Materiais & Britas"), [this]() { m_tabWidget->setCurrentIndex(1); });
    m_actViewTabs[2] = m_menuView->addAction(tr("3. Teor de Argamassa (α)"), [this]() { m_tabWidget->setCurrentIndex(2); });
    m_actViewTabs[3] = m_menuView->addAction(tr("4. Traços & Diagrama IPT"), [this]() { m_tabWidget->setCurrentIndex(3); });
    m_actViewTabs[4] = m_menuView->addAction(tr("5. Traço Final & Amassadas"), [this]() { m_tabWidget->setCurrentIndex(4); });
    m_actViewTabs[5] = m_menuView->addAction(tr("6. Análise de Custos"), [this]() { m_tabWidget->setCurrentIndex(5); });
    m_actViewTabs[6] = m_menuView->addAction(tr("7. Relatório Técnico"), [this]() { m_tabWidget->setCurrentIndex(6); });

    // Menu Configurações
    m_menuSettings = menuBar()->addMenu(tr("&Configurações"));

    m_presetWidgetAction = new QWidgetAction(m_menuSettings);
    auto *presetContainer = new QWidget(m_menuSettings);
    auto *presetLayout = new QHBoxLayout(presetContainer);
    presetLayout->setContentsMargins(12, 6, 12, 6);
    presetLayout->setSpacing(8);

    m_lblPreset = new QLabel(tr("📂 Cenário da Planilha:"), presetContainer);
    presetLayout->addWidget(m_lblPreset);

    m_comboPresets = new QComboBox(presetContainer);
    m_comboPresets->addItem(tr("Cenário 0: Personalizável pelo usuário"));
    m_comboPresets->addItem(tr("Cenário 1: Concreto C25 Urbano (CAA II) - CP II-F-32, Slump 100mm, Obra 30 m³"));
    m_comboPresets->addItem(tr("Cenário 2: Concreto C30 Marinho (CAA III) - CP III-40 RS, Britas 1+2, Obra 50 m³"));
    m_comboPresets->addItem(tr("Cenário 3: Concreto C40 Alto Desempenho (CAA IV) - CP V-ARI, Slump 160mm, Obra 100 m³"));
    m_comboPresets->addItem(tr("Cenário 4: Concreto C20 Fundações (CAA I - Condição B) - CP II-Z-32, Brita 2, Obra 40 m³"));
    m_comboPresets->addItem(tr("Cenário 5: Planilha UFPR Original Corrigida (TC031 - C30, Sd 3,5 MPa, α 52%, Obra 30 m³)"));
    m_comboPresets->setMinimumWidth(420);
    m_comboPresets->blockSignals(true);
    m_comboPresets->setCurrentIndex(1);
    m_comboPresets->blockSignals(false);
    presetLayout->addWidget(m_comboPresets, 1);

    m_presetWidgetAction->setDefaultWidget(presetContainer);
    m_menuSettings->addAction(m_presetWidgetAction);
    connect(m_comboPresets, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        onPresetSelected(index);
        if (m_menuSettings && m_menuSettings->isVisible()) {
            m_menuSettings->close();
        }
    });
    m_menuSettings->addSeparator();

    m_actProjectSettings = m_menuSettings->addAction(
        tr("📝 &Dados do Projeto e Identificação (Projeto, Responsável, Instituição, ID)..."),
        this,
        &MainWindow::showProjectSettingsDialog
    );
    m_actProjectSettings->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_I));
    m_menuSettings->addSeparator();

    auto projModeGroup = new QActionGroup(this);
    projModeGroup->setExclusive(true);

    m_actProjectUseScenario = m_menuSettings->addAction(tr("📂 Projeto: Reportar 'Cenário da Planilha' Automaticamente"));
    m_actProjectUseScenario->setCheckable(true);
    m_actProjectUseScenario->setChecked(m_projectData.usePresetProjectName);
    projModeGroup->addAction(m_actProjectUseScenario);

    m_actProjectUseManual = m_menuSettings->addAction(tr("✏️ Projeto: Editar Informações Manualmente..."));
    m_actProjectUseManual->setCheckable(true);
    m_actProjectUseManual->setChecked(!m_projectData.usePresetProjectName);
    projModeGroup->addAction(m_actProjectUseManual);

    connect(m_actProjectUseScenario, &QAction::triggered, this, [this]() {
        m_projectData.usePresetProjectName = true;
        m_projectData.projectName = ProjectData::getPresetName(m_projectData.currentPresetIndex, false);
        saveProjectMetadataSettings();
        recalculateAll();
    });

    connect(m_actProjectUseManual, &QAction::triggered, this, [this]() {
        m_projectData.usePresetProjectName = false;
        showProjectSettingsDialog();
    });

    // Menu Idioma
    m_menuLanguage = menuBar()->addMenu(tr("&Idioma"));
    auto *langGroup = new QActionGroup(this);
    langGroup->setExclusive(true);

    m_actLangPt = m_menuLanguage->addAction(QStringLiteral("🇧🇷 Português (BR)"), [this]() {
        setApplicationLanguage(QStringLiteral("pt_BR"));
    });
    m_actLangPt->setCheckable(true);
    langGroup->addAction(m_actLangPt);

    m_actLangEn = m_menuLanguage->addAction(QStringLiteral("🇺🇸 English (US)"), [this]() {
        setApplicationLanguage(QStringLiteral("en_US"));
    });
    m_actLangEn->setCheckable(true);
    langGroup->addAction(m_actLangEn);

    const QString langAtual = Language::GerenciadorTraducao::instance().codigoIdiomaAtual();
    if (langAtual == QStringLiteral("en_US")) {
        m_actLangEn->setChecked(true);
    } else {
        m_actLangPt->setChecked(true);
    }

    // Menu Ajuda
    m_menuHelp = menuBar()->addMenu(tr("A&juda"));
    m_actManual = m_menuHelp->addAction(tr("📖 &Manual do Usuário"), this, &MainWindow::showManualDialog);
    m_actManual->setShortcut(QKeySequence::HelpContents);
    m_actNorms = m_menuHelp->addAction(tr("📋 &Normas Técnicas (NBR)"), this, &MainWindow::showNormsDialog);
    m_menuHelp->addSeparator();
    m_actAbout = m_menuHelp->addAction(tr("ℹ️ &Sobre o Módulo de Dosagem de Concreto"), this, &MainWindow::showAboutDialog);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_sobreDialog) {
        m_sobreDialog->close();
    }
    if (m_manualDialog) {
        m_manualDialog->close();
    }
    if (m_normsDialog) {
        m_normsDialog->close();
    }
    if (qApp) {
        qApp->closeAllWindows();
    }
    QMainWindow::closeEvent(event);
}

bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
{
#if defined(Q_OS_WIN)
    MSG *msg = static_cast<MSG *>(message);
    if (msg->message == WM_SETTINGCHANGE) {
        const wchar_t *area = reinterpret_cast<const wchar_t *>(msg->lParam);
        if (!area || wcscmp(area, L"ImmersiveColorSet") == 0 || wcscmp(area, L"WindowsThemeElement") == 0) {
            QTimer::singleShot(50, this, &MainWindow::applySystemTheme);
        }
    } else if (msg->message == WM_THEMECHANGED) {
        QTimer::singleShot(50, this, &MainWindow::applySystemTheme);
    }
#endif
    return QMainWindow::nativeEvent(eventType, message, result);
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        retranslateUi();
    } else if (event->type() == QEvent::ThemeChange || event->type() == QEvent::ApplicationPaletteChange || event->type() == QEvent::PaletteChange) {
        applySystemTheme();
    }
    QMainWindow::changeEvent(event);
}

bool MainWindow::isSystemInDarkMode() const
{
#if defined(Q_OS_WIN)
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD val = 1;
        DWORD size = sizeof(val);
        if (RegQueryValueExW(hKey, L"AppsUseLightTheme", nullptr, nullptr, reinterpret_cast<LPBYTE>(&val), &size) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return (val == 0);
        }
        RegCloseKey(hKey);
    }
#endif
    if (QGuiApplication::styleHints()) {
        return QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;
    }
    return false;
}

static QPalette createThemePalette(bool isDark)
{
    QPalette pal;
    if (isDark) {
        pal.setColor(QPalette::Window, QColor(0x1e, 0x1e, 0x1e));
        pal.setColor(QPalette::WindowText, QColor(0xff, 0xff, 0xff));
        pal.setColor(QPalette::Base, QColor(0x2d, 0x2d, 0x2d));
        pal.setColor(QPalette::AlternateBase, QColor(0x35, 0x35, 0x35));
        pal.setColor(QPalette::ToolTipBase, QColor(0x3c, 0x3c, 0x3c));
        pal.setColor(QPalette::ToolTipText, QColor(0xff, 0xff, 0xff));
        pal.setColor(QPalette::Text, QColor(0xff, 0xff, 0xff));
        pal.setColor(QPalette::Button, QColor(0x2d, 0x2d, 0x2d));
        pal.setColor(QPalette::ButtonText, QColor(0xff, 0xff, 0xff));
        pal.setColor(QPalette::BrightText, QColor(0x65, 0xbc, 0xff));
        pal.setColor(QPalette::Link, QColor(0x65, 0xbc, 0xff));
        pal.setColor(QPalette::Highlight, QColor(0x00, 0x78, 0xd4));
        pal.setColor(QPalette::HighlightedText, QColor(0xff, 0xff, 0xff));
        pal.setColor(QPalette::Light, QColor(0x50, 0x50, 0x50));
        pal.setColor(QPalette::Midlight, QColor(0x3c, 0x3c, 0x3c));
        pal.setColor(QPalette::Dark, QColor(0x15, 0x15, 0x15));
        pal.setColor(QPalette::Mid, QColor(0x40, 0x40, 0x40));
        pal.setColor(QPalette::Shadow, QColor(0x00, 0x00, 0x00));

        pal.setColor(QPalette::Disabled, QPalette::WindowText, QColor(0x80, 0x80, 0x80));
        pal.setColor(QPalette::Disabled, QPalette::Text, QColor(0x80, 0x80, 0x80));
        pal.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(0x80, 0x80, 0x80));
    } else {
        pal.setColor(QPalette::Window, QColor(0xf3, 0xf3, 0xf3));
        pal.setColor(QPalette::WindowText, QColor(0x00, 0x00, 0x00));
        pal.setColor(QPalette::Base, QColor(0xff, 0xff, 0xff));
        pal.setColor(QPalette::AlternateBase, QColor(0xf8, 0xf8, 0xf8));
        pal.setColor(QPalette::ToolTipBase, QColor(0xff, 0xff, 0xdc));
        pal.setColor(QPalette::ToolTipText, QColor(0x00, 0x00, 0x00));
        pal.setColor(QPalette::Text, QColor(0x00, 0x00, 0x00));
        pal.setColor(QPalette::Button, QColor(0xf0, 0xf0, 0xf0));
        pal.setColor(QPalette::ButtonText, QColor(0x00, 0x00, 0x00));
        pal.setColor(QPalette::BrightText, QColor(0xff, 0xff, 0xff));
        pal.setColor(QPalette::Link, QColor(0x00, 0x67, 0xb8));
        pal.setColor(QPalette::Highlight, QColor(0x00, 0x78, 0xd4));
        pal.setColor(QPalette::HighlightedText, QColor(0xff, 0xff, 0xff));
        pal.setColor(QPalette::Light, QColor(0xff, 0xff, 0xff));
        pal.setColor(QPalette::Midlight, QColor(0xe5, 0xe5, 0xe5));
        pal.setColor(QPalette::Dark, QColor(0xa0, 0xa0, 0xa0));
        pal.setColor(QPalette::Mid, QColor(0xd0, 0xd0, 0xd0));
        pal.setColor(QPalette::Shadow, QColor(0x70, 0x70, 0x70));

        pal.setColor(QPalette::Disabled, QPalette::WindowText, QColor(0x80, 0x80, 0x80));
        pal.setColor(QPalette::Disabled, QPalette::Text, QColor(0x80, 0x80, 0x80));
        pal.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(0x80, 0x80, 0x80));
    }
    return pal;
}

void MainWindow::applySystemTheme()
{
    static bool inThemeUpdate = false;
    if (inThemeUpdate) return;
    inThemeUpdate = true;

    const bool isDark = isSystemInDarkMode();

#if defined(Q_OS_WIN)
    HWND hwnd = reinterpret_cast<HWND>(winId());
    if (hwnd) {
        BOOL useDark = isDark ? TRUE : FALSE;
        // DWMWA_USE_IMMERSIVE_DARK_MODE: 20 no Windows 11 e Windows 10 build 19041+, 19 em builds anteriores
        if (FAILED(DwmSetWindowAttribute(hwnd, 20, &useDark, sizeof(useDark)))) {
            DwmSetWindowAttribute(hwnd, 19, &useDark, sizeof(useDark));
        }
    }
#endif

    // Aplica paleta nativa completa sincronizada com o tema do sistema
    QPalette themePal = createThemePalette(isDark);
    qApp->setPalette(themePal);
    setPalette(themePal);

    // Garante que os controles utilizem a renderização e spin buttons 100% nativos e padronizados do Qt6 (Windows 11)
    setStyleSheet(QString());

    // Atualiza tema dos diálogos se existirem
    if (m_sobreDialog) {
        m_sobreDialog->setPalette(themePal);
#if defined(Q_OS_WIN)
        HWND sHwnd = reinterpret_cast<HWND>(m_sobreDialog->winId());
        if (sHwnd) {
            BOOL useDark = isDark ? TRUE : FALSE;
            if (FAILED(DwmSetWindowAttribute(sHwnd, 20, &useDark, sizeof(useDark)))) {
                DwmSetWindowAttribute(sHwnd, 19, &useDark, sizeof(useDark));
            }
        }
#endif
    }
    if (m_manualDialog) {
        m_manualDialog->setPalette(themePal);
#if defined(Q_OS_WIN)
        HWND mHwnd = reinterpret_cast<HWND>(m_manualDialog->winId());
        if (mHwnd) {
            BOOL useDark = isDark ? TRUE : FALSE;
            if (FAILED(DwmSetWindowAttribute(mHwnd, 20, &useDark, sizeof(useDark)))) {
                DwmSetWindowAttribute(mHwnd, 19, &useDark, sizeof(useDark));
            }
        }
#endif
    }
    if (m_normsDialog) {
        m_normsDialog->setPalette(themePal);
        updateNormsDialogContent();
#if defined(Q_OS_WIN)
        HWND nHwnd = reinterpret_cast<HWND>(m_normsDialog->winId());
        if (nHwnd) {
            BOOL useDark = isDark ? TRUE : FALSE;
            if (FAILED(DwmSetWindowAttribute(nHwnd, 20, &useDark, sizeof(useDark)))) {
                DwmSetWindowAttribute(nHwnd, 19, &useDark, sizeof(useDark));
            }
        }
#endif
    }

    // Atualiza tema dos gráficos do Diagrama IPT (4 quadrantes)
    if (m_tabExpMixes) {
        m_tabExpMixes->updateTheme(isDark);
    }

    // Atualiza tema do memorial de cálculo em HTML
    if (m_tabReport) {
        m_tabReport->refreshReport();
    }

    update();
    inThemeUpdate = false;
}

void MainWindow::retranslateUi()
{
    bool isEn = (Language::GerenciadorTraducao::instance().codigoIdiomaAtual() == QStringLiteral("en_US"));

    setWindowTitle(isEn ? QStringLiteral("Concrete Mix Design Module (IPT/EPUSP Method) - NBR 12655 / NBR 6118")
                        : QStringLiteral("Módulo de Dosagem de Concreto (Método IPT/EPUSP) - NBR 12655 / NBR 6118"));

    if (m_menuFile) m_menuFile->setTitle(tr("&Arquivo"));
    if (m_menuView) m_menuView->setTitle(tr("&Exibir"));
    if (m_menuSettings) m_menuSettings->setTitle(tr("&Configurações"));
    if (m_menuLanguage) m_menuLanguage->setTitle(tr("&Idioma"));
    if (m_menuHelp) m_menuHelp->setTitle(tr("A&juda"));

    if (m_actRecalculate) m_actRecalculate->setText(tr("🔄 Recalcular Tudo"));
    if (m_actExit) m_actExit->setText(tr("&Sair"));
    if (m_actProjectSettings) m_actProjectSettings->setText(tr("📝 &Dados do Projeto e Identificação (Projeto, Responsável, Instituição, ID)..."));
    if (m_actProjectUseScenario) m_actProjectUseScenario->setText(tr("📂 Projeto: Reportar 'Cenário da Planilha' Automaticamente"));
    if (m_actProjectUseManual) m_actProjectUseManual->setText(tr("✏️ Projeto: Editar Informações Manualmente..."));
    if (m_actManual) m_actManual->setText(tr("📖 &Manual do Usuário"));
    if (m_actNorms) m_actNorms->setText(tr("📋 &Normas Técnicas (NBR)"));
    if (m_actAbout) m_actAbout->setText(tr("ℹ️ &Sobre o Módulo de Dosagem de Concreto"));

    if (m_actViewTabs[0]) m_actViewTabs[0]->setText(tr("1. Parâmetros de Projeto"));
    if (m_actViewTabs[1]) m_actViewTabs[1]->setText(tr("2. Materiais & Britas"));
    if (m_actViewTabs[2]) m_actViewTabs[2]->setText(tr("3. Teor de Argamassa (α)"));
    if (m_actViewTabs[3]) m_actViewTabs[3]->setText(tr("4. Traços & Diagrama IPT"));
    if (m_actViewTabs[4]) m_actViewTabs[4]->setText(tr("5. Traço Final & Amassadas"));
    if (m_actViewTabs[5]) m_actViewTabs[5]->setText(tr("6. Análise de Custos"));
    if (m_actViewTabs[6]) m_actViewTabs[6]->setText(tr("7. Relatório Técnico"));

    if (m_lblPreset) m_lblPreset->setText(tr("📂 Cenário da Planilha:"));

    if (m_comboPresets) {
        m_comboPresets->blockSignals(true);
        m_comboPresets->setItemText(0, tr("Cenário 0: Personalizável pelo usuário"));
        m_comboPresets->setItemText(1, tr("Cenário 1: Concreto C25 Urbano (CAA II) - CP II-F-32, Slump 100mm, Obra 30 m³"));
        m_comboPresets->setItemText(2, tr("Cenário 2: Concreto C30 Marinho (CAA III) - CP III-40 RS, Britas 1+2, Obra 50 m³"));
        m_comboPresets->setItemText(3, tr("Cenário 3: Concreto C40 Alto Desempenho (CAA IV) - CP V-ARI, Slump 160mm, Obra 100 m³"));
        m_comboPresets->setItemText(4, tr("Cenário 4: Concreto C20 Fundações (CAA I - Condição B) - CP II-Z-32, Brita 2, Obra 40 m³"));
        if (m_comboPresets->count() > 5) {
            m_comboPresets->setItemText(5, tr("Cenário 5: Planilha UFPR Original Corrigida (TC031 - C30, Sd 3,5 MPa, α 52%, Obra 30 m³)"));
        }
        m_comboPresets->blockSignals(false);
    }

    if (m_tabWidget) {
        m_tabWidget->setTabText(0, tr("1. Parâmetros de Projeto"));
        m_tabWidget->setTabText(1, tr("2. Materiais & Britas"));
        m_tabWidget->setTabText(2, tr("3. Teor de Argamassa (α)"));
        m_tabWidget->setTabText(3, tr("4. Traços & Diagrama IPT"));
        m_tabWidget->setTabText(4, tr("5. Traço Final & Amassadas"));
        m_tabWidget->setTabText(5, tr("6. Análise de Custos"));
        m_tabWidget->setTabText(6, tr("7. Relatório Técnico"));
    }

    if (m_tabParams) m_tabParams->retranslateUi();
    if (m_tabMaterials) m_tabMaterials->retranslateUi();
    if (m_tabMortar) m_tabMortar->retranslateUi();
    if (m_tabExpMixes) m_tabExpMixes->retranslateUi();
    if (m_tabFinalMix) m_tabFinalMix->retranslateUi();
    if (m_tabCosts) m_tabCosts->retranslateUi();
    if (m_tabReport) m_tabReport->retranslateUi();

    if (m_sobreDialog) {
        m_sobreDialog->retranslateUi();
    }
    if (m_manualDialog) {
        m_manualDialog->retranslateUi();
    }
    if (m_normsDialog) {
        updateNormsDialogContent();
    }

    const QString langAtual = Language::GerenciadorTraducao::instance().codigoIdiomaAtual();
    if (m_actLangPt && m_actLangEn) {
        if (langAtual == QStringLiteral("en_US")) {
            m_actLangEn->setChecked(true);
        } else {
            m_actLangPt->setChecked(true);
        }
    }

    recalculateAll();
}

void MainWindow::setApplicationLanguage(const QString &langCode)
{
    Language::GerenciadorTraducao::instance().definirIdioma(langCode);
    retranslateUi();
}

void MainWindow::onPresetSelected(int index)
{
    switch (index) {
    case 0:
        m_projectData.loadPresetUserCustom();
        break;
    case 1:
        m_projectData.loadPresetReference0();
        break;
    case 2:
        m_projectData.loadPresetResultados1();
        break;
    case 3:
        m_projectData.loadPresetResultados2();
        break;
    case 4:
        m_projectData.loadPresetResultados3();
        break;
    case 5:
        m_projectData.loadPresetPlanilhaUFPR();
        break;
    default:
        break;
    }

    syncAllTabsFromData();
    recalculateAll();
}

void MainWindow::syncAllTabsFromData()
{
    m_tabParams->updateUIFromData();
    m_tabMaterials->updateUIFromData();
    m_tabMortar->updateUIFromData();
    m_tabExpMixes->updateUIFromData();
    m_tabFinalMix->updateUIFromData();
    m_tabCosts->updateUIFromData();
    m_tabReport->refreshReport();
}

void MainWindow::onParametersChanged()
{
    m_tabParams->updateDataFromUI();
    recalculateAll();
}

void MainWindow::onMaterialsChanged()
{
    m_tabMaterials->updateDataFromUI();
    m_projectData.updatePilotBatchFromMixerVolume();
    if (m_tabMortar) m_tabMortar->updateUIFromData();
    recalculateAll();
}

void MainWindow::onMortarContentChanged()
{
    m_tabMortar->updateDataFromUI();
    recalculateAll();
}

void MainWindow::onExperimentalMixesChanged()
{
    m_tabExpMixes->updateDataFromUI();
    recalculateAll();
}

void MainWindow::onCostsChanged()
{
    m_tabCosts->refreshResults();
    m_tabReport->refreshReport();
    MixDesignResult res = m_projectData.calculate();
    updateStatusBar(res);
}

void MainWindow::recalculateAll()
{
    if (m_tabMortar) {
        m_tabMortar->updateUIFromData();
    }
    if (m_tabExpMixes) {
        m_tabExpMixes->calculateAndRefresh(false);
    }

    MixDesignResult res = m_projectData.calculate();

    if (m_tabParams) m_tabParams->updateNormativeCards();
    if (m_tabFinalMix) m_tabFinalMix->refreshResults();
    if (m_tabCosts) m_tabCosts->refreshResults();
    if (m_tabReport) m_tabReport->refreshReport();
    updateStatusBar(res);
}

void MainWindow::onTabChanged(int index)
{
    if (index == 0 && m_tabParams) {
        m_tabParams->updateNormativeCards();
    } else if (index == 3 && m_tabExpMixes) {
        m_tabExpMixes->calculateAndRefresh(false);
    } else if (index == 4 && m_tabFinalMix) {
        m_tabFinalMix->refreshResults();
    } else if (index == 5 && m_tabCosts) {
        m_tabCosts->refreshResults();
    } else if (index == 6 && m_tabReport) {
        m_tabReport->refreshReport();
    }
}

void MainWindow::updateStatusBar(const MixDesignResult &res)
{
    if (m_projectData.fck <= 0.0 || res.targetWaterCement <= 0.0) {
        m_statusFcj->setText(tr("<b>fcj:</b> - MPa (fck - MPa, Sd - MPa)"));
        m_statusAc->setText(tr("<b>a/c:</b> -"));
        m_statusMix->setText(tr("<b>Traço Seco:</b> -"));
        m_statusCement->setText(tr("<b>Consumo Cimento:</b> - kg/m³"));
        m_statusCompliance->setText(tr("<span style='color: #888888; font-weight: bold;'>Aguardando dados</span>"));
        return;
    }

    m_statusFcj->setText(tr("<b>fcj:</b> %1 MPa (fck %2 MPa, Sd %3 MPa)")
                             .arg(res.fcj, 0, 'f', 2)
                             .arg(m_projectData.fck, 0, 'f', 1)
                             .arg(res.sd, 0, 'f', 1));

    m_statusAc->setText(tr("<b>a/c:</b> %1").arg(res.targetWaterCement, 0, 'f', 4));

    m_statusMix->setText(tr("<b>Traço Seco:</b> 1 : %1 : %2 (1 : %3)")
                             .arg(res.sandRatioA, 0, 'f', 2)
                             .arg(res.gravelRatioP, 0, 'f', 2)
                             .arg(res.targetDryMixM, 0, 'f', 2));

    m_statusCement->setText(tr("<b>Consumo Cimento:</b> %1 kg/m³")
                               .arg(res.targetCementConsumption, 0, 'f', 1));

    if (res.compliance.isCompliant) {
        m_statusCompliance->setText(tr("<span style='color: green; font-weight: bold;'>✔ NBR 12655: CONFORME</span>"));
    } else {
        m_statusCompliance->setText(tr("<span style='color: red; font-weight: bold;'>⚠ NBR: NÃO CONFORME</span>"));
    }
}

void MainWindow::updateNormsDialogContent()
{
    if (!m_normsDialog) {
        return;
    }

    const bool isDark = isSystemInDarkMode();
    const QString styleSheet = isDark
        ? QStringLiteral(
            "body { font-family: sans-serif; line-height: 1.5; padding: 10px; color: #e8e8e8; background-color: #2d2d2d; }"
            "h3 { color: #65bcff; border-bottom: 2px solid #3d8bd4; padding-bottom: 4px; margin-top: 4px; }"
            "li { margin-bottom: 8px; }"
          )
        : QStringLiteral(
            "body { font-family: sans-serif; line-height: 1.5; padding: 10px; color: #1a1a1a; background-color: #ffffff; }"
            "h3 { color: #005a9e; border-bottom: 2px solid #005a9e; padding-bottom: 4px; margin-top: 4px; }"
            "li { margin-bottom: 8px; }"
          );

    const QString info = QStringLiteral("<html><head><style>") + styleSheet + QStringLiteral("</style></head><body>") + tr(
        "<h3>Normas Técnicas Brasileiras Aplicadas</h3>"
        "<ul>"
        "<li><b>ABNT NBR 12655:2022:</b> Concreto de cimento Portland – Preparo, controle, recebimento e aceitação. Define classes de agressividade ambiental (CAA I a IV), relação a/c máxima, consumo mínimo de cimento, fck mínimo e desvios-padrão de dosagem (Sd = 4,0; 5,5; 7,0 MPa ou histórico com n ≥ 20).</li>"
        "<li><b>ABNT NBR 6118:2023:</b> Projeto de estruturas de concreto – Procedimento. Especifica cobrimentos nominais de armadura (cnom de 20 a 50 mm) e durabilidade estrutural.</li>"
        "<li><b>ABNT NBR 7211:2022:</b> Agregados para concreto – Requisitos. Zonas granulométricas (ótima e utilizável), módulo de finura (MF), dimensão máxima característica (DMC) e limites de substâncias nocivas.</li>"
        "<li><b>ABNT NBR 7212:2025:</b> Concreto dosado em central – Preparo, fornecimento e controle. Tolerâncias de pesagem em balança dosadora (item 5.5): Cimento e Adições (±1%), Agregados (±3%), Água (±3%) e Aditivos (±5%).</li>"
        "<li><b>ABNT NBR 8953:2015:</b> Concreto para fins estruturais – Classificação por grupos de resistência (C20 a C100) e consistência (S10 a S220 pelo abatimento de tronco de cone).</li>"
        "<li><b>Mehta & Monteiro (2014):</b> Concreto: Microestrutura, Propriedades e Materiais. Fundamentação da Lei de Abrams, transição interfacial matriz-agregado e empacotamento granular.</li>"
        "</ul>") + QStringLiteral("</body></html>");

    m_normsDialog->setWindowTitle(tr("Quadro de Normas Técnicas"));
    if (m_normsBrowser) {
        m_normsBrowser->setHtml(info);
    }
    if (m_normsCloseBtn) {
        m_normsCloseBtn->setText(tr("Fechar"));
    }
}

void MainWindow::showNormsDialog()
{
    if (!m_normsDialog) {
        m_normsDialog = new QDialog(nullptr);
        m_normsDialog->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint |
                                      Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
        m_normsDialog->setWindowModality(Qt::NonModal);
        m_normsDialog->setModal(false);
        m_normsDialog->setWindowIcon(windowIcon());
        m_normsDialog->resize(720, 480);
        m_normsDialog->setMinimumSize(560, 380);

        auto *layout = new QVBoxLayout(m_normsDialog);
        layout->setContentsMargins(12, 12, 12, 12);
        layout->setSpacing(10);

        m_normsBrowser = new QTextBrowser(m_normsDialog);
        m_normsBrowser->setReadOnly(true);
        m_normsBrowser->setOpenExternalLinks(true);
        layout->addWidget(m_normsBrowser, 1);

        auto *btnLayout = new QHBoxLayout();
        btnLayout->addStretch(1);
        m_normsCloseBtn = new QPushButton(m_normsDialog);
        m_normsCloseBtn->setAutoDefault(false);
        m_normsCloseBtn->setDefault(false);
        btnLayout->addWidget(m_normsCloseBtn);
        layout->addLayout(btnLayout);

        connect(m_normsCloseBtn, &QPushButton::clicked, m_normsDialog, &QDialog::close);
    }

    const bool isDark = isSystemInDarkMode();
    QPalette themePal = createThemePalette(isDark);
    m_normsDialog->setPalette(themePal);
    updateNormsDialogContent();
#if defined(Q_OS_WIN)
    HWND nHwnd = reinterpret_cast<HWND>(m_normsDialog->winId());
    if (nHwnd) {
        BOOL useDark = isDark ? TRUE : FALSE;
        if (FAILED(DwmSetWindowAttribute(nHwnd, 20, &useDark, sizeof(useDark)))) {
            DwmSetWindowAttribute(nHwnd, 19, &useDark, sizeof(useDark));
        }
    }
#endif
    if (m_normsDialog->isMinimized()) {
        m_normsDialog->showNormal();
    }
    m_normsDialog->show();
    m_normsDialog->raise();
    m_normsDialog->activateWindow();
}

void MainWindow::showAboutDialog()
{
    if (!m_sobreDialog) {
        m_sobreDialog = new ConcreteMix::Gui::SobreDialog(nullptr);
    }
    const bool isDark = isSystemInDarkMode();
    QPalette themePal = createThemePalette(isDark);
    m_sobreDialog->setPalette(themePal);
#if defined(Q_OS_WIN)
    HWND sHwnd = reinterpret_cast<HWND>(m_sobreDialog->winId());
    if (sHwnd) {
        BOOL useDark = isDark ? TRUE : FALSE;
        if (FAILED(DwmSetWindowAttribute(sHwnd, 20, &useDark, sizeof(useDark)))) {
            DwmSetWindowAttribute(sHwnd, 19, &useDark, sizeof(useDark));
        }
    }
#endif
    if (m_sobreDialog->isMinimized()) {
        m_sobreDialog->showNormal();
    }
    m_sobreDialog->show();
    m_sobreDialog->raise();
    m_sobreDialog->activateWindow();
}

void MainWindow::showManualDialog()
{
    if (!m_manualDialog) {
        m_manualDialog = new ConcreteMix::Gui::ManualDialog(nullptr);
    }
    const bool isDark = isSystemInDarkMode();
    QPalette themePal = createThemePalette(isDark);
    m_manualDialog->setPalette(themePal);
#if defined(Q_OS_WIN)
    HWND mHwnd = reinterpret_cast<HWND>(m_manualDialog->winId());
    if (mHwnd) {
        BOOL useDark = isDark ? TRUE : FALSE;
        if (FAILED(DwmSetWindowAttribute(mHwnd, 20, &useDark, sizeof(useDark)))) {
            DwmSetWindowAttribute(mHwnd, 19, &useDark, sizeof(useDark));
        }
    }
#endif
    if (m_manualDialog->isMinimized()) {
        m_manualDialog->showNormal();
    }
    m_manualDialog->show();
    m_manualDialog->raise();
    m_manualDialog->activateWindow();
}

void MainWindow::loadProjectMetadataSettings()
{
    QSettings settings(QStringLiteral("AppsCpp"), QStringLiteral("ConcreteMixDesign"));
    m_projectData.usePresetProjectName = settings.value(QStringLiteral("project/usePresetProjectName"), true).toBool();
    m_projectData.customProjectName = settings.value(QStringLiteral("project/customProjectName"), QString()).toString().toStdString();
    m_projectData.engineerName = settings.value(QStringLiteral("project/engineerName"), QStringLiteral("Eng. Civil")).toString().toStdString();
    m_projectData.institution = settings.value(QStringLiteral("project/institution"), QStringLiteral("UFPR - Universidade Federal do Paraná")).toString().toStdString();
    m_projectData.studentId = settings.value(QStringLiteral("project/studentId"), QStringLiteral("GRR20182393")).toString().toStdString();

    if (!m_projectData.usePresetProjectName && !m_projectData.customProjectName.empty()) {
        m_projectData.projectName = m_projectData.customProjectName;
    } else {
        m_projectData.projectName = ProjectData::getPresetName(m_projectData.currentPresetIndex, false);
    }
}

void MainWindow::saveProjectMetadataSettings()
{
    QSettings settings(QStringLiteral("AppsCpp"), QStringLiteral("ConcreteMixDesign"));
    settings.setValue(QStringLiteral("project/usePresetProjectName"), m_projectData.usePresetProjectName);
    settings.setValue(QStringLiteral("project/customProjectName"), QString::fromStdString(m_projectData.customProjectName));
    settings.setValue(QStringLiteral("project/engineerName"), QString::fromStdString(m_projectData.engineerName));
    settings.setValue(QStringLiteral("project/institution"), QString::fromStdString(m_projectData.institution));
    settings.setValue(QStringLiteral("project/studentId"), QString::fromStdString(m_projectData.studentId));
    settings.sync();
}

void MainWindow::showProjectSettingsDialog()
{
    QDialog dlg(this);
    dlg.setWindowTitle(tr("Configurações — Dados do Projeto e Identificação"));
    dlg.setMinimumWidth(620);

    const bool isDark = isSystemInDarkMode();
    QPalette themePal = createThemePalette(isDark);
    dlg.setPalette(themePal);
#if defined(Q_OS_WIN)
    HWND dHwnd = reinterpret_cast<HWND>(dlg.winId());
    if (dHwnd) {
        BOOL useDark = isDark ? TRUE : FALSE;
        if (FAILED(DwmSetWindowAttribute(dHwnd, 20, &useDark, sizeof(useDark)))) {
            DwmSetWindowAttribute(dHwnd, 19, &useDark, sizeof(useDark));
        }
    }
#endif

    auto mainLayout = new QVBoxLayout(&dlg);
    mainLayout->setContentsMargins(18, 18, 18, 18);
    mainLayout->setSpacing(14);

    // 1. Modo de preenchimento do campo "Projeto:"
    auto grpMode = new QGroupBox(tr("1. Origem das Informações Reportadas em 'Projeto:'"), &dlg);
    auto vMode = new QVBoxLayout(grpMode);
    vMode->setSpacing(8);

    auto rbScenario = new QRadioButton(tr("Reportar as informações selecionadas em 'Cenário da Planilha' (tradução dinâmica)"), grpMode);
    auto rbManual = new QRadioButton(tr("Editar as informações de 'Projeto:' manualmente"), grpMode);

    rbScenario->setChecked(m_projectData.usePresetProjectName);
    rbManual->setChecked(!m_projectData.usePresetProjectName);

    vMode->addWidget(rbScenario);
    vMode->addWidget(rbManual);
    mainLayout->addWidget(grpMode);

    // 2. Campos de Identificação do Cabeçalho
    auto grpFields = new QGroupBox(tr("2. Dados de Identificação do Relatório Técnico"), &dlg);
    auto form = new QFormLayout(grpFields);
    form->setSpacing(10);

    const bool isEn = (Language::GerenciadorTraducao::instance().codigoIdiomaAtual() == QStringLiteral("en_US"));
    const QString currentScenarioText = m_comboPresets
        ? m_comboPresets->currentText()
        : QString::fromStdString(ProjectData::getPresetName(m_projectData.currentPresetIndex, isEn));

    auto editProject = new QLineEdit(grpFields);
    editProject->setPlaceholderText(tr("Digite o nome ou identificação personalizada do projeto..."));

    QString initialManualText = QString::fromStdString(m_projectData.customProjectName);
    if (initialManualText.isEmpty() && !m_projectData.usePresetProjectName) {
        initialManualText = QString::fromStdString(m_projectData.projectName);
    }

    auto syncProjectEditState = [rbScenario, editProject, currentScenarioText, &initialManualText]() {
        if (rbScenario->isChecked()) {
            if (!editProject->isReadOnly()) {
                initialManualText = editProject->text();
            }
            editProject->setText(currentScenarioText);
            editProject->setReadOnly(true);
            editProject->setEnabled(false);
        } else {
            editProject->setReadOnly(false);
            editProject->setEnabled(true);
            editProject->setText(initialManualText);
            editProject->setFocus();
            editProject->selectAll();
        }
    };

    syncProjectEditState();

    connect(rbScenario, &QRadioButton::toggled, &dlg, [syncProjectEditState](bool) {
        syncProjectEditState();
    });

    auto editEngineer = new QLineEdit(QString::fromStdString(m_projectData.engineerName), grpFields);
    editEngineer->setPlaceholderText(tr("Ex.: Eng. Civil / Responsável Técnico / CREA"));

    auto editInstitution = new QLineEdit(QString::fromStdString(m_projectData.institution), grpFields);
    editInstitution->setPlaceholderText(tr("Ex.: UFPR - Universidade Federal do Paraná / Empresa"));

    auto editStudentId = new QLineEdit(QString::fromStdString(m_projectData.studentId), grpFields);
    editStudentId->setPlaceholderText(tr("Ex.: GRR20182393 / Matrícula / Código ART"));

    form->addRow(tr("Projeto:"), editProject);
    form->addRow(tr("Responsável:"), editEngineer);
    form->addRow(tr("Instituição:"), editInstitution);
    form->addRow(tr("Matrícula / ID:"), editStudentId);

    mainLayout->addWidget(grpFields);

    // 3. Botões de Ação
    auto hBtns = new QHBoxLayout();
    auto btnReset = new QPushButton(tr("Restaurar Padrão"), &dlg);
    auto btnCancel = new QPushButton(tr("Cancelar"), &dlg);
    auto btnSave = new QPushButton(tr("Salvar e Aplicar"), &dlg);
    btnSave->setDefault(true);

    hBtns->addWidget(btnReset);
    hBtns->addStretch();
    hBtns->addWidget(btnCancel);
    hBtns->addWidget(btnSave);
    mainLayout->addLayout(hBtns);

    connect(btnReset, &QPushButton::clicked, &dlg, [&]() {
        initialManualText.clear();
        rbScenario->setChecked(true);
        editEngineer->setText(QStringLiteral("Eng. Civil"));
        editInstitution->setText(QStringLiteral("UFPR - Universidade Federal do Paraná"));
        editStudentId->setText(QStringLiteral("GRR20182393"));
        syncProjectEditState();
    });

    connect(btnCancel, &QPushButton::clicked, &dlg, &QDialog::reject);
    connect(btnSave, &QPushButton::clicked, &dlg, &QDialog::accept);

    if (dlg.exec() == QDialog::Accepted) {
        m_projectData.usePresetProjectName = rbScenario->isChecked();
        if (!m_projectData.usePresetProjectName) {
            m_projectData.customProjectName = editProject->text().trimmed().toStdString();
            m_projectData.projectName = m_projectData.customProjectName;
        } else {
            m_projectData.projectName = ProjectData::getPresetName(m_projectData.currentPresetIndex, false);
        }
        m_projectData.engineerName = editEngineer->text().trimmed().toStdString();
        m_projectData.institution = editInstitution->text().trimmed().toStdString();
        m_projectData.studentId = editStudentId->text().trimmed().toStdString();

        saveProjectMetadataSettings();
        recalculateAll();
    }

    if (m_actProjectUseScenario) m_actProjectUseScenario->setChecked(m_projectData.usePresetProjectName);
    if (m_actProjectUseManual) m_actProjectUseManual->setChecked(!m_projectData.usePresetProjectName);
}

} // namespace ConcreteMix
