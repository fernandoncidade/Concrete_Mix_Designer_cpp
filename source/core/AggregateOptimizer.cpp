#include "AggregateOptimizer.hpp"
#include <cmath>
#include <algorithm>

namespace ConcreteMix {

std::vector<CoarseAggregateBlendRow> AggregateOptimizer::calculateBlendTable(
    double baseCoarse1Mass,
    const std::vector<double>& netMassesInContainer,
    double containerVolumeLiters
) {
    const std::vector<double> proportions = {100.0, 90.0, 80.0, 70.0, 60.0, 50.0};
    std::vector<CoarseAggregateBlendRow> table;
    table.reserve(proportions.size());

    double prevW2 = 0.0;
    double maxUnitWeight = -1.0;
    size_t bestIdx = 0;

    for (size_t i = 0; i < proportions.size(); ++i) {
        CoarseAggregateBlendRow row;
        row.percentCoarse1 = proportions[i];
        row.percentCoarse2 = 100.0 - proportions[i];
        row.weightCoarse1 = baseCoarse1Mass;

        if (row.percentCoarse1 > 0.0) {
            row.weightCoarse2 = (baseCoarse1Mass / (row.percentCoarse1 / 100.0)) - baseCoarse1Mass;
        } else {
            row.weightCoarse2 = 0.0;
        }

        if (i == 0) {
            row.incrementalCoarse2 = 0.0;
        } else {
            row.incrementalCoarse2 = row.weightCoarse2 - prevW2;
        }
        prevW2 = row.weightCoarse2;

        row.totalMass = row.weightCoarse1 + row.weightCoarse2;

        if (i < netMassesInContainer.size() && netMassesInContainer[i] > 0.0) {
            row.netMassInContainer = netMassesInContainer[i];
            if (containerVolumeLiters > 0.0) {
                row.compactedUnitWeight = row.netMassInContainer / containerVolumeLiters;
            }
        } else {
            // Default experimental values from spreadsheet:
            // 24.2, 24.5, 24.8, 25.4, 25.4, 25.4 kg
            static const std::vector<double> defNet = {24.2, 24.5, 24.8, 25.4, 25.4, 25.4};
            row.netMassInContainer = defNet[i];
            row.compactedUnitWeight = row.netMassInContainer / 15.04;
        }

        if (row.compactedUnitWeight > maxUnitWeight) {
            maxUnitWeight = row.compactedUnitWeight;
            bestIdx = i;
        }

        table.push_back(row);
    }

    if (bestIdx < table.size()) {
        table[bestIdx].isOptimum = true;
    }

    return table;
}

std::pair<double, double> AggregateOptimizer::getOptimumProportions(const std::vector<CoarseAggregateBlendRow>& table) {
    for (const auto& row : table) {
        if (row.isOptimum) {
            return {row.percentCoarse1, row.percentCoarse2};
        }
    }
    return {70.0, 30.0}; // padrão de ensaio clássico (70% B2 / 30% B1)
}

} // namespace ConcreteMix
