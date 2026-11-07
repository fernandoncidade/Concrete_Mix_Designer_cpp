#pragma once

#include <vector>

namespace ConcreteMix {

struct MortarContentRow {
    double alphaPercent = 40.0;    // Teor de argamassa (%)
    double cementRatio = 1.0;      // c = 1
    double sandRatio = 1.4;        // a
    double gravelRatio = 3.6;      // b = m - a
    double totalSandMass = 0.0;    // kg
    double incrementalSand = 0.0;  // kg
    double totalCementMass = 0.0;  // kg
    double incrementalCement = 0.0;// kg
    double totalWaterMass = 0.0;   // kg
    double incrementalWater = 0.0; // kg
    double totalGravelMass = 0.0;  // kg
    bool isSelected = false;
};

struct PilotBatchMasses {
    double cementMassKg = 12.0; // Massa inicial de cimento para α = 40% (kg)
    double sandMassKg = 16.8;   // Massa inicial de areia para α = 40% (kg)
    double gravelMassKg = 43.2; // Massa fixa de brita para o ensaio (kg)
    double waterMassKg = 6.0;   // Massa inicial de água para o ensaio (kg)
};

class MortarContentOptimizer {
public:
    // Calcula as massas da amassada piloto pela Lei dos Volumes Absolutos
    // (conforme planilha 'Memorial de cálculo'!I138..I360):
    // m_c0 = V_betoneira / (1/gamma_c + a0/gamma_a + b0/gamma_b + ac0/1.0)
    static PilotBatchMasses calculatePilotBatchFromVolume(
        double mixerVolumeL,
        double pilotDryMixM = 5.0,
        double initialAlphaPercent = 40.0,
        double initialWaterCementRatio = 0.50,
        double cementSpecificMass = 2.96,
        double sandSpecificMass = 2.60,
        double gravelSpecificMass = 2.70
    );

    // Gera a tabela experimental de variação de alfa de 40% a 68% com passo de 2%
    // pilotDryMixM: relação agregados/cimento do traço piloto (padrão m = 5,0)
    // batchGravelMassKg: massa fixa de brita colocada na betoneira para o ensaio
    // batchWaterMassKg: massa de água do ensaio piloto
    // selectedAlpha: alfa ideal escolhido pelo operador
    static std::vector<MortarContentRow> generateTable(
        double pilotDryMixM,
        double batchGravelMassKg,
        double batchWaterMassKg,
        double selectedAlpha = 52.0
    );

    // Converte alfa em razão unitária de areia (a) para qualquer traço m
    static double calculateSandRatio(double alphaPercent, double dryMixM);

    // Converte razão unitária de areia em alfa (%)
    static double calculateAlphaFromRatios(double sandRatio, double dryMixM);
};

} // namespace ConcreteMix
