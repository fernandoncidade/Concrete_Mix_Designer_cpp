#include "TabFinalMix.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QScrollArea>
#include <QEvent>

namespace ConcreteMix {

TabFinalMix::TabFinalMix(ProjectData *data, QWidget *parent)
    : QWidget(parent), m_data(data)
{
    setupUI();
    refreshResults();
}

void TabFinalMix::setupUI() {
    auto scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    auto container = new QWidget(scrollArea);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto gridMain = new QGridLayout(container);
    gridMain->setContentsMargins(12, 12, 12, 12);
    gridMain->setSpacing(12);

    // 1. Grupo: Síntese do Traço Unitário de Dosagem (Canto superior esquerdo: 0, 0)
    m_groupSummary = new QGroupBox(tr("1. Síntese do Traço Unitário Final de Dosagem"), container);
    m_groupSummary->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto gridSummary = new QGridLayout(m_groupSummary);
    gridSummary->setSpacing(10);

    auto makeBadge = [](QLabel*& titleLbl, const QString& titleText, QLabel*& val, QWidget* parent) -> QWidget* {
        auto w = new QFrame(parent);
        w->setFrameShape(QFrame::StyledPanel);
        w->setFrameShadow(QFrame::Plain);
        auto l = new QVBoxLayout(w);
        l->setContentsMargins(8, 6, 8, 6);
        l->setSpacing(2);
        titleLbl = new QLabel(titleText, w);
        QFont fTitle = titleLbl->font();
        fTitle.setBold(true);
        titleLbl->setFont(fTitle);
        val = new QLabel("-", w);
        QFont fVal = val->font();
        fVal.setBold(true);
        fVal.setPointSize(fVal.pointSize() + 2);
        val->setFont(fVal);
        l->addWidget(titleLbl);
        l->addWidget(val);
        return w;
    };

    gridSummary->addWidget(makeBadge(m_lblDryRatioTitle, tr("Traço Unitário em Massa Seca (1 : a : p)"), m_lblDryRatio, m_groupSummary), 0, 0);
    gridSummary->addWidget(makeBadge(m_lblWetRatioTitle, tr("Traço em Massa Úmida (1 : a_um : p_um)"), m_lblWetRatio, m_groupSummary), 0, 1);
    gridSummary->addWidget(makeBadge(m_lblCementConsumptionTitle, tr("Consumo de Cimento por m³"), m_lblCementConsumption, m_groupSummary), 1, 0);
    gridSummary->addWidget(makeBadge(m_lblWaterCementRatioTitle, tr("Relação Água/Cimento (a/c)"), m_lblWaterCementRatio, m_groupSummary), 1, 1);
    gridSummary->addWidget(makeBadge(m_lblSolutionTypeTitle, tr("Resolução Matemática & Diagnóstico"), m_lblSolutionType, m_groupSummary), 2, 0, 1, 2);

    gridMain->addWidget(m_groupSummary, 0, 0);

    // 2. Grupo: Consumo de Materiais por 1 m³ de Concreto (Canto superior direito: 0, 1)
    m_groupPerM3 = new QGroupBox(tr("2. Consumo de Materiais por Metro Cúbico de Concreto (1 m³)"), container);
    m_groupPerM3->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto vM3 = new QVBoxLayout(m_groupPerM3);

    m_tablePerM3 = new QTableWidget(5, 5, m_groupPerM3);
    m_tablePerM3->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tablePerM3->setHorizontalHeaderLabels({
        tr("Material Constituinte"), tr("Massa Seca (kg)"), tr("Umidade h (%)"),
        tr("Massa Úmida Corrigida (kg)"), tr("Volume Úmido (m³ ou L)")
    });
    m_tablePerM3->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tablePerM3->verticalHeader()->setVisible(false);
    m_tablePerM3->setMinimumHeight(150);
    m_tablePerM3->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    vM3->addWidget(m_tablePerM3);

    gridMain->addWidget(m_groupPerM3, 0, 1);

    // 3. Grupo: Amassada de Laboratório (Betoneira) (Canto inferior esquerdo: 1, 0)
    m_groupLab = new QGroupBox(tr("3. Amassada de Laboratório para o Volume do Misturador (Betoneira)"), container);
    m_groupLab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto vLab = new QVBoxLayout(m_groupLab);

    auto hVol = new QHBoxLayout();
    m_lblMixerVolume = new QLabel(tr("Volume da Amassada / Betoneira:"), m_groupLab);
    hVol->addWidget(m_lblMixerVolume);
    m_spinMixerVolume = new QDoubleSpinBox(m_groupLab);
    m_spinMixerVolume->setRange(0.0, 500.0);
    m_spinMixerVolume->setValue(29.0);
    m_spinMixerVolume->setSuffix(tr(" Litros"));
    hVol->addWidget(m_spinMixerVolume);
    hVol->addStretch();
    vLab->addLayout(hVol);

    m_tableLabBatch = new QTableWidget(4, 3, m_groupLab);
    m_tableLabBatch->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableLabBatch->setHorizontalHeaderLabels({tr("Material"), tr("Massa Seca (kg)"), tr("Massa Efetiva a Pesar (kg)")});
    m_tableLabBatch->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tableLabBatch->verticalHeader()->setVisible(false);
    m_tableLabBatch->setMinimumHeight(130);
    m_tableLabBatch->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    vLab->addWidget(m_tableLabBatch);

    gridMain->addWidget(m_groupLab, 1, 0);

    // 4. Grupo: Dosagem em Canteiro de Obra (1 Saco de 50 kg de Cimento / Padiolas) (Canto inferior direito: 1, 1)
    m_groupJobsite = new QGroupBox(tr("4. Dosagem em Canteiro de Obra (1 Saco de 50 kg de Cimento - Padiolas)"), container);
    m_groupJobsite->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto vJobsite = new QVBoxLayout(m_groupJobsite);

    m_tableJobsiteBatch = new QTableWidget(4, 4, m_groupJobsite);
    m_tableJobsiteBatch->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableJobsiteBatch->setHorizontalHeaderLabels({
        tr("Componente"), tr("Massa Úmida (kg)"), tr("Volume Aparente (L)"), tr("Dimensionamento de Padiolas (35x45 cm)")
    });
    m_tableJobsiteBatch->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tableJobsiteBatch->verticalHeader()->setVisible(false);
    m_tableJobsiteBatch->setMinimumHeight(130);
    m_tableJobsiteBatch->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    vJobsite->addWidget(m_tableJobsiteBatch);

    m_lblPadiolaDetails = new QLabel(m_groupJobsite);
    m_lblPadiolaDetails->setWordWrap(true);
    vJobsite->addWidget(m_lblPadiolaDetails);

    gridMain->addWidget(m_groupJobsite, 1, 1);

    gridMain->setColumnStretch(0, 1);
    gridMain->setColumnStretch(1, 1);
    gridMain->setRowStretch(0, 1);
    gridMain->setRowStretch(1, 1);

    scrollArea->setWidget(container);
    auto rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->addWidget(scrollArea);

    connect(m_spinMixerVolume, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double val) {
        if (m_data) {
            m_data->labMixerVolumeL = val;
            m_data->updatePilotBatchFromMixerVolume();
        }
        refreshResults();
        emit finalMixChanged();
    });
}

