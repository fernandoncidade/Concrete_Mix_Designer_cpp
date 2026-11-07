#include "SobreDialog.hpp"
#include "source/language/GerenciadorTraducao.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>
#include <QTabWidget>
#include <QTextBrowser>
#include <QPushButton>
#include <QIcon>
#include <QPixmap>
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QTextCursor>
#include <QTextDocument>
#include <QEvent>

namespace ConcreteMix::Gui {

SobreDialog::SobreDialog(QWidget* parent)
    : QDialog(parent)
{
    setupUi();
    retranslateUi();
    loadContents();
}

void SobreDialog::setupUi()
{
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint |
                   Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    setWindowModality(Qt::NonModal);
    setModal(false);
    setAttribute(Qt::WA_DeleteOnClose, true);
    resize(780, 580);
    setMinimumSize(600, 450);

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

    browserNotices_ = new QTextBrowser(this);
    configureBrowser(browserNotices_);
    tabWidget_->addTab(browserNotices_, QString());

    browserPrivacy_ = new QTextBrowser(this);
    configureBrowser(browserPrivacy_);
    tabWidget_->addTab(browserPrivacy_, QString());

    browserReleaseNotes_ = new QTextBrowser(this);
    configureBrowser(browserReleaseNotes_);
    tabWidget_->addTab(browserReleaseNotes_, QString());

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
        "<b>" + tr("Versão") + ":</b> 2026.11.6.0| C++20 | Qt 6.11.1<br>"
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
    tabWidget_->setTabText(3, tr("Avisos"));
    tabWidget_->setTabText(4, tr("Privacidade"));
    tabWidget_->setTabText(5, tr("Notas de Lançamento"));

    closeButton_->setText(tr("Fechar"));

    loadContents();
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
        details = (lang == "en_US")
            ? "Concrete Mix Designer — Structural Concrete Proportioning via IPT / EPUSP Method."
            : "Módulo de Dosagem de Concreto — Dosagem Experimental de Concreto Estrutural pelo Método IPT / EPUSP.";
    }
    browserDetails_->setPlainText(details);

    QString history = loadAssetFile("source/assets/ABOUT/History_APP" + suffix);
    browserHistory_->setPlainText(history);

    QString license = loadAssetFile("source/assets/EULA/EULA" + suffix);
    browserLicense_->setPlainText(license);

    QString notices = loadAssetFile("source/assets/NOTICES/NOTICE" + suffix);
    browserNotices_->setPlainText(notices);

    QString privacy = loadAssetFile("source/assets/PRIVACY_POLICY/Privacy_Policy" + suffix);
    browserPrivacy_->setPlainText(privacy);

    QString release = loadAssetFile("source/assets/RELEASE/RELEASE NOTES" + suffix);
    browserReleaseNotes_->setPlainText(release);
}

void SobreDialog::onTabChanged(int index)
{
    switch (index) {
        case 0: currentBrowser_ = browserDetails_; break;
        case 1: currentBrowser_ = browserHistory_; break;
        case 2: currentBrowser_ = browserLicense_; break;
        case 3: currentBrowser_ = browserNotices_; break;
        case 4: currentBrowser_ = browserPrivacy_; break;
        case 5: currentBrowser_ = browserReleaseNotes_; break;
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
