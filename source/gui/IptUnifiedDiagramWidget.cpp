#include "IptUnifiedDiagramWidget.hpp"
#include <QPainter>
#include <QPainterPath>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QClipboard>
#include <QMenu>
#include <QContextMenuEvent>
#include <QFontMetricsF>
#include <QPdfWriter>
#include <QPageLayout>
#include <QPageSize>
#include <cmath>
#include <algorithm>

namespace ConcreteMix {

IptUnifiedDiagramWidget::IptUnifiedDiagramWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(600, 480);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAttribute(Qt::WA_OpaquePaintEvent, false);
}

void IptUnifiedDiagramWidget::setData(const MixDesignResult& result, const std::vector<ExperimentalMixRecord>& mixes) {
    m_hasData = true;
    m_result = result;
    m_mixes = mixes;
    update();
}

void IptUnifiedDiagramWidget::setTheme(bool isDark) {
    m_isDark = isDark;
    update();
}

void IptUnifiedDiagramWidget::changeEvent(QEvent *event) {
    if (event->type() == QEvent::LanguageChange) {
        retranslateUi();
    }
    QWidget::changeEvent(event);
}

void IptUnifiedDiagramWidget::retranslateUi() {
    update();
}

void IptUnifiedDiagramWidget::contextMenuEvent(QContextMenuEvent *event) {
    QMenu menu(this);
    auto actCopy = menu.addAction(tr("📋 Copiar Imagem (Área de Transferência)"));
    auto actExport = menu.addAction(tr("💾 Salvar Gráfico em Alta Resolução (PNG)..."));

    connect(actCopy, &QAction::triggered, this, &IptUnifiedDiagramWidget::copyToClipboard);
    connect(actExport, &QAction::triggered, this, [this]() {
        QString file = QFileDialog::getSaveFileName(
            this,
            tr("Salvar Composição Gráfica IPT"),
            QStringLiteral("Diagrama_Dosagem_IPT_Abrams_Lyse_Molinari.png"),
            tr("Imagens PNG (*.png);;Todos os Arquivos (*.*)")
        );
        if (!file.isEmpty()) {
            exportImage(file);
        }
    });

    menu.exec(event->globalPos());
}

void IptUnifiedDiagramWidget::copyToClipboard() {
    const int baseW = width() > 300 ? width() : 1280;
    const int baseH = height() > 300 ? height() : 850;
    const double scale = 2.0;
    const int outW = static_cast<int>(baseW * scale);
    const int outH = static_cast<int>(baseH * scale);

    QImage img(outW, outH, QImage::Format_ARGB32_Premultiplied);
    img.fill(m_isDark ? QColor(32, 33, 36) : Qt::white);
    QPainter p(&img);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);
    p.scale(scale, scale);
    drawDiagram(p, QRect(0, 0, baseW, baseH));
    p.end();

    QClipboard *cb = QApplication::clipboard();
    if (cb) {
        cb->setImage(img);
        auto *box = new QMessageBox(QMessageBox::Information, tr("Copiado"),
            tr("Diagrama copiado para a Área de Transferência em alta resolução!"), QMessageBox::Ok, nullptr);
        box->setAttribute(Qt::WA_DeleteOnClose);
        box->show();
    }
}

bool IptUnifiedDiagramWidget::exportImage(const QString& filePath, int targetWidth, int targetHeight) {
    Q_UNUSED(targetWidth);
    Q_UNUSED(targetHeight);
    const int baseW = width() > 300 ? width() : 1280;
    const int baseH = height() > 300 ? height() : 850;

    auto showModelessMsg = [this](QMessageBox::Icon icon, const QString &title, const QString &text) {
        auto *box = new QMessageBox(icon, title, text, QMessageBox::Ok, nullptr);
        box->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
        box->setWindowModality(Qt::NonModal);
        box->setModal(false);
        box->setAttribute(Qt::WA_DeleteOnClose, true);
        box->setPalette(palette());
        if (window()) {
            box->setWindowIcon(window()->windowIcon());
        }
        box->show();
        box->raise();
        box->activateWindow();
    };

    if (filePath.endsWith(QStringLiteral(".pdf"), Qt::CaseInsensitive)) {
        QPdfWriter pdf(filePath);
        pdf.setPageSize(QPageSize(QPageSize::A4));
        pdf.setPageOrientation(QPageLayout::Landscape);
        pdf.setResolution(300);
        QPainter p(&pdf);
        p.setRenderHint(QPainter::Antialiasing, true);
        p.setRenderHint(QPainter::TextAntialiasing, true);
        const QRect pageRect = pdf.pageLayout().paintRectPixels(pdf.resolution());
        double scale = std::min(double(pageRect.width()) / baseW, double(pageRect.height()) / baseH);
        p.scale(scale, scale);
        drawDiagram(p, QRect(0, 0, baseW, baseH));
        p.end();
        showModelessMsg(QMessageBox::Information, tr("Sucesso"),
            tr("Composição gráfica IPT exportada com sucesso para:\n%1").arg(filePath));
        return true;
    }

    // Renderiza em alta resolução vetorial 2x mantendo a proporção geométrica exata
    const double scale = 2.0;
    const int outW = static_cast<int>(baseW * scale);
    const int outH = static_cast<int>(baseH * scale);

    QImage img(outW, outH, QImage::Format_ARGB32_Premultiplied);
    img.fill(m_isDark ? QColor(32, 33, 36) : Qt::white);
    QPainter p(&img);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);
    p.scale(scale, scale);
    drawDiagram(p, QRect(0, 0, baseW, baseH));
    p.end();

    if (img.save(filePath)) {
        showModelessMsg(QMessageBox::Information, tr("Sucesso"),
            tr("Composição gráfica IPT exportada com sucesso para:\n%1").arg(filePath));
        return true;
    }
    showModelessMsg(QMessageBox::Warning, tr("Erro"),
        tr("Não foi possível salvar o arquivo de imagem."));
    return false;
}

void IptUnifiedDiagramWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    drawDiagram(painter, rect());
}

void IptUnifiedDiagramWidget::drawDiagram(QPainter &painter, const QRect &area) {
    // 1. Cores e Estilo conforme o tema
    const QColor bgColor = m_isDark ? QColor(32, 33, 36) : Qt::white;
    const QColor axisColor = m_isDark ? QColor(220, 220, 225) : QColor(30, 30, 30);
    const QColor gridColor = m_isDark ? QColor(70, 72, 78) : QColor(225, 225, 230);
    const QColor textPrimary = m_isDark ? QColor(240, 240, 245) : QColor(20, 20, 25);
    const QColor textSecondary = m_isDark ? QColor(180, 180, 190) : QColor(80, 80, 85);
    const QColor nomogramRed = QColor(215, 38, 56); // Vermelho vibrante de engenharia
    const QColor curveColor = m_isDark ? QColor(210, 215, 225) : QColor(40, 40, 45);

    painter.fillRect(area, bgColor);

    // Margens adaptativas proporcionais à área de desenho
    const double scaleFactor = std::max(0.65, std::min(area.width(), area.height()) / 700.0);
    const double marginLeft = 85.0 * scaleFactor;
    const double marginRight = 80.0 * scaleFactor;
    const double marginTop = 70.0 * scaleFactor;
    const double marginBottom = 70.0 * scaleFactor;

    const QRectF plot(
        area.left() + marginLeft,
        area.top() + marginTop,
        area.width() - marginLeft - marginRight,
        area.height() - marginTop - marginBottom
    );

    if (plot.width() <= 100 || plot.height() <= 100) {
        return;
    }

    // 2. Determinação da Origem Central (x0, y0)
    // Conforme a imagem de referência IPT: os eixos cruzam no centro
    const double x0 = plot.left() + plot.width() * 0.46;
    const double y0 = plot.top() + plot.height() * 0.48;

    // 3. Faixas de Escala (Ranges)
    // Eixo Horizontal Direito: a/c (0.25 a 1.05 L/kg)
    double maxAc = 1.00;
    double minAc = 0.25;
    for (const auto& m : m_mixes) {
        if (m.waterCementRatio > maxAc) maxAc = std::ceil(m.waterCementRatio * 10.0) / 10.0 + 0.05;
        if (m.waterCementRatio > 0.0 && m.waterCementRatio < minAc) minAc = std::floor(m.waterCementRatio * 10.0) / 10.0;
    }
    if (m_hasData && m_result.targetWaterCement > maxAc) maxAc = m_result.targetWaterCement + 0.10;

    // Eixo Horizontal Esquerdo: Consumo C (100 a 650 kg/m³)
    double maxC = 600.0;
    double minC = 150.0;
    for (const auto& m : m_mixes) {
        if (m.cementConsumption > maxC) maxC = std::ceil(m.cementConsumption / 50.0) * 50.0 + 50.0;
        if (m.cementConsumption > 0.0 && m.cementConsumption < minC) minC = std::floor(m.cementConsumption / 50.0) * 50.0;
    }
    if (m_hasData && m_result.targetCementConsumption > maxC) maxC = m_result.targetCementConsumption + 50.0;

    // Eixo Vertical Superior: Resistência Fc (0 a 65 MPa)
    double maxFc = 60.0;
    for (const auto& m : m_mixes) {
        if (m.compressiveStrength28d > maxFc) maxFc = std::ceil(m.compressiveStrength28d / 10.0) * 10.0 + 10.0;
    }
    if (m_hasData && m_result.fcj > maxFc) maxFc = std::ceil(m_result.fcj / 10.0) * 10.0 + 10.0;

    // Eixo Vertical Inferior: Relação agregados/cimento m (0.0 a 12.0 ou 13.0 kg/kg)
    double maxM = 12.0;
    for (const auto& m : m_mixes) {
        if (m.dryRatioM > maxM) maxM = std::ceil(m.dryRatioM / 2.0) * 2.0 + 1.0;
    }
    if (m_hasData && m_result.targetDryMixM > maxM) maxM = std::ceil(m_result.targetDryMixM / 2.0) * 2.0 + 1.0;

    // Funções de Projeção Cartesiana
    auto xFromAc = [&](double ac) -> double {
        double ratio = (ac - minAc) / (maxAc - minAc);
        return x0 + ratio * (plot.right() - x0);
    };

    auto xFromC = [&](double c) -> double {
        double ratio = (c - minC) / (maxC - minC);
        return x0 - ratio * (x0 - plot.left());
    };

    auto yFromFc = [&](double fc) -> double {
        double ratio = fc / maxFc;
        return y0 - ratio * (y0 - plot.top());
    };

    auto yFromM = [&](double m) -> double {
        double ratio = m / maxM;
        return y0 + ratio * (plot.bottom() - y0);
    };

    // 3.5 Linhas de Escala dos Eixos (Grids de Alta Precisão dos Quadrantes IPT)
    QPen majorGridPen(m_isDark ? QColor(65, 68, 75) : QColor(215, 220, 228), 1.0, Qt::SolidLine);
    QPen minorGridPen(m_isDark ? QColor(48, 51, 56) : QColor(238, 241, 246), 0.75, Qt::DotLine);
    painter.setBrush(Qt::NoBrush);

    // 3.5.1 Grade Vertical Direita (Relação a/c nos Quadrantes 1 e 4 - Abrams e Lyse)
    painter.setPen(minorGridPen);
    for (double ac = minAc; ac <= maxAc; ac += 0.02) {
        double xPos = xFromAc(ac);
        if (xPos > x0 && xPos <= plot.right()) {
            painter.drawLine(QPointF(xPos, plot.top()), QPointF(xPos, plot.bottom()));
        }
    }
    painter.setPen(majorGridPen);
    for (double ac = 0.30; ac <= maxAc; ac += 0.10) {
        double xPos = xFromAc(ac);
        if (xPos > x0 && xPos <= plot.right()) {
            painter.drawLine(QPointF(xPos, plot.top()), QPointF(xPos, plot.bottom()));
        }
    }

    // 3.5.2 Grade Vertical Esquerda (Consumo Ci no Quadrante 3 - Molinari)
    painter.setPen(minorGridPen);
    for (double cVal = minC; cVal <= maxC; cVal += 10.0) {
        double xPos = xFromC(cVal);
        if (xPos < x0 && xPos >= plot.left()) {
            painter.drawLine(QPointF(xPos, y0), QPointF(xPos, plot.bottom()));
        }
    }
    painter.setPen(majorGridPen);
    for (double cVal = 200.0; cVal <= maxC; cVal += 50.0) {
        double xPos = xFromC(cVal);
        if (xPos < x0 && xPos >= plot.left()) {
            painter.drawLine(QPointF(xPos, y0), QPointF(xPos, plot.bottom()));
        }
    }

    // 3.5.3 Grade Horizontal Superior (Resistência Fc no Quadrante 1 - Abrams)
    painter.setPen(minorGridPen);
    for (double fc = 2.0; fc <= maxFc; fc += 2.0) {
        double yPos = yFromFc(fc);
        if (yPos < y0 && yPos >= plot.top()) {
            painter.drawLine(QPointF(x0, yPos), QPointF(plot.right(), yPos));
        }
    }
    painter.setPen(majorGridPen);
    for (double fc = 10.0; fc <= maxFc; fc += 10.0) {
        double yPos = yFromFc(fc);
        if (yPos < y0 && yPos >= plot.top()) {
            painter.drawLine(QPointF(x0, yPos), QPointF(plot.right(), yPos));
        }
    }

    // 3.5.4 Grade Horizontal Inferior (Traço m nos Quadrantes 3 e 4 - Molinari e Lyse)
    painter.setPen(minorGridPen);
    for (double mVal = 0.5; mVal <= maxM; mVal += 0.5) {
        double yPos = yFromM(mVal);
        if (yPos > y0 && yPos <= plot.bottom()) {
            painter.drawLine(QPointF(plot.left(), yPos), QPointF(plot.right(), yPos));
        }
    }
    painter.setPen(majorGridPen);
    for (double mVal = 2.0; mVal <= maxM; mVal += 2.0) {
        double yPos = yFromM(mVal);
        if (yPos > y0 && yPos <= plot.bottom()) {
            painter.drawLine(QPointF(plot.left(), yPos), QPointF(plot.right(), yPos));
        }
    }

    // Moldura externa delimitadora dos quadrantes ativos
    painter.setPen(majorGridPen);
    painter.drawLine(QPointF(x0, plot.top()), QPointF(plot.right(), plot.top()));
    painter.drawLine(QPointF(plot.right(), plot.top()), QPointF(plot.right(), plot.bottom()));
    painter.drawLine(QPointF(plot.left(), plot.bottom()), QPointF(plot.right(), plot.bottom()));
    painter.drawLine(QPointF(plot.left(), y0), QPointF(plot.left(), plot.bottom()));

    // 4. Desenho dos Eixos Cruzados Principais
    QPen axisPen(axisColor, std::max(1.5, 1.8 * scaleFactor), Qt::SolidLine, Qt::RoundCap);
    painter.setPen(axisPen);

    // Eixo Horizontal Central (Consumo C à esquerda <---> a/c à direita)
    painter.drawLine(QPointF(plot.left() - 5 * scaleFactor, y0), QPointF(plot.right() + 10 * scaleFactor, y0));
    // Eixo Vertical Central (Resistência Fc acima <---> m abaixo)
    painter.drawLine(QPointF(x0, plot.bottom() + 10 * scaleFactor), QPointF(x0, plot.top() - 5 * scaleFactor));

    QFont tickFont = painter.font();
    tickFont.setPointSize(std::max(8, static_cast<int>(9.5 * scaleFactor)));
    painter.setFont(tickFont);
    QFontMetricsF fmTick(tickFont);

    const double tickLen = 4.5 * scaleFactor;
    const double subTickLen = 2.5 * scaleFactor;

    // Graduação e Ticks no Eixo Horizontal Direito (Relação a/c)
    painter.setPen(axisPen);
    for (double ac = std::ceil(minAc * 10.0) / 10.0; ac <= maxAc + 1e-5; ac += 0.10) {
        double xPos = xFromAc(ac);
        if (xPos > plot.right()) break;
        painter.drawLine(QPointF(xPos, y0 - tickLen), QPointF(xPos, y0 + tickLen));

        painter.setPen(textSecondary);
        QString label = QString::asprintf("%.2f", ac);
        QRectF b = fmTick.boundingRect(label);
        painter.drawText(QRectF(xPos - b.width() / 2.0 - 5, y0 + 6 * scaleFactor, b.width() + 10, b.height() + 4), Qt::AlignCenter, label);
        painter.setPen(axisPen);
    }
    // Sub-ticks finos para a/c
    for (double ac = minAc; ac <= maxAc + 1e-5; ac += 0.02) {
        double xPos = xFromAc(ac);
        if (xPos >= x0 && xPos <= plot.right()) {
            painter.drawLine(QPointF(xPos, y0 - subTickLen), QPointF(xPos, y0 + subTickLen));
        }
    }

    // Graduação e Ticks no Eixo Horizontal Esquerdo (Consumo de Cimento Ci)
    for (double cVal = std::ceil(minC / 50.0) * 50.0; cVal <= maxC + 1e-5; cVal += 50.0) {
        double xPos = xFromC(cVal);
        if (xPos < plot.left()) break;
        painter.drawLine(QPointF(xPos, y0 - tickLen), QPointF(xPos, y0 + tickLen));

        painter.setPen(textSecondary);
        QString label = QString::asprintf("%.0f", cVal);
        QRectF b = fmTick.boundingRect(label);
        painter.drawText(QRectF(xPos - b.width() / 2.0 - 5, y0 + 6 * scaleFactor, b.width() + 10, b.height() + 4), Qt::AlignCenter, label);
        painter.setPen(axisPen);
    }
    for (double cVal = minC; cVal <= maxC + 1e-5; cVal += 10.0) {
        double xPos = xFromC(cVal);
        if (xPos <= x0 && xPos >= plot.left()) {
            painter.drawLine(QPointF(xPos, y0 - subTickLen), QPointF(xPos, y0 + subTickLen));
        }
    }

    // Graduação e Ticks no Eixo Vertical Superior (Resistência Fc)
    for (double fc = 10.0; fc <= maxFc + 1e-5; fc += 10.0) {
        double yPos = yFromFc(fc);
        if (yPos < plot.top()) break;
        painter.drawLine(QPointF(x0 - tickLen, yPos), QPointF(x0 + tickLen, yPos));

        painter.setPen(textSecondary);
        QString label = QString::asprintf("%.0f", fc);
        QRectF b = fmTick.boundingRect(label);
        painter.drawText(QRectF(x0 - b.width() - 8 * scaleFactor, yPos - b.height() / 2.0, b.width() + 4, b.height() + 4), Qt::AlignRight | Qt::AlignVCenter, label);
        painter.setPen(axisPen);
    }
    for (double fc = 2.0; fc <= maxFc + 1e-5; fc += 2.0) {
        double yPos = yFromFc(fc);
        if (yPos <= y0 && yPos >= plot.top()) {
            painter.drawLine(QPointF(x0 - subTickLen, yPos), QPointF(x0 + subTickLen, yPos));
        }
    }

    // Graduação e Ticks no Eixo Vertical Inferior (m - Agregado/Cimento)
    for (double mVal = 2.0; mVal <= maxM + 1e-5; mVal += 2.0) {
        double yPos = yFromM(mVal);
        if (yPos > plot.bottom()) break;
        painter.drawLine(QPointF(x0 - tickLen, yPos), QPointF(x0 + tickLen, yPos));

        painter.setPen(textSecondary);
        QString label = QString::asprintf("%.1f", mVal);
        QRectF b = fmTick.boundingRect(label);
        painter.drawText(QRectF(x0 - b.width() - 8 * scaleFactor, yPos - b.height() / 2.0, b.width() + 4, b.height() + 4), Qt::AlignRight | Qt::AlignVCenter, label);
        painter.setPen(axisPen);
    }
    for (double mVal = 0.5; mVal <= maxM + 1e-5; mVal += 0.5) {
        double yPos = yFromM(mVal);
        if (yPos >= y0 && yPos <= plot.bottom()) {
            painter.drawLine(QPointF(x0 - subTickLen, yPos), QPointF(x0 + subTickLen, yPos));
        }
    }

    // 5. Títulos e Rótulos dos Eixos (Exatamente como no Modelo IPT)
    QFont titleFont = painter.font();
    titleFont.setBold(true);
    titleFont.setPointSize(std::max(9, static_cast<int>(10.5 * scaleFactor)));
    painter.setFont(titleFont);
    painter.setPen(textPrimary);
    QFontMetricsF fmTitle(titleFont);

    // Rótulo Horizontal Direito: "Relação a/c" e "a/c - L/Kg"
    QString strAcTitle = tr("Relação a/c");
    QRectF bAcTitle = fmTitle.boundingRect(strAcTitle);
    painter.drawText(QRectF(x0 + 40 * scaleFactor, y0 + 32 * scaleFactor, bAcTitle.width() + 10, bAcTitle.height() + 6), Qt::AlignLeft | Qt::AlignVCenter, strAcTitle);

    QFont unitFont = titleFont;
    unitFont.setBold(false);
    painter.setFont(unitFont);
    QFontMetricsF fmUnit(unitFont);
    QString strAcUnit = tr("a/c - L/Kg");
    QRectF bAcUnit = fmUnit.boundingRect(strAcUnit);
    painter.drawText(QRectF(plot.right() - bAcUnit.width() - 10 * scaleFactor, y0 + 6 * scaleFactor, bAcUnit.width() + 10, bAcUnit.height() + 6), Qt::AlignRight | Qt::AlignVCenter, strAcUnit);

    // Rótulo Horizontal Esquerdo: "Consumo de Cimento - Ci" e "Ci - Kg/m³"
    painter.setFont(titleFont);
    QString strCTitle = tr("Consumo de Cimento - Ci");
    QRectF bCTitle = fmTitle.boundingRect(strCTitle);
    painter.drawText(QRectF(x0 - bCTitle.width() - 35 * scaleFactor, y0 - 36 * scaleFactor, bCTitle.width() + 10, bCTitle.height() + 6), Qt::AlignRight | Qt::AlignVCenter, strCTitle);

    painter.setFont(unitFont);
    QString strCUnit = tr("Ci - Kg/m³");
    QRectF bCUnit = fmUnit.boundingRect(strCUnit);
    painter.drawText(QRectF(plot.left() + 10 * scaleFactor, y0 + 6 * scaleFactor, bCUnit.width() + 10, bCUnit.height() + 6), Qt::AlignLeft | Qt::AlignVCenter, strCUnit);

    // Rótulo Vertical Superior: "Resistência à Compressão - Fc" (Rotacionado 90°)
    painter.save();
    double fcCenterY = y0 - (y0 - plot.top()) * 0.5;
    painter.translate(x0 - 55 * scaleFactor, fcCenterY);
    painter.rotate(-90);
    painter.setFont(titleFont);
    QString strFcTitle = tr("Resistência à Compressão - Fc");
    QRectF bFcTitle = fmTitle.boundingRect(strFcTitle);
    painter.drawText(QRectF(-bFcTitle.width() / 2.0 - 5, -bFcTitle.height() / 2.0, bFcTitle.width() + 10, bFcTitle.height() + 6), Qt::AlignCenter, strFcTitle);
    painter.restore();

    // Rótulo Vertical Inferior: "Relação agregado/Cimento - m" (Rotacionado 90°)
    painter.save();
    double mCenterY = y0 + (plot.bottom() - y0) * 0.5;
    painter.translate(x0 - 55 * scaleFactor, mCenterY);
    painter.rotate(-90);
    painter.setFont(titleFont);
    QString strMTitle = tr("Relação agregado/Cimento - m");
    QRectF bMTitle = fmTitle.boundingRect(strMTitle);
    painter.drawText(QRectF(-bMTitle.width() / 2.0 - 5, -bMTitle.height() / 2.0, bMTitle.width() + 10, bMTitle.height() + 6), Qt::AlignCenter, strMTitle);
    painter.restore();

    painter.setFont(unitFont);
    QString strMUnit = tr("m - Kg/Kg");
    QRectF bMUnit = fmUnit.boundingRect(strMUnit);
    painter.drawText(QRectF(x0 - bMUnit.width() / 2.0 - 25 * scaleFactor, plot.bottom() + 16 * scaleFactor, bMUnit.width() + 10, bMUnit.height() + 6), Qt::AlignCenter, strMUnit);

    // 6. Rótulos das Leis em cada Quadrante
    QFont lawFont = painter.font();
    lawFont.setBold(true);
    lawFont.setPointSize(std::max(11, static_cast<int>(13.0 * scaleFactor)));
    painter.setFont(lawFont);
    painter.setPen(textPrimary);
    QFontMetricsF fmLaw(lawFont);

    // Q1: Abrams
    QString strAbrams = tr("Abrams");
    QRectF bAb = fmLaw.boundingRect(strAbrams);
    painter.drawText(QRectF(plot.right() - bAb.width() - 25 * scaleFactor, y0 - 65 * scaleFactor, bAb.width() + 15, bAb.height() + 10), Qt::AlignRight | Qt::AlignVCenter, strAbrams);

    // Q4: Lyse
    QString strLyse = tr("Lyse");
    QRectF bLy = fmLaw.boundingRect(strLyse);
    painter.drawText(QRectF(plot.right() - bLy.width() - 25 * scaleFactor, y0 + 45 * scaleFactor, bLy.width() + 15, bLy.height() + 10), Qt::AlignRight | Qt::AlignVCenter, strLyse);

    // Q3: Molinari
    QString strMolinari = tr("Molinari");
    QRectF bMo = fmLaw.boundingRect(strMolinari);
    painter.drawText(QRectF(plot.left() + 20 * scaleFactor, y0 + 45 * scaleFactor, bMo.width() + 15, bMo.height() + 10), Qt::AlignLeft | Qt::AlignVCenter, strMolinari);

    // 7. Desenho das Curvas e Pontos Experimentais
    QPen curvePen(curveColor, std::max(1.8, 2.2 * scaleFactor), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter.setPen(curvePen);

    // 7.1 Quadrante 1 (Abrams): f_c = f(a/c)
    if (m_result.abramsPoly.isValid || m_result.abramsExp.isValid) {
        QPainterPath pathAbrams;
        bool inSegment = false;
        for (double ac = minAc; ac <= maxAc; ac += 0.01) {
            double fc = m_result.abramsExp.isValid ? m_result.abramsExp.evaluate(ac) : m_result.abramsPoly.evaluate(ac);
            if (fc >= 0.0 && fc <= maxFc * 1.05) {
                QPointF pt(xFromAc(ac), yFromFc(fc));
                if (!inSegment) {
                    pathAbrams.moveTo(pt);
                    inSegment = true;
                } else {
                    pathAbrams.lineTo(pt);
                }
            } else {
                inSegment = false;
            }
        }
        painter.drawPath(pathAbrams);

        // Equação de Abrams no Q1
        QFont eqFont = painter.font();
        eqFont.setBold(true);
        eqFont.setPointSize(std::max(9, static_cast<int>(10.0 * scaleFactor)));
        painter.setFont(eqFont);
        painter.setPen(textPrimary);
        QFontMetricsF fmEq(eqFont);

        QString eqStr;
        double r2 = m_result.abramsPoly.rSquared;
        if (m_result.abramsExp.isValid) {
            eqStr = QString::asprintf("y = %.2fe^{-%.3fx}\nR² = %.4f", m_result.abramsExp.k1, std::log(m_result.abramsExp.k2), m_result.abramsExp.rSquared);
            r2 = m_result.abramsExp.rSquared;
        } else {
            eqStr = QString::asprintf("y = %.1fx² + (%.1f)x + %.1f\nR² = %.4f", m_result.abramsPoly.a, m_result.abramsPoly.b, m_result.abramsPoly.c, r2);
        }
        QRectF bEqAb = fmEq.boundingRect(QRectF(0, 0, 450 * scaleFactor, 200 * scaleFactor), Qt::AlignRight | Qt::AlignTop, eqStr);
        painter.drawText(QRectF(plot.right() - bEqAb.width() - 15 * scaleFactor, plot.top() + 10 * scaleFactor, bEqAb.width() + 10, bEqAb.height() + 6), Qt::AlignRight | Qt::AlignTop, eqStr);

        // Pontos de Abrams
        const QColor ptColor1(52, 152, 219); // Azul celeste da referência
        for (const auto& mix : m_mixes) {
            if (mix.waterCementRatio > 0 && mix.compressiveStrength28d > 0) {
                double px = xFromAc(mix.waterCementRatio);
                double py = yFromFc(mix.compressiveStrength28d);
                painter.setBrush(ptColor1);
                painter.setPen(QPen(m_isDark ? Qt::white : Qt::black, 1.2));
                painter.drawEllipse(QPointF(px, py), 4.5 * scaleFactor, 4.5 * scaleFactor);
            }
        }
        painter.setBrush(Qt::NoBrush);
    }

    // 7.2 Quadrante 4 (Lyse): m = a*(a/c) + b
    if (m_result.lyseLinear.isValid) {
        painter.setPen(curvePen);
        QPainterPath pathLyse;
        double x1 = xFromAc(minAc);
        double y1 = yFromM(m_result.lyseLinear.evaluate(minAc));
        double x2 = xFromAc(maxAc);
        double y2 = yFromM(m_result.lyseLinear.evaluate(maxAc));
        pathLyse.moveTo(x1, y1);
        pathLyse.lineTo(x2, y2);
        painter.drawPath(pathLyse);

        // Equação de Lyse no Q4
        QFont eqFont = painter.font();
        eqFont.setBold(true);
        eqFont.setPointSize(std::max(9, static_cast<int>(10.0 * scaleFactor)));
        painter.setFont(eqFont);
        painter.setPen(textPrimary);

        QString lyseStr = QString::asprintf("y = %.3fx + (%.3f)\nR² = %.4f", m_result.lyseLinear.a, m_result.lyseLinear.b, m_result.lyseLinear.rSquared);
        QFontMetricsF fmEqLy(eqFont);
        QRectF bEqLy = fmEqLy.boundingRect(QRectF(0, 0, 400 * scaleFactor, 200 * scaleFactor), Qt::AlignLeft | Qt::AlignBottom, lyseStr);
        painter.drawText(QRectF(x0 + 35 * scaleFactor, plot.bottom() - bEqLy.height() - 15 * scaleFactor, bEqLy.width() + 10, bEqLy.height() + 6), Qt::AlignLeft | Qt::AlignBottom, lyseStr);

        // Pontos de Lyse
        for (const auto& mix : m_mixes) {
            if (mix.waterCementRatio > 0 && mix.dryRatioM > 0) {
                double px = xFromAc(mix.waterCementRatio);
                double py = yFromM(mix.dryRatioM);
                painter.setBrush(QColor(241, 196, 15)); // Ouro / Amarelo da referência
                painter.setPen(QPen(m_isDark ? Qt::white : Qt::black, 1.2));
                painter.drawEllipse(QPointF(px, py), 4.5 * scaleFactor, 4.5 * scaleFactor);
            }
        }
        painter.setBrush(Qt::NoBrush);
    }

    // 7.3 Quadrante 3 (Molinari): Curva m x Ci (m = f(Ci))
    if (m_result.molinariPoly.isValid) {
        painter.setBrush(Qt::NoBrush);
        painter.setPen(curvePen);
        QPainterPath pathMolinari;
        bool inSegment = false;
        const double stepC = std::max(1.0, (maxC - minC) / 150.0);
        for (double cVal = minC; cVal <= maxC; cVal += stepC) {
            double mVal = m_result.molinariPoly.evaluate(cVal);
            if (mVal >= 0.0 && mVal <= maxM * 1.08) {
                QPointF pt(xFromC(cVal), yFromM(mVal));
                if (!inSegment) {
                    pathMolinari.moveTo(pt);
                    inSegment = true;
                } else {
                    pathMolinari.lineTo(pt);
                }
            } else {
                inSegment = false;
            }
        }
        painter.drawPath(pathMolinari);

        // Equação de Molinari no Q3
        QFont eqFont = painter.font();
        eqFont.setBold(true);
        eqFont.setPointSize(std::max(9, static_cast<int>(10.0 * scaleFactor)));
        painter.setFont(eqFont);
        painter.setPen(textPrimary);

        QString molStr;
        if (std::abs(m_result.molinariPoly.a) > 1e-8) {
            molStr = QString::asprintf("m = %.6f·C² + (%.4f)·C + %.2f\nR² = %.4f",
                                       m_result.molinariPoly.a, m_result.molinariPoly.b, m_result.molinariPoly.c,
                                       m_result.molinariPoly.rSquared);
        } else {
            molStr = QString::asprintf("m = %.4f·C + %.2f\nR² = %.4f",
                                       m_result.molinariPoly.b, m_result.molinariPoly.c,
                                       m_result.molinariPoly.rSquared);
        }
        QFontMetricsF fmEqMo(eqFont);
        QRectF bEqMo = fmEqMo.boundingRect(QRectF(0, 0, 500 * scaleFactor, 200 * scaleFactor), Qt::AlignLeft | Qt::AlignBottom, molStr);
        painter.drawText(QRectF(plot.left() + 20 * scaleFactor, plot.bottom() - bEqMo.height() - 15 * scaleFactor, bEqMo.width() + 10, bEqMo.height() + 6), Qt::AlignLeft | Qt::AlignBottom, molStr);

        // Pontos de Molinari
        for (const auto& mix : m_mixes) {
            if (mix.cementConsumption > 0 && mix.dryRatioM > 0) {
                double px = xFromC(mix.cementConsumption);
                double py = yFromM(mix.dryRatioM);
                painter.setBrush(QColor(46, 204, 113)); // Verde esmeralda da referência
                painter.setPen(QPen(m_isDark ? Qt::white : Qt::black, 1.2));
                painter.drawEllipse(QPointF(px, py), 4.5 * scaleFactor, 4.5 * scaleFactor);
            }
        }
        painter.setBrush(Qt::NoBrush);
    }

    // 8. O Circuito de Dosagem Vermelho Pontilhado (Nomograma IPT / EPUSP)
    if (m_hasData && m_result.fcj > 0.0 && m_result.targetWaterCement > 0.0 &&
        m_result.targetDryMixM > 0.0 && m_result.targetCementConsumption > 0.0)
    {
        const double fcjVal = m_result.fcj;
        const double acVal = m_result.targetWaterCement;
        const double mVal = m_result.targetDryMixM;
        const double cVal = m_result.targetCementConsumption;

        const QPointF pStart(x0, yFromFc(fcjVal));
        const QPointF pAbrams(xFromAc(acVal), yFromFc(fcjVal));
        const QPointF pAcAxis(xFromAc(acVal), y0);
        const QPointF pLyse(xFromAc(acVal), yFromM(mVal));
        const QPointF pMolinari(xFromC(cVal), yFromM(mVal));
        const QPointF pCAxis(xFromC(cVal), y0);

        QPen redDashedPen(nomogramRed, std::max(1.8, 2.0 * scaleFactor), Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(redDashedPen);

        // 8.1 Trajeto de leitura: Eixo Fc -> Abrams -> Eixo a/c -> Lyse -> Molinari -> Eixo Ci
        painter.drawLine(pStart, pAbrams);
        painter.drawLine(pAbrams, pLyse);
        painter.drawLine(pLyse, pMolinari);
        painter.drawLine(pMolinari, pCAxis);

        // Marcadores circulares nos vértices de leitura
        painter.setPen(QPen(nomogramRed, 1.5));
        painter.setBrush(nomogramRed);
        painter.drawEllipse(pStart, 3.5 * scaleFactor, 3.5 * scaleFactor);
        painter.drawEllipse(pAbrams, 4.0 * scaleFactor, 4.0 * scaleFactor);
        painter.drawEllipse(pAcAxis, 4.0 * scaleFactor, 4.0 * scaleFactor);
        painter.drawEllipse(pLyse, 4.0 * scaleFactor, 4.0 * scaleFactor);
        painter.drawEllipse(pMolinari, 4.0 * scaleFactor, 4.0 * scaleFactor);
        painter.drawEllipse(pCAxis, 3.5 * scaleFactor, 3.5 * scaleFactor);
        painter.setBrush(Qt::NoBrush);

        // Textos e Badges Vermelhos dos Valores de Dosagem (conforme a imagem)
        QFont redFont = painter.font();
        redFont.setBold(true);
        redFont.setPointSize(std::max(8, static_cast<int>(9.5 * scaleFactor)));
        painter.setFont(redFont);
        painter.setPen(nomogramRed);
        QFontMetricsF fmRed(redFont);

        // Rótulo 1: fcj no ponto de início (ou acima da linha de Abrams)
        QString lblFc = QString::asprintf("fcj %.1f MPa", fcjVal);
        QRectF bFc = fmRed.boundingRect(lblFc);
        painter.drawText(QRectF(pAbrams.x() - bFc.width() - 10 * scaleFactor, pAbrams.y() - bFc.height() - 6 * scaleFactor, bFc.width() + 10, bFc.height() + 6), Qt::AlignRight | Qt::AlignVCenter, lblFc);

        // Rótulo 2: a/c no eixo horizontal central
        QString lblAc = QString::asprintf("a/c %.3f", acVal);
        QRectF bAc = fmRed.boundingRect(lblAc);
        painter.drawText(QRectF(pAcAxis.x() - bAc.width() - 8 * scaleFactor, y0 - bAc.height() - 6 * scaleFactor, bAc.width() + 10, bAc.height() + 6), Qt::AlignRight | Qt::AlignVCenter, lblAc);

        // Rótulo 3: m no cruzamento de Lyse
        QString lblM = QString::asprintf("m %.3f", mVal);
        QRectF bM = fmRed.boundingRect(lblM);
        painter.drawText(QRectF(pLyse.x() + 8 * scaleFactor, pLyse.y() - bM.height() / 2.0, bM.width() + 10, bM.height() + 6), Qt::AlignLeft | Qt::AlignVCenter, lblM);

        // Rótulo 4: Ci no cruzamento de Molinari
        QString lblC = QString::asprintf("Ci %.0f", cVal);
        QRectF bC = fmRed.boundingRect(lblC);
        painter.drawText(QRectF(pMolinari.x() - bC.width() - 8 * scaleFactor, pMolinari.y() - bC.height() / 2.0, bC.width() + 10, bC.height() + 6), Qt::AlignRight | Qt::AlignVCenter, lblC);
    }
}

} // namespace ConcreteMix
