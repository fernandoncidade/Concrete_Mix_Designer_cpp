#include "TabMaterials.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QScrollArea>
#include <QEvent>

namespace ConcreteMix {

TabMaterials::TabMaterials(ProjectData *data, QWidget *parent)
    : QWidget(parent), m_data(data)
{
    setupUI();
    updateUIFromData();
}

void TabMaterials::setupUI() {
    auto scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    auto container = new QWidget(scrollArea);
    auto mainLayout = new QVBoxLayout(container);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(16);

    setupMaterialsForm(mainLayout);
    setupGravelBlendSection(mainLayout);

    scrollArea->setWidget(container);

    auto rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->addWidget(scrollArea);

    retranslateUi();
}

void TabMaterials::setupMaterialsForm(QVBoxLayout *layout) {
    m_groupMat = new QGroupBox(this);
    auto grid = new QGridLayout(m_groupMat);
    grid->setSpacing(10);

    // Cabeçalhos
    m_lblColMat = new QLabel(m_groupMat);
    m_lblColType = new QLabel(m_groupMat);
    m_lblColSpec = new QLabel(m_groupMat);
    m_lblColUnit = new QLabel(m_groupMat);
    m_lblColMoist = new QLabel(m_groupMat);
    m_lblColCost = new QLabel(m_groupMat);
    m_lblColCostUnit = new QLabel(m_groupMat);

    grid->addWidget(m_lblColMat, 0, 0);
    grid->addWidget(m_lblColType, 0, 1);
    grid->addWidget(m_lblColSpec, 0, 2);
    grid->addWidget(m_lblColUnit, 0, 3);
    grid->addWidget(m_lblColMoist, 0, 4);
    grid->addWidget(m_lblColCost, 0, 5);
    grid->addWidget(m_lblColCostUnit, 0, 6);

    // Cimento
    m_lblRowCement = new QLabel(m_groupMat);
    grid->addWidget(m_lblRowCement, 1, 0);
    m_comboCement = new QComboBox(m_groupMat);
    m_comboCement->setEditable(true);
    m_comboCement->setInsertPolicy(QComboBox::NoInsert);

    m_spinCementSpecific = new QDoubleSpinBox(m_groupMat);
    m_spinCementSpecific->setRange(0.0, 4.0);
    m_spinCementSpecific->setDecimals(3);
    m_spinCementSpecific->setValue(2.960);
    m_spinCementUnit = new QDoubleSpinBox(m_groupMat);
    m_spinCementUnit->setRange(0.0, 3.0);
    m_spinCementUnit->setDecimals(3);
    m_spinCementUnit->setValue(1.100);
    m_spinCementCost = new QDoubleSpinBox(m_groupMat);
    m_spinCementCost->setRange(0.0, 100000.0);
    m_spinCementCost->setDecimals(2);
    m_spinCementCost->setValue(38.0);

    m_comboCementCostUnit = new QComboBox(m_groupMat);
    m_comboCementCostUnit->addItem(tr("R$/sc (50 kg)"), static_cast<int>(PriceUnit::PerSack50kg));
    m_comboCementCostUnit->addItem(tr("R$/kg"), static_cast<int>(PriceUnit::PerKg));
    m_comboCementCostUnit->addItem(tr("R$/t"), static_cast<int>(PriceUnit::PerTon));

    grid->addWidget(m_comboCement, 1, 1);
    grid->addWidget(m_spinCementSpecific, 1, 2);
    grid->addWidget(m_spinCementUnit, 1, 3);
    grid->addWidget(new QLabel("-", m_groupMat), 1, 4);
    grid->addWidget(m_spinCementCost, 1, 5);
    grid->addWidget(m_comboCementCostUnit, 1, 6);

    // Areia
    m_lblRowSand = new QLabel(m_groupMat);
    grid->addWidget(m_lblRowSand, 2, 0);
    m_comboSand = new QComboBox(m_groupMat);
    m_comboSand->setEditable(true);
    m_comboSand->setInsertPolicy(QComboBox::NoInsert);

    m_spinSandSpecific = new QDoubleSpinBox(m_groupMat);
    m_spinSandSpecific->setRange(0.0, 4.0);
    m_spinSandSpecific->setDecimals(3);
    m_spinSandSpecific->setValue(2.620);
    m_spinSandUnit = new QDoubleSpinBox(m_groupMat);
    m_spinSandUnit->setRange(0.0, 3.0);
    m_spinSandUnit->setDecimals(3);
    m_spinSandUnit->setValue(1.480);
    m_spinSandMoisture = new QDoubleSpinBox(m_groupMat);
    m_spinSandMoisture->setRange(0.0, 25.0);
    m_spinSandMoisture->setDecimals(2);
    m_spinSandMoisture->setSuffix(" %");
    m_spinSandMoisture->setValue(3.0);
    m_spinSandCost = new QDoubleSpinBox(m_groupMat);
    m_spinSandCost->setRange(0.0, 100000.0);
    m_spinSandCost->setDecimals(2);
    m_spinSandCost->setValue(65.0);

    m_comboSandCostUnit = new QComboBox(m_groupMat);
    m_comboSandCostUnit->addItem(tr("R$/m³"), static_cast<int>(PriceUnit::PerM3));
    m_comboSandCostUnit->addItem(tr("R$/t"), static_cast<int>(PriceUnit::PerTon));
    m_comboSandCostUnit->addItem(tr("R$/kg"), static_cast<int>(PriceUnit::PerKg));

    grid->addWidget(m_comboSand, 2, 1);
    grid->addWidget(m_spinSandSpecific, 2, 2);
    grid->addWidget(m_spinSandUnit, 2, 3);
    grid->addWidget(m_spinSandMoisture, 2, 4);
    grid->addWidget(m_spinSandCost, 2, 5);
    grid->addWidget(m_comboSandCostUnit, 2, 6);

    // Brita 1
    m_lblRowGravel1 = new QLabel(m_groupMat);
    grid->addWidget(m_lblRowGravel1, 3, 0);
    m_comboGravel1 = new QComboBox(m_groupMat);
    m_comboGravel1->setEditable(true);
    m_comboGravel1->setInsertPolicy(QComboBox::NoInsert);

    m_spinGravel1Specific = new QDoubleSpinBox(m_groupMat);
    m_spinGravel1Specific->setRange(0.0, 4.0);
    m_spinGravel1Specific->setDecimals(3);
    m_spinGravel1Specific->setValue(2.700);
    m_spinGravel1Unit = new QDoubleSpinBox(m_groupMat);
    m_spinGravel1Unit->setRange(0.0, 3.0);
    m_spinGravel1Unit->setDecimals(3);
    m_spinGravel1Unit->setValue(1.450);
    m_spinGravel1Moisture = new QDoubleSpinBox(m_groupMat);
    m_spinGravel1Moisture->setRange(0.0, 25.0);
    m_spinGravel1Moisture->setDecimals(2);
    m_spinGravel1Moisture->setSuffix(" %");
    m_spinGravel1Moisture->setValue(1.5);
    m_spinGravel1Cost = new QDoubleSpinBox(m_groupMat);
    m_spinGravel1Cost->setRange(0.0, 100000.0);
    m_spinGravel1Cost->setDecimals(2);
    m_spinGravel1Cost->setValue(70.0);

    m_comboGravel1CostUnit = new QComboBox(m_groupMat);
    m_comboGravel1CostUnit->addItem(tr("R$/m³"), static_cast<int>(PriceUnit::PerM3));
    m_comboGravel1CostUnit->addItem(tr("R$/t"), static_cast<int>(PriceUnit::PerTon));
    m_comboGravel1CostUnit->addItem(tr("R$/kg"), static_cast<int>(PriceUnit::PerKg));

    grid->addWidget(m_comboGravel1, 3, 1);
    grid->addWidget(m_spinGravel1Specific, 3, 2);
    grid->addWidget(m_spinGravel1Unit, 3, 3);
    grid->addWidget(m_spinGravel1Moisture, 3, 4);
    grid->addWidget(m_spinGravel1Cost, 3, 5);
    grid->addWidget(m_comboGravel1CostUnit, 3, 6);

    // Brita 2
    m_lblRowGravel2 = new QLabel(m_groupMat);
    grid->addWidget(m_lblRowGravel2, 4, 0);
    m_comboGravel2 = new QComboBox(m_groupMat);
    m_comboGravel2->setEditable(true);
    m_comboGravel2->setInsertPolicy(QComboBox::NoInsert);

    m_spinGravel2Specific = new QDoubleSpinBox(m_groupMat);
    m_spinGravel2Specific->setRange(0.0, 4.0);
    m_spinGravel2Specific->setDecimals(3);
    m_spinGravel2Specific->setValue(2.700);
    m_spinGravel2Unit = new QDoubleSpinBox(m_groupMat);
    m_spinGravel2Unit->setRange(0.0, 3.0);
    m_spinGravel2Unit->setDecimals(3);
    m_spinGravel2Unit->setValue(1.550);
    m_spinGravel2Moisture = new QDoubleSpinBox(m_groupMat);
    m_spinGravel2Moisture->setRange(0.0, 25.0);
    m_spinGravel2Moisture->setDecimals(2);
    m_spinGravel2Moisture->setSuffix(" %");
    m_spinGravel2Moisture->setValue(1.5);
    m_spinGravel2Cost = new QDoubleSpinBox(m_groupMat);
    m_spinGravel2Cost->setRange(0.0, 100000.0);
    m_spinGravel2Cost->setDecimals(2);
    m_spinGravel2Cost->setValue(70.0);

    m_comboGravel2CostUnit = new QComboBox(m_groupMat);
    m_comboGravel2CostUnit->addItem(tr("R$/m³"), static_cast<int>(PriceUnit::PerM3));
    m_comboGravel2CostUnit->addItem(tr("R$/t"), static_cast<int>(PriceUnit::PerTon));
    m_comboGravel2CostUnit->addItem(tr("R$/kg"), static_cast<int>(PriceUnit::PerKg));

    grid->addWidget(m_comboGravel2, 4, 1);
    grid->addWidget(m_spinGravel2Specific, 4, 2);
    grid->addWidget(m_spinGravel2Unit, 4, 3);
    grid->addWidget(m_spinGravel2Moisture, 4, 4);
    grid->addWidget(m_spinGravel2Cost, 4, 5);
    grid->addWidget(m_comboGravel2CostUnit, 4, 6);

    layout->addWidget(m_groupMat);

    populateNormativeCombos();

    // Auto-save connections
    auto triggerChange = [this]() {
        updateDataFromUI();
        emit materialsChanged();
    };

    connect(m_comboCement, &QComboBox::currentTextChanged, this, triggerChange);
    connect(m_comboCement, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int idx) {
        if (idx < 0) return;
        QString code = m_comboCement->currentData().toString();
        auto rec = NormativeDatabase::instance().getCementByCode(code);
        if (!rec.code.isEmpty()) {
            m_spinCementSpecific->blockSignals(true);
            m_spinCementUnit->blockSignals(true);
            m_spinCementCost->blockSignals(true);
            m_spinCementSpecific->setValue(rec.specificMass);
            m_spinCementUnit->setValue(rec.unitMass);
            if (rec.defaultCostPerBag > 0) {
                m_spinCementCost->setValue(rec.defaultCostPerBag);
            }
            m_spinCementSpecific->blockSignals(false);
            m_spinCementUnit->blockSignals(false);
            m_spinCementCost->blockSignals(false);
        }
        updateDataFromUI();
        emit materialsChanged();
    });
    connect(m_spinCementSpecific, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, triggerChange);
    connect(m_spinCementUnit, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, triggerChange);
    connect(m_spinCementCost, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, triggerChange);
    connect(m_comboCementCostUnit, QOverload<int>::of(&QComboBox::currentIndexChanged), this, triggerChange);