void TabFinalMix::updateUIFromData() {
    if (!m_data) return;
    m_spinMixerVolume->blockSignals(true);
    m_spinMixerVolume->setValue(m_data->labMixerVolumeL);
    m_spinMixerVolume->blockSignals(false);
    refreshResults();
}

void TabFinalMix::refreshResults() {
    if (!m_data) return;

    auto res = m_data->calculate();

    if (res.targetCementConsumption <= 0.0) {
        m_lblDryRatio->setText("-");
        m_lblWetRatio->setText("-");
        m_lblCementConsumption->setText("-");
        m_lblWaterCementRatio->setText("-");
        m_lblSolutionType->setText("-");
        m_lblSolutionType->setStyleSheet("");
        m_lblPadiolaDetails->setText("-");
        m_tablePerM3->clearContents();
        m_tableLabBatch->clearContents();
        m_tableJobsiteBatch->clearContents();
        return;
    }

    // 1. Síntese dos traços
    if (m_data->useGravelBlend) {
        double p1 = res.gravelRatioP * (m_data->gravel1Percent / 100.0);
        double p2 = res.gravelRatioP * (m_data->gravel2Percent / 100.0);
        m_lblDryRatio->setText(QString::asprintf("1 : %.2f : %.2f (B1 %.2f + B2 %.2f)", res.sandRatioA, res.gravelRatioP, p1, p2));

        double a_um = res.sandRatioA * (1.0 + m_data->sand.moisturePercent / 100.0);
        double p1_um = p1 * (1.0 + m_data->gravel1.moisturePercent / 100.0);
        double p2_um = p2 * (1.0 + m_data->gravel2.moisturePercent / 100.0);
        m_lblWetRatio->setText(QString::asprintf("1 : %.2f : %.2f (B1 %.2f + B2 %.2f)", a_um, p1_um + p2_um, p1_um, p2_um));
    } else {
        m_lblDryRatio->setText(QString::asprintf("1 : %.2f : %.2f", res.sandRatioA, res.gravelRatioP));

        double a_um = res.sandRatioA * (1.0 + m_data->sand.moisturePercent / 100.0);
        double p_um = res.gravelRatioP * (1.0 + m_data->gravel1.moisturePercent / 100.0);
        m_lblWetRatio->setText(QString::asprintf("1 : %.2f : %.2f", a_um, p_um));
    }

    m_lblCementConsumption->setText(QString::asprintf("%.1f kg/m³", res.targetCementConsumption));
    m_lblWaterCementRatio->setText(QString::asprintf("%.3f ", res.targetWaterCement) + tr("(água = %1 L/m³)").arg(res.perM3Analytical.waterCorrectedL, 0, 'f', 1));

    QString solStr;
    QString colorStyle;
    if (res.waterCementSolutionType == SolutionType::AnalyticalExact) {
        solStr = tr("✔ Analítica Exata (Abrams)");
        colorStyle = "color: #28a745; font-weight: bold;";
    } else if (res.waterCementSolutionType == SolutionType::ExponentialFallback) {
        solStr = tr("⚠ Ajuste Exponencial");
        colorStyle = "color: #b8860b; font-weight: bold;";
    } else if (res.waterCementSolutionType == SolutionType::ExperimentalAverage) {
        solStr = tr("✖ Média dos Ensaios (Não Conforme)");
        colorStyle = "color: #dc3545; font-weight: bold;";
    } else {
        solStr = tr("✖ Sem Solução Válida");
        colorStyle = "color: #dc3545; font-weight: bold;";
    }

    if (res.isExtrapolated) {
        solStr += tr(" | ⚠ Extrapolação de fcj");
    }
    m_lblSolutionType->setText(solStr);
    m_lblSolutionType->setStyleSheet(colorStyle);

    // 2. Tabela por 1 m³
    auto setRowM3 = [this](int row, const QString& mat, double mSeca, double h, double mUmida, double vol, const QString& volUnit) {
        m_tablePerM3->setItem(row, 0, new QTableWidgetItem(mat));
        m_tablePerM3->setItem(row, 1, new QTableWidgetItem(QString::asprintf("%.2f kg", mSeca)));
        m_tablePerM3->setItem(row, 2, new QTableWidgetItem(h >= 0 ? QString::asprintf("%.1f %%", h) : "-"));
        m_tablePerM3->setItem(row, 3, new QTableWidgetItem(QString::asprintf("%.2f kg", mUmida)));
        m_tablePerM3->setItem(row, 4, new QTableWidgetItem(QString::asprintf("%.3f %s", vol, volUnit.toUtf8().constData())));
    };

    const double cementVolM3 = (m_data->cement.unitMass > 0.0) ? (res.perM3Analytical.cementKg / (m_data->cement.unitMass * 1000.0)) : 0.0;
    const double cementBagVolL = (m_data->cement.unitMass > 0.0) ? (50.0 / m_data->cement.unitMass) : 0.0;

    if (m_data->useGravelBlend) {
        m_tablePerM3->setRowCount(6);
        double b1Dry = res.perM3Analytical.gravel1DryKg;
        double b2Dry = res.perM3Analytical.gravel2DryKg;
        double b1Wet = res.perM3Analytical.gravel1WetKg;
        double b2Wet = res.perM3Analytical.gravel2WetKg;
        double b1Vol = res.perM3Analytical.gravel1VolumeM3;
        double b2Vol = res.perM3Analytical.gravel2VolumeM3;

        setRowM3(0, tr("Cimento Portland"), res.perM3Analytical.cementKg, -1, res.perM3Analytical.cementKg, cementVolM3, "m³");
        setRowM3(1, tr("Areia Natural Média"), res.perM3Analytical.sandDryKg, m_data->sand.moisturePercent, res.perM3Analytical.sandWetKg, res.perM3Analytical.sandVolumeM3, "m³");
        setRowM3(2, tr("Brita 1 (%1%)").arg(m_data->gravel1Percent, 0, 'f', 0), b1Dry, m_data->gravel1.moisturePercent, b1Wet, b1Vol, "m³");
        setRowM3(3, tr("Brita 2 (%1%)").arg(m_data->gravel2Percent, 0, 'f', 0), b2Dry, m_data->gravel2.moisturePercent, b2Wet, b2Vol, "m³");
        setRowM3(4, tr("Água de Amassamento"), res.perM3Analytical.waterTheoreticalL, -1, res.perM3Analytical.waterCorrectedL, res.perM3Analytical.waterCorrectedL, "L");
        setRowM3(5, tr("Total Concreto Fresco"), res.perM3Analytical.cementKg + res.perM3Analytical.sandDryKg + res.perM3Analytical.gravelDryKg + res.perM3Analytical.waterTheoreticalL, -1, res.perM3Analytical.totalMassKg, 1.0, "m³");
    } else {
        m_tablePerM3->setRowCount(5);
        setRowM3(0, tr("Cimento Portland"), res.perM3Analytical.cementKg, -1, res.perM3Analytical.cementKg, cementVolM3, "m³");
        setRowM3(1, tr("Areia Natural Média"), res.perM3Analytical.sandDryKg, m_data->sand.moisturePercent, res.perM3Analytical.sandWetKg, res.perM3Analytical.sandVolumeM3, "m³");
        setRowM3(2, tr("Agregado Graúdo (Brita)"), res.perM3Analytical.gravelDryKg, m_data->gravel1.moisturePercent, res.perM3Analytical.gravelWetKg, res.perM3Analytical.gravelVolumeM3, "m³");
        setRowM3(3, tr("Água de Amassamento"), res.perM3Analytical.waterTheoreticalL, -1, res.perM3Analytical.waterCorrectedL, res.perM3Analytical.waterCorrectedL, "L");
        setRowM3(4, tr("Total Concreto Fresco"), res.perM3Analytical.cementKg + res.perM3Analytical.sandDryKg + res.perM3Analytical.gravelDryKg + res.perM3Analytical.waterTheoreticalL, -1, res.perM3Analytical.totalMassKg, 1.0, "m³");
    }

    // 3. Amassada de Laboratório
    if (!m_spinMixerVolume->hasFocus()) {
        m_spinMixerVolume->blockSignals(true);
        m_spinMixerVolume->setValue(res.labMixerVolumeLiters);
        m_spinMixerVolume->blockSignals(false);
    }

    auto setRowLab = [this](int row, const QString& mat, double seca, double efetiva) {
        m_tableLabBatch->setItem(row, 0, new QTableWidgetItem(mat));
        m_tableLabBatch->setItem(row, 1, new QTableWidgetItem(QString::asprintf("%.3f kg", seca)));
        m_tableLabBatch->setItem(row, 2, new QTableWidgetItem(QString::asprintf("%.3f kg", efetiva)));
    };

    if (m_data->useGravelBlend) {
        m_tableLabBatch->setRowCount(5);
        double b1LabDry = res.labBatch.gravelDryKg * (m_data->gravel1Percent / 100.0);
        double b2LabDry = res.labBatch.gravelDryKg * (m_data->gravel2Percent / 100.0);
        double b1LabWet = b1LabDry * (1.0 + m_data->gravel1.moisturePercent / 100.0);
        double b2LabWet = b2LabDry * (1.0 + m_data->gravel2.moisturePercent / 100.0);

        setRowLab(0, tr("Cimento"), res.labBatch.cementKg, res.labBatch.cementKg);
        setRowLab(1, tr("Areia"), res.labBatch.sandDryKg, res.labBatch.sandWetKg);
        setRowLab(2, tr("Brita 1 (%1%)").arg(m_data->gravel1Percent, 0, 'f', 0), b1LabDry, b1LabWet);
        setRowLab(3, tr("Brita 2 (%1%)").arg(m_data->gravel2Percent, 0, 'f', 0), b2LabDry, b2LabWet);
        setRowLab(4, tr("Água Efetiva"), res.labBatch.waterTheoreticalL, res.labBatch.waterCorrectedL);
    } else {
        m_tableLabBatch->setRowCount(4);
        setRowLab(0, tr("Cimento"), res.labBatch.cementKg, res.labBatch.cementKg);
        setRowLab(1, tr("Areia"), res.labBatch.sandDryKg, res.labBatch.sandWetKg);
        setRowLab(2, tr("Brita"), res.labBatch.gravelDryKg, res.labBatch.gravelWetKg);
        setRowLab(3, tr("Água Efetiva"), res.labBatch.waterTheoreticalL, res.labBatch.waterCorrectedL);
    }

    // 4. Canteiro (50 kg)
    auto setRowJob = [this](int row, const QString& comp, double mUm, double volL, const QString& pad) {
        m_tableJobsiteBatch->setItem(row, 0, new QTableWidgetItem(comp));
        m_tableJobsiteBatch->setItem(row, 1, new QTableWidgetItem(QString::asprintf("%.2f kg", mUm)));
        m_tableJobsiteBatch->setItem(row, 2, new QTableWidgetItem(QString::asprintf("%.1f L", volL)));
        m_tableJobsiteBatch->setItem(row, 3, new QTableWidgetItem(pad));
    };

    if (m_data->useGravelBlend) {
        m_tableJobsiteBatch->setRowCount(5);
        double b1JobWet = res.jobsiteBatch50kg.gravelWetKg * (m_data->gravel1Percent / 100.0);
        double b2JobWet = res.jobsiteBatch50kg.gravelWetKg * (m_data->gravel2Percent / 100.0);
        double b1JobVol = (m_data->gravel1.unitMass > 0.0) ? (b1JobWet / m_data->gravel1.unitMass) : 0.0;
        double b2JobVol = (m_data->gravel2.unitMass > 0.0) ? (b2JobWet / m_data->gravel2.unitMass) : 0.0;

        int b1Count = (b1JobVol <= 40.0) ? 1 : 2;
        double b1H = ((b1JobVol / b1Count) / 15.75) * 10.0;
        int b2Count = (b2JobVol <= 40.0) ? 1 : 2;
        double b2H = ((b2JobVol / b2Count) / 15.75) * 10.0;

        setRowJob(0, tr("Cimento"), 50.0, cementBagVolL, tr("1 saco fechado (50 kg)"));
        setRowJob(1, tr("Areia Úmida"), res.jobsiteBatch50kg.sandWetKg, res.jobsiteBatch50kg.sandVolumeLiters,
                  tr("%1 padiola(s) de 35x45 cm com h = %2 cm").arg(res.jobsiteBatch50kg.sandPadiolaCount).arg(res.jobsiteBatch50kg.sandPadiolaHeightCm, 0, 'f', 1));
        setRowJob(2, tr("Brita 1 Úmida"), b1JobWet, b1JobVol,
                  tr("%1 padiola(s) de 35x45 cm com h = %2 cm").arg(b1Count).arg(b1H, 0, 'f', 1));
        setRowJob(3, tr("Brita 2 Úmida"), b2JobWet, b2JobVol,
                  tr("%1 padiola(s) de 35x45 cm com h = %2 cm").arg(b2Count).arg(b2H, 0, 'f', 1));
        setRowJob(4, tr("Água a Adicionar"), res.jobsiteBatch50kg.waterLiters, res.jobsiteBatch50kg.waterLiters,
                  tr("Adicionar exatamente %1 Litros").arg(res.jobsiteBatch50kg.waterLiters, 0, 'f', 1));
    } else {
        m_tableJobsiteBatch->setRowCount(4);
        setRowJob(0, tr("Cimento"), 50.0, cementBagVolL, tr("1 saco fechado (50 kg)"));
        setRowJob(1, tr("Areia Úmida"), res.jobsiteBatch50kg.sandWetKg, res.jobsiteBatch50kg.sandVolumeLiters,
                  tr("%1 padiola(s) de 35x45 cm com h = %2 cm").arg(res.jobsiteBatch50kg.sandPadiolaCount).arg(res.jobsiteBatch50kg.sandPadiolaHeightCm, 0, 'f', 1));
        setRowJob(2, tr("Brita Úmida"), res.jobsiteBatch50kg.gravelWetKg, res.jobsiteBatch50kg.gravelVolumeLiters,
                  tr("%1 padiolas de 35x45 cm com h = %2 cm").arg(res.jobsiteBatch50kg.gravelPadiolaCount).arg(res.jobsiteBatch50kg.gravelPadiolaHeightCm, 0, 'f', 1));
        setRowJob(3, tr("Água a Adicionar"), res.jobsiteBatch50kg.waterLiters, res.jobsiteBatch50kg.waterLiters,
                  tr("Adicionar exatamente %1 Litros").arg(res.jobsiteBatch50kg.waterLiters, 0, 'f', 1));
    }

    m_lblPadiolaDetails->setText(
        tr("<b>Nota para o encarregado da obra:</b> Caixa/padiola padrão de madeira com base interna de 35 cm x 45 cm (área = 15,75 dm²). "
           "A água a ser adicionada na betoneira já desconta a umidade natural da areia (%1%) e da brita (%2%).")
        .arg(m_data->sand.moisturePercent, 0, 'f', 1).arg(m_data->gravel1.moisturePercent, 0, 'f', 1)
    );
}

