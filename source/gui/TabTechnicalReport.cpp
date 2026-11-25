#include "TabTechnicalReport.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QClipboard>
#include <QApplication>
#include <QGuiApplication>
#include <QStyleHints>
#include <QPdfWriter>
#include <QPainter>
#include <QTextDocument>
#include <QFile>
#include <QTextStream>
#include <QEvent>
#include "../language/GerenciadorTraducao.hpp"

namespace ConcreteMix {

namespace {
void showModelessNotification(QMessageBox::Icon icon, const QString &title, const QString &text, QWidget *refWidget)
{
    auto *box = new QMessageBox(icon, title, text, QMessageBox::Ok, nullptr);
    box->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
    box->setWindowModality(Qt::NonModal);
    box->setModal(false);
    box->setAttribute(Qt::WA_DeleteOnClose, true);
    if (refWidget) {
        box->setPalette(refWidget->palette());
        if (refWidget->window()) {
            box->setWindowIcon(refWidget->window()->windowIcon());
        }
    }
    box->show();
    box->raise();
    box->activateWindow();
}
} // namespace

TabTechnicalReport::TabTechnicalReport(ProjectData *data, QWidget *parent)
    : QWidget(parent), m_data(data)
{
    setupUI();
    refreshReport();
}

void TabTechnicalReport::setupUI() {
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(8);

    auto toolbar = new QHBoxLayout();
    
    m_btnCopy = new QPushButton(tr("📋 Copiar Texto"), this);
    m_btnMd = new QPushButton(tr("💾 Salvar Markdown (.md)..."), this);
    m_btnHtml = new QPushButton(tr("🌐 Salvar HTML..."), this);
    m_btnPdf = new QPushButton(tr("📄 Exportar PDF Oficial..."), this);

    toolbar->addWidget(m_btnCopy);
    toolbar->addWidget(m_btnMd);
    toolbar->addWidget(m_btnHtml);
    toolbar->addStretch();
    toolbar->addWidget(m_btnPdf);

    layout->addLayout(toolbar);

    m_textBrowser = new QTextBrowser(this);
    m_textBrowser->setOpenExternalLinks(true);
    layout->addWidget(m_textBrowser, 1);

    connect(m_btnCopy, &QPushButton::clicked, this, [this]() {
        if (!m_data) return;
        auto res = m_data->calculate();
        const bool isEn = (Language::GerenciadorTraducao::instance().codigoIdiomaAtual() == QStringLiteral("en_US"));
        QApplication::clipboard()->setText(QString::fromStdString(m_data->generateMarkdownReport(res, isEn)));
        showModelessNotification(QMessageBox::Information, tr("Copiado"), tr("Memorial de cálculo copiado para a área de transferência!"), this);
    });

    connect(m_btnMd, &QPushButton::clicked, this, &TabTechnicalReport::exportMarkdown);
    connect(m_btnHtml, &QPushButton::clicked, this, &TabTechnicalReport::exportHtml);
    connect(m_btnPdf, &QPushButton::clicked, this, &TabTechnicalReport::exportPdf);
}

void TabTechnicalReport::refreshReport() {
    if (!m_data) return;
    auto res = m_data->calculate();
    bool isDark = (palette().color(QPalette::Window).lightness() < 128);
    const bool isEn = (Language::GerenciadorTraducao::instance().codigoIdiomaAtual() == QStringLiteral("en_US"));
    m_textBrowser->setHtml(QString::fromStdString(m_data->generateHtmlReport(res, isDark, isEn)));
}

void TabTechnicalReport::retranslateUi() {
    if (m_btnCopy) m_btnCopy->setText(tr("📋 Copiar Texto"));
    if (m_btnMd) m_btnMd->setText(tr("💾 Salvar Markdown (.md)..."));
    if (m_btnHtml) m_btnHtml->setText(tr("🌐 Salvar HTML..."));
    if (m_btnPdf) m_btnPdf->setText(tr("📄 Exportar PDF Oficial..."));

    refreshReport();
}

void TabTechnicalReport::changeEvent(QEvent *event) {
    if (event->type() == QEvent::LanguageChange) {
        retranslateUi();
    } else if (event->type() == QEvent::PaletteChange || event->type() == QEvent::ApplicationPaletteChange) {
        refreshReport();
    }
    QWidget::changeEvent(event);
}

void TabTechnicalReport::exportMarkdown() {
    if (!m_data) return;
    const bool isEn = (Language::GerenciadorTraducao::instance().codigoIdiomaAtual() == QStringLiteral("en_US"));
    const QString defaultName = isEn ? QStringLiteral("Concrete_Mix_Design_Report.md") : QStringLiteral("Memorial_Calculo_Dosagem.md");
    QString filename = QFileDialog::getSaveFileName(this, tr("Salvar Memorial em Markdown"), defaultName, tr("Arquivos Markdown (*.md)"));
    if (filename.isEmpty()) return;

    QFile file(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out.setEncoding(QStringConverter::Utf8);
        auto res = m_data->calculate();
        out << QString::fromStdString(m_data->generateMarkdownReport(res, isEn));
        file.close();
        showModelessNotification(QMessageBox::Information, tr("Sucesso"), tr("Memorial salvo com sucesso em Markdown!"), this);
    }
}

void TabTechnicalReport::exportHtml() {
    if (!m_data) return;
    const bool isEn = (Language::GerenciadorTraducao::instance().codigoIdiomaAtual() == QStringLiteral("en_US"));
    const QString defaultName = isEn ? QStringLiteral("Concrete_Mix_Design_Report.html") : QStringLiteral("Memorial_Calculo_Dosagem.html");
    QString filename = QFileDialog::getSaveFileName(this, tr("Salvar Memorial em HTML"), defaultName, tr("Arquivos HTML (*.html)"));
    if (filename.isEmpty()) return;

    QFile file(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out.setEncoding(QStringConverter::Utf8);
        auto res = m_data->calculate();
        out << QString::fromStdString(m_data->generateHtmlReport(res, false, isEn));
        file.close();
        showModelessNotification(QMessageBox::Information, tr("Sucesso"), tr("Memorial salvo com sucesso em HTML!"), this);
    }
}

void TabTechnicalReport::exportPdf() {
    if (!m_data) return;
    const bool isEn = (Language::GerenciadorTraducao::instance().codigoIdiomaAtual() == QStringLiteral("en_US"));
    const QString defaultName = isEn ? QStringLiteral("Concrete_Mix_Design_Report.pdf") : QStringLiteral("Memorial_Calculo_Dosagem.pdf");
    QString filename = QFileDialog::getSaveFileName(this, tr("Exportar Memorial em PDF"), defaultName, tr("Documento PDF (*.pdf)"));
    if (filename.isEmpty()) return;

    QPdfWriter writer(filename);
    writer.setPageSize(QPageSize(QPageSize::A4));
    writer.setPageOrientation(QPageLayout::Portrait);
    writer.setPageMargins(QMarginsF(15, 15, 15, 15), QPageLayout::Millimeter);
    writer.setResolution(300);

    QTextDocument doc;
    auto res = m_data->calculate();
    doc.setHtml(QString::fromStdString(m_data->generateHtmlReport(res, false, isEn)));
    doc.print(&writer);

    showModelessNotification(QMessageBox::Information, tr("PDF Gerado"), tr("Memorial técnico oficial exportado em PDF com sucesso!\n%1").arg(filename), this);
}

} // namespace ConcreteMix
