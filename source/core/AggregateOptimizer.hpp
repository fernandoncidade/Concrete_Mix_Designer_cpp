#pragma once

#include <vector>
#include <string>

namespace ConcreteMix {

struct CoarseAggregateBlendRow {
    double percentCoarse1 = 100.0; // ex: Brita 2 (%)
    double percentCoarse2 = 0.0;   // ex: Brita 1 (%)
    double weightCoarse1 = 0.0;    // kg
    double weightCoarse2 = 0.0;    // kg
    double incrementalCoarse2 = 0.0;// kg de acréscimo
    double totalMass = 0.0;        // kg
    double netMassInContainer = 0.0;// kg no recipiente cilíndrico
    double compactedUnitWeight = 0.0; // kg/dm³ (M.U. Compactada)
    bool isOptimum = false;
};

class AggregateOptimizer {
public:
    // Calcula a tabela de mistura de agregados graúdos conforme NBR 7211 / NM 45
    // baseCoarse1Mass: massa inicial de brita 2 (ou brita 1)
    // containerVolumeLiters: volume do recipiente cilíndrico de ensaio (padrão 15,04 L ou customizado)
    static std::vector<CoarseAggregateBlendRow> calculateBlendTable(
        double baseCoarse1Mass,
        const std::vector<double>& netMassesInContainer,
        double containerVolumeLiters = 15.04
    );

    // Retorna a melhor proporção de mistura (máxima massa unitária compactada / menor índice de vazios)
    static std::pair<double, double> getOptimumProportions(const std::vector<CoarseAggregateBlendRow>& table);
};

} // namespace ConcreteMix