    connect(m_comboSand, &QComboBox::currentTextChanged, this, triggerChange);
    connect(m_comboSand, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int idx) {
        if (idx < 0) return;
        QString code = m_comboSand->currentData().toString();
        auto rec = NormativeDatabase::instance().getAggregateByCode(code);
        if (!rec.code.isEmpty()) {
            m_spinSandSpecific->blockSignals(true);
            m_spinSandUnit->blockSignals(true);
            m_spinSandMoisture->blockSignals(true);
            m_spinSandCost->blockSignals(true);
            m_spinSandSpecific->setValue(rec.specificMass);
            m_spinSandUnit->setValue(rec.unitMass);
            m_spinSandMoisture->setValue(rec.defaultMoisture);
            if (rec.defaultCostPerM3 > 0) m_spinSandCost->setValue(rec.defaultCostPerM3);
            m_spinSandSpecific->blockSignals(false);
            m_spinSandUnit->blockSignals(false);
            m_spinSandMoisture->blockSignals(false);
            m_spinSandCost->blockSignals(false);
        }
        updateDataFromUI();
        emit materialsChanged();
    });
    connect(m_spinSandSpecific, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, triggerChange);
    connect(m_spinSandUnit, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, triggerChange);
    connect(m_spinSandMoisture, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, triggerChange);
    connect(m_spinSandCost, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, triggerChange);
    connect(m_comboSandCostUnit, QOverload<int>::of(&QComboBox::currentIndexChanged), this, triggerChange);

    connect(m_comboGravel1, &QComboBox::currentTextChanged, this, triggerChange);
    connect(m_comboGravel1, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int idx) {
        if (idx < 0) return;
        QString code = m_comboGravel1->currentData().toString();
        auto rec = NormativeDatabase::instance().getAggregateByCode(code);
        if (!rec.code.isEmpty()) {
            m_spinGravel1Specific->blockSignals(true);
            m_spinGravel1Unit->blockSignals(true);
            m_spinGravel1Moisture->blockSignals(true);
            m_spinGravel1Cost->blockSignals(true);
            m_spinGravel1Specific->setValue(rec.specificMass);
            m_spinGravel1Unit->setValue(rec.unitMass);
            m_spinGravel1Moisture->setValue(rec.defaultMoisture);
            if (rec.defaultCostPerM3 > 0) m_spinGravel1Cost->setValue(rec.defaultCostPerM3);
            if (m_data && rec.dmcMm > 0) m_data->gravel1DmcMm = rec.dmcMm;
            m_spinGravel1Specific->blockSignals(false);
            m_spinGravel1Unit->blockSignals(false);
            m_spinGravel1Moisture->blockSignals(false);
            m_spinGravel1Cost->blockSignals(false);
        }
        updateDataFromUI();
        emit materialsChanged();
    });
    connect(m_spinGravel1Specific, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, triggerChange);
    connect(m_spinGravel1Unit, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, triggerChange);
    connect(m_spinGravel1Moisture, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, triggerChange);
    connect(m_spinGravel1Cost, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, triggerChange);
    connect(m_comboGravel1CostUnit, QOverload<int>::of(&QComboBox::currentIndexChanged), this, triggerChange);

    connect(m_comboGravel2, &QComboBox::currentTextChanged, this, triggerChange);
    connect(m_comboGravel2, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int idx) {
        if (idx < 0) return;
        QString code = m_comboGravel2->currentData().toString();
        auto rec = NormativeDatabase::instance().getAggregateByCode(code);
        if (!rec.code.isEmpty()) {
            m_spinGravel2Specific->blockSignals(true);
            m_spinGravel2Unit->blockSignals(true);
            m_spinGravel2Moisture->blockSignals(true);
            m_spinGravel2Cost->blockSignals(true);
            m_spinGravel2Specific->setValue(rec.specificMass);
            m_spinGravel2Unit->setValue(rec.unitMass);
            m_spinGravel2Moisture->setValue(rec.defaultMoisture);
            if (rec.defaultCostPerM3 > 0) m_spinGravel2Cost->setValue(rec.defaultCostPerM3);
            if (m_data && rec.dmcMm > 0) m_data->gravel2DmcMm = rec.dmcMm;
            m_spinGravel2Specific->blockSignals(false);
            m_spinGravel2Unit->blockSignals(false);
            m_spinGravel2Moisture->blockSignals(false);
            m_spinGravel2Cost->blockSignals(false);
        }
        updateDataFromUI();
        emit materialsChanged();
    });
    connect(m_spinGravel2Specific, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, triggerChange);
    connect(m_spinGravel2Unit, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, triggerChange);
    connect(m_spinGravel2Moisture, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, triggerChange);
    connect(m_spinGravel2Cost, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, triggerChange);
    connect(m_comboGravel2CostUnit, QOverload<int>::of(&QComboBox::currentIndexChanged), this, triggerChange);
}

