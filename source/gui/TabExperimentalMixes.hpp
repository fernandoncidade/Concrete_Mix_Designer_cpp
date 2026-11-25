#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include "../core/ProjectData.hpp"
#include "QuadChartWidget.hpp"

namespace ConcreteMix {

class TabExperimentalMixes : public QWidget {
    Q_OBJECT

public:
    explicit TabExperimentalMixes(ProjectData *data, QWidget *parent = nullptr);

    void updateUIFromData();
    void updateDataFromUI();
    void calculateAndRefresh(bool emitSignal = true);
    void retranslateUi();
    void updateTheme(bool isDark);

signals:
    void experimentalMixesChanged();

protected:
    void changeEvent(QEvent *event) override;

private:
    void setupUI();

    ProjectData *m_data = nullptr;

    QGroupBox *m_groupInput = nullptr;
    QTableWidget *m_tableMixes = nullptr;
    QPushButton *m_btnRecalculate = nullptr;

    QGroupBox *m_groupResults = nullptr;
    QLabel *m_lblAbramsTitle = nullptr;
    QLabel *m_lblAbramsEq = nullptr;

    QLabel *m_lblLyseTitle = nullptr;
    QLabel *m_lblLyseEq = nullptr;

    QLabel *m_lblMolinariTitle = nullptr;
    QLabel *m_lblMolinariEq = nullptr;

    QLabel *m_lblBadgeAcTitle = nullptr;
    QLabel *m_lblOptimalAc = nullptr;

    QLabel *m_lblBadgeMTitle = nullptr;
    QLabel *m_lblOptimalM = nullptr;

    QLabel *m_lblBadgeCTitle = nullptr;
    QLabel *m_lblOptimalC = nullptr;

    QuadChartWidget *m_quadCharts = nullptr;
    class InputCellDelegate *m_cellDelegate = nullptr;
};

} // namespace ConcreteMix
