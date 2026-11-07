#include "TabCosts.hpp"
#include "../language/GerenciadorTraducao.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QScrollArea>
#include <QEvent>

namespace ConcreteMix {

TabCosts::TabCosts(ProjectData *data, QWidget *parent)
    : QWidget(parent), m_data(data)
{
    setupUI();
    refreshResults();
}

void TabCosts::setupUI() {
    auto scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    auto container = new QWidget(scrollArea);
    auto mainLayout = new QVBoxLayout(container);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(12);

    // 1. Grupo: Volume da Obra e Leitura Gráfica Manual
    m_groupInputs = new QGroupBox(tr("1. Parâmetros de Volume e Leituras do Ábaco Gráfico Manual"), container);
    auto gridIn = new QGridLayout(m_groupInputs);
    gridIn->setSpacing(10);

    m_lblProjectVolume = new QLabel(tr("<b>Volume Total da Obra (m³):</b>"), m_groupInputs);
    gridIn->addWidget(m_lblProjectVolume, 0, 0);
    m_spinProjectVolume = new QDoubleSpinBox(m_groupInputs);
    m_spinProjectVolume->setRange(0.0, 100000.0);
    m_spinProjectVolume->setValue(30.0);
    m_spinProjectVolume->setSuffix(" m³");
    gridIn->addWidget(m_spinProjectVolume, 0, 1);

    m_lblManualAc = new QLabel(tr("Leitura Gráfica Manual a/c:"), m_groupInputs);
    gridIn->addWidget(m_lblManualAc, 1, 0);
    m_spinManualAc = new QDoubleSpinBox(m_groupInputs);
    m_spinManualAc->setRange(0.0, 2.0);
    m_spinManualAc->setDecimals(4);
    m_spinManualAc->setValue(0.4285);
    gridIn->addWidget(m_spinManualAc, 1, 1);

    m_lblManualM = new QLabel(tr("Leitura Gráfica Manual Traço m:"), m_groupInputs);
    gridIn->addWidget(m_lblManualM, 1, 2);
    m_spinManualM = new QDoubleSpinBox(m_groupInputs);
    m_spinManualM->setRange(0.0, 20.0);
    m_spinManualM->setDecimals(2);
    m_spinManualM->setValue(5.80);
    gridIn->addWidget(m_spinManualM, 1, 3);

    m_lblManualC = new QLabel(tr("Leitura Gráfica Manual Consumo C (kg/m³):"), m_groupInputs);
    gridIn->addWidget(m_lblManualC, 1, 4);
    m_spinManualC = new QDoubleSpinBox(m_groupInputs);
    m_spinManualC->setRange(0.0, 1000.0);
    m_spinManualC->setDecimals(1);
    m_spinManualC->setValue(332.0);
    gridIn->addWidget(m_spinManualC, 1, 5);

    mainLayout->addWidget(m_groupInputs);

    // 2. Grupo: Tabela Comparativa Analítico vs Gráfico (Expansível dinamicamente com a GUI)
    m_groupTable = new QGroupBox(tr("2. Comparativo Detalhado de Custos e Consumos (Analítico x Gráfico)"), container);
    auto vTable = new QVBoxLayout(m_groupTable);
    vTable->setContentsMargins(8, 8, 8, 8);

    m_tableComparison = new QTableWidget(10, 4, m_groupTable);
    m_tableComparison->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableComparison->setHorizontalHeaderLabels({
        tr("Item de Comparação"),
        tr("Solução Analítica (Equações)"),
        tr("Leitura Manual (Ábaco)"),
        tr("Diferença (|Δ|)")
    });
    m_tableComparison->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tableComparison->verticalHeader()->setVisible(false);
    m_tableComparison->setMinimumHeight(240);
    m_tableComparison->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    vTable->addWidget(m_tableComparison);

    mainLayout->addWidget(m_groupTable, 1);

    // 3. Grupo: Síntese de Diferenças e Impacto Econômico
    m_groupDiff = new QGroupBox(tr("3. Impacto Financeiro da Precisão Analítica das Equações na Obra"), container);
    auto hDiff = new QHBoxLayout(m_groupDiff);
    hDiff->setSpacing(12);

    auto makeDiffCard = [](QLabel*& titleLbl, const QString& title, QLabel*& lbl, QWidget* parent) -> QWidget* {
        auto w = new QFrame(parent);
        w->setFrameShape(QFrame::StyledPanel);
        w->setFrameShadow(QFrame::Plain);
        auto l = new QVBoxLayout(w);
        l->setContentsMargins(10, 8, 10, 8);
        l->setSpacing(4);
        titleLbl = new QLabel(title, w);
        QFont fTitle = titleLbl->font();
        fTitle.setBold(true);
        titleLbl->setFont(fTitle);
        lbl = new QLabel("-", w);
        QFont fLbl = lbl->font();
        fLbl.setBold(true);
        fLbl.setPointSize(fLbl.pointSize() + 3);
        lbl->setFont(fLbl);
        l->addWidget(titleLbl);
        l->addWidget(lbl);
        return w;
    };

    hDiff->addWidget(makeDiffCard(m_lblCostDiffTitle, tr("Diferença Total no Custo da Obra"), m_lblCostDiffSummary, m_groupDiff));
    hDiff->addWidget(makeDiffCard(m_lblBagsDiffTitle, tr("Diferença na Quantidade de Cimento (Sacos 50 kg)"), m_lblBagsDiffSummary, m_groupDiff));

    mainLayout->addWidget(m_groupDiff);

    scrollArea->setWidget(container);
    auto rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->addWidget(scrollArea);

    // Conexões
    auto triggerUpdate = [this]() {
        if (m_data) {
            m_data->projectVolumeM3 = m_spinProjectVolume->value();
            m_data->manualAc = m_spinManualAc->value();
            m_data->manualM = m_spinManualM->value();
            m_data->manualC = m_spinManualC->value();
        }
        refreshResults();
        emit costsChanged();
    };

    connect(m_spinProjectVolume, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, triggerUpdate);
    connect(m_spinManualAc, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, triggerUpdate);
    connect(m_spinManualM, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, triggerUpdate);
    connect(m_spinManualC, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, triggerUpdate);
}

void TabCosts::updateUIFromData() {
    if (!m_data) return;

    if (!m_spinProjectVolume->hasFocus()) {
        m_spinProjectVolume->blockSignals(true);
        m_spinProjectVolume->setValue(m_data->projectVolumeM3);
        m_spinProjectVolume->blockSignals(false);
    }

    if (!m_spinManualAc->hasFocus()) {
        m_spinManualAc->blockSignals(true);
        m_spinManualAc->setValue(m_data->manualAc);
        m_spinManualAc->blockSignals(false);
    }

    if (!m_spinManualM->hasFocus()) {
        m_spinManualM->blockSignals(true);
        m_spinManualM->setValue(m_data->manualM);
        m_spinManualM->blockSignals(false);
    }

    if (!m_spinManualC->hasFocus()) {
        m_spinManualC->blockSignals(true);
        m_spinManualC->setValue(m_data->manualC);
        m_spinManualC->blockSignals(false);
    }

    refreshResults();
}

void TabCosts::refreshResults() {
    if (!m_data) return;

    auto res = m_data->calculate();

    if (res.targetCementConsumption <= 0.0) {
        m_lblCostDiffSummary->setText("-");
        m_lblBagsDiffSummary->setText("-");
        m_tableComparison->clearContents();
        return;
    }

    auto setRowComp = [this](int row, const QString& item, const QString& analit, const QString& graf, const QString& diff) {
        m_tableComparison->setItem(row, 0, new QTableWidgetItem(item));
        m_tableComparison->setItem(row, 1, new QTableWidgetItem(analit));
        m_tableComparison->setItem(row, 2, new QTableWidgetItem(graf));
        m_tableComparison->setItem(row, 3, new QTableWidgetItem(diff));
    };

    const QString curr = Language::GerenciadorTraducao::currencySymbol();

    // 10 linhas de comparação
    setRowComp(0, tr("Relação Água/Cimento (a/c)"),
               QString::asprintf("%.4f", res.targetWaterCement),
               QString::asprintf("%.4f", res.graphicalWaterCement),
               QString::asprintf("%.4f", std::abs(res.targetWaterCement - res.graphicalWaterCement)));

    setRowComp(1, tr("Traço Seco Total (m)"),
               QString::asprintf("%.2f", res.targetDryMixM),
               QString::asprintf("%.2f", res.graphicalDryMixM),
               QString::asprintf("%.2f", std::abs(res.targetDryMixM - res.graphicalDryMixM)));

    setRowComp(2, tr("Consumo de Cimento C (kg/m³)"),
               QString::asprintf("%.1f kg/m³", res.targetCementConsumption),
               QString::asprintf("%.1f kg/m³", res.graphicalCementConsumption),
               QString::asprintf("%.1f kg/m³", std::abs(res.targetCementConsumption - res.graphicalCementConsumption)));

    setRowComp(3, tr("Custo de Cimento (%1/m³)").arg(curr),
               QStringLiteral("%1 %2").arg(curr, QString::number(res.costPerM3Analytical.cementCost, 'f', 2)),
               QStringLiteral("%1 %2").arg(curr, QString::number(res.costPerM3Graphical.cementCost, 'f', 2)),
               QStringLiteral("%1 %2").arg(curr, QString::number(std::abs(res.costPerM3Analytical.cementCost - res.costPerM3Graphical.cementCost), 'f', 2)));

    setRowComp(4, tr("Custo de Areia (%1/m³)").arg(curr),
               QStringLiteral("%1 %2").arg(curr, QString::number(res.costPerM3Analytical.sandCost, 'f', 2)),
               QStringLiteral("%1 %2").arg(curr, QString::number(res.costPerM3Graphical.sandCost, 'f', 2)),
               QStringLiteral("%1 %2").arg(curr, QString::number(std::abs(res.costPerM3Analytical.sandCost - res.costPerM3Graphical.sandCost), 'f', 2)));

    setRowComp(5, tr("Custo de Brita (%1/m³)").arg(curr),
               QStringLiteral("%1 %2").arg(curr, QString::number(res.costPerM3Analytical.gravelCost, 'f', 2)),
               QStringLiteral("%1 %2").arg(curr, QString::number(res.costPerM3Graphical.gravelCost, 'f', 2)),
               QStringLiteral("%1 %2").arg(curr, QString::number(std::abs(res.costPerM3Analytical.gravelCost - res.costPerM3Graphical.gravelCost), 'f', 2)));

    setRowComp(6, tr("Custo Total por m³ de Concreto"),
               QStringLiteral("%1 %2 / m³").arg(curr, QString::number(res.costPerM3Analytical.totalCost, 'f', 2)),
               QStringLiteral("%1 %2 / m³").arg(curr, QString::number(res.costPerM3Graphical.totalCost, 'f', 2)),
               QStringLiteral("%1 %2 / m³").arg(curr, QString::number(res.costDiffPerM3, 'f', 2)));

    setRowComp(7, tr("Custo Total da Obra (%1 m³)").arg(m_data->projectVolumeM3, 0, 'f', 1),
               QStringLiteral("%1 %2").arg(curr, QString::number(res.projectCostAnalytical.totalCost, 'f', 2)),
               QStringLiteral("%1 %2").arg(curr, QString::number(res.projectCostGraphical.totalCost, 'f', 2)),
               QStringLiteral("%1 %2").arg(curr, QString::number(res.projectCostDiff, 'f', 2)));

    setRowComp(8, tr("Consumo Total de Cimento na Obra"),
               QString::asprintf("%.0f kg (%.1f t)", res.projectTotalAnalytical.cementKg, res.projectTotalAnalytical.cementKg / 1000.0),
               QString::asprintf("%.0f kg (%.1f t)", res.projectTotalGraphical.cementKg, res.projectTotalGraphical.cementKg / 1000.0),
               QString::asprintf("%.0f kg", std::abs(res.projectTotalAnalytical.cementKg - res.projectTotalGraphical.cementKg)));

    setRowComp(9, tr("Total de Sacos de Cimento (50 kg)"),
               tr("%1 sacos").arg(res.totalCementBagsAnalytical),
               tr("%1 sacos").arg(res.totalCementBagsGraphical),
               tr("%1 sacos").arg(res.diffCementBags));

    // Cards inferiores
    m_lblCostDiffSummary->setText(QStringLiteral("%1 %2").arg(curr, QString::number(res.projectCostDiff, 'f', 2)));
    m_lblBagsDiffSummary->setText(tr("%1 sacos de 50 kg").arg(res.diffCementBags));
}

void TabCosts::retranslateUi() {
    if (m_groupInputs) {
        m_groupInputs->setTitle(tr("1. Parâmetros de Volume e Leituras do Ábaco Gráfico Manual"));
    }
    if (m_lblProjectVolume) {
        m_lblProjectVolume->setText(tr("<b>Volume Total da Obra (m³):</b>"));
    }
    if (m_lblManualAc) {
        m_lblManualAc->setText(tr("Leitura Gráfica Manual a/c:"));
    }
    if (m_lblManualM) {
        m_lblManualM->setText(tr("Leitura Gráfica Manual Traço m:"));
    }
    if (m_lblManualC) {
        m_lblManualC->setText(tr("Leitura Gráfica Manual Consumo C (kg/m³):"));
    }

    if (m_groupTable) {
        m_groupTable->setTitle(tr("2. Comparativo Detalhado de Custos e Consumos (Analítico x Gráfico)"));
    }
    if (m_tableComparison) {
        m_tableComparison->setHorizontalHeaderLabels({
            tr("Item de Comparação"),
            tr("Solução Analítica (Equações)"),
            tr("Leitura Manual (Ábaco)"),
            tr("Diferença (|Δ|)")
        });
    }

    if (m_groupDiff) {
        m_groupDiff->setTitle(tr("3. Impacto Financeiro da Precisão Analítica das Equações na Obra"));
    }
    if (m_lblCostDiffTitle) {
        m_lblCostDiffTitle->setText(tr("Diferença Total no Custo da Obra"));
    }
    if (m_lblBagsDiffTitle) {
        m_lblBagsDiffTitle->setText(tr("Diferença na Quantidade de Cimento (Sacos 50 kg)"));
    }

    refreshResults();
}

void TabCosts::changeEvent(QEvent *event) {
    if (event->type() == QEvent::LanguageChange) {
        retranslateUi();
    }
    QWidget::changeEvent(event);
}

} // namespace ConcreteMix
