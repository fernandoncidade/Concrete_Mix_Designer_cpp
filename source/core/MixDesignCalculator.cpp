#include "MixDesignCalculator.hpp"
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace ConcreteMix {

double MixDesignCalculator::getCementPricePerKg(double costPerUnit) {
    if (costPerUnit <= 0.0) return 0.0;
    // Heurística legada para compatibilidade de dados sem unidade explícita:
    // <= 5,00       -> R$/kg (ex: R$ 0,38/kg a R$ 1,20/kg)
    // 5,00 a 150,00 -> R$/saco de 50 kg (ex: R$ 35,00/sc a R$ 55,00/sc)
    // > 150,00      -> R$/tonelada de 1000 kg (ex: R$ 380,00/t a R$ 900,00/t)
    if (costPerUnit <= 5.0) {
        return costPerUnit;
    }
    if (costPerUnit <= 150.0) {
        return costPerUnit / 50.0;
    }
    return costPerUnit / 1000.0;
}

double MixDesignCalculator::getCementPricePerKg(double costPerUnit, PriceUnit unit) {
    if (costPerUnit <= 0.0) return 0.0;
    switch (unit) {
        case PriceUnit::PerKg:
            // Compatibilidade inteligente caso valor seja claramente de saco (>5 e <=150)
            if (costPerUnit > 5.0 && costPerUnit <= 150.0) return costPerUnit / 50.0;
            if (costPerUnit > 150.0) return costPerUnit / 1000.0;
            return costPerUnit;
        case PriceUnit::PerBag50kg:
            return costPerUnit / 50.0;
        case PriceUnit::PerTon:
            return costPerUnit / 1000.0;
        case PriceUnit::PerM3:
            return costPerUnit / 1100.0;
    }
    return costPerUnit;
}

double MixDesignCalculator::getAggregatePricePerM3(double costPerUnit, PriceUnit unit, double unitMassKgL) {
    if (costPerUnit <= 0.0) return 0.0;
    double rhoU_kgM3 = (unitMassKgL > 10.0) ? unitMassKgL : (unitMassKgL * 1000.0);
    if (rhoU_kgM3 <= 0.0) rhoU_kgM3 = 1450.0;

    switch (unit) {
        case PriceUnit::PerM3:
            return costPerUnit;
        case PriceUnit::PerTon:
            return costPerUnit * (rhoU_kgM3 / 1000.0);
        case PriceUnit::PerKg:
            return costPerUnit * rhoU_kgM3;
        case PriceUnit::PerBag50kg:
            return (costPerUnit / 50.0) * rhoU_kgM3;
    }
    return costPerUnit;
}

double MixDesignCalculator::getAggregatePricePerKg(double costPerUnit, PriceUnit unit, double unitMassKgL) {
    if (costPerUnit <= 0.0) return 0.0;
    double rhoU_kgM3 = (unitMassKgL > 10.0) ? unitMassKgL : (unitMassKgL * 1000.0);
    if (rhoU_kgM3 <= 0.0) rhoU_kgM3 = 1450.0;

    switch (unit) {
        case PriceUnit::PerKg:
            return costPerUnit;
        case PriceUnit::PerTon:
            return costPerUnit / 1000.0;
        case PriceUnit::PerM3:
            return costPerUnit / rhoU_kgM3;
        case PriceUnit::PerBag50kg:
            return costPerUnit / 50.0;
    }
    return costPerUnit;
}

MixDesignResult MixDesignCalculator::calculate(
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
    double projectVolumeM3,
    double labMixerVolumeL,
    double manualAc,
    double manualM,
    double manualC,
    double gravel1DmcMm,
    double gravel2DmcMm
) {
    MixDesignResult res;
    res.fck = fck;
    res.sd = (prepCond == PreparationCondition::Arbitrary) ? customSd : ConcreteStandards::getStandardDeviation(prepCond, customSd);
    res.fcj = fck > 0.0 ? ConcreteStandards::calculateDosageStrength(res.fck, res.sd) : 0.0;
    res.projectVolumeM3 = projectVolumeM3;
    res.labMixerVolumeLiters = labMixerVolumeL;

    bool hasValidMixes = false;
    for (const auto& mix : experimentalMixes) {
        if (mix.waterCementRatio > 0.0 && mix.dryRatioM > 0.0) {
            hasValidMixes = true;
            break;
        }
    }

    if (fck <= 0.0 || !hasValidMixes) {
        res.targetWaterCement = 0.0;
        res.targetDryMixM = 0.0;
        res.targetCementConsumption = 0.0;
        res.waterCementSolutionType = SolutionType::NoSolution;
        res.cementConsumptionSolutionType = SolutionType::NoSolution;
        res.hasValidAnalyticalSolution = false;
        res.compliance.isCompliant = false;
        res.compliance.errors.push_back("Aguardando preenchimento dos dados de entrada (fck e traços experimentais).");
        return res;
    }

    // 1. Montagem dos pontos para as regressões e limites do domínio experimental
    std::vector<Point2D> ptsAbrams;
    std::vector<Point2D> ptsLyse;
    std::vector<Point2D> ptsMolinari;

    double sumAc = 0.0, sumC = 0.0;
    size_t validCount = 0;
    double minFc = 1e9, maxFc = -1e9;
    double minAc = 1e9, maxAc = -1e9;

    for (const auto& mix : experimentalMixes) {
        if (mix.waterCementRatio <= 0.0 || mix.dryRatioM <= 0.0) continue;
        double cVal = mix.cementConsumption;
        if (cVal <= 0.0 && mix.freshDensity > 0.0) {
            double denom = 1.0 + mix.dryRatioM + mix.waterCementRatio;
            if (denom > 0.0) cVal = (mix.freshDensity / denom) * 1000.0;
        }
        ptsAbrams.push_back({mix.waterCementRatio, mix.compressiveStrength28d});
        ptsLyse.push_back({mix.waterCementRatio, mix.dryRatioM});
        ptsMolinari.push_back({cVal, mix.dryRatioM});
        sumAc += mix.waterCementRatio;
        sumC += cVal;

        if (mix.compressiveStrength28d < minFc) minFc = mix.compressiveStrength28d;
        if (mix.compressiveStrength28d > maxFc) maxFc = mix.compressiveStrength28d;
        if (mix.waterCementRatio < minAc) minAc = mix.waterCementRatio;
        if (mix.waterCementRatio > maxAc) maxAc = mix.waterCementRatio;

        ++validCount;
    }

    // 2. Ajuste das curvas
    res.abramsPoly = RegressionEngine::fitPolynomial2(ptsAbrams);
    res.lyseLinear = RegressionEngine::fitLinear(ptsLyse);
    res.molinariPoly = RegressionEngine::fitPolynomial2(ptsMolinari);
    res.abramsExp = RegressionEngine::fitAbramsExponential(ptsAbrams);

    double avgAc = (validCount > 0) ? (sumAc / static_cast<double>(validCount)) : 0.45;
    double avgC = (validCount > 0) ? (sumC / static_cast<double>(validCount)) : 350.0;

    // Verificação de extrapolação experimental
    if (res.fcj < minFc || res.fcj > maxFc) {
        res.isExtrapolated = true;
        std::ostringstream ss;
        ss << "Extrapolação estatística: a resistência de dosagem fcj ("
           << std::fixed << std::setprecision(1) << res.fcj << " MPa) está fora da faixa ensaiada ["
           << minFc << " MPa a " << maxFc << " MPa]. Maior incerteza nos resultados calculados.";
        res.compliance.warnings.push_back(ss.str());
    }

    // 3. Determinação analítica de a/c, m e C a partir de fcj
    auto optAc = res.abramsPoly.solveForX(res.fcj, 0.20, 0.90, avgAc);
    if (optAc.has_value()) {
        res.targetWaterCement = optAc.value();
        res.waterCementSolutionType = SolutionType::AnalyticalExact;
        if (res.targetWaterCement < minAc || res.targetWaterCement > maxAc) {
            res.isExtrapolated = true;
        }
    } else {
        // Fallback para abrams exponencial
        auto optExp = res.abramsExp.solveForX(res.fcj);
        if (optExp.has_value() && optExp.value() >= 0.20 && optExp.value() <= 0.90) {
            res.targetWaterCement = optExp.value();
            res.waterCementSolutionType = SolutionType::ExponentialFallback;
            res.hasValidAnalyticalSolution = false;
            std::ostringstream ss;
            ss << "Solução polinomial de Abrams não encontrou raiz física para fcj = "
               << std::fixed << std::setprecision(1) << res.fcj << " MPa. "
               << "Adotada estimativa por ajuste exponencial alternativo (a/c = "
               << std::setprecision(3) << res.targetWaterCement << ").";
            res.compliance.warnings.push_back(ss.str());
        } else {
            // Regressão falhou completamente
            res.targetWaterCement = avgAc;
            res.waterCementSolutionType = SolutionType::ExperimentalAverage;
            res.hasValidAnalyticalSolution = false;
            res.compliance.isCompliant = false; // BLOQUEIA APROVAÇÃO NORMATIVA
            std::ostringstream ss;
            ss << "ERRO CRÍTICO: Não foi possível determinar a relação a/c pela lei de Abrams para fcj = "
               << std::fixed << std::setprecision(1) << res.fcj << " MPa. "
               << "O valor utilizado (" << std::setprecision(3) << avgAc 
               << ") é uma estimativa substituta baseada na média dos ensaios e NÃO atende à resistência requerida. "
               << "Traço bloqueado para aprovação técnica.";
            res.compliance.errors.push_back(ss.str());
        }
    }

    res.targetDryMixM = res.lyseLinear.evaluate(res.targetWaterCement);

    auto optC = res.molinariPoly.solveForX(res.targetDryMixM, 150.0, 700.0, avgC);
    if (optC.has_value()) {
        res.targetCementConsumption = optC.value();
        res.cementConsumptionSolutionType = SolutionType::AnalyticalExact;
    } else {
        res.targetCementConsumption = avgC;
        res.cementConsumptionSolutionType = SolutionType::ExperimentalAverage;
        res.hasValidAnalyticalSolution = false;
        res.compliance.isCompliant = false; // BLOQUEIA
        std::ostringstream ss;
        ss << "ERRO CRÍTICO: Regressão de Molinari não encontrou consumo de cimento para o agregado m = "
           << std::fixed << std::setprecision(2) << res.targetDryMixM << ". "
           << "Consumo arbitrado pela média experimental (" << std::setprecision(1) << avgC << " kg/m³).";
        res.compliance.errors.push_back(ss.str());
    }

    // 4. Parâmetros gráficos manuais
    res.graphicalWaterCement = manualAc;
    res.graphicalDryMixM = manualM;
    res.graphicalCementConsumption = manualC;

    // 5. Decomposição do traço unitário seco a partir de alfa (NBR 12655 / IPT)
    // a = ((alpha / 100) * (1 + m)) - 1
    res.sandRatioA = ((selectedAlphaPercent / 100.0) * (1.0 + res.targetDryMixM)) - 1.0;
    res.gravelRatioP = res.targetDryMixM - res.sandRatioA;

    res.graphSandRatioA = ((selectedAlphaPercent / 100.0) * (1.0 + res.graphicalDryMixM)) - 1.0;
    res.graphGravelRatioP = res.graphicalDryMixM - res.graphSandRatioA;

    if (useGravelBlend) {
        double g2Frac = gravel2PercentInBlend / 100.0;
        double g1Frac = 1.0 - g2Frac;
        res.gravel2Ratio = res.gravelRatioP * g2Frac;
        res.gravel1Ratio = res.gravelRatioP * g1Frac;
    } else {
        res.gravel1Ratio = res.gravelRatioP;
        res.gravel2Ratio = 0.0;
    }

    // Massa específica composta do agregado graúdo
    double effectiveGravelDensity = gravel1.specificMass;
    double effectiveGravelUnitMass = gravel1.unitMass;
    double effectiveGravelMoisture = gravel1.moisturePercent;
    if (useGravelBlend && gravel2PercentInBlend > 0.0) {
        double f2 = gravel2PercentInBlend / 100.0;
        double f1 = 1.0 - f2;
        effectiveGravelDensity = (f2 * gravel2.specificMass) + (f1 * gravel1.specificMass);
        effectiveGravelUnitMass = (f2 * gravel2.unitMass) + (f1 * gravel1.unitMass);
        effectiveGravelMoisture = (f2 * gravel2.moisturePercent) + (f1 * gravel1.moisturePercent);
    }

    // Consumo teórico de cimento pelos volumes absolutos
    res.theoreticalCementConsumption = RegressionEngine::calculateTheoreticalConsumption(
        res.sandRatioA,
        res.gravelRatioP,
        res.targetWaterCement,
        cement.specificMass,
        sand.specificMass,
        effectiveGravelDensity
    );

    // 6. Cálculo das quantidades por 1 m³ (Analítico)
    auto computeMixQuantities = [&](double C, double a, double p, double ac) {
        MixQuantities q;
        q.cementKg = C;
        q.sandDryKg = a * C;
        q.gravelDryKg = p * C;
        q.waterTheoreticalL = ac * C;

        if (useGravelBlend) {
            double g2Frac = gravel2PercentInBlend / 100.0;
            double g1Frac = 1.0 - g2Frac;
            q.gravel2DryKg = q.gravelDryKg * g2Frac;
            q.gravel1DryKg = q.gravelDryKg * g1Frac;

            q.gravel2WetKg = q.gravel2DryKg * (1.0 + gravel2.moisturePercent / 100.0);
            q.gravel1WetKg = q.gravel1DryKg * (1.0 + gravel1.moisturePercent / 100.0);
            q.gravelWetKg = q.gravel2WetKg + q.gravel1WetKg;
        } else {
            q.gravelWetKg = q.gravelDryKg * (1.0 + gravel1.moisturePercent / 100.0);
            q.gravel1DryKg = q.gravelDryKg;
            q.gravel1WetKg = q.gravelWetKg;
        }

        q.sandWetKg = q.sandDryKg * (1.0 + sand.moisturePercent / 100.0);

        double waterFromSand = q.sandWetKg - q.sandDryKg;
        double waterFromGravel = q.gravelWetKg - q.gravelDryKg;
        q.waterCorrectedL = q.waterTheoreticalL - waterFromSand - waterFromGravel;

        // Volumes em m³ usando massa unitária aparente
        if (sand.unitMass > 0.0) {
            double rhoU = (sand.unitMass < 10.0) ? (sand.unitMass * 1000.0) : sand.unitMass;
            q.sandVolumeM3 = q.sandWetKg / rhoU;
        }
        if (useGravelBlend && gravel2PercentInBlend > 0.0) {
            if (gravel1.unitMass > 0.0) {
                double rhoU1 = (gravel1.unitMass < 10.0) ? (gravel1.unitMass * 1000.0) : gravel1.unitMass;
                q.gravel1VolumeM3 = q.gravel1WetKg / rhoU1;
            }
            if (gravel2.unitMass > 0.0) {
                double rhoU2 = (gravel2.unitMass < 10.0) ? (gravel2.unitMass * 1000.0) : gravel2.unitMass;
                q.gravel2VolumeM3 = q.gravel2WetKg / rhoU2;
            }
            q.gravelVolumeM3 = q.gravel1VolumeM3 + q.gravel2VolumeM3;
        } else {
            if (gravel1.unitMass > 0.0) {
                double rhoU1 = (gravel1.unitMass < 10.0) ? (gravel1.unitMass * 1000.0) : gravel1.unitMass;
                q.gravel1VolumeM3 = q.gravelWetKg / rhoU1;
            }
            q.gravel2VolumeM3 = 0.0;
            q.gravelVolumeM3 = q.gravel1VolumeM3;
        }

        q.totalMassKg = q.cementKg + q.sandWetKg + q.gravelWetKg + q.waterCorrectedL;
        return q;
    };

    res.perM3Analytical = computeMixQuantities(res.targetCementConsumption, res.sandRatioA, res.gravelRatioP, res.targetWaterCement);
    res.perM3Graphical = computeMixQuantities(res.graphicalCementConsumption, res.graphSandRatioA, res.graphGravelRatioP, res.graphicalWaterCement);

    // 7. Cálculo de custos por m³ com unidades explícitas
    auto computeCosts = [&](const MixQuantities& q) {
        CostBreakdown c;
        double cPricePerKg = getCementPricePerKg(cement.costPerUnit, cement.costUnit);
        c.cementCost = q.cementKg * cPricePerKg;

        double sandPricePerM3 = getAggregatePricePerM3(sand.costPerUnit, sand.costUnit, sand.unitMass);
        c.sandCost = q.sandVolumeM3 * sandPricePerM3;

        if (useGravelBlend && gravel2PercentInBlend > 0.0) {
            double g1Price = getAggregatePricePerM3(gravel1.costPerUnit, gravel1.costUnit, gravel1.unitMass);
            double g2Price = getAggregatePricePerM3(gravel2.costPerUnit, gravel2.costUnit, gravel2.unitMass);
            c.gravelCost = (q.gravel1VolumeM3 * g1Price) + (q.gravel2VolumeM3 * g2Price);
        } else {
            double g1Price = getAggregatePricePerM3(gravel1.costPerUnit, gravel1.costUnit, gravel1.unitMass);
            c.gravelCost = q.gravelVolumeM3 * g1Price;
        }
        c.totalCost = c.cementCost + c.sandCost + c.gravelCost;
        return c;
    };

    res.costPerM3Analytical = computeCosts(res.perM3Analytical);
    res.costPerM3Graphical = computeCosts(res.perM3Graphical);
    res.costDiffPerM3 = std::abs(res.costPerM3Analytical.totalCost - res.costPerM3Graphical.totalCost);

    // 8. Obra total
    auto scaleQuantities = [&](const MixQuantities& q, double vol) {
        MixQuantities tot;
        tot.cementKg = q.cementKg * vol;
        tot.sandDryKg = q.sandDryKg * vol;
        tot.sandWetKg = q.sandWetKg * vol;
        tot.gravelDryKg = q.gravelDryKg * vol;
        tot.gravelWetKg = q.gravelWetKg * vol;
        tot.gravel2DryKg = q.gravel2DryKg * vol;
        tot.gravel2WetKg = q.gravel2WetKg * vol;
        tot.gravel1DryKg = q.gravel1DryKg * vol;
        tot.gravel1WetKg = q.gravel1WetKg * vol;
        tot.waterTheoreticalL = q.waterTheoreticalL * vol;
        tot.waterCorrectedL = q.waterCorrectedL * vol;
        tot.sandVolumeM3 = q.sandVolumeM3 * vol;
        tot.gravelVolumeM3 = q.gravelVolumeM3 * vol;
        tot.gravel1VolumeM3 = q.gravel1VolumeM3 * vol;
        tot.gravel2VolumeM3 = q.gravel2VolumeM3 * vol;
        tot.totalMassKg = q.totalMassKg * vol;
        return tot;
    };

    res.projectTotalAnalytical = scaleQuantities(res.perM3Analytical, projectVolumeM3);
    res.projectCostAnalytical = computeCosts(res.projectTotalAnalytical);

    res.projectTotalGraphical = scaleQuantities(res.perM3Graphical, projectVolumeM3);
    res.projectCostGraphical = computeCosts(res.projectTotalGraphical);

    res.projectCostDiff = std::abs(res.projectCostAnalytical.totalCost - res.projectCostGraphical.totalCost);

    res.totalCementBagsAnalytical = static_cast<int>(std::ceil(res.projectTotalAnalytical.cementKg / 50.0));
    res.totalCementBagsGraphical = static_cast<int>(std::ceil(res.projectTotalGraphical.cementKg / 50.0));
    res.diffCementBags = std::abs(res.totalCementBagsAnalytical - res.totalCementBagsGraphical);

    // 9. Amassada de laboratório para o volume do misturador (ex: 29 L, 90 L, 100 L)
    double labMixRatio = labMixerVolumeL / 1000.0;
    res.labBatch = scaleQuantities(res.perM3Analytical, labMixRatio);

    // 10. Dosagem em canteiro por 1 saco de cimento (50 kg)
    res.jobsiteBatch50kg.cementKg = 50.0;
    res.jobsiteBatch50kg.sandWetKg = 50.0 * res.sandRatioA * (1.0 + sand.moisturePercent / 100.0);
    res.jobsiteBatch50kg.gravelWetKg = 50.0 * res.gravelRatioP * (1.0 + effectiveGravelMoisture / 100.0);
    
    double waterSeca50 = 50.0 * res.targetWaterCement;
    double waterFromSand50 = res.jobsiteBatch50kg.sandWetKg - (50.0 * res.sandRatioA);
    double waterFromGravel50 = res.jobsiteBatch50kg.gravelWetKg - (50.0 * res.gravelRatioP);
    res.jobsiteBatch50kg.waterLiters = std::max(0.0, waterSeca50 - waterFromSand50 - waterFromGravel50);

    double sandUnitMassKgL = (sand.unitMass > 10.0) ? (sand.unitMass / 1000.0) : sand.unitMass;
    double gravelUnitMassKgL = (effectiveGravelUnitMass > 10.0) ? (effectiveGravelUnitMass / 1000.0) : effectiveGravelUnitMass;

    res.jobsiteBatch50kg.sandVolumeLiters = (sandUnitMassKgL > 0.0) ? (res.jobsiteBatch50kg.sandWetKg / sandUnitMassKgL) : 0.0;
    res.jobsiteBatch50kg.gravelVolumeLiters = (gravelUnitMassKgL > 0.0) ? (res.jobsiteBatch50kg.gravelWetKg / gravelUnitMassKgL) : 0.0;

    // Dimensões de padiolas padrão brasileiras (base 35 cm x 45 cm = 15.75 dm²):
    constexpr double padiolaBaseDm2 = 15.75;
    res.jobsiteBatch50kg.sandPadiolaCount = 1;
    res.jobsiteBatch50kg.sandPadiolaHeightCm = (res.jobsiteBatch50kg.sandVolumeLiters / (padiolaBaseDm2 * res.jobsiteBatch50kg.sandPadiolaCount)) * 10.0;

    res.jobsiteBatch50kg.gravelPadiolaCount = 2; // tradicionalmente 2 padiolas para brita
    res.jobsiteBatch50kg.gravelPadiolaHeightCm = (res.jobsiteBatch50kg.gravelVolumeLiters / (padiolaBaseDm2 * res.jobsiteBatch50kg.gravelPadiolaCount)) * 10.0;

    // 11. Validação de Rigor Técnico e Normas
    double dmc = (gravel1DmcMm > 0.0) ? gravel1DmcMm : 19.0;
    if (useGravelBlend && gravel2PercentInBlend > 0.0) {
        double g2Dmc = (gravel2DmcMm > 0.0) ? gravel2DmcMm : 25.0;
        dmc = std::max(dmc, g2Dmc);
    }
    auto durability = ConcreteStandards::getDurabilityRequirement(envClass, structType);
    double cover = durability.nominalCoverBeamsCols;

    auto compNorm = ConcreteStandards::validateMixDesign(
        fck,
        res.targetWaterCement,
        res.targetCementConsumption,
        envClass,
        structType,
        targetSlumpMm,
        dmc,
        cover,
        prepCond
    );

    // Preserva integridade de status: se a matemática falhou, conformidade é estritamente falsa
    if (!compNorm.isCompliant || !res.hasValidAnalyticalSolution) {
        res.compliance.isCompliant = false;
    }
    for (const auto& p : compNorm.passedChecks) res.compliance.passedChecks.push_back(p);
    for (const auto& w : compNorm.warnings) res.compliance.warnings.push_back(w);
    for (const auto& e : compNorm.errors) res.compliance.errors.push_back(e);
    for (const auto& r : compNorm.recommendations) res.compliance.recommendations.push_back(r);

    return res;
}

} // namespace ConcreteMix