void TabMaterials::setupGravelBlendSection(QVBoxLayout *layout) {
    m_groupBlend = new QGroupBox(this);
    auto vLayout = new QVBoxLayout(m_groupBlend);
    vLayout->setSpacing(10);

    auto hTop = new QHBoxLayout();
    m_chkUseBlend = new QCheckBox(m_groupBlend);
    QFont fChk = m_chkUseBlend->font();
    fChk.setBold(true);
    m_chkUseBlend->setFont(fChk);

    hTop->addWidget(m_chkUseBlend);
    hTop->addStretch();

    m_lblBaseGravelMass = new QLabel(m_groupBlend);
    hTop->addWidget(m_lblBaseGravelMass);
    m_spinBaseGravelMass = new QDoubleSpinBox(m_groupBlend);
    m_spinBaseGravelMass->setRange(0.0, 100.0);
    m_spinBaseGravelMass->setValue(12.0);
    hTop->addWidget(m_spinBaseGravelMass);

    m_lblContainerVol = new QLabel(m_groupBlend);
    hTop->addWidget(m_lblContainerVol);
    m_spinContainerVol = new QDoubleSpinBox(m_groupBlend);
    m_spinContainerVol->setRange(1.0, 50.0);
    m_spinContainerVol->setValue(15.04);
    hTop->addWidget(m_spinContainerVol);

    vLayout->addLayout(hTop);

    m_tableBlend = new QTableWidget(6, 8, m_groupBlend);
    m_tableBlend->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableBlend->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tableBlend->verticalHeader()->setVisible(false);
    m_tableBlend->setMinimumHeight(180);
    m_tableBlend->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    vLayout->addWidget(m_tableBlend, 1);

    m_lblBlendResult = new QLabel(m_groupBlend);
    QFont fBlend = m_lblBlendResult->font();
    fBlend.setBold(true);
    fBlend.setPointSize(fBlend.pointSize() + 1);
    m_lblBlendResult->setFont(fBlend);
    m_lblBlendResult->setFrameShape(QFrame::StyledPanel);
    m_lblBlendResult->setFrameShadow(QFrame::Plain);
    m_lblBlendResult->setMargin(8);
    vLayout->addWidget(m_lblBlendResult);

    m_groupBlend->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(m_groupBlend, 1);

    connect(m_chkUseBlend, &QCheckBox::toggled, this, [this](bool checked) {
        if (m_data) m_data->useGravelBlend = checked;
        emit materialsChanged();
    });

    connect(m_spinBaseGravelMass, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double val) {
        if (m_data) m_data->baseGravelMassKg = val;
        updateBlendTable();
        emit materialsChanged();
    });
    connect(m_spinContainerVol, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double val) {
        if (m_data) m_data->containerVolumeL = val;
        updateBlendTable();
        emit materialsChanged();
    });

    updateBlendTable();
}