void TabFinalMix::retranslateUi() {
    if (m_groupSummary) {
        m_groupSummary->setTitle(tr("1. Síntese do Traço Unitário Final de Dosagem"));
    }
    if (m_lblDryRatioTitle) {
        m_lblDryRatioTitle->setText(tr("Traço Unitário em Massa Seca (1 : a : p)"));
    }
    if (m_lblWetRatioTitle) {
        m_lblWetRatioTitle->setText(tr("Traço em Massa Úmida (1 : a_um : p_um)"));
    }
    if (m_lblCementConsumptionTitle) {
        m_lblCementConsumptionTitle->setText(tr("Consumo de Cimento por m³"));
    }
    if (m_lblWaterCementRatioTitle) {
        m_lblWaterCementRatioTitle->setText(tr("Relação Água/Cimento (a/c)"));
    }
    if (m_lblSolutionTypeTitle) {
        m_lblSolutionTypeTitle->setText(tr("Resolução Matemática & Diagnóstico"));
    }

    if (m_groupPerM3) {
        m_groupPerM3->setTitle(tr("2. Consumo de Materiais por Metro Cúbico de Concreto (1 m³)"));
    }
    if (m_tablePerM3) {
        m_tablePerM3->setHorizontalHeaderLabels({
            tr("Material Constituinte"), tr("Massa Seca (kg)"), tr("Umidade h (%)"),
            tr("Massa Úmida Corrigida (kg)"), tr("Volume Úmido (m³ ou L)")
        });
    }

    if (m_groupLab) {
        m_groupLab->setTitle(tr("3. Amassada de Laboratório para o Volume do Misturador (Betoneira)"));
    }
    if (m_lblMixerVolume) {
        m_lblMixerVolume->setText(tr("Volume da Amassada / Betoneira:"));
    }
    if (m_spinMixerVolume) {
        m_spinMixerVolume->setSuffix(tr(" Litros"));
    }
    if (m_tableLabBatch) {
        m_tableLabBatch->setHorizontalHeaderLabels({tr("Material"), tr("Massa Seca (kg)"), tr("Massa Efetiva a Pesar (kg)")});
    }

    if (m_groupJobsite) {
        m_groupJobsite->setTitle(tr("4. Dosagem em Canteiro de Obra (1 Saco de 50 kg de Cimento - Padiolas)"));
    }
    if (m_tableJobsiteBatch) {
        m_tableJobsiteBatch->setHorizontalHeaderLabels({
            tr("Componente"), tr("Massa Úmida (kg)"), tr("Volume Aparente (L)"), tr("Dimensionamento de Padiolas (35x45 cm)")
        });
    }

    refreshResults();
}

void TabFinalMix::changeEvent(QEvent *event) {
    if (event->type() == QEvent::LanguageChange) {
        retranslateUi();
    }
    QWidget::changeEvent(event);
}

} // namespace ConcreteMix
