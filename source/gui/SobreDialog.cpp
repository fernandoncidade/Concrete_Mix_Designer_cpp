#include "SobreDialog.hpp"
#include "source/language/GerenciadorTraducao.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>
#include <QTabWidget>
#include <QTextBrowser>
#include <QPushButton>
#include <QIcon>
#include <QPixmap>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>
#include <QSignalBlocker>
#include <QTextCursor>
#include <QTextDocument>
#include <QEvent>

namespace ConcreteMix::Gui {

SobreDialog::SobreDialog(QWidget* parent)
    : QDialog(parent)
{
    setupUi();
    retranslateUi();
}

void SobreDialog::setupUi()
{
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint |
                   Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    setWindowModality(Qt::NonModal);
    setModal(false);
    setAttribute(Qt::WA_DeleteOnClose, true);
    resize(860, 620);
    setMinimumSize(660, 480);

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

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(10);

    // Header layout with icon and details
    auto* headerLayout = new QHBoxLayout();
    headerLayout->setSpacing(14);

    headerIconLabel_ = new QLabel(this);
    QPixmap iconPix = appIcon.pixmap(56, 56);
    if (!iconPix.isNull()) {
        headerIconLabel_->setPixmap(iconPix);
    }
    headerIconLabel_->setAlignment(Qt::AlignCenter);
    headerLayout->addWidget(headerIconLabel_);

    headerTextLabel_ = new QLabel(this);
    headerTextLabel_->setTextFormat(Qt::RichText);
    headerTextLabel_->setWordWrap(true);
    headerLayout->addWidget(headerTextLabel_, 1);

    mainLayout->addLayout(headerLayout);

    // Search Layout
    auto* searchLayout = new QHBoxLayout();
    searchLayout->setSpacing(6);

    searchLabel_ = new QLabel(this);
    searchLayout->addWidget(searchLabel_);

    searchEdit_ = new QLineEdit(this);
    searchEdit_->setClearButtonEnabled(true);
    searchLayout->addWidget(searchEdit_, 1);

    searchPrevBtn_ = new QToolButton(this);
    searchPrevBtn_->setArrowType(Qt::LeftArrow);
    searchPrevBtn_->setEnabled(false);
    searchLayout->addWidget(searchPrevBtn_);

    searchNextBtn_ = new QToolButton(this);
    searchNextBtn_->setArrowType(Qt::RightArrow);
    searchNextBtn_->setEnabled(false);
    searchLayout->addWidget(searchNextBtn_);

    mainLayout->addLayout(searchLayout);

    // Tab Widget
    tabWidget_ = new QTabWidget(this);
    tabWidget_->setDocumentMode(true);

    browserDetails_ = new QTextBrowser(this);
    configureBrowser(browserDetails_);
    tabWidget_->addTab(browserDetails_, QString());

    browserHistory_ = new QTextBrowser(this);
    configureBrowser(browserHistory_);
    tabWidget_->addTab(browserHistory_, QString());

    browserLicense_ = new QTextBrowser(this);
    configureBrowser(browserLicense_);
    tabWidget_->addTab(browserLicense_, QString());

    browserClc_ = new QTextBrowser(this);
    configureBrowser(browserClc_);
    tabWidget_->addTab(browserClc_, QString());

    browserCopyright_ = new QTextBrowser(this);
    configureBrowser(browserCopyright_);
    tabWidget_->addTab(browserCopyright_, QString());

    browserNotices_ = new QTextBrowser(this);
    configureBrowser(browserNotices_);
    tabWidget_->addTab(browserNotices_, QString());

    browserPrivacy_ = new QTextBrowser(this);
    configureBrowser(browserPrivacy_);
    tabWidget_->addTab(browserPrivacy_, QString());

    browserReleaseNotes_ = new QTextBrowser(this);
    configureBrowser(browserReleaseNotes_);
    tabWidget_->addTab(browserReleaseNotes_, QString());

    licensesTab_ = new QWidget(this);
    auto* licensesLayout = new QVBoxLayout(licensesTab_);
    licensesLayout->setContentsMargins(6, 6, 6, 6);
    licensesLayout->setSpacing(8);

    auto* comboRow = new QHBoxLayout();
    comboRow->setSpacing(8);
    licenseComboLabel_ = new QLabel(licensesTab_);
    licenseCombo_ = new QComboBox(licensesTab_);
    comboRow->addWidget(licenseComboLabel_);
    comboRow->addWidget(licenseCombo_, 1);
    licensesLayout->addLayout(comboRow);

    browserThirdPartyLicenses_ = new QTextBrowser(licensesTab_);
    configureBrowser(browserThirdPartyLicenses_);
    licensesLayout->addWidget(browserThirdPartyLicenses_, 1);

    tabWidget_->addTab(licensesTab_, QString());

    currentBrowser_ = browserDetails_;
    mainLayout->addWidget(tabWidget_, 1);

    // Bottom button bar
    auto* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch(1);

    closeButton_ = new QPushButton(this);
    closeButton_->setAutoDefault(false);
    closeButton_->setDefault(false);
    buttonLayout->addWidget(closeButton_);

    mainLayout->addLayout(buttonLayout);

    // Connections
    connect(closeButton_, &QPushButton::clicked, this, &QDialog::close);
    connect(searchEdit_, &QLineEdit::textChanged, this, &SobreDialog::onSearchTextChanged);
    connect(searchEdit_, &QLineEdit::returnPressed, this, &SobreDialog::onSearchNext);
    connect(searchNextBtn_, &QToolButton::clicked, this, &SobreDialog::onSearchNext);
    connect(searchPrevBtn_, &QToolButton::clicked, this, &SobreDialog::onSearchPrevious);
    connect(tabWidget_, &QTabWidget::currentChanged, this, &SobreDialog::onTabChanged);
    connect(licenseCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SobreDialog::onLicenseSelectionChanged);
}

void SobreDialog::changeEvent(QEvent* event)
{
    if (event != nullptr && event->type() == QEvent::LanguageChange) {
        retranslateUi();
    }
    QDialog::changeEvent(event);
}

void SobreDialog::configureBrowser(QTextBrowser* browser)
{
    browser->setOpenExternalLinks(true);
    browser->setReadOnly(true);
}

void SobreDialog::retranslateUi()
{
    setWindowTitle(tr("Sobre o Módulo de Dosagem de Concreto"));

    headerTextLabel_->setText(
        "<b><span style='font-size:12pt;'>" + tr("Módulo de Dosagem de Concreto") + "</span></b><br>"
        "<b>" + tr("Versão") + ":</b> 2026.11.22.0 | C++23 | Qt 6.12.0<br>"
        "<b>" + tr("Autor") + ":</b> Fernando Nillsson Cidade<br>"
        "<b>" + tr("Descrição") + ":</b> " + tr("Módulo de Dosagem de Concreto pelo Método IPT / EPUSP (Helene & Terzian)")
    );

    searchLabel_->setText(tr("Pesquisar:"));
    searchEdit_->setPlaceholderText(tr("Digite um termo para pesquisar..."));
    searchPrevBtn_->setToolTip(tr("Ocorrência anterior"));
    searchNextBtn_->setToolTip(tr("Próxima ocorrência"));

    tabWidget_->setTabText(0, tr("Detalhes"));
    tabWidget_->setTabText(1, tr("Histórico"));
    tabWidget_->setTabText(2, tr("Licença"));
    tabWidget_->setTabText(3, tr("CLC / Licenciamento"));
    tabWidget_->setTabText(4, tr("Copyright"));
    tabWidget_->setTabText(5, tr("Avisos"));
    tabWidget_->setTabText(6, tr("Privacidade"));
    tabWidget_->setTabText(7, tr("Notas de Lançamento"));
    tabWidget_->setTabText(8, tr("Licenças Completas"));

    licenseComboLabel_->setText(tr("Selecione a licença:"));
    licenseCombo_->setToolTip(tr("Escolha qual documento de licença completa deseja visualizar"));

    closeButton_->setText(tr("Fechar"));

    loadContents();
}

QString SobreDialog::resolveAssetDir(const QString& relativeDir) const
{
    const QString base = QCoreApplication::applicationDirPath();
    QString altRel = relativeDir;
    if (altRel.startsWith(QStringLiteral("source/assets/"))) {
        altRel.replace(0, 14, QStringLiteral("assets/"));
    }

    const QStringList candidates = {
        base + "/" + relativeDir,
        base + "/" + altRel,
        base + "/../" + relativeDir,
        base + "/../../" + relativeDir,
        relativeDir,
        altRel
    };

    QString firstExistingDir;
    for (const QString& path : candidates) {
        const QDir dir(path);
        if (!dir.exists()) {
            continue;
        }
        if (firstExistingDir.isEmpty()) {
            firstExistingDir = dir.absolutePath();
        }
        if (!dir.entryList({QStringLiteral("*.txt")}, QDir::Files).isEmpty()) {
            return dir.absolutePath();
        }
    }
    return firstExistingDir;
}

QString SobreDialog::loadAssetFile(const QString& relativePath)
{
    QString base = QCoreApplication::applicationDirPath();
    QString altRel = relativePath;
    if (altRel.startsWith("source/assets/")) {
        altRel.replace(0, 14, "assets/");
    }
    QStringList candidates = {
        base + "/" + relativePath,
        base + "/" + altRel,
        base + "/../" + relativePath,
        base + "/../../" + relativePath,
        relativePath,
        altRel
    };

    for (const auto& path : candidates) {
        QFile file(path);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return QString::fromUtf8(file.readAll());
        }
    }
    return QString();
}