void TabMaterials::updateBlendTable() {
    double baseMass = m_spinBaseGravelMass->value();
    double vol = m_spinContainerVol->value();

    std::vector<double> netMasses = {24.2, 24.5, 24.8, 25.4, 25.4, 25.4};
    auto blendRows = AggregateOptimizer::calculateBlendTable(baseMass, netMasses, vol);

    for (int r = 0; r < static_cast<int>(blendRows.size()); ++r) {
        const auto& row = blendRows[r];
        m_tableBlend->setItem(r, 0, new QTableWidgetItem(QString::asprintf("%.0f %%", row.percentCoarse1)));
        m_tableBlend->setItem(r, 1, new QTableWidgetItem(QString::asprintf("%.0f %%", row.percentCoarse2)));
        m_tableBlend->setItem(r, 2, new QTableWidgetItem(QString::asprintf("%.2f", row.weightCoarse1)));
        m_tableBlend->setItem(r, 3, new QTableWidgetItem(QString::asprintf("%.2f", row.weightCoarse2)));
        m_tableBlend->setItem(r, 4, new QTableWidgetItem(r == 0 ? "-" : QString::asprintf("%.2f", row.incrementalCoarse2)));
        m_tableBlend->setItem(r, 5, new QTableWidgetItem(QString::asprintf("%.2f", row.totalMass)));
        m_tableBlend->setItem(r, 6, new QTableWidgetItem(QString::asprintf("%.2f", row.netMassInContainer)));

        QString muStr = QString::asprintf("%.3f", row.compactedUnitWeight);
        if (row.isOptimum) muStr += tr(" ★ Ótimo");
        auto itemMu = new QTableWidgetItem(muStr);
        if (row.isOptimum) {
            const bool isDark = (palette().color(QPalette::Window).lightness() < 128);
            const QColor selBg = isDark ? QColor(30, 70, 32) : QColor(212, 237, 218);
            const QColor selFg = isDark ? QColor(144, 238, 144) : QColor(21, 87, 36);
            itemMu->setBackground(selBg);
            itemMu->setForeground(selFg);
            itemMu->setFont(QFont("Segoe UI", 9, QFont::Bold));
        }
        m_tableBlend->setItem(r, 7, itemMu);
    }

    auto opt = AggregateOptimizer::getOptimumProportions(blendRows);
    m_lblBlendResult->setText(tr("Proporção Ótima Determinada: %1% Brita 2 / %2% Brita 1 (Máximo empacotamento granular / menor consumo de pasta).")
                             .arg(opt.first, 0, 'f', 0).arg(opt.second, 0, 'f', 0));

    if (m_data) {
        m_data->gravel2Percent = opt.first;
        m_data->gravel1Percent = opt.second;
    }
}

