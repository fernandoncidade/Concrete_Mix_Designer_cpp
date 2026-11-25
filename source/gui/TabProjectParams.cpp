#include "TabProjectParams.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QScrollArea>
#include <QEvent>

namespace ConcreteMix {

TabProjectParams::TabProjectParams(ProjectData *data, QWidget *parent)
    : QWidget(parent), m_data(data)
{
    setupUI();
    updateUIFromData();
}

void TabProjectParams::setupUI() {
    auto scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    auto container = new QWidget(scrollArea);
    auto mainLayout = new QVBoxLayout(container);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(16);

    // 1. Grupo: Resistência de Projeto e Condição de Preparo
    m_groupStrength = new QGroupBox(container);
    auto formStrength = new QFormLayout(m_groupStrength);
    formStrength->setSpacing(12);

    m_spinFck = new QDoubleSpinBox(m_groupStrength);
    m_spinFck->setRange(0.0, 100.0);
    m_spinFck->setDecimals(1);
    m_spinFck->setSuffix(" MPa");
    m_spinFck->setValue(30.0);
    m_lblFckTitle = new QLabel(this);
    formStrength->addRow(m_lblFckTitle, m_spinFck);

    m_comboCondition = new QComboBox(m_groupStrength);
    m_comboCondition->addItem(QString(), static_cast<int>(PreparationCondition::ConditionA));
    m_comboCondition->addItem(QString(), static_cast<int>(PreparationCondition::ConditionB));
    m_comboCondition->addItem(QString(), static_cast<int>(PreparationCondition::ConditionC));
    m_comboCondition->addItem(QString(), static_cast<int>(PreparationCondition::KnownSd));
    m_comboCondition->addItem(QString(), static_cast<int>(PreparationCondition::Arbitrary));
    m_lblConditionTitle = new QLabel(this);
    formStrength->addRow(m_lblConditionTitle, m_comboCondition);

    m_spinSd = new QDoubleSpinBox(m_groupStrength);
    m_spinSd->setRange(0.0, 15.0);
    m_spinSd->setDecimals(2);
    m_spinSd->setSuffix(" MPa");
    m_spinSd->setValue(3.5);
    m_lblSdTitle = new QLabel(this);
    formStrength->addRow(m_lblSdTitle, m_spinSd);

    m_lblFcj = new QLabel(m_groupStrength);
    QFont fFcj = m_lblFcj->font();
    fFcj.setBold(true);
    fFcj.setPointSize(fFcj.pointSize() + 3);
    m_lblFcj->setFont(fFcj);
    m_lblFcj->setMargin(4);
    m_lblFcjTitle = new QLabel(this);
    formStrength->addRow(m_lblFcjTitle, m_lblFcj);

    mainLayout->addWidget(m_groupStrength);

    // 2. Grupo: Durabilidade e Meio Ambiente (NBR 6118:2023 / NBR 12655:2022)
    m_groupDurability = new QGroupBox(container);
    auto formDurability = new QFormLayout(m_groupDurability);
    formDurability->setSpacing(12);

    m_comboEnvClass = new QComboBox(m_groupDurability);
    m_comboEnvClass->addItem(QString(), static_cast<int>(EnvironmentalClass::CAA_I));
    m_comboEnvClass->addItem(QString(), static_cast<int>(EnvironmentalClass::CAA_II));
    m_comboEnvClass->addItem(QString(), static_cast<int>(EnvironmentalClass::CAA_III));
    m_comboEnvClass->addItem(QString(), static_cast<int>(EnvironmentalClass::CAA_IV));
    m_lblEnvClassTitle = new QLabel(this);
    formDurability->addRow(m_lblEnvClassTitle, m_comboEnvClass);

    m_comboStructType = new QComboBox(m_groupDurability);
    m_comboStructType->addItem(QString(), static_cast<int>(StructuralType::ReinforcedConcrete));
    m_comboStructType->addItem(QString(), static_cast<int>(StructuralType::PrestressedConcrete));
    m_lblStructTypeTitle = new QLabel(this);
    formDurability->addRow(m_lblStructTypeTitle, m_comboStructType);

    m_spinSlump = new QDoubleSpinBox(m_groupDurability);
    m_spinSlump->setRange(0.0, 300.0);
    m_spinSlump->setDecimals(0);
    m_spinSlump->setSuffix(" mm");
    m_spinSlump->setValue(80.0);
    m_lblSlumpTitle = new QLabel(this);
    formDurability->addRow(m_lblSlumpTitle, m_spinSlump);

    m_lblSlumpClass = new QLabel(m_groupDurability);
    QFont fSlump = m_lblSlumpClass->font();
    fSlump.setBold(true);
    m_lblSlumpClass->setFont(fSlump);
    m_lblSlumpClassTitle = new QLabel(this);
    formDurability->addRow(m_lblSlumpClassTitle, m_lblSlumpClass);

    mainLayout->addWidget(m_groupDurability);

    // 3. Grupo: Limitações Normativas Obrigatórias
    m_groupCards = new QGroupBox(container);
    auto gridCards = new QGridLayout(m_groupCards);
    gridCards->setSpacing(12);

    auto makeCard = [](QLabel*& titleLabel, QLabel*& valLabel) -> QWidget* {
        auto w = new QFrame();
        w->setFrameShape(QFrame::StyledPanel);
        w->setFrameShadow(QFrame::Plain);
        auto l = new QVBoxLayout(w);
        l->setContentsMargins(8, 8, 8, 8);
        titleLabel = new QLabel(w);
        QFont fTitle = titleLabel->font();
        fTitle.setBold(true);
        titleLabel->setFont(fTitle);
        valLabel = new QLabel("-", w);
        QFont fVal = valLabel->font();
        fVal.setBold(true);
        fVal.setPointSize(fVal.pointSize() + 2);
        valLabel->setFont(fVal);
        l->addWidget(titleLabel);
        l->addWidget(valLabel);
        return w;
    };

    gridCards->addWidget(makeCard(m_lblMaxAcCardTitle, m_lblMaxAc), 0, 0);
    gridCards->addWidget(makeCard(m_lblMinClassCardTitle, m_lblMinClass), 0, 1);
    gridCards->addWidget(makeCard(m_lblMinCementCardTitle, m_lblMinCement), 0, 2);
    gridCards->addWidget(makeCard(m_lblNominalCoverCardTitle, m_lblNominalCover), 1, 0);
    gridCards->addWidget(makeCard(m_lblDmcCoverCardTitle, m_lblDmcCover), 1, 1, 1, 2);

    m_lblEnvDesc = new QLabel(m_groupCards);
    m_lblEnvDesc->setWordWrap(true);
    m_lblEnvDesc->setFrameShape(QFrame::StyledPanel);
    m_lblEnvDesc->setFrameShadow(QFrame::Plain);
    m_lblEnvDesc->setMargin(8);
    gridCards->addWidget(m_lblEnvDesc, 2, 0, 1, 3);

    m_lblTolerances = new QLabel(m_groupCards);
    m_lblTolerances->setWordWrap(true);
    m_lblTolerances->setFrameShape(QFrame::StyledPanel);
    m_lblTolerances->setFrameShadow(QFrame::Plain);
    m_lblTolerances->setMargin(8);
    gridCards->addWidget(m_lblTolerances, 3, 0, 1, 3);

    mainLayout->addWidget(m_groupCards);
    mainLayout->addStretch();

    scrollArea->setWidget(container);
    auto rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->addWidget(scrollArea);

    retranslateUi();

    // Conexões de sinais
    connect(m_spinFck, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this]() {
        updateDataFromUI();
        updateNormativeCards();
        emit parametersChanged();
    });