void SobreDialog::loadContents()
{
    QString lang = Language::GerenciadorTraducao::instance().codigoIdiomaAtual();
    QString suffix = (lang == "en_US") ? "_en_US.txt" : "_pt_BR.txt";

    QString details = loadAssetFile("source/assets/ABOUT/ABOUT" + suffix);
    if (details.isEmpty()) {
        details = tr("Módulo de Dosagem de Concreto — Dosagem Experimental de Concreto Estrutural pelo Método IPT / EPUSP.");
    }
    browserDetails_->setPlainText(details);

    QString history = loadAssetFile("source/assets/ABOUT/History_APP" + suffix);
    browserHistory_->setPlainText(history);

    QString license = loadAssetFile("source/assets/EULA/EULA" + suffix);
    browserLicense_->setPlainText(license);

    QString clc = loadAssetFile("source/assets/CLC/CLC" + suffix);
    browserClc_->setPlainText(clc);

    QString copyright = loadAssetFile(
        (lang == "en_US")
            ? QStringLiteral("source/assets/COPYRIGHT/COPYRIGHT AND TRADEMARK NOTICE_en_US.txt")
            : QStringLiteral("source/assets/COPYRIGHT/AVISO DE COPYRIGHT E MARCA REGISTRADA_pt_BR.txt"));
    browserCopyright_->setPlainText(copyright);

    QString notices = loadAssetFile("source/assets/NOTICES/NOTICE" + suffix);
    browserNotices_->setPlainText(notices);

    QString privacy = loadAssetFile("source/assets/PRIVACY_POLICY/Privacy_Policy" + suffix);
    browserPrivacy_->setPlainText(privacy);

    QString release = loadAssetFile("source/assets/RELEASE/RELEASE NOTES" + suffix);
    browserReleaseNotes_->setPlainText(release);

    populateLicensesCombo();
}

