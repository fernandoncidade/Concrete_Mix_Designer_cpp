#pragma once

#include <QWidget>
#include <QChartView>
#include <QChart>
#include <QLineSeries>
#include <QScatterSeries>
#include <QValueAxis>
#include <QGridLayout>
#include <QTabWidget>
#include <QPushButton>
#include "../core/MixDesignCalculator.hpp"

namespace ConcreteMix {

class QuadChartWidget : public QWidget {
    Q_OBJECT

public:
    explicit QuadChartWidget(QWidget *parent = nullptr);

    // Atualiza os gráficos com base nos resultados do cálculo
    void updateCharts(const MixDesignResult& result, const std::vector<ExperimentalMixRecord>& experimentalMixes);

    // Atualiza o tema dos gráficos (modo claro ou escuro)
    void updateTheme(bool isDark);

    // Salva imagem de todos os gráficos
    void exportChartImage(const QString& filePath);

    void retranslateUi();

protected:
    void changeEvent(QEvent *event) override;

private:
    void setupUI();
    void setupAbramsChart();
    void setupLyseChart();
    void setupMolinariChart();
    void setupCostChart();

    QPushButton *m_btnExport = nullptr;
    QTabWidget *m_viewTabs = nullptr;

    // 4 Gráficos do Diagrama 2x2
    QChartView *m_viewAbrams = nullptr;
    QChart *m_chartAbrams = nullptr;

    QChartView *m_viewLyse = nullptr;
    QChart *m_chartLyse = nullptr;

    QChartView *m_viewMolinari = nullptr;
    QChart *m_chartMolinari = nullptr;

    QChartView *m_viewCost = nullptr;
    QChart *m_chartCost = nullptr;

    // 4 Gráficos individuais ampliados para abas de zoom (sem conflito de cena)
    QChartView *m_viewAbramsZoom = nullptr;
    QChart *m_chartAbramsZoom = nullptr;

    QChartView *m_viewLyseZoom = nullptr;
    QChart *m_chartLyseZoom = nullptr;

    QChartView *m_viewMolinariZoom = nullptr;
    QChart *m_chartMolinariZoom = nullptr;

    QChartView *m_viewCostZoom = nullptr;
    QChart *m_chartCostZoom = nullptr;

    void populateAbrams(QChart *chart, const MixDesignResult& result, const std::vector<ExperimentalMixRecord>& experimentalMixes);
    void populateLyse(QChart *chart, const MixDesignResult& result, const std::vector<ExperimentalMixRecord>& experimentalMixes);
    void populateMolinari(QChart *chart, const MixDesignResult& result, const std::vector<ExperimentalMixRecord>& experimentalMixes);
    void populateCost(QChart *chart, const MixDesignResult& result, const std::vector<ExperimentalMixRecord>& experimentalMixes);

    void applyFlatBackground(QChart *chart);
    void configureValueAxis(QValueAxis *axis, int tickCount, int minorTickCount);

    bool m_isDark = false;
    bool m_hasData = false;
    MixDesignResult m_lastResult;
    std::vector<ExperimentalMixRecord> m_lastMixes;

    // Visão combinada de 4 quadrantes
    QWidget *m_quadWidget = nullptr;
    QWidget *m_tabZoomWidgets[4] = {nullptr, nullptr, nullptr, nullptr};

    // 5º Gráfico: Composição unificada dos eixos cruzados Abrams / Lyse / Molinari (IPT)
    class IptUnifiedDiagramWidget *m_iptUnifiedDiagram = nullptr;
};

} // namespace ConcreteMix