    connect(m_comboCondition, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int) {
        auto cond = static_cast<PreparationCondition>(m_comboCondition->currentData().toInt());
        m_spinSd->blockSignals(true);
        m_spinSd->setMinimum(cond == PreparationCondition::KnownSd ? 2.0 : 0.0);
        if (cond == PreparationCondition::ConditionA) m_spinSd->setValue(4.0);
        else if (cond == PreparationCondition::ConditionB) m_spinSd->setValue(5.5);
        else if (cond == PreparationCondition::ConditionC) m_spinSd->setValue(7.0);
        else if (cond == PreparationCondition::KnownSd && m_spinSd->value() < 2.0) m_spinSd->setValue(2.0);
        m_spinSd->blockSignals(false);
        m_spinSd->setEnabled(cond == PreparationCondition::KnownSd || cond == PreparationCondition::Arbitrary);
        updateDataFromUI();
        updateNormativeCards();
        emit parametersChanged();
    });

    connect(m_spinSd, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this]() {
        updateDataFromUI();
        updateNormativeCards();
        emit parametersChanged();
    });

    connect(m_comboEnvClass, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this]() {
        updateDataFromUI();
        updateNormativeCards();
        emit parametersChanged();
    });

    connect(m_comboStructType, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this]() {
        updateDataFromUI();
        updateNormativeCards();
        emit parametersChanged();
    });

    connect(m_spinSlump, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this]() {
        updateDataFromUI();
        updateNormativeCards();
        emit parametersChanged();
    });
}