void SobreDialog::populateLicensesCombo()
{
    if (!licenseCombo_ || !browserThirdPartyLicenses_) {
        return;
    }

    const QString prevRelPath = licenseCombo_->currentData().toString();
    int restoredIndex = -1;

    {
        QSignalBlocker blocker(licenseCombo_);
        licenseCombo_->clear();

        const QString licDirPath = resolveAssetDir(QStringLiteral("source/assets/LICENSES"));
        if (!licDirPath.isEmpty()) {
            const QDir licDir(licDirPath);
            const QStringList files = licDir.entryList({QStringLiteral("*.txt")}, QDir::Files, QDir::Name);
            for (const QString& fileName : files) {
                const QString displayName = QFileInfo(fileName).completeBaseName();
                const QString relPath = QStringLiteral("source/assets/LICENSES/%1").arg(fileName);
                licenseCombo_->addItem(displayName, relPath);
                if (!prevRelPath.isEmpty() && relPath == prevRelPath) {
                    restoredIndex = licenseCombo_->count() - 1;
                }
            }
        }

        if (licenseCombo_->count() > 0) {
            licenseCombo_->setCurrentIndex(restoredIndex >= 0 ? restoredIndex : 0);
        }
    }

    if (licenseCombo_->count() > 0) {
        onLicenseSelectionChanged(licenseCombo_->currentIndex());
    } else {
        browserThirdPartyLicenses_->setPlainText(tr("Nenhum arquivo de licença encontrado em assets/LICENSES."));
    }
}

