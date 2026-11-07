#pragma once

#include "ConcreteStandards.hpp"
#include "RegressionEngine.hpp"
#include <string>
#include <vector>

namespace ConcreteMix {

enum class SolutionType {
    AnalyticalExact,     // Solução analítica exata obtida por Abrams
    ExponentialFallback, // Estimativa por modelo alternativo exponencial
    ExperimentalAverage, // Estimativa substituta por média experimental (NÃO CONFORME para fins oficiais)
    NoSolution           // Ausência de dados ou sem solução matemática
};

enum class PriceUnit {
    PerKg,      // R$/kg
    PerBag50kg, // R$/saco de 50 kg
    PerSack50kg = PerBag50kg, // Alias para saco de 50 kg
    PerTon,     // R$/tonelada (1000 kg)
    PerM3       // R$/m³
};

struct MaterialProperties {
    std::string name;
    double specificMass = 2.96; // kg/dm³ (g/cm³) - Massa específica real
    double unitMass = 1.75;     // kg/dm³ (kg/l) - Massa unitária aparente solta
    double moisturePercent = 0.0; // Umidade h (%)
    double costPerUnit = 0.38;   // Valor numérico do custo
    PriceUnit costUnit = PriceUnit::PerKg; // Unidade explícita selecionada
};

struct ExperimentalMixRecord {
    std::string label;       // ex: "Traço 1 (Rico)", "Traço 2 (Médio)", "Traço 3 (Pobre)"
    double dryRatioM = 5.0;  // m = a + p
    double waterCementRatio = 0.50; // a/c
    double compressiveStrength28d = 35.0; // fcj (MPa)
    double slumpMm = 80.0;   // Abatimento (mm)
    double freshDensity = 2.40; // Massa específica do concreto fresco (kg/dm³)
    double cementConsumption = 350.0; // Consumo C (kg/m³)
};

struct MixQuantities {
    double cementKg = 0.0;
    double sandDryKg = 0.0;
    double sandWetKg = 0.0;
    double gravelDryKg = 0.0;
    double gravelWetKg = 0.0;
    double gravel2DryKg = 0.0; // Se houver mistura de britas
    double gravel2WetKg = 0.0;
    double gravel1DryKg = 0.0;
    double gravel1WetKg = 0.0;
    double waterTheoreticalL = 0.0;
    double waterCorrectedL = 0.0;
    double sandVolumeM3 = 0.0;
    double gravelVolumeM3 = 0.0;
    double gravel1VolumeM3 = 0.0;
    double gravel2VolumeM3 = 0.0;
    double totalMassKg = 0.0;
};

struct CostBreakdown {
    double cementCost = 0.0;
    double sandCost = 0.0;
    double gravelCost = 0.0;
    double totalCost = 0.0;
};

struct BatchProportion50kg {
    double cementKg = 50.0;
    double sandWetKg = 0.0;
    double gravelWetKg = 0.0;
    double waterLiters = 0.0;
    double sandVolumeLiters = 0.0;
    double gravelVolumeLiters = 0.0;
    // Padiolas (caixas) com base 35 cm x 45 cm (área = 15.75 dm²)
    double sandPadiolaHeightCm = 0.0;
    int sandPadiolaCount = 1;
    double gravelPadiolaHeightCm = 0.0;
    int gravelPadiolaCount = 2;
};

struct MixDesignResult {
    // Resistência de dosagem
    double fck = 30.0;
    double sd = 3.5;
    double fcj = 35.775;

    // Diagnóstico e rastreabilidade da solução matemática (Prioridade Alta #1)
    SolutionType waterCementSolutionType = SolutionType::AnalyticalExact;
    SolutionType cementConsumptionSolutionType = SolutionType::AnalyticalExact;
    SolutionType cementSolutionType = SolutionType::AnalyticalExact;
    bool hasValidAnalyticalSolution = true;
    bool isExtrapolated = false;
    std::string solutionDiagnostic;