void TabProjectParams::updateUIFromData() {
    if (!m_data) return;

    m_spinFck->blockSignals(true);
    m_spinFck->setValue(m_data->fck);
    m_spinFck->blockSignals(false);

    m_comboCondition->blockSignals(true);
    int condIdx = m_comboCondition->findData(static_cast<int>(m_data->prepCondition));
    if (condIdx >= 0) m_comboCondition->setCurrentIndex(condIdx);
    m_comboCondition->blockSignals(false);

    m_spinSd->blockSignals(true);
    m_spinSd->setMinimum(m_data->prepCondition == PreparationCondition::KnownSd ? 2.0 : 0.0);
    m_spinSd->setValue(m_data->customSd);
    m_spinSd->blockSignals(false);
    m_spinSd->setEnabled(m_data->prepCondition == PreparationCondition::KnownSd ||
                         m_data->prepCondition == PreparationCondition::Arbitrary);

    m_comboEnvClass->blockSignals(true);
    int envIdx = m_comboEnvClass->findData(static_cast<int>(m_data->envClass));
    if (envIdx >= 0) m_comboEnvClass->setCurrentIndex(envIdx);
    m_comboEnvClass->blockSignals(false);

    m_comboStructType->blockSignals(true);
    int structIdx = m_comboStructType->findData(static_cast<int>(m_data->structType));
    if (structIdx >= 0) m_comboStructType->setCurrentIndex(structIdx);
    m_comboStructType->blockSignals(false);

    m_spinSlump->blockSignals(true);
    m_spinSlump->setValue(m_data->targetSlumpMm);
    m_spinSlump->blockSignals(false);

    updateNormativeCards();
}

void TabProjectParams::updateDataFromUI() {
    if (!m_data) return;

    m_data->fck = m_spinFck->value();
    m_data->customSd = m_spinSd->value();
    m_data->prepCondition = static_cast<PreparationCondition>(m_comboCondition->currentData().toInt());
    m_data->envClass = static_cast<EnvironmentalClass>(m_comboEnvClass->currentData().toInt());
    m_data->structType = static_cast<StructuralType>(m_comboStructType->currentData().toInt());
    m_data->targetSlumpMm = m_spinSlump->value();
}

QString TabProjectParams::getSlumpClassNameTr(SlumpClass sc) {
    switch (sc) {
        case SlumpClass::S10:  return tr("S10 (10 <= A < 50 mm)");
        case SlumpClass::S50:  return tr("S50 (50 <= A < 100 mm)");
        case SlumpClass::S100: return tr("S100 (100 <= A < 160 mm)");
        case SlumpClass::S160: return tr("S160 (160 <= A < 220 mm)");
        case SlumpClass::S220: return tr("S220 (>= 220 mm)");
    }
    return tr("S100 (100 <= A < 160 mm)");
}

QString TabProjectParams::getSlumpApplicationTr(SlumpClass sc) {
    switch (sc) {
        case SlumpClass::S10:  return tr("Concreto extrusado, vibroprensado ou centrifugado.");
        case SlumpClass::S50:  return tr("Pavimentos de concreto e elementos de fundações diretas.");
        case SlumpClass::S100: return tr("Elementos estruturais usuais com lançamento convencional.");
        case SlumpClass::S160: return tr("Elementos estruturais esbeltos com concreto bombeado.");
        case SlumpClass::S220: return tr("Elementos com altíssima taxa de armadura ou auto-adensável.");
    }
    return QString();
}