void SobreDialog::onLicenseSelectionChanged(int index)
{
    if (!licenseCombo_ || !browserThirdPartyLicenses_) {
        return;
    }

    if (index < 0 || index >= licenseCombo_->count()) {
        browserThirdPartyLicenses_->clear();
        return;
    }

    const QString relPath = licenseCombo_->itemData(index).toString();
    const QString content = loadAssetFile(relPath);
    if (content.isEmpty()) {
        browserThirdPartyLicenses_->setPlainText(tr("Não foi possível carregar o documento selecionado."));
    } else {
        browserThirdPartyLicenses_->setPlainText(content);
    }

    if (tabWidget_ && tabWidget_->currentIndex() == 8 && !searchEdit_->text().trimmed().isEmpty()) {
        performSearch(1);
    }
}

void SobreDialog::onTabChanged(int index)
{
    switch (index) {
        case 0: currentBrowser_ = browserDetails_; break;
        case 1: currentBrowser_ = browserHistory_; break;
        case 2: currentBrowser_ = browserLicense_; break;
        case 3: currentBrowser_ = browserClc_; break;
        case 4: currentBrowser_ = browserCopyright_; break;
        case 5: currentBrowser_ = browserNotices_; break;
        case 6: currentBrowser_ = browserPrivacy_; break;
        case 7: currentBrowser_ = browserReleaseNotes_; break;
        case 8: currentBrowser_ = browserThirdPartyLicenses_; break;
        default: currentBrowser_ = browserDetails_; break;
    }

    if (!searchEdit_->text().isEmpty()) {
        performSearch(1);
    }
}

void SobreDialog::onSearchTextChanged(const QString& text)
{
    bool hasText = !text.trimmed().isEmpty();
    searchPrevBtn_->setEnabled(hasText);
    searchNextBtn_->setEnabled(hasText);

    if (hasText) {
        performSearch(1);
    }
}

void SobreDialog::onSearchNext()
{
    performSearch(1);
}

void SobreDialog::onSearchPrevious()
{
    performSearch(-1);
}

void SobreDialog::performSearch(int direction)
{
    if (!currentBrowser_) return;

    QString text = searchEdit_->text().trimmed();
    if (text.isEmpty()) return;

    QTextDocument::FindFlags flags;
    if (direction < 0) {
        flags |= QTextDocument::FindBackward;
    }

    bool found = currentBrowser_->find(text, flags);
    if (!found) {
        // Wrap around to start or end
        QTextCursor cursor = currentBrowser_->textCursor();
        if (direction < 0) {
            cursor.movePosition(QTextCursor::End);
        } else {
            cursor.movePosition(QTextCursor::Start);
        }
        currentBrowser_->setTextCursor(cursor);
        currentBrowser_->find(text, flags);
    }
}

} // namespace ConcreteMix::Gui
