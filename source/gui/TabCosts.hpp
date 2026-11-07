#pragma once

#include <QWidget>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QTableWidget>
#include <QGroupBox>
#include "../core/ProjectData.hpp"

namespace ConcreteMix {

class TabCosts : public QWidget {
    Q_OBJECT

public:
    explicit TabCosts(ProjectData *data, QWidget *parent = nullptr);

    void updateUIFromData();
    void refreshResults();
    void retranslateUi();

signals:
    void costsChanged();

protected:
    void changeEvent(QEvent *event) override;

private:
    void setupUI();

    ProjectData *m_data = nullptr;

    QGroupBox *m_groupInputs = nullptr;
    QLabel *m_lblProjectVolume = nullptr;
    QDoubleSpinBox *m_spinProjectVolume = nullptr;

    QLabel *m_lblManualAc = nullptr;
    QDoubleSpinBox *m_spinManualAc = nullptr;

    QLabel *m_lblManualM = nullptr;
    QDoubleSpinBox *m_spinManualM = nullptr;

    QLabel *m_lblManualC = nullptr;
    QDoubleSpinBox *m_spinManualC = nullptr;

    // Tabela comparativa
    QGroupBox *m_groupTable = nullptr;
    QTableWidget *m_tableComparison = nullptr;

    QGroupBox *m_groupDiff = nullptr;
    QLabel *m_lblCostDiffTitle = nullptr;
    QLabel *m_lblCostDiffSummary = nullptr;
    QLabel *m_lblBagsDiffTitle = nullptr;
    QLabel *m_lblBagsDiffSummary = nullptr;
};

} // namespace ConcreteMix