QString TabProjectParams::getEnvClassDescriptionTr(EnvironmentalClass c) {
    switch (c) {
        case EnvironmentalClass::CAA_I:
            return tr("Ambiente rural ou submerso em água doce. Pouco agressivo às armaduras.");
        case EnvironmentalClass::CAA_II:
            return tr("Ambiente urbano comum. Agressividade média por carbonatação do concreto.");
        case EnvironmentalClass::CAA_III:
            return tr("Ambiente marinho ou industrial com névoa salina ou gases ácidos moderados.");
        case EnvironmentalClass::CAA_IV:
            return tr("Respingos de maré, indústrias químicas severas, tanques industriais e galvanoplastia.");
    }
    return QString();
}

QString TabProjectParams::getDeteriorationRiskTr(EnvironmentalClass c) {
    switch (c) {
        case EnvironmentalClass::CAA_I:   return tr("Insignificante");
        case EnvironmentalClass::CAA_II:  return tr("Pequeno");
        case EnvironmentalClass::CAA_III: return tr("Grande");
        case EnvironmentalClass::CAA_IV:  return tr("Elevado");
    }
    return tr("Insignificante");
}

void TabProjectParams::updateNormativeCards() {
    double fck = m_spinFck->value();
    auto cond = static_cast<PreparationCondition>(m_comboCondition->currentData().toInt());
    double sd = (cond == PreparationCondition::Arbitrary)
                    ? m_spinSd->value()
                    : ConcreteStandards::getStandardDeviation(cond, m_spinSd->value());
    if (fck <= 0.0) {
        m_lblFcj->setText("-");
        m_lblSlumpClass->setText("-");
        m_lblMaxAc->setText("-");
        m_lblMinClass->setText("-");
        m_lblMinCement->setText("-");
        m_lblNominalCover->setText("-");
        m_lblDmcCover->setText("-");
        m_lblDmcCover->setStyleSheet("");
        m_lblEnvDesc->setText("-");
        return;
    }
    double fcj = ConcreteStandards::calculateDosageStrength(fck, sd);
    m_lblFcj->setText(QString::asprintf("%.3f MPa", fcj));

    auto sc = ConcreteStandards::classifySlump(m_spinSlump->value());
    m_lblSlumpClass->setText(getSlumpClassNameTr(sc) + " - " + getSlumpApplicationTr(sc));

    auto env = static_cast<EnvironmentalClass>(m_comboEnvClass->currentData().toInt());
    auto st = static_cast<StructuralType>(m_comboStructType->currentData().toInt());
    auto req = ConcreteStandards::getDurabilityRequirement(env, st);

    m_lblMaxAc->setText(QString::asprintf("≤ %.2f", req.maxWaterCementRatio));
    m_lblMinClass->setText(QString::fromStdString(req.minConcreteClass) + QString::asprintf(" (≥ %.0f MPa)", req.minFck));
    m_lblMinCement->setText(QString::asprintf("≥ %.0f kg/m³", req.minCementConsumption));
    m_lblNominalCover->setText(QString::asprintf("%.0f mm / %.0f mm", req.nominalCoverSlabs, req.nominalCoverBeamsCols));

    QString dmcDiag;
    bool dmcOk = m_data ? m_data->checkDmcCoverCompliance(dmcDiag) : true;
    double maxDmc = m_data ? m_data->getMaxCoarseDmc() : 19.0;
    double maxAllowedBeam = 1.2 * req.nominalCoverBeamsCols;
    double maxAllowedSlab = 1.2 * req.nominalCoverSlabs;
    bool beamOk = (maxDmc <= maxAllowedBeam);
    bool slabOk = (maxDmc <= maxAllowedSlab);

    QString statusText = tr("Vigas: %1 | Lajes: %2")
        .arg(beamOk ? tr("OK (≤%1mm)").arg(maxAllowedBeam, 0, 'f', 0) : tr("NC (>%1mm)").arg(maxAllowedBeam, 0, 'f', 0))
        .arg(slabOk ? tr("OK (≤%1mm)").arg(maxAllowedSlab, 0, 'f', 0) : tr("NC (>%1mm)").arg(maxAllowedSlab, 0, 'f', 0));

    m_lblDmcCover->setText(statusText);
    m_lblDmcCover->setToolTip(dmcDiag);
    if (beamOk && slabOk) {
        m_lblDmcCover->setStyleSheet("color: #28a745; font-weight: bold;");
    } else if (beamOk) {
        m_lblDmcCover->setStyleSheet("color: #b8860b; font-weight: bold;");
    } else {
        m_lblDmcCover->setStyleSheet("color: #dc3545; font-weight: bold;");
    }

    m_lblEnvDesc->setText(getEnvClassDescriptionTr(env) + tr(" | Risco de deterioração: ") + getDeteriorationRiskTr(env));
}