void TabMaterials::populateNormativeCombos() {
    auto& db = NormativeDatabase::instance();

    // 1. Cimentos (NBR 16697)
    if (m_comboCement) {
        m_comboCement->blockSignals(true);
        m_comboCement->clear();
        auto cements = db.getAllCementTypes();
        for (const auto& c : cements) {
            m_comboCement->addItem(QString("%1 - %2").arg(c.code, c.name), c.code);
        }
        m_comboCement->blockSignals(false);
    }

    // 2. Areia (NBR 7211)
    if (m_comboSand) {
        m_comboSand->blockSignals(true);
        m_comboSand->clear();
        auto aggs = db.getAllAggregates();
        for (const auto& a : aggs) {
            if (a.aggregateType == "MIUDO") {
                m_comboSand->addItem(a.commercialName, a.code);
            }
        }
        m_comboSand->blockSignals(false);
    }

    // 3. Britas 1 e 2 (NBR 7211)
    if (m_comboGravel1 && m_comboGravel2) {
        m_comboGravel1->blockSignals(true);
        m_comboGravel1->clear();
        m_comboGravel2->blockSignals(true);
        m_comboGravel2->clear();
        auto aggs = db.getAllAggregates();
        for (const auto& a : aggs) {
            if (a.aggregateType == "GRAUDO") {
                m_comboGravel1->addItem(a.commercialName, a.code);
                m_comboGravel2->addItem(a.commercialName, a.code);
            }
        }
        m_comboGravel1->blockSignals(false);
        m_comboGravel2->blockSignals(false);
    }
}

