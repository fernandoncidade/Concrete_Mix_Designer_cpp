#include "TestFramework.hpp"

#include "../source/core/AggregateOptimizer.hpp"
#include "../source/core/MixDesignCalculator.hpp"
#include "../source/core/MortarContentOptimizer.hpp"
#include "../source/core/ProjectData.hpp"
#include "../source/core/RegressionEngine.hpp"

#include <cmath>
#include <vector>

using namespace ConcreteMix;

int main() {
    concrete_tests::TestRunner runner("MathematicalCoreAndOptimizers");

    runner.add("AggregateOptimizerBlendTableAndOptimum", []() {
        const std::vector<double> netMasses = {22.10, 22.80, 23.45, 24.05, 23.90, 23.10};
        const auto rows = AggregateOptimizer::calculateBlendTable(12.0, netMasses, 15.04);
        CMD_ASSERT(rows.size() == netMasses.size());

        int optimumCount = 0;
        for (size_t i = 0; i < rows.size(); ++i) {
            CMD_ASSERT_NEAR(rows[i].percentCoarse1 + rows[i].percentCoarse2, 100.0, 1e-6);
            CMD_ASSERT_NEAR(rows[i].compactedUnitWeight, netMasses[i] / 15.04, 1e-5);
            if (rows[i].isOptimum) {
                ++optimumCount;
                CMD_ASSERT(i == 3); // Pico em netMasses[3] = 24.05 kg
            }
        }
        CMD_ASSERT(optimumCount == 1);

        const auto [opt1, opt2] = AggregateOptimizer::getOptimumProportions(rows);
        CMD_ASSERT_NEAR(opt1 + opt2, 100.0, 1e-6);
        CMD_ASSERT_NEAR(opt1, rows[3].percentCoarse1, 1e-6);
        CMD_ASSERT_NEAR(opt2, rows[3].percentCoarse2, 1e-6);

        // Valida recipiente customizado (ex: 10.0 L) mantendo as 6 linhas padronizadas da tabela
        const auto customRows = AggregateOptimizer::calculateBlendTable(10.0, {15.0, 16.5}, 10.0);
        CMD_ASSERT(customRows.size() == 6);
        CMD_ASSERT_NEAR(customRows[0].compactedUnitWeight, 1.50, 1e-6);
        CMD_ASSERT_NEAR(customRows[1].compactedUnitWeight, 1.65, 1e-6);
    });

    runner.add("MortarContentOptimizerPilotBatchAndTable", []() {
        const double mixerVol = 29.0;
        const double m0 = 5.0;
        const double alpha0 = 40.0;
        const double ac0 = 0.50;
        const double gc = 2.96;
        const double ga = 2.60;
        const double gb = 2.70;

        const PilotBatchMasses pilot = MortarContentOptimizer::calculatePilotBatchFromVolume(
            mixerVol, m0, alpha0, ac0, gc, ga, gb);

        // a = 0.40 * (1 + 5.0) - 1 = 1.40; b = 5.0 - 1.40 = 3.60
        const double expectedA = 1.40;
        const double expectedB = 3.60;
        const double solidVolPerKg = (1.0 / gc) + (expectedA / ga) + (expectedB / gb) + (ac0 / 1.0);
        const double expectedCementKg = mixerVol / solidVolPerKg;

        CMD_ASSERT_NEAR(pilot.cementMassKg, expectedCementKg, 1e-4);
        CMD_ASSERT_NEAR(pilot.sandMassKg, expectedA * expectedCementKg, 1e-4);
        CMD_ASSERT_NEAR(pilot.gravelMassKg, expectedB * expectedCementKg, 1e-4);
        CMD_ASSERT_NEAR(pilot.waterMassKg, ac0 * expectedCementKg, 1e-4);

        const auto rows = MortarContentOptimizer::generateTable(m0, pilot.gravelMassKg, pilot.waterMassKg, 52.0);
        CMD_ASSERT(rows.size() == 15); // 40% .. 68% passo 2%
        int selectedCount = 0;
        for (const auto& r : rows) {
            CMD_ASSERT_NEAR(r.sandRatio + r.gravelRatio, m0, 1e-6);
            CMD_ASSERT(r.incrementalSand >= -1e-9);
            CMD_ASSERT(r.totalSandMass > 0.0);
            if (r.isSelected) {
                ++selectedCount;
                CMD_ASSERT_NEAR(r.alphaPercent, 52.0, 1e-6);
            }
        }
        CMD_ASSERT(selectedCount == 1);

        const double a52 = MortarContentOptimizer::calculateSandRatio(52.0, 5.0);
        CMD_ASSERT_NEAR(a52, 2.12, 1e-6);
        CMD_ASSERT_NEAR(MortarContentOptimizer::calculateAlphaFromRatios(a52, 5.0), 52.0, 1e-6);
    });

    runner.add("RegressionEngineLinearPolynomialExponentialAndDegenerateGuards", []() {
        // Linear exata: y = 12.0 * x - 0.5
        const std::vector<Point2D> linPts = {{0.40, 4.3}, {0.50, 5.5}, {0.60, 6.7}};
        const auto lin = RegressionEngine::fitLinear(linPts);
        CMD_ASSERT(lin.isValid);
        CMD_ASSERT_NEAR(lin.a, 12.0, 1e-6);
        CMD_ASSERT_NEAR(lin.b, -0.5, 1e-6);
        CMD_ASSERT_NEAR(lin.rSquared, 1.0, 1e-6);
        CMD_ASSERT_NEAR(lin.solveForX(5.5), 0.50, 1e-6);

        // Quadrática exata: y = 2*x^2 - 3*x + 10
        const std::vector<Point2D> polyPts = {{0.0, 10.0}, {1.0, 9.0}, {2.0, 12.0}, {3.0, 19.0}};
        const auto poly = RegressionEngine::fitPolynomial2(polyPts);
        CMD_ASSERT(poly.isValid);
        CMD_ASSERT_NEAR(poly.a, 2.0, 1e-6);
        CMD_ASSERT_NEAR(poly.b, -3.0, 1e-6);
        CMD_ASSERT_NEAR(poly.c, 10.0, 1e-6);
        CMD_ASSERT_NEAR(poly.rSquared, 1.0, 1e-6);
        CMD_ASSERT(poly.isInsideDomain(1.5));
        CMD_ASSERT(!poly.isInsideDomain(3.5));

        const auto solvedX = poly.solveForX(12.0, 0.0, 3.0, 2.1);
        CMD_ASSERT(solvedX.has_value());
        CMD_ASSERT_NEAR(*solvedX, 2.0, 1e-5);

        // Exponencial de Abrams exata: y = 110 / (12^x)
        std::vector<Point2D> expPts;
        for (double x : {0.38, 0.48, 0.58}) {
            expPts.push_back({x, 110.0 / std::pow(12.0, x)});
        }
        const auto abramsExp = RegressionEngine::fitAbramsExponential(expPts);
        CMD_ASSERT(abramsExp.isValid);
        CMD_ASSERT_NEAR(abramsExp.k1, 110.0, 1e-4);
        CMD_ASSERT_NEAR(abramsExp.k2, 12.0, 1e-4);
        CMD_ASSERT_NEAR(abramsExp.rSquared, 1.0, 1e-6);
        const auto expX = abramsExp.solveForX(abramsExp.evaluate(0.48));
        CMD_ASSERT(expX.has_value());
        CMD_ASSERT_NEAR(*expX, 0.48, 1e-5);

        // Consumo teórico por volumes absolutos
        const double cTheo = RegressionEngine::calculateTheoreticalConsumption(
            2.12, 2.88, 0.50, 2.96, 2.60, 2.70, 1.0);
        const double expectedC = 1000.0 / ((1.0 / 2.96) + (2.12 / 2.60) + (2.88 / 2.70) + (0.50 / 1.0));
        CMD_ASSERT_NEAR(cTheo, expectedC, 1e-6);

        // Guardas contra entrada insuficiente / degenerada (zero NaN/crash)
        const auto emptyPoly = RegressionEngine::fitPolynomial2({});
        CMD_ASSERT(!emptyPoly.isValid);
        const auto singleLin = RegressionEngine::fitLinear({{1.0, 2.0}});
        CMD_ASSERT(!singleLin.isValid);
    });

    runner.add("MixDesignCalculatorUnitPricingAndBinaryBlendFullFlow", []() {
        CMD_ASSERT_NEAR(MixDesignCalculator::getCementPricePerKg(0.80, PriceUnit::PerKg), 0.80, 1e-9);
        CMD_ASSERT_NEAR(MixDesignCalculator::getCementPricePerKg(40.0, PriceUnit::PerBag50kg), 0.80, 1e-9);
        CMD_ASSERT_NEAR(MixDesignCalculator::getCementPricePerKg(800.0, PriceUnit::PerTon), 0.80, 1e-9);

        const double sandPerM3 = MixDesignCalculator::getAggregatePricePerM3(140.0, PriceUnit::PerM3, 1.40);
        const double sandPerKg = MixDesignCalculator::getAggregatePricePerKg(140.0, PriceUnit::PerM3, 1.40);
        CMD_ASSERT_NEAR(sandPerM3, 140.0, 1e-9);
        CMD_ASSERT_NEAR(sandPerKg, 0.10, 1e-9);

        // Cenário 5 (UFPR TC031)
        ProjectData ufpr;
        ufpr.loadPresetPlanilhaUFPR();
        const MixDesignResult resUfpr = ufpr.calculate();
        CMD_ASSERT(resUfpr.hasValidAnalyticalSolution);
        CMD_ASSERT_NEAR(resUfpr.fcj, 30.0 + 1.65 * 3.5, 1e-6);
        CMD_ASSERT(resUfpr.targetWaterCement > 0.30 && resUfpr.targetWaterCement < 0.70);
        CMD_ASSERT(resUfpr.targetDryMixM > 3.0 && resUfpr.targetDryMixM < 9.0);
        CMD_ASSERT_NEAR(resUfpr.sandRatioA + resUfpr.gravelRatioP, resUfpr.targetDryMixM, 1e-4);
        CMD_ASSERT(resUfpr.perM3Analytical.cementKg > 250.0);
        CMD_ASSERT_NEAR(resUfpr.jobsiteBatch50kg.cementKg, 50.0, 1e-6);
        CMD_ASSERT(resUfpr.jobsiteBatch50kg.sandPadiolaHeightCm > 0.0);
        CMD_ASSERT_NEAR(resUfpr.projectTotalAnalytical.cementKg,
                        ufpr.projectVolumeM3 * resUfpr.perM3Analytical.cementKg, 0.5);

        // Cenário 2 (Mistura Binária Brita 1 + Brita 2)
        ProjectData marine;
        marine.loadPreset(2);
        marine.useGravelBlend = true;
        marine.gravel2Percent = 65.0;
        const MixDesignResult resMarine = marine.calculate();
        CMD_ASSERT(resMarine.hasValidAnalyticalSolution);
        CMD_ASSERT(resMarine.perM3Analytical.gravel1WetKg > 0.0);
        CMD_ASSERT(resMarine.perM3Analytical.gravel2WetKg > 0.0);
        CMD_ASSERT_NEAR(
            resMarine.perM3Analytical.gravel1DryKg + resMarine.perM3Analytical.gravel2DryKg,
            resMarine.perM3Analytical.gravelDryKg, 1.0);
    });

    return runner.runAll();
}