void TabProjectParams::changeEvent(QEvent *event) {
    if (event->type() == QEvent::LanguageChange) {
        retranslateUi();
    }
    QWidget::changeEvent(event);
}

void TabProjectParams::retranslateUi() {
    if (m_groupStrength) m_groupStrength->setTitle(tr("1. Resistência Estrutural e Controle Tecnológico (NBR 12655:2022)"));
    if (m_groupDurability) m_groupDurability->setTitle(tr("2. Durabilidade e Agressividade Ambiental (NBR 6118 / NBR 12655)"));
    if (m_groupCards) m_groupCards->setTitle(tr("3. Limitações Normativas Obrigatórias de Durabilidade"));

    if (m_lblFckTitle) m_lblFckTitle->setText(tr("Resistência Característica (fck):"));
    if (m_lblConditionTitle) m_lblConditionTitle->setText(tr("Condição de Preparo da Obra:"));
    if (m_lblSdTitle) m_lblSdTitle->setText(tr("Desvio-Padrão da Dosagem (Sd):"));
    if (m_lblFcjTitle) m_lblFcjTitle->setText(tr("Resistência de Dosagem (fcj = fck + 1,65·Sd):"));

    if (m_lblEnvClassTitle) m_lblEnvClassTitle->setText(tr("Classe de Agressividade Ambiental (CAA):"));
    if (m_lblStructTypeTitle) m_lblStructTypeTitle->setText(tr("Tipo de Estrutura:"));
    if (m_lblSlumpTitle) m_lblSlumpTitle->setText(tr("Consistência / Abatimento Alvo (Slump):"));
    if (m_lblSlumpClassTitle) m_lblSlumpClassTitle->setText(tr("Classe de Consistência (NBR 8953):"));

    if (m_lblMaxAcCardTitle) m_lblMaxAcCardTitle->setText(tr("Relação Água/Cimento Máxima (a/c)"));
    if (m_lblMinClassCardTitle) m_lblMinClassCardTitle->setText(tr("Classe Mínima de Resistência"));
    if (m_lblMinCementCardTitle) m_lblMinCementCardTitle->setText(tr("Consumo Mínimo de Cimento"));
    if (m_lblNominalCoverCardTitle) m_lblNominalCoverCardTitle->setText(tr("Cobrimento Nominal (Lajes / Vigas)"));
    if (m_lblDmcCoverCardTitle) m_lblDmcCoverCardTitle->setText(tr("Verificação DMC vs Cobrimento (NBR 6118 - 7.4.7.6)"));

    if (m_comboCondition) {
        m_comboCondition->setItemText(0, tr("Condição A - Todas as classes (Sd = 4,0 MPa)"));
        m_comboCondition->setItemText(1, tr("Condição B - Classes C10 a C20 (Sd = 5,5 MPa)"));
        m_comboCondition->setItemText(2, tr("Condição C - Classes C10 e C15 (Sd = 7,0 MPa)"));
        m_comboCondition->setItemText(3, tr("Desvio Conhecido Histórico (Sd >= 2,0 MPa)"));
        m_comboCondition->setItemText(4, tr("Laboratório / Arbitrário (Personalizado)"));
    }

    if (m_comboEnvClass) {
        m_comboEnvClass->setItemText(0, tr("Classe I - Fraca (Rural / Submersa)"));
        m_comboEnvClass->setItemText(1, tr("Classe II - Moderada (Urbana)"));
        m_comboEnvClass->setItemText(2, tr("Classe III - Forte (Marinha / Industrial)"));
        m_comboEnvClass->setItemText(3, tr("Classe IV - Muito Forte (Respingos de maré / Química)"));
    }

    if (m_comboStructType) {
        m_comboStructType->setItemText(0, tr("Concreto Armado (CA)"));
        m_comboStructType->setItemText(1, tr("Concreto Protendido (CP)"));
    }

    if (m_lblTolerances) {
        m_lblTolerances->setText(tr("<b>ABNT NBR 7212:2025 (item 5.5) - Tolerâncias de Pesagem em Central:</b> Cimento e Adições: ±1% | Agregados: ±3% | Água: ±3% | Aditivos: ±5%"));
    }

    updateNormativeCards();
}

} // namespace ConcreteMix
