#pragma once

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QTableWidget>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>
#include "../core/ProjectData.hpp"
#include "../core/AggregateOptimizer.hpp"
#include "../core/NormativeDatabase.hpp"

namespace ConcreteMix {

class TabMaterials : public QWidget {
    Q_OBJECT

public:
    explicit TabMaterials(ProjectData *data, QWidget *parent = nullptr);

    void updateUIFromData();
    void updateDataFromUI();

    void retranslateUi();

signals:
    void materialsChanged();

protected:
    void changeEvent(QEvent *event) override;

private:
    void setupUI();
    void setupMaterialsForm(QVBoxLayout *layout);
    void setupGravelBlendSection(QVBoxLayout *layout);
    void updateBlendTable();
    void populateNormativeCombos();

    ProjectData *m_data = nullptr;

    QGroupBox *m_groupMat = nullptr;
    QGroupBox *m_groupBlend = nullptr;

    QLabel *m_lblColMat = nullptr;
    QLabel *m_lblColType = nullptr;
    QLabel *m_lblColSpec = nullptr;
    QLabel *m_lblColUnit = nullptr;
    QLabel *m_lblColMoist = nullptr;
    QLabel *m_lblColCost = nullptr;
    QLabel *m_lblColCostUnit = nullptr;

    QLabel *m_lblRowCement = nullptr;
    QLabel *m_lblRowSand = nullptr;
    QLabel *m_lblRowGravel1 = nullptr;
    QLabel *m_lblRowGravel2 = nullptr;

    QLabel *m_lblBaseGravelMass = nullptr;
    QLabel *m_lblContainerVol = nullptr;

    // Campos de Cimento
    QComboBox *m_comboCement = nullptr;
    QDoubleSpinBox *m_spinCementSpecific = nullptr;
    QDoubleSpinBox *m_spinCementUnit = nullptr;
    QDoubleSpinBox *m_spinCementCost = nullptr;
    QComboBox *m_comboCementCostUnit = nullptr;

    // Campos de Areia
    QComboBox *m_comboSand = nullptr;
    QDoubleSpinBox *m_spinSandSpecific = nullptr;
    QDoubleSpinBox *m_spinSandUnit = nullptr;
    QDoubleSpinBox *m_spinSandMoisture = nullptr;
    QDoubleSpinBox *m_spinSandCost = nullptr;
    QComboBox *m_comboSandCostUnit = nullptr;

    // Campos de Brita 1
    QComboBox *m_comboGravel1 = nullptr;
    QDoubleSpinBox *m_spinGravel1Specific = nullptr;
    QDoubleSpinBox *m_spinGravel1Unit = nullptr;
    QDoubleSpinBox *m_spinGravel1Moisture = nullptr;
    QDoubleSpinBox *m_spinGravel1Cost = nullptr;
    QComboBox *m_comboGravel1CostUnit = nullptr;

    // Campos de Brita 2
    QComboBox *m_comboGravel2 = nullptr;
    QDoubleSpinBox *m_spinGravel2Specific = nullptr;
    QDoubleSpinBox *m_spinGravel2Unit = nullptr;
    QDoubleSpinBox *m_spinGravel2Moisture = nullptr;
    QDoubleSpinBox *m_spinGravel2Cost = nullptr;
    QComboBox *m_comboGravel2CostUnit = nullptr;

    // Mistura de Britas
    QCheckBox *m_chkUseBlend = nullptr;
    QDoubleSpinBox *m_spinBaseGravelMass = nullptr;
    QDoubleSpinBox *m_spinContainerVol = nullptr;
    QTableWidget *m_tableBlend = nullptr;
    QLabel *m_lblBlendResult = nullptr;
};

} // namespace ConcreteMix