void TabMaterials::updateUIFromData() {
    if (!m_data) return;

    m_comboCement->blockSignals(true);
    QString cementName = QString::fromStdString(m_data->cement.name);
    int cIdx = m_comboCement->findText(cementName, Qt::MatchContains);
    if (cIdx >= 0) m_comboCement->setCurrentIndex(cIdx);
    else m_comboCement->setEditText(cementName);
    m_comboCement->blockSignals(false);

    m_spinCementSpecific->blockSignals(true);
    m_spinCementUnit->blockSignals(true);
    m_spinCementCost->blockSignals(true);
    m_comboCementCostUnit->blockSignals(true);
    m_spinCementSpecific->setValue(m_data->cement.specificMass);
    m_spinCementUnit->setValue(m_data->cement.unitMass);
    m_spinCementCost->setValue(m_data->cement.costPerUnit);
    int cUnitIdx = m_comboCementCostUnit->findData(static_cast<int>(m_data->cement.costUnit));
    if (cUnitIdx >= 0) m_comboCementCostUnit->setCurrentIndex(cUnitIdx);
    m_spinCementSpecific->blockSignals(false);
    m_spinCementUnit->blockSignals(false);
    m_spinCementCost->blockSignals(false);
    m_comboCementCostUnit->blockSignals(false);

    m_comboSand->blockSignals(true);
    QString sandName = QString::fromStdString(m_data->sand.name);
    int sIdx = m_comboSand->findText(sandName, Qt::MatchContains);
    if (sIdx >= 0) m_comboSand->setCurrentIndex(sIdx);
    else m_comboSand->setEditText(sandName);
    m_comboSand->blockSignals(false);

    m_spinSandSpecific->blockSignals(true);
    m_spinSandUnit->blockSignals(true);
    m_spinSandMoisture->blockSignals(true);
    m_spinSandCost->blockSignals(true);
    m_comboSandCostUnit->blockSignals(true);
    m_spinSandSpecific->setValue(m_data->sand.specificMass);
    m_spinSandUnit->setValue(m_data->sand.unitMass);
    m_spinSandMoisture->setValue(m_data->sand.moisturePercent);
    m_spinSandCost->setValue(m_data->sand.costPerUnit);
    int sUnitIdx = m_comboSandCostUnit->findData(static_cast<int>(m_data->sand.costUnit));
    if (sUnitIdx >= 0) m_comboSandCostUnit->setCurrentIndex(sUnitIdx);
    m_spinSandSpecific->blockSignals(false);
    m_spinSandUnit->blockSignals(false);
    m_spinSandMoisture->blockSignals(false);
    m_spinSandCost->blockSignals(false);
    m_comboSandCostUnit->blockSignals(false);

    m_comboGravel1->blockSignals(true);
    QString g1Name = QString::fromStdString(m_data->gravel1.name);
    int g1Idx = m_comboGravel1->findText(g1Name, Qt::MatchContains);
    if (g1Idx >= 0) m_comboGravel1->setCurrentIndex(g1Idx);
    else m_comboGravel1->setEditText(g1Name);
    m_comboGravel1->blockSignals(false);

    m_spinGravel1Specific->blockSignals(true);
    m_spinGravel1Unit->blockSignals(true);
    m_spinGravel1Moisture->blockSignals(true);
    m_spinGravel1Cost->blockSignals(true);
    m_comboGravel1CostUnit->blockSignals(true);
    m_spinGravel1Specific->setValue(m_data->gravel1.specificMass);
    m_spinGravel1Unit->setValue(m_data->gravel1.unitMass);
    m_spinGravel1Moisture->setValue(m_data->gravel1.moisturePercent);
    m_spinGravel1Cost->setValue(m_data->gravel1.costPerUnit);
    int g1UnitIdx = m_comboGravel1CostUnit->findData(static_cast<int>(m_data->gravel1.costUnit));
    if (g1UnitIdx >= 0) m_comboGravel1CostUnit->setCurrentIndex(g1UnitIdx);
    m_spinGravel1Specific->blockSignals(false);
    m_spinGravel1Unit->blockSignals(false);
    m_spinGravel1Moisture->blockSignals(false);
    m_spinGravel1Cost->blockSignals(false);
    m_comboGravel1CostUnit->blockSignals(false);

    m_comboGravel2->blockSignals(true);
    QString g2Name = QString::fromStdString(m_data->gravel2.name);
    int g2Idx = m_comboGravel2->findText(g2Name, Qt::MatchContains);
    if (g2Idx >= 0) m_comboGravel2->setCurrentIndex(g2Idx);
    else m_comboGravel2->setEditText(g2Name);
    m_comboGravel2->blockSignals(false);

    m_spinGravel2Specific->blockSignals(true);
    m_spinGravel2Unit->blockSignals(true);
    m_spinGravel2Moisture->blockSignals(true);
    m_spinGravel2Cost->blockSignals(true);
    m_comboGravel2CostUnit->blockSignals(true);
    m_spinGravel2Specific->setValue(m_data->gravel2.specificMass);
    m_spinGravel2Unit->setValue(m_data->gravel2.unitMass);
    m_spinGravel2Moisture->setValue(m_data->gravel2.moisturePercent);
    m_spinGravel2Cost->setValue(m_data->gravel2.costPerUnit);
    int g2UnitIdx = m_comboGravel2CostUnit->findData(static_cast<int>(m_data->gravel2.costUnit));
    if (g2UnitIdx >= 0) m_comboGravel2CostUnit->setCurrentIndex(g2UnitIdx);
    m_spinGravel2Specific->blockSignals(false);
    m_spinGravel2Unit->blockSignals(false);
    m_spinGravel2Moisture->blockSignals(false);
    m_spinGravel2Cost->blockSignals(false);
    m_comboGravel2CostUnit->blockSignals(false);

    m_chkUseBlend->blockSignals(true);
    m_chkUseBlend->setChecked(m_data->useGravelBlend);
    m_chkUseBlend->blockSignals(false);

    m_spinBaseGravelMass->blockSignals(true);
    m_spinContainerVol->blockSignals(true);
    m_spinBaseGravelMass->setValue(m_data->baseGravelMassKg);
    m_spinContainerVol->setValue(m_data->containerVolumeL);
    m_spinBaseGravelMass->blockSignals(false);
    m_spinContainerVol->blockSignals(false);

    updateBlendTable();
}

