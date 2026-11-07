#pragma once

#include <QWidget>
#include <QLabel>
#include <QTableWidget>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include "../core/ProjectData.hpp"

namespace ConcreteMix {

class TabFinalMix : public QWidget {
    Q_OBJECT

public:
    explicit TabFinalMix(ProjectData *data, QWidget *parent = nullptr);

    void updateUIFromData();
    void refreshResults();
    void retranslateUi();

signals:
    void finalMixChanged();

protected:
    void changeEvent(QEvent *event) override;

private:
    void setupUI();

    ProjectData *m_data = nullptr;

    QGroupBox *m_groupSummary = nullptr;
    QLabel *m_lblDryRatioTitle = nullptr;
    QLabel *m_lblDryRatio = nullptr;
    QLabel *m_lblWetRatioTitle = nullptr;
    QLabel *m_lblWetRatio = nullptr;
    QLabel *m_lblCementConsumptionTitle = nullptr;
    QLabel *m_lblCementConsumption = nullptr;
    QLabel *m_lblWaterCementRatioTitle = nullptr;
    QLabel *m_lblWaterCementRatio = nullptr;
    QLabel *m_lblSolutionTypeTitle = nullptr;
    QLabel *m_lblSolutionType = nullptr;

    QGroupBox *m_groupPerM3 = nullptr;
    QTableWidget *m_tablePerM3 = nullptr;

    // Amassada para Betoneira
    QGroupBox *m_groupLab = nullptr;
    QLabel *m_lblMixerVolume = nullptr;
    QDoubleSpinBox *m_spinMixerVolume = nullptr;
    QTableWidget *m_tableLabBatch = nullptr;

    // Padiolas de Canteiro (50 kg cimento)
    QGroupBox *m_groupJobsite = nullptr;
    QTableWidget *m_tableJobsiteBatch = nullptr;
    QLabel *m_lblPadiolaDetails = nullptr;
};

} // namespace ConcreteMix