    // Equações ajustadas
    Polynomial2 abramsPoly;
    LinearEquation lyseLinear;
    Polynomial2 molinariPoly;
    ExponentialEquation abramsExp;

    // Valores calculados analiticamente
    double targetWaterCement = 0.411; // a/c
    double targetDryMixM = 5.508;     // m
    double targetCementConsumption = 371.07; // C (kg/m³)
    double theoreticalCementConsumption = 371.0;

    // Valores manuais/gráficos (para comparação com a planilha)
    double graphicalWaterCement = 0.4285;
    double graphicalDryMixM = 5.8;
    double graphicalCementConsumption = 332.0;

    // Traço unitário em massa seca (1 : a : p)
    double sandRatioA = 2.384;
    double gravelRatioP = 3.124;
    double gravel2Ratio = 0.0;
    double gravel1Ratio = 0.0;

    // Traço unitário gráfico (1 : a_g : p_g)
    double graphSandRatioA = 2.536;
    double graphGravelRatioP = 3.264;

    // Quantidades por 1 m³ (analítico)
    MixQuantities perM3Analytical;
    CostBreakdown costPerM3Analytical;

    // Quantidades por 1 m³ (gráfico)
    MixQuantities perM3Graphical;
    CostBreakdown costPerM3Graphical;

    // Diferença de custo por m³
    double costDiffPerM3 = 0.0;

    // Quantidades para a obra total
    double projectVolumeM3 = 30.0;
    MixQuantities projectTotalAnalytical;
    CostBreakdown projectCostAnalytical;
    MixQuantities projectTotalGraphical;
    CostBreakdown projectCostGraphical;
    double projectCostDiff = 0.0;
    int totalCementBagsAnalytical = 0;
    int totalCementBagsGraphical = 0;
    int diffCementBags = 0;

    // Amassada para betoneira de laboratório
    double labMixerVolumeLiters = 29.0;
    MixQuantities labBatch;

    // Dosagem em canteiro para 1 saco de 50 kg (padiolas)
    BatchProportion50kg jobsiteBatch50kg;

    // Rigor normativo e alertas técnicos
    TechnicalComplianceResult compliance;
};

class MixDesignCalculator {
public:
    // Converte o preço unitário do cimento para R$/kg
    static double getCementPricePerKg(double costPerUnit);
    static double getCementPricePerKg(double costPerUnit, PriceUnit unit);

    // Converte preço unitário de agregados para R$/m³ e R$/kg
    static double getAggregatePricePerM3(double costPerUnit, PriceUnit unit, double unitMassKgL);
    static double getAggregatePricePerKg(double costPerUnit, PriceUnit unit, double unitMassKgL);

    static double convertCementPriceToKg(double costPerUnit, PriceUnit unit) { return getCementPricePerKg(costPerUnit, unit); }
    static double convertAggregatePriceToKg(double costPerUnit, PriceUnit unit, double unitMassKgL) { return getAggregatePricePerKg(costPerUnit, unit, unitMassKgL); }

    static MixDesignResult calculate(
        double fck,
        PreparationCondition prepCond,
        double customSd,
        EnvironmentalClass envClass,
        StructuralType structType,
        double targetSlumpMm,
        double selectedAlphaPercent,
        bool useGravelBlend,
        double gravel2PercentInBlend,
        const MaterialProperties& cement,
        const MaterialProperties& sand,
        const MaterialProperties& gravel1,
        const MaterialProperties& gravel2,
        const std::vector<ExperimentalMixRecord>& experimentalMixes,
        double projectVolumeM3 = 30.0,
        double labMixerVolumeL = 29.0,
        double manualAc = 0.4285,
        double manualM = 5.8,
        double manualC = 332.0,
        double gravel1DmcMm = 19.0,
        double gravel2DmcMm = 25.0
    );
};

} // namespace ConcreteMix
