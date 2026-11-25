#include "QuadChartWidget.hpp"
#include "IptUnifiedDiagramWidget.hpp"
#include "../language/GerenciadorTraducao.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QFileDialog>
#include <QMessageBox>
#include <QPen>
#include <QFont>
#include <QEvent>
#include <QGuiApplication>
#include <QStyleHints>
#include <QPdfWriter>
#include <QPageLayout>
#include <QPageSize>
#include <cmath>
#include <algorithm>

namespace ConcreteMix {

QuadChartWidget::QuadChartWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void QuadChartWidget::setupUI() {
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(4, 4, 4, 4);
    mainLayout->setSpacing(6);

    // Barra de ferramentas superior dos gráficos
    auto toolbarLayout = new QHBoxLayout();
    m_btnExport = new QPushButton(tr("📷 Exportar Imagem dos Gráficos..."), this);
    connect(m_btnExport, &QPushButton::clicked, this, [this]() {
        QString file = QFileDialog::getSaveFileName(this, tr("Salvar Gráfico"), "Diagrama_Dosagem_IPT.png", tr("Imagens PNG (*.png);;PDF (*.pdf)"));
        if (!file.isEmpty()) {
            exportChartImage(file);
        }
    });

    toolbarLayout->addStretch();
    toolbarLayout->addWidget(m_btnExport);
    mainLayout->addLayout(toolbarLayout);

    m_viewTabs = new QTabWidget(this);
    m_viewTabs->setDocumentMode(true);

    // 1. Setup dos 4 gráficos para o diagrama unificado 2x2
    setupAbramsChart();
    setupLyseChart();
    setupMolinariChart();
    setupCostChart();

    // 1. Visão Unificada de 4 Quadrantes (Diagrama IPT clássico em grelha 2x2)
    m_quadWidget = new QWidget(this);
    auto grid = new QGridLayout(m_quadWidget);
    grid->setContentsMargins(4, 4, 4, 4);
    grid->setSpacing(6);

    // Layout padrão do Diagrama de Dosagem de 4 Quadrantes:
    // [ Quadrante 1: Abrams (fcj x a/c) ] [ Quadrante 2: Lyse (m x a/c) ]
    // [ Quadrante 4: Custo/Consumo x m  ] [ Quadrante 3: Molinari (m x C) ]
    grid->addWidget(m_viewAbrams, 0, 0);
    grid->addWidget(m_viewLyse, 0, 1);
    grid->addWidget(m_viewCost, 1, 0);
    grid->addWidget(m_viewMolinari, 1, 1);

    m_viewTabs->addTab(m_quadWidget, tr("📊 Diagrama Completo de 4 Quadrantes (IPT)"));

    // 2. Setup dos 4 gráficos independentes para as abas ampliadas (sem colisão de QGraphicsScene)
    m_chartAbramsZoom = new QChart();
    m_chartAbramsZoom->setTitle(tr("1º Quadrante - Lei de Abrams (fcj x a/c)"));
    m_chartAbramsZoom->legend()->setAlignment(Qt::AlignBottom);
    m_chartAbramsZoom->setAnimationOptions(QChart::NoAnimation);
    m_viewAbramsZoom = new QChartView(m_chartAbramsZoom, this);
    m_viewAbramsZoom->setRenderHint(QPainter::Antialiasing);
    auto w1 = new QWidget();
    auto l1 = new QVBoxLayout(w1);
    l1->setContentsMargins(4, 4, 4, 4);
    l1->addWidget(m_viewAbramsZoom);
    m_tabZoomWidgets[0] = w1;
    m_viewTabs->addTab(w1, tr("1º Quad: Abrams (fcj x a/c)"));

    m_chartLyseZoom = new QChart();
    m_chartLyseZoom->setTitle(tr("2º Quadrante - Lei de Lyse (m x a/c)"));
    m_chartLyseZoom->legend()->setAlignment(Qt::AlignBottom);
    m_chartLyseZoom->setAnimationOptions(QChart::NoAnimation);
    m_viewLyseZoom = new QChartView(m_chartLyseZoom, this);
    m_viewLyseZoom->setRenderHint(QPainter::Antialiasing);
    auto w2 = new QWidget();
    auto l2 = new QVBoxLayout(w2);
    l2->setContentsMargins(4, 4, 4, 4);
    l2->addWidget(m_viewLyseZoom);
    m_tabZoomWidgets[1] = w2;
    m_viewTabs->addTab(w2, tr("2º Quad: Lyse (m x a/c)"));

    m_chartMolinariZoom = new QChart();
    m_chartMolinariZoom->setTitle(tr("3º Quadrante - Lei de Molinari (m x C)"));
    m_chartMolinariZoom->legend()->setAlignment(Qt::AlignBottom);
    m_chartMolinariZoom->setAnimationOptions(QChart::NoAnimation);
    m_viewMolinariZoom = new QChartView(m_chartMolinariZoom, this);
    m_viewMolinariZoom->setRenderHint(QPainter::Antialiasing);
    auto w3 = new QWidget();
    auto l3 = new QVBoxLayout(w3);
    l3->setContentsMargins(4, 4, 4, 4);
    l3->addWidget(m_viewMolinariZoom);
    m_tabZoomWidgets[2] = w3;
    m_viewTabs->addTab(w3, tr("3º Quad: Molinari (m x C)"));

    m_chartCostZoom = new QChart();
    m_chartCostZoom->setTitle(tr("4º Quadrante - Consumo x Custo por m³"));
    m_chartCostZoom->legend()->setAlignment(Qt::AlignBottom);
    m_chartCostZoom->setAnimationOptions(QChart::NoAnimation);
    m_viewCostZoom = new QChartView(m_chartCostZoom, this);
    m_viewCostZoom->setRenderHint(QPainter::Antialiasing);
    auto w4 = new QWidget();
    auto l4 = new QVBoxLayout(w4);
    l4->setContentsMargins(4, 4, 4, 4);
    l4->addWidget(m_viewCostZoom);
    m_tabZoomWidgets[3] = w4;
    m_viewTabs->addTab(w4, tr("4º Quad: Consumo e Custo"));

    // 5. Composição Unificada de Eixos Cruzados Abrams / Lyse / Molinari (Nomograma IPT)
    m_iptUnifiedDiagram = new IptUnifiedDiagramWidget(this);
    m_viewTabs->addTab(m_iptUnifiedDiagram, tr("5° Abrams/Lyse/Molinari (IPT)"));

    mainLayout->addWidget(m_viewTabs);

    bool isDark = (QGuiApplication::styleHints() && QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark);
    updateTheme(isDark);
}

void QuadChartWidget::applyFlatBackground(QChart *chart) {
    if (!chart) return;
    QColor bg = m_isDark ? QColor(32, 33, 36) : Qt::white;
    chart->setBackgroundBrush(QBrush(bg));
    chart->setBackgroundPen(QPen(Qt::NoPen));
    chart->setPlotAreaBackgroundBrush(QBrush(bg));
    chart->setPlotAreaBackgroundPen(QPen(m_isDark ? QColor(65, 68, 75) : QColor(210, 215, 225), 1.0));
    chart->setPlotAreaBackgroundVisible(true);

    QFont chartTitleFont("Segoe UI", 11, QFont::Bold);
    chart->setTitleFont(chartTitleFont);
    chart->setTitleBrush(QBrush(m_isDark ? QColor(245, 248, 255) : QColor(10, 15, 25)));
    chart->setMargins(QMargins(12, 8, 12, 8));
}

void QuadChartWidget::configureValueAxis(QValueAxis *axis, int tickCount, int minorTickCount) {
    if (!axis) return;
    axis->setTickCount(tickCount);
    axis->setMinorTickCount(minorTickCount);
    axis->setGridLineVisible(true);
    axis->setMinorGridLineVisible(true);

    QPen majorGrid(m_isDark ? QColor(65, 68, 75) : QColor(215, 220, 228), 1.0, Qt::SolidLine);
    QPen minorGrid(m_isDark ? QColor(48, 51, 56) : QColor(238, 241, 246), 0.75, Qt::DotLine);
    axis->setGridLinePen(majorGrid);
    axis->setMinorGridLinePen(minorGrid);

    QPen linePen(m_isDark ? QColor(160, 165, 175) : QColor(90, 95, 105), 1.2, Qt::SolidLine);
    axis->setLinePen(linePen);

    // Fontes grandes e em negrito para máxima legibilidade das escalas
    QFont labelFont("Segoe UI", 10, QFont::Bold);
    axis->setLabelsFont(labelFont);
    axis->setLabelsColor(m_isDark ? QColor(250, 250, 255) : QColor(15, 20, 30));

    QFont titleFont("Segoe UI", 10, QFont::Bold);
    axis->setTitleFont(titleFont);
    axis->setTitleBrush(QBrush(m_isDark ? QColor(245, 248, 255) : QColor(10, 15, 25)));
}

void QuadChartWidget::setupAbramsChart() {
    m_chartAbrams = new QChart();
    m_chartAbrams->setTitle(tr("1º Quadrante - Lei de Abrams (fcj x a/c)"));
    m_chartAbrams->legend()->setAlignment(Qt::AlignBottom);
    m_chartAbrams->setAnimationOptions(QChart::NoAnimation);
    applyFlatBackground(m_chartAbrams);

    m_viewAbrams = new QChartView(m_chartAbrams, this);
    m_viewAbrams->setRenderHint(QPainter::Antialiasing);
}

void QuadChartWidget::setupLyseChart() {
    m_chartLyse = new QChart();
    m_chartLyse->setTitle(tr("2º Quadrante - Lei de Lyse (m x a/c)"));
    m_chartLyse->legend()->setAlignment(Qt::AlignBottom);
    m_chartLyse->setAnimationOptions(QChart::NoAnimation);
    applyFlatBackground(m_chartLyse);

    m_viewLyse = new QChartView(m_chartLyse, this);
    m_viewLyse->setRenderHint(QPainter::Antialiasing);
}

void QuadChartWidget::setupMolinariChart() {
    m_chartMolinari = new QChart();
    m_chartMolinari->setTitle(tr("3º Quadrante - Lei de Molinari (m x C)"));
    m_chartMolinari->legend()->setAlignment(Qt::AlignBottom);
    m_chartMolinari->setAnimationOptions(QChart::NoAnimation);
    applyFlatBackground(m_chartMolinari);

    m_viewMolinari = new QChartView(m_chartMolinari, this);
    m_viewMolinari->setRenderHint(QPainter::Antialiasing);
}

void QuadChartWidget::setupCostChart() {
    m_chartCost = new QChart();
    m_chartCost->setTitle(tr("4º Quadrante - Consumo x Custo por m³"));
    m_chartCost->legend()->setAlignment(Qt::AlignBottom);
    m_chartCost->setAnimationOptions(QChart::NoAnimation);
    applyFlatBackground(m_chartCost);

    m_viewCost = new QChartView(m_chartCost, this);
    m_viewCost->setRenderHint(QPainter::Antialiasing);
}

void QuadChartWidget::populateAbrams(QChart *chart, const MixDesignResult& result, const std::vector<ExperimentalMixRecord>& experimentalMixes) {
    if (!chart) return;
    chart->removeAllSeries();
    for (auto ax : chart->axes()) chart->removeAxis(ax);
    applyFlatBackground(chart);

    chart->setTitle(tr("1º Quadrante - Lei de Abrams (fcj x a/c)"));

    bool hasData = false;
    for (const auto& mix : experimentalMixes) {
        if (mix.waterCementRatio > 0.0 && mix.compressiveStrength28d > 0.0) {
            hasData = true;
            break;
        }
    }
    if (!hasData || result.targetWaterCement <= 0.0) {
        auto axisX = new QValueAxis();
        axisX->setTitleText(tr("Relação Água/Cimento (a/c)"));
        axisX->setRange(0.20, 0.80);
        axisX->setLabelFormat("%.2f");
        configureValueAxis(axisX, 7, 4);
        chart->addAxis(axisX, Qt::AlignBottom);

        auto axisY = new QValueAxis();
        axisY->setTitleText(tr("Resistência fcj (MPa)"));
        axisY->setRange(0.0, 50.0);
        axisY->setLabelFormat("%.0f");
        configureValueAxis(axisY, 6, 4);
        chart->addAxis(axisY, Qt::AlignLeft);
        return;
    }

    auto scatterAbrams = new QScatterSeries();
    scatterAbrams->setName(tr("Pontos Experimentais"));
    scatterAbrams->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    scatterAbrams->setMarkerSize(12.0);
    scatterAbrams->setColor(QColor(220, 53, 69)); // Red

    double minAc = 1.0, maxAc = 0.0;
    double minFcj = 100.0, maxFcj = 0.0;

    for (const auto& mix : experimentalMixes) {
        scatterAbrams->append(mix.waterCementRatio, mix.compressiveStrength28d);
        minAc = std::min(minAc, mix.waterCementRatio);
        maxAc = std::max(maxAc, mix.waterCementRatio);
        minFcj = std::min(minFcj, mix.compressiveStrength28d);
        maxFcj = std::max(maxFcj, mix.compressiveStrength28d);
    }
    minAc = std::min(minAc, result.targetWaterCement);
    maxAc = std::max(maxAc, result.targetWaterCement);
    minFcj = std::min(minFcj, result.fcj);
    maxFcj = std::max(maxFcj, result.fcj);

    double xMargin = (maxAc - minAc) * 0.20;
    if (xMargin < 0.05) xMargin = 0.05;
    double xStart = std::max(0.10, minAc - xMargin);
    double xEnd = maxAc + xMargin;

    auto lineAbrams = new QLineSeries();
    lineAbrams->setName(tr("Curva de Regressão"));
    QPen penCurve(QColor(0, 122, 255), 2.5);
    lineAbrams->setPen(penCurve);

    for (double x = xStart; x <= xEnd; x += (xEnd - xStart) / 60.0) {
        lineAbrams->append(x, result.abramsPoly.evaluate(x));
    }

    // Linhas guia do ponto ótimo (fcj_alvo -> ac_alvo)
    auto guideAbramsH = new QLineSeries();
    guideAbramsH->setName(tr("fcj de dosagem = %1 MPa").arg(result.fcj, 0, 'f', 2));
    QPen penGuide(QColor(40, 167, 69), 2.0, Qt::DashLine);
    guideAbramsH->setPen(penGuide);
    guideAbramsH->append(xStart, result.fcj);
    guideAbramsH->append(result.targetWaterCement, result.fcj);

    auto guideAbramsV = new QLineSeries();
    guideAbramsV->setName(tr("a/c ótimo = %1").arg(result.targetWaterCement, 0, 'f', 3));
    guideAbramsV->setPen(penGuide);
    guideAbramsV->append(result.targetWaterCement, result.fcj);
    guideAbramsV->append(result.targetWaterCement, 0.0);

    chart->addSeries(lineAbrams);
    chart->addSeries(guideAbramsH);
    chart->addSeries(guideAbramsV);
    chart->addSeries(scatterAbrams);

    auto axisXAbrams = new QValueAxis();
    axisXAbrams->setTitleText(tr("Relação Água/Cimento (a/c)"));
    axisXAbrams->setRange(xStart, xEnd);
    axisXAbrams->setLabelFormat("%.2f");
    configureValueAxis(axisXAbrams, 8, 4);
    chart->addAxis(axisXAbrams, Qt::AlignBottom);

    double yEndFc = std::ceil((maxFcj * 1.25) / 10.0) * 10.0;
    if (yEndFc < 40.0) yEndFc = 40.0;
    int ticksFc = static_cast<int>(yEndFc / 10.0) + 1;
    if (ticksFc < 5) ticksFc = 5;
    if (ticksFc > 8) ticksFc = 7;

    auto axisYAbrams = new QValueAxis();
    axisYAbrams->setTitleText(tr("Resistência fcj (MPa)"));
    axisYAbrams->setRange(0.0, yEndFc);
    axisYAbrams->setLabelFormat("%.0f");
    configureValueAxis(axisYAbrams, ticksFc, 4);
    chart->addAxis(axisYAbrams, Qt::AlignLeft);

    lineAbrams->attachAxis(axisXAbrams);
    lineAbrams->attachAxis(axisYAbrams);
    guideAbramsH->attachAxis(axisXAbrams);
    guideAbramsH->attachAxis(axisYAbrams);
    guideAbramsV->attachAxis(axisXAbrams);
    guideAbramsV->attachAxis(axisYAbrams);
    scatterAbrams->attachAxis(axisXAbrams);
    scatterAbrams->attachAxis(axisYAbrams);
}

void QuadChartWidget::populateLyse(QChart *chart, const MixDesignResult& result, const std::vector<ExperimentalMixRecord>& experimentalMixes) {
    if (!chart) return;
    chart->removeAllSeries();
    for (auto ax : chart->axes()) chart->removeAxis(ax);
    applyFlatBackground(chart);

    chart->setTitle(tr("2º Quadrante - Lei de Lyse (m x a/c)"));

    bool hasData = false;
    for (const auto& mix : experimentalMixes) {
        if (mix.waterCementRatio > 0.0 && mix.dryRatioM > 0.0) {
            hasData = true;
            break;
        }
    }
    if (!hasData || result.targetWaterCement <= 0.0) {
        auto axisX = new QValueAxis();
        axisX->setTitleText(tr("Relação Água/Cimento (a/c)"));
        axisX->setRange(0.20, 0.80);
        axisX->setLabelFormat("%.2f");
        configureValueAxis(axisX, 7, 4);
        chart->addAxis(axisX, Qt::AlignBottom);

        auto axisY = new QValueAxis();
        axisY->setTitleText(tr("Traço Seco Total m (1:m)"));
        axisY->setRange(0.0, 12.0);
        axisY->setLabelFormat("%.1f");
        configureValueAxis(axisY, 7, 3);
        chart->addAxis(axisY, Qt::AlignLeft);
        return;
    }

    auto scatterLyse = new QScatterSeries();
    scatterLyse->setName(tr("Pontos Experimentais"));
    scatterLyse->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    scatterLyse->setMarkerSize(12.0);
    scatterLyse->setColor(QColor(220, 53, 69));

    double minAc = 1.0, maxAc = 0.0;
    double minM = 10.0, maxM = 0.0;
    for (const auto& mix : experimentalMixes) {
        scatterLyse->append(mix.waterCementRatio, mix.dryRatioM);
        minAc = std::min(minAc, mix.waterCementRatio);
        maxAc = std::max(maxAc, mix.waterCementRatio);
        minM = std::min(minM, mix.dryRatioM);
        maxM = std::max(maxM, mix.dryRatioM);
    }
    minAc = std::min(minAc, result.targetWaterCement);
    maxAc = std::max(maxAc, result.targetWaterCement);
    minM = std::min(minM, result.targetDryMixM);
    maxM = std::max(maxM, result.targetDryMixM);

    double xMargin = (maxAc - minAc) * 0.20;
    if (xMargin < 0.05) xMargin = 0.05;
    double xStart = std::max(0.10, minAc - xMargin);
    double xEnd = maxAc + xMargin;

    auto lineLyse = new QLineSeries();
    lineLyse->setName(tr("Reta de Lyse"));
    QPen penLyse(QColor(40, 167, 69), 2.5);
    lineLyse->setPen(penLyse);

    for (double x = xStart; x <= xEnd; x += (xEnd - xStart) / 40.0) {
        lineLyse->append(x, result.lyseLinear.evaluate(x));
    }

    QPen penGuide(QColor(40, 167, 69), 2.0, Qt::DashLine);
    auto guideLyseV = new QLineSeries();
    guideLyseV->setName(tr("a/c = %1").arg(result.targetWaterCement, 0, 'f', 3));
    guideLyseV->setPen(penGuide);
    guideLyseV->append(result.targetWaterCement, 0.0);
    guideLyseV->append(result.targetWaterCement, result.targetDryMixM);

    auto guideLyseH = new QLineSeries();
    guideLyseH->setName(tr("m ótimo = %1").arg(result.targetDryMixM, 0, 'f', 2));
    guideLyseH->setPen(penGuide);
    guideLyseH->append(result.targetWaterCement, result.targetDryMixM);
    guideLyseH->append(xEnd, result.targetDryMixM);

    chart->addSeries(lineLyse);
    chart->addSeries(guideLyseV);
    chart->addSeries(guideLyseH);
    chart->addSeries(scatterLyse);

    auto axisXLyse = new QValueAxis();
    axisXLyse->setTitleText(tr("Relação Água/Cimento (a/c)"));
    axisXLyse->setRange(xStart, xEnd);
    axisXLyse->setLabelFormat("%.2f");
    configureValueAxis(axisXLyse, 8, 4);
    chart->addAxis(axisXLyse, Qt::AlignBottom);

    double yEndM = std::ceil((maxM * 1.25) / 2.0) * 2.0;
    if (yEndM < 8.0) yEndM = 8.0;
    int ticksM = static_cast<int>(yEndM / 2.0) + 1;
    if (ticksM < 5) ticksM = 5;
    if (ticksM > 8) ticksM = 7;

    auto axisYLyse = new QValueAxis();
    axisYLyse->setTitleText(tr("Traço Seco Total m (1:m)"));
    axisYLyse->setRange(0.0, yEndM);
    axisYLyse->setLabelFormat("%.1f");
    configureValueAxis(axisYLyse, ticksM, 3);
    chart->addAxis(axisYLyse, Qt::AlignLeft);

    lineLyse->attachAxis(axisXLyse);
    lineLyse->attachAxis(axisYLyse);
    guideLyseV->attachAxis(axisXLyse);
    guideLyseV->attachAxis(axisYLyse);
    guideLyseH->attachAxis(axisXLyse);
    guideLyseH->attachAxis(axisYLyse);
    scatterLyse->attachAxis(axisXLyse);
    scatterLyse->attachAxis(axisYLyse);
}

void QuadChartWidget::populateMolinari(QChart *chart, const MixDesignResult& result, const std::vector<ExperimentalMixRecord>& experimentalMixes) {
    if (!chart) return;
    chart->removeAllSeries();
    for (auto ax : chart->axes()) chart->removeAxis(ax);
    applyFlatBackground(chart);

    chart->setTitle(tr("3º Quadrante - Lei de Molinari (m x C)"));

    bool hasData = false;
    for (const auto& mix : experimentalMixes) {
        if (mix.cementConsumption > 0.0 && mix.dryRatioM > 0.0) {
            hasData = true;
            break;
        }
    }
    if (!hasData || result.targetCementConsumption <= 0.0) {
        auto axisX = new QValueAxis();
        axisX->setTitleText(tr("Consumo de Cimento C (kg/m³)"));
        axisX->setRange(200.0, 600.0);
        axisX->setLabelFormat("%.0f");
        configureValueAxis(axisX, 9, 4);
        chart->addAxis(axisX, Qt::AlignBottom);

        auto axisY = new QValueAxis();
        axisY->setTitleText(tr("Traço Seco Total m (1:m)"));
        axisY->setRange(0.0, 12.0);
        axisY->setLabelFormat("%.1f");
        configureValueAxis(axisY, 7, 3);
        chart->addAxis(axisY, Qt::AlignLeft);
        return;
    }

    auto scatterMolinari = new QScatterSeries();
    scatterMolinari->setName(tr("Pontos Experimentais"));
    scatterMolinari->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    scatterMolinari->setMarkerSize(12.0);
    scatterMolinari->setColor(QColor(220, 53, 69));

    double minC = 1000.0, maxC = 0.0;
    double maxM = 0.0;
    for (const auto& mix : experimentalMixes) {
        scatterMolinari->append(mix.cementConsumption, mix.dryRatioM);
        minC = std::min(minC, mix.cementConsumption);
        maxC = std::max(maxC, mix.cementConsumption);
        maxM = std::max(maxM, mix.dryRatioM);
    }
    minC = std::min(minC, result.targetCementConsumption);
    maxC = std::max(maxC, result.targetCementConsumption);
    maxM = std::max(maxM, result.targetDryMixM);

    double cStart = std::max(150.0, minC - 60.0);
    double cEnd = maxC + 60.0;

    auto lineMolinari = new QLineSeries();
    lineMolinari->setName(tr("Curva de Molinari"));
    QPen penMoli(QColor(255, 149, 0), 2.5); // Orange
    lineMolinari->setPen(penMoli);

    for (double c = cStart; c <= cEnd; c += (cEnd - cStart) / 50.0) {
        lineMolinari->append(c, result.molinariPoly.evaluate(c));
    }

    QPen penGuide(QColor(40, 167, 69), 2.0, Qt::DashLine);
    auto guideMoliH = new QLineSeries();
    guideMoliH->setName(tr("m = %1").arg(result.targetDryMixM, 0, 'f', 2));
    guideMoliH->setPen(penGuide);
    guideMoliH->append(cStart, result.targetDryMixM);
    guideMoliH->append(result.targetCementConsumption, result.targetDryMixM);

    auto guideMoliV = new QLineSeries();
    guideMoliV->setName(tr("Consumo C = %1 kg/m³").arg(result.targetCementConsumption, 0, 'f', 1));
    guideMoliV->setPen(penGuide);
    guideMoliV->append(result.targetCementConsumption, result.targetDryMixM);
    guideMoliV->append(result.targetCementConsumption, 0.0);

    chart->addSeries(lineMolinari);
    chart->addSeries(guideMoliH);
    chart->addSeries(guideMoliV);
    chart->addSeries(scatterMolinari);

    auto axisXMolinari = new QValueAxis();
    axisXMolinari->setTitleText(tr("Consumo de Cimento C (kg/m³)"));
    axisXMolinari->setRange(cStart, cEnd);
    axisXMolinari->setLabelFormat("%.0f");
    configureValueAxis(axisXMolinari, 9, 4);
    chart->addAxis(axisXMolinari, Qt::AlignBottom);

    double yEndMoli = std::ceil((maxM * 1.25) / 2.0) * 2.0;
    if (yEndMoli < 8.0) yEndMoli = 8.0;
    int ticksMoli = static_cast<int>(yEndMoli / 2.0) + 1;
    if (ticksMoli < 5) ticksMoli = 5;
    if (ticksMoli > 8) ticksMoli = 7;

    auto axisYMolinari = new QValueAxis();
    axisYMolinari->setTitleText(tr("Traço Seco Total m (1:m)"));
    axisYMolinari->setRange(0.0, yEndMoli);
    axisYMolinari->setLabelFormat("%.1f");
    configureValueAxis(axisYMolinari, ticksMoli, 3);
    chart->addAxis(axisYMolinari, Qt::AlignLeft);

    lineMolinari->attachAxis(axisXMolinari);
    lineMolinari->attachAxis(axisYMolinari);
    guideMoliH->attachAxis(axisXMolinari);
    guideMoliH->attachAxis(axisYMolinari);
    guideMoliV->attachAxis(axisXMolinari);
    guideMoliV->attachAxis(axisYMolinari);
    scatterMolinari->attachAxis(axisXMolinari);
    scatterMolinari->attachAxis(axisYMolinari);
}

void QuadChartWidget::populateCost(QChart *chart, const MixDesignResult& result, const std::vector<ExperimentalMixRecord>& experimentalMixes) {
    if (!chart) return;
    chart->removeAllSeries();
    for (auto ax : chart->axes()) chart->removeAxis(ax);
    applyFlatBackground(chart);

    chart->setTitle(tr("4º Quadrante - Consumo x Custo por m³"));

    if (result.targetCementConsumption <= 0.0 || result.costPerM3Analytical.totalCost <= 0.0) {
        auto axisX = new QValueAxis();
        axisX->setTitleText(tr("Consumo de Cimento C (kg/m³)"));
        axisX->setRange(200.0, 600.0);
        axisX->setLabelFormat("%.0f");
        configureValueAxis(axisX, 9, 4);
        chart->addAxis(axisX, Qt::AlignBottom);

        const QString curr = Language::GerenciadorTraducao::currencySymbol();
        auto axisY = new QValueAxis();
        axisY->setTitleText(tr("Custo Estimado (%1/m³)").arg(curr));
        axisY->setRange(0.0, 600.0);
        axisY->setLabelFormat("%.0f");
        configureValueAxis(axisY, 7, 4);
        chart->addAxis(axisY, Qt::AlignLeft);
        return;
    }

    const QString curr = Language::GerenciadorTraducao::currencySymbol();

    double minC = 1000.0, maxC = 0.0;
    for (const auto& mix : experimentalMixes) {
        minC = std::min(minC, mix.cementConsumption);
        maxC = std::max(maxC, mix.cementConsumption);
    }
    minC = std::min(minC, result.targetCementConsumption);
    maxC = std::max(maxC, result.targetCementConsumption);

    double cStart = std::max(150.0, minC - 60.0);
    double cEnd = maxC + 60.0;

    auto lineCost = new QLineSeries();
    lineCost->setName(tr("Custo Total (%1/m³) x Consumo").arg(curr));
    QPen penCost(QColor(175, 82, 222), 2.5); // Purple
    lineCost->setPen(penCost);

    double costUnitRate = (result.costPerM3Analytical.totalCost / (result.targetCementConsumption > 0.0 ? result.targetCementConsumption : 350.0));
    for (double c = cStart; c <= cEnd; c += (cEnd - cStart) / 40.0) {
        lineCost->append(c, c * costUnitRate);
    }

    QPen penGuide(QColor(40, 167, 69), 2.0, Qt::DashLine);
    auto guideCostV = new QLineSeries();
    guideCostV->setName(tr("C = %1 kg/m³").arg(result.targetCementConsumption, 0, 'f', 1));
    guideCostV->setPen(penGuide);
    guideCostV->append(result.targetCementConsumption, 0.0);
    guideCostV->append(result.targetCementConsumption, result.costPerM3Analytical.totalCost);

    auto guideCostH = new QLineSeries();
    guideCostH->setName(tr("Custo = %1 %2/m³").arg(curr).arg(result.costPerM3Analytical.totalCost, 0, 'f', 2));
    guideCostH->setPen(penGuide);
    guideCostH->append(cStart, result.costPerM3Analytical.totalCost);
    guideCostH->append(result.targetCementConsumption, result.costPerM3Analytical.totalCost);

    chart->addSeries(lineCost);
    chart->addSeries(guideCostV);
    chart->addSeries(guideCostH);

    auto axisXCost = new QValueAxis();
    axisXCost->setTitleText(tr("Consumo de Cimento C (kg/m³)"));
    axisXCost->setRange(cStart, cEnd);
    axisXCost->setLabelFormat("%.0f");
    configureValueAxis(axisXCost, 9, 4);
    chart->addAxis(axisXCost, Qt::AlignBottom);

    double maxCost = result.costPerM3Analytical.totalCost * 1.30;
    double yEndCost = std::ceil(maxCost / 100.0) * 100.0;
    if (yEndCost < 400.0) yEndCost = 400.0;
    int ticksCost = static_cast<int>(yEndCost / 100.0) + 1;
    if (ticksCost < 5) ticksCost = 5;
    if (ticksCost > 8) ticksCost = 7;

    auto axisYCost = new QValueAxis();
    axisYCost->setTitleText(tr("Custo Estimado (%1/m³)").arg(curr));
    axisYCost->setRange(0.0, yEndCost);
    axisYCost->setLabelFormat("%.0f");
    configureValueAxis(axisYCost, ticksCost, 4);
    chart->addAxis(axisYCost, Qt::AlignLeft);

    lineCost->attachAxis(axisXCost);
    lineCost->attachAxis(axisYCost);
    guideCostV->attachAxis(axisXCost);
    guideCostV->attachAxis(axisYCost);
    guideCostH->attachAxis(axisXCost);
    guideCostH->attachAxis(axisYCost);
}

void QuadChartWidget::updateCharts(const MixDesignResult& result, const std::vector<ExperimentalMixRecord>& experimentalMixes) {
    m_hasData = true;
    m_lastResult = result;
    m_lastMixes = experimentalMixes;

    // Popula visão de 4 Quadrantes (2x2 grid)
    populateAbrams(m_chartAbrams, result, experimentalMixes);
    populateLyse(m_chartLyse, result, experimentalMixes);
    populateMolinari(m_chartMolinari, result, experimentalMixes);
    populateCost(m_chartCost, result, experimentalMixes);

    // Popula abas de zoom ampliadas
    populateAbrams(m_chartAbramsZoom, result, experimentalMixes);
    populateLyse(m_chartLyseZoom, result, experimentalMixes);
    populateMolinari(m_chartMolinariZoom, result, experimentalMixes);
    populateCost(m_chartCostZoom, result, experimentalMixes);

    // Popula o 5º Gráfico: Composição unificada dos eixos cruzados IPT
    if (m_iptUnifiedDiagram) {
        m_iptUnifiedDiagram->setData(result, experimentalMixes);
    }
}

void QuadChartWidget::updateTheme(bool isDark) {
    m_isDark = isDark;
    auto theme = isDark ? QChart::ChartThemeDark : QChart::ChartThemeLight;
    QList<QChart*> allCharts = {
        m_chartAbrams, m_chartLyse, m_chartMolinari, m_chartCost,
        m_chartAbramsZoom, m_chartLyseZoom, m_chartMolinariZoom, m_chartCostZoom
    };
    for (auto c : allCharts) {
        if (c) {
            c->setTheme(theme);
            applyFlatBackground(c);
        }
    }

    QColor bg = isDark ? QColor(32, 33, 36) : Qt::white;
    QList<QChartView*> allViews = {
        m_viewAbrams, m_viewLyse, m_viewMolinari, m_viewCost,
        m_viewAbramsZoom, m_viewLyseZoom, m_viewMolinariZoom, m_viewCostZoom
    };
    for (auto v : allViews) {
        if (v) v->setBackgroundBrush(QBrush(bg));
    }

    if (m_quadWidget) {
        m_quadWidget->setStyleSheet(isDark ? QStringLiteral("background-color: #202124;") : QStringLiteral("background-color: #ffffff;"));
    }
    for (int i = 0; i < 4; ++i) {
        if (m_tabZoomWidgets[i]) {
            m_tabZoomWidgets[i]->setStyleSheet(isDark ? QStringLiteral("background-color: #202124;") : QStringLiteral("background-color: #ffffff;"));
        }
    }

    if (m_iptUnifiedDiagram) {
        m_iptUnifiedDiagram->setTheme(isDark);
    }

    if (m_hasData) {
        updateCharts(m_lastResult, m_lastMixes);
    }
}

void QuadChartWidget::retranslateUi() {
    if (m_btnExport) {
        m_btnExport->setText(tr("📷 Exportar Imagem dos Gráficos..."));
    }

    if (m_viewTabs) {
        m_viewTabs->setTabText(0, tr("📊 Diagrama Completo de 4 Quadrantes (IPT)"));
        m_viewTabs->setTabText(1, tr("1º Quad: Abrams (fcj x a/c)"));
        m_viewTabs->setTabText(2, tr("2º Quad: Lyse (m x a/c)"));
        m_viewTabs->setTabText(3, tr("3º Quad: Molinari (m x C)"));
        m_viewTabs->setTabText(4, tr("4º Quad: Consumo e Custo"));
        m_viewTabs->setTabText(5, tr("5° Abrams/Lyse/Molinari (IPT)"));
    }

    if (m_iptUnifiedDiagram) {
        m_iptUnifiedDiagram->retranslateUi();
    }

    if (m_hasData) {
        updateCharts(m_lastResult, m_lastMixes);
    }
}

void QuadChartWidget::changeEvent(QEvent *event) {
    if (event->type() == QEvent::LanguageChange) {
        retranslateUi();
    }
    QWidget::changeEvent(event);
}

void QuadChartWidget::exportChartImage(const QString& filePath) {
    if (m_viewTabs && m_viewTabs->currentWidget() == m_iptUnifiedDiagram && m_iptUnifiedDiagram) {
        m_iptUnifiedDiagram->exportImage(filePath);
        return;
    }

    QWidget *targetWidget = (m_viewTabs && m_viewTabs->currentWidget()) ? m_viewTabs->currentWidget() : m_quadWidget;
    if (!targetWidget) return;

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
        QPdfWriter pdfWriter(filePath);
        pdfWriter.setPageSize(QPageSize(QPageSize::A4));
        pdfWriter.setPageOrientation(QPageLayout::Landscape);
        pdfWriter.setResolution(300);
        QPainter painter(&pdfWriter);
        const QRect pageRect = pdfWriter.pageLayout().paintRectPixels(pdfWriter.resolution());
        double scale = std::min(double(pageRect.width()) / std::max(1, targetWidget->width()),
                                double(pageRect.height()) / std::max(1, targetWidget->height()));
        painter.scale(scale, scale);
        targetWidget->render(&painter);
        painter.end();
        showModelessMsg(QMessageBox::Information, tr("Sucesso"), tr("Gráficos exportados com sucesso para:\n%1").arg(filePath));
        return;
    }

    // Renderiza em alta resolução 2x (retina) com excelente nitidez e sem distorção
    const int scale = 2;
    QPixmap pixmap(targetWidget->size() * scale);
    pixmap.setDevicePixelRatio(scale);
    pixmap.fill(m_isDark ? QColor(32, 33, 36) : Qt::white);
    targetWidget->render(&pixmap);

    if (pixmap.save(filePath)) {
        showModelessMsg(QMessageBox::Information, tr("Sucesso"), tr("Gráficos exportados com sucesso para:\n%1").arg(filePath));
    } else {
        showModelessMsg(QMessageBox::Warning, tr("Erro"), tr("Não foi possível salvar o arquivo de imagem."));
    }
}

} // namespace ConcreteMix
