#include "MortarContentOptimizer.hpp"
#include <cmath>

namespace ConcreteMix {

double MortarContentOptimizer::calculateSandRatio(double alphaPercent, double dryMixM) {
    // a = ((alpha / 100) * (1 + m)) - 1
    return ((alphaPercent / 100.0) * (1.0 + dryMixM)) - 1.0;
}

double MortarContentOptimizer::calculateAlphaFromRatios(double sandRatio, double dryMixM) {
    // alpha = (1 + a) / (1 + m) * 100
    if (1.0 + dryMixM <= 0.0) return 0.0;
    return ((1.0 + sandRatio) / (1.0 + dryMixM)) * 100.0;
}

PilotBatchMasses MortarContentOptimizer::calculatePilotBatchFromVolume(
    double mixerVolumeL,
    double pilotDryMixM,
    double initialAlphaPercent,
    double initialWaterCementRatio,
    double cementSpecificMass,
    double sandSpecificMass,
    double gravelSpecificMass
) {
    PilotBatchMasses res{0.0, 0.0, 0.0, 0.0};
    if (mixerVolumeL <= 0.0 || pilotDryMixM <= 0.0) {
        return res;
    }

    const double gammaC = (cementSpecificMass > 0.0) ? cementSpecificMass : 2.96;
    const double gammaA = (sandSpecificMass > 0.0) ? sandSpecificMass : 2.60;
    const double gammaB = (gravelSpecificMass > 0.0) ? gravelSpecificMass : 2.70;

    const double a0 = calculateSandRatio(initialAlphaPercent, pilotDryMixM);
    const double b0 = pilotDryMixM - a0;
    const double ac0 = (initialWaterCementRatio > 0.0) ? initialWaterCementRatio : 0.50;

    const double specificVolumePerKgCement = (1.0 / gammaC) + (a0 / gammaA) + (b0 / gammaB) + (ac0 / 1.0);
    if (specificVolumePerKgCement <= 0.0) {
        return res;
    }

    res.cementMassKg = mixerVolumeL / specificVolumePerKgCement;
    res.sandMassKg = a0 * res.cementMassKg;
    res.gravelMassKg = b0 * res.cementMassKg;
    res.waterMassKg = ac0 * res.cementMassKg;
    return res;
}

std::vector<MortarContentRow> MortarContentOptimizer::generateTable(
    double pilotDryMixM,
    double batchGravelMassKg,
    double batchWaterMassKg,
    double selectedAlpha
) {
    std::vector<MortarContentRow> rows;
    rows.reserve(15);

    double prevSand = 0.0;
    double prevCement = 0.0;
    double prevWater = batchWaterMassKg;

    for (int alphaInt = 40; alphaInt <= 68; alphaInt += 2) {
        double alpha = static_cast<double>(alphaInt);
        MortarContentRow r;
        r.alphaPercent = alpha;
        r.cementRatio = 1.0;
        r.sandRatio = calculateSandRatio(alpha, pilotDryMixM);
        r.gravelRatio = pilotDryMixM - r.sandRatio;
        r.totalGravelMass = batchGravelMassKg;

        if (r.gravelRatio > 0.0) {
            r.totalCementMass = batchGravelMassKg / r.gravelRatio;
            r.totalSandMass = r.totalCementMass * r.sandRatio;
        } else {
            r.totalCementMass = 0.0;
            r.totalSandMass = 0.0;
        }

        r.totalWaterMass = batchWaterMassKg;

        if (rows.empty()) {
            r.incrementalSand = 0.0;
            r.incrementalCement = 0.0;
            r.incrementalWater = 0.0;
        } else {
            r.incrementalSand = r.totalSandMass - prevSand;
            r.incrementalCement = r.totalCementMass - prevCement;
            r.incrementalWater = r.totalWaterMass - prevWater;
        }

        prevSand = r.totalSandMass;
        prevCement = r.totalCementMass;
        prevWater = r.totalWaterMass;

        if (std::abs(alpha - selectedAlpha) < 1.0) {
            r.isSelected = true;
        }

        rows.push_back(r);
    }

    return rows;
}

} // namespace ConcreteMix
