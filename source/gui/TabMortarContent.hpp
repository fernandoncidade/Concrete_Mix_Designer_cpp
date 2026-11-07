#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QGroupBox>
#include "../core/ProjectData.hpp"
#include "../core/MortarContentOptimizer.hpp"

namespace ConcreteMix {

class TabMortarContent : public QWidget {
    Q_OBJECT

public:
    explicit TabMortarContent(ProjectData *data, QWidget *parent = nullptr);

    void updateUIFromData();
    void updateDataFromUI();

    void retranslateUi();

signals:
    void mortarContentChanged();

protected:
    void changeEvent(QEvent *event) override;

private:
    void setupUI();
    void updateTable();

    ProjectData *m_data = nullptr;

    QGroupBox *m_groupConfig = nullptr;
    QGroupBox *m_groupTable = nullptr;

    QLabel *m_lblPilotMTitle = nullptr;
    QLabel *m_lblBatchGravelTitle = nullptr;
    QLabel *m_lblBatchWaterTitle = nullptr;
    QLabel *m_lblSelectedAlphaTitle = nullptr;
    QLabel *m_lblHelp = nullptr;

    QDoubleSpinBox *m_spinPilotM = nullptr;
    QDoubleSpinBox *m_spinBatchGravel = nullptr;
    QDoubleSpinBox *m_spinBatchWater = nullptr;
    QDoubleSpinBox *m_spinSelectedAlpha = nullptr;

    QTableWidget *m_tableAlpha = nullptr;
    QLabel *m_lblSelectedSummary = nullptr;
};

} // namespace ConcreteMix
