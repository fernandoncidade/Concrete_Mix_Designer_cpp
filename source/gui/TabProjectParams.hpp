#pragma once

#include <QWidget>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include "../core/ProjectData.hpp"

namespace ConcreteMix {

class TabProjectParams : public QWidget {
    Q_OBJECT

public:
    explicit TabProjectParams(ProjectData *data, QWidget *parent = nullptr);

    void updateUIFromData();
    void updateDataFromUI();

    void retranslateUi();
    void updateNormativeCards();

signals:
    void parametersChanged();

protected:
    void changeEvent(QEvent *event) override;

private:
    void setupUI();

    QString getSlumpClassNameTr(SlumpClass sc);
    QString getSlumpApplicationTr(SlumpClass sc);
    QString getEnvClassDescriptionTr(EnvironmentalClass c);
    QString getDeteriorationRiskTr(EnvironmentalClass c);

    ProjectData *m_data = nullptr;

    QGroupBox *m_groupStrength = nullptr;
    QGroupBox *m_groupDurability = nullptr;
    QGroupBox *m_groupCards = nullptr;

    QLabel *m_lblFckTitle = nullptr;
    QLabel *m_lblConditionTitle = nullptr;
    QLabel *m_lblSdTitle = nullptr;
    QLabel *m_lblFcjTitle = nullptr;

    QLabel *m_lblEnvClassTitle = nullptr;
    QLabel *m_lblStructTypeTitle = nullptr;
    QLabel *m_lblSlumpTitle = nullptr;
    QLabel *m_lblSlumpClassTitle = nullptr;

    QLabel *m_lblMaxAcCardTitle = nullptr;
    QLabel *m_lblMinClassCardTitle = nullptr;
    QLabel *m_lblMinCementCardTitle = nullptr;
    QLabel *m_lblNominalCoverCardTitle = nullptr;
    QLabel *m_lblDmcCoverCardTitle = nullptr;

    QDoubleSpinBox *m_spinFck = nullptr;
    QComboBox *m_comboCondition = nullptr;
    QDoubleSpinBox *m_spinSd = nullptr;
    QLabel *m_lblFcj = nullptr;

    QComboBox *m_comboEnvClass = nullptr;
    QComboBox *m_comboStructType = nullptr;
    QDoubleSpinBox *m_spinSlump = nullptr;
    QLabel *m_lblSlumpClass = nullptr;

    // Cartões informativos de requisitos normativos
    QLabel *m_lblMaxAc = nullptr;
    QLabel *m_lblMinClass = nullptr;
    QLabel *m_lblMinCement = nullptr;
    QLabel *m_lblNominalCover = nullptr;
    QLabel *m_lblDmcCover = nullptr;
    QLabel *m_lblEnvDesc = nullptr;
    QLabel *m_lblTolerances = nullptr;
};

} // namespace ConcreteMix