void TabMaterials::updateDataFromUI() {
    if (!m_data) return;

    m_data->cement.name = m_comboCement->currentText().toStdString();
    m_data->cement.specificMass = m_spinCementSpecific->value();
    m_data->cement.unitMass = m_spinCementUnit->value();
    m_data->cement.costPerUnit = m_spinCementCost->value();
    m_data->cement.costUnit = static_cast<PriceUnit>(m_comboCementCostUnit->currentData().toInt());

    m_data->sand.name = m_comboSand->currentText().toStdString();
    m_data->sand.specificMass = m_spinSandSpecific->value();
    m_data->sand.unitMass = m_spinSandUnit->value();
    m_data->sand.moisturePercent = m_spinSandMoisture->value();
    m_data->sand.costPerUnit = m_spinSandCost->value();
    m_data->sand.costUnit = static_cast<PriceUnit>(m_comboSandCostUnit->currentData().toInt());

    m_data->gravel1.name = m_comboGravel1->currentText().toStdString();
    m_data->gravel1.specificMass = m_spinGravel1Specific->value();
    m_data->gravel1.unitMass = m_spinGravel1Unit->value();
    m_data->gravel1.moisturePercent = m_spinGravel1Moisture->value();
    m_data->gravel1.costPerUnit = m_spinGravel1Cost->value();
    m_data->gravel1.costUnit = static_cast<PriceUnit>(m_comboGravel1CostUnit->currentData().toInt());

    m_data->gravel2.name = m_comboGravel2->currentText().toStdString();
    m_data->gravel2.specificMass = m_spinGravel2Specific->value();
    m_data->gravel2.unitMass = m_spinGravel2Unit->value();
    m_data->gravel2.moisturePercent = m_spinGravel2Moisture->value();
    m_data->gravel2.costPerUnit = m_spinGravel2Cost->value();
    m_data->gravel2.costUnit = static_cast<PriceUnit>(m_comboGravel2CostUnit->currentData().toInt());

    m_data->useGravelBlend = m_chkUseBlend->isChecked();
    m_data->baseGravelMassKg = m_spinBaseGravelMass->value();
    m_data->containerVolumeL = m_spinContainerVol->value();
}

void TabMaterials::changeEvent(QEvent *event) {
    if (event->type() == QEvent::LanguageChange) {
        retranslateUi();
    } else if (event->type() == QEvent::PaletteChange || event->type() == QEvent::ApplicationPaletteChange) {
        updateBlendTable();
    }
    QWidget::changeEvent(event);
}

void TabMaterials::retranslateUi() {
    if (m_groupMat) m_groupMat->setTitle(tr("1. Caracterização Físico-Mecânica dos Materiais (NBR 7211:2022)"));
    if (m_groupBlend) m_groupBlend->setTitle(tr("2. Ensaio de Otimização da Mistura de Britas (B2/B1 ou B1/B0 - NBR 7211 / NM 45)"));

    if (m_lblColMat) m_lblColMat->setText(tr("<b>Material</b>"));
    if (m_lblColType) m_lblColType->setText(tr("<b>Identificação / Tipo</b>"));
    if (m_lblColSpec) m_lblColSpec->setText(tr("<b>Massa Específica Real (kg/dm³)</b>"));
    if (m_lblColUnit) m_lblColUnit->setText(tr("<b>Massa Unitária Aparente (kg/dm³)</b>"));
    if (m_lblColMoist) m_lblColMoist->setText(tr("<b>Umidade de Campo h (%)</b>"));
    if (m_lblColCost) m_lblColCost->setText(tr("<b>Preço Unitário</b>"));
    if (m_lblColCostUnit) m_lblColCostUnit->setText(tr("<b>Unidade Base</b>"));

    if (m_lblRowCement) m_lblRowCement->setText(tr("Cimento Portland:"));
    if (m_lblRowSand) m_lblRowSand->setText(tr("Agregado Miúdo (Areia):"));
    if (m_lblRowGravel1) m_lblRowGravel1->setText(tr("Agregado Graúdo 1 (Brita 1):"));
    if (m_lblRowGravel2) m_lblRowGravel2->setText(tr("Agregado Graúdo 2 (Brita 2):"));

    if (m_chkUseBlend) m_chkUseBlend->setText(tr("Ativar mistura binária de agregados graúdos no traço final"));
    if (m_lblBaseGravelMass) m_lblBaseGravelMass->setText(tr("Massa Base B2 (kg):"));
    if (m_lblContainerVol) m_lblContainerVol->setText(tr("Vol. Recipiente (L):"));

    if (m_tableBlend) {
        m_tableBlend->setHorizontalHeaderLabels({
            tr("Brita 2 (%)"), tr("Brita 1 (%)"), tr("Massa Brita 2 (kg)"),
            tr("Massa Brita 1 (kg)"), tr("Acréscimo B1 (kg)"), tr("Massa Total (kg)"),
            tr("Massa Líquida Recipiente (kg)"), tr("M.U. Compactada (kg/dm³)")
        });
    }

    if (m_comboCementCostUnit) {
        int idx = m_comboCementCostUnit->currentIndex();
        m_comboCementCostUnit->blockSignals(true);
        m_comboCementCostUnit->setItemText(0, tr("R$/sc (50 kg)"));
        m_comboCementCostUnit->setItemText(1, tr("R$/kg"));
        m_comboCementCostUnit->setItemText(2, tr("R$/t"));
        m_comboCementCostUnit->setCurrentIndex(idx);
        m_comboCementCostUnit->blockSignals(false);
    }
    auto updateAggCombo = [this](QComboBox* combo) {
        if (!combo) return;
        int idx = combo->currentIndex();
        combo->blockSignals(true);
        combo->setItemText(0, tr("R$/m³"));
        combo->setItemText(1, tr("R$/t"));
        combo->setItemText(2, tr("R$/kg"));
        combo->setCurrentIndex(idx);
        combo->blockSignals(false);
    };
    updateAggCombo(m_comboSandCostUnit);
    updateAggCombo(m_comboGravel1CostUnit);
    updateAggCombo(m_comboGravel2CostUnit);

    updateBlendTable();
}

} // namespace ConcreteMix
