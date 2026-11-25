#undef NDEBUG
#include <iostream>
#include <iomanip>
#include <cassert>
#include <cmath>
#include <QCoreApplication>
#include <QJsonObject>
#include "source/core/ProjectData.hpp"
#include "source/core/MixDesignCalculator.hpp"
#include "source/core/AggregateOptimizer.hpp"
#include "source/core/MortarContentOptimizer.hpp"
#include "source/core/NormativeDatabase.hpp"

using namespace ConcreteMix;

void printSeparator() {
    std::cout << "========================================================================\n";
}

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    NormativeDatabase::instance().initialize();
    printSeparator();
    std::cout << "VALIDACAO DOS CALCULOS DO METODO IPT/EPUSP E CASOS DA PLANILHA EXCEL\n";
    printSeparator();

    // -------------------------------------------------------------
    // CENÁRIO 0: Personalizável pelo usuário (Valores em branco/zerados)
    // -------------------------------------------------------------
    {
        std::cout << "\n>>> TESTANDO CENÁRIO 0: Personalizável pelo usuário (Valores em branco)\n";
        ProjectData p;
        p.loadPresetUserCustom();
        assert(p.fck == 0.0);
        assert(p.customSd == 0.0);
        assert(p.targetSlumpMm == 0.0);
        assert(p.selectedAlphaPercent == 0.0);
        assert(!p.getDisplayProjectName(false).empty());
        assert(!p.getDisplayProjectName(true).empty());
        assert(p.experimentalMixes.size() == 3);
        assert(p.experimentalMixes[0].dryRatioM == 0.0);

        MixDesignResult res = p.calculate();
        assert(res.fcj == 0.0);
        assert(res.targetWaterCement == 0.0);
        assert(res.targetDryMixM == 0.0);
        assert(res.targetCementConsumption == 0.0);
        assert(!res.compliance.isCompliant);

        std::string html = p.generateHtmlReport(res, false, false);
        assert(!html.empty());
        assert(html.find("Aguardando Preenchimento dos Dados de Entrada") != std::string::npos);

        std::string md = p.generateMarkdownReport(res, false);
        assert(!md.empty());
        assert(md.find("Aguardando preenchimento dos dados de entrada") != std::string::npos);

        std::cout << "Cenário 0 verificado com sucesso: dados limpos e relatórios protegidos contra divisão por zero!\n";
    }

    // -------------------------------------------------------------
    // CENÁRIO 1: Concreto C25 Urbano (CAA II)
    // -------------------------------------------------------------
    {
        std::cout << "\n>>> TESTANDO CENÁRIO 1: Concreto Estrutural C25 Urbano (CAA II)\n";
        ProjectData p;
        p.loadPresetReference0();
        MixDesignResult res = p.calculate();

        std::cout << "fcj calculada: " << res.fcj << " MPa (Esperado: 31.60 MPa)\n";
        assert(std::abs(res.fcj - 31.60) < 0.05);

        std::cout << "Abrams Poly: R2 = " << res.abramsPoly.rSquared 
                  << ", a=" << res.abramsPoly.a << ", b=" << res.abramsPoly.b << ", c=" << res.abramsPoly.c << "\n";
        assert(res.abramsPoly.rSquared > 0.99);

        std::cout << "Lyse Linear: R2 = " << res.lyseLinear.rSquared 
                  << ", a=" << res.lyseLinear.a << ", b=" << res.lyseLinear.b << "\n";
        assert(res.lyseLinear.rSquared > 0.98);

        std::cout << "Molinari Poly: R2 = " << res.molinariPoly.rSquared 
                  << ", a=" << res.molinariPoly.a << ", b=" << res.molinariPoly.b << ", c=" << res.molinariPoly.c << "\n";
        assert(res.molinariPoly.rSquared > 0.99);

        std::cout << "Analitico: a/c = " << res.targetWaterCement 
                  << ", m = " << res.targetDryMixM 
                  << ", C = " << res.targetCementConsumption << " kg/m3\n";
        std::cout << "Grafico: a/c = " << res.graphicalWaterCement 
                  << ", m = " << res.graphicalDryMixM 
                  << ", C = " << res.graphicalCementConsumption << " kg/m3\n";

        std::cout << "Traco Seco Analitico: 1 : " << res.sandRatioA << " : " << res.gravelRatioP 
                  << " (m = " << res.targetDryMixM << ")\n";
        std::cout << "Traco Seco Grafico: 1 : " << res.graphSandRatioA << " : " << res.graphGravelRatioP 
                  << " (m = " << res.graphicalDryMixM << ")\n";

        std::cout << "Padiola Areia: h = " << res.jobsiteBatch50kg.sandPadiolaHeightCm << " cm\n";
        std::cout << "Padiola Brita: h = " << res.jobsiteBatch50kg.gravelPadiolaHeightCm << " cm (" 
                  << res.jobsiteBatch50kg.gravelPadiolaCount << " padiolas)\n";
        std::cout << "Agua canteiro p/ 50kg: " << res.jobsiteBatch50kg.waterLiters << " L\n";

        std::cout << "NBR 12655 Conforme: " << (res.compliance.isCompliant ? "SIM" : "NAO") << "\n";
        for (const auto& err : res.compliance.errors) {
            std::cout << "  [ERRO]: " << err << "\n";
        }
        for (const auto& w : res.compliance.warnings) {
            std::cout << "  [AVISO]: " << w << "\n";
        }
        for (const auto& p : res.compliance.passedChecks) {
            std::cout << "  [OK]: " << p << "\n";
        }
        assert(res.compliance.isCompliant);
    }

    // -------------------------------------------------------------
    // CENÁRIO 2: Concreto C30 Marinho (CAA III - Britas 1+2)
    // -------------------------------------------------------------
    {
        std::cout << "\n>>> TESTANDO CENÁRIO 2: Concreto Estrutural C30 Marinho (CAA III)\n";
        ProjectData p;
        p.loadPresetResultados1();
        MixDesignResult res = p.calculate();

        std::cout << "fcj calculada: " << res.fcj << " MPa (Esperado: 36.60 MPa)\n";
        assert(std::abs(res.fcj - 36.60) < 0.05);

        std::cout << "Analitico: a/c = " << res.targetWaterCement 
                  << ", m = " << res.targetDryMixM 
                  << ", C = " << res.targetCementConsumption << " kg/m3\n";
        std::cout << "Betoneira (" << res.labMixerVolumeLiters << " L): Cimento = " 
                  << res.labBatch.cementKg << " kg, Areia Seca = " << res.labBatch.sandDryKg 
                  << " kg, Brita 1 = " << res.labBatch.gravel1DryKg << " kg, Brita 2 = " << res.labBatch.gravel2DryKg << " kg\n";
        assert(res.labBatch.cementKg > 0.0);
        assert(res.compliance.isCompliant);
    }

    // -------------------------------------------------------------
    // CENÁRIO 3: Concreto C40 Especial (CAA IV)
    // -------------------------------------------------------------
    {
        std::cout << "\n>>> TESTANDO CENÁRIO 3: Concreto C40 Alto Desempenho (CAA IV)\n";
        ProjectData p;
        p.loadPresetResultados2();
        MixDesignResult res = p.calculate();

        std::cout << "fcj calculada: " << res.fcj << " MPa (Esperado: 46.60 MPa)\n";
        assert(std::abs(res.fcj - 46.60) < 0.05);

        std::cout << "Volume de Obra: " << res.projectVolumeM3 << " m3\n";
        std::cout << "Custo Obra Analitico: R$ " << res.projectCostAnalytical.totalCost << "\n";
        std::cout << "Custo Obra Grafico: R$ " << res.projectCostGraphical.totalCost << "\n";
        std::cout << "Diferenca de Custo: R$ " << res.projectCostDiff 
                  << " (Diferenca de Sacos: " << res.diffCementBags << " sacos)\n";
        assert(res.projectCostAnalytical.totalCost > 0.0);
        assert(res.compliance.isCompliant);
    }

    // -------------------------------------------------------------
    // CENÁRIO 4: Concreto C20 Fundações (Condição B - CAA I)
    // -------------------------------------------------------------
    {
        std::cout << "\n>>> TESTANDO CENÁRIO 4: Concreto C20 Fundações (Condição B)\n";
        ProjectData p;
        p.loadPresetResultados3();
        MixDesignResult res = p.calculate();

        std::cout << "fcj calculada: " << res.fcj << " MPa (Esperado: 29.08 MPa)\n";
        assert(std::abs(res.fcj - 29.08) < 0.05);

        std::cout << "Volume de Obra: " << res.projectVolumeM3 << " m3\n";
        std::cout << "Custo Obra Analitico: R$ " << res.projectCostAnalytical.totalCost << "\n";
        std::cout << "Custo Obra Grafico: R$ " << res.projectCostGraphical.totalCost << "\n";
        std::cout << "Diferenca de Custo: R$ " << res.projectCostDiff 
                  << " (Diferenca de Sacos: " << res.diffCementBags << " sacos)\n";
        assert(res.projectCostAnalytical.totalCost > 0.0);
        assert(res.compliance.isCompliant);
    }

    // -------------------------------------------------------------
    // TESTE DE OTIMIZACAO DE MISTURA DE BRITAS (NBR 7211)
    // -------------------------------------------------------------
    {
        std::cout << "\n>>> TESTANDO OTIMIZACAO DE MISTURA DE BRITAS\n";
        std::vector<double> netMasses = {23.2, 23.5, 24.1, 24.5, 23.8, 23.0};
        auto blendRows = AggregateOptimizer::calculateBlendTable(100.0, netMasses, 15.04);
        std::cout << "Linhas geradas: " << blendRows.size() << "\n";
        assert(blendRows.size() == 6);
        auto [optB2, optB1] = AggregateOptimizer::getOptimumProportions(blendRows);
        std::cout << "Proporcao otima: B2=" << optB2 << "%, B1=" << optB1 << "%\n";
        assert(optB2 > 0.0 && optB1 >= 0.0);
    }

    // -------------------------------------------------------------
    // TESTE DE OTIMIZACAO DO TEOR DE ARGAMASSA (ALFA)
    // -------------------------------------------------------------
    {
        std::cout << "\n>>> TESTANDO VARIACAO DO TEOR DE ARGAMASSA (40% a 68%)\n";
        auto alphaRows = MortarContentOptimizer::generateTable(5.0, 15.0, 4.0, 52.0);
        std::cout << "Linhas de argamassa geradas: " << alphaRows.size() << "\n";
        assert(alphaRows.size() == 15);
    }

    // -------------------------------------------------------------
    // TESTE DO BANCO DE DADOS NORMATIVO (NBR 16697, 7211, 7212, 8953, 6118, 12655)
    // -------------------------------------------------------------
    {
        std::cout << "\n>>> TESTANDO BANCO DE DADOS NORMATIVO SQLITE\n";
        auto& db = NormativeDatabase::instance();
        assert(db.isInitialized());

        auto cements = db.getAllCementTypes();
        std::cout << "Tipos de cimento cadastrados: " << cements.size() << "\n";
        assert(cements.size() >= 10);

        auto cpiif = db.getCementByCode("CP II-F");
        assert(!cpiif.code.isEmpty());
        std::cout << "Cimento verificado: " << cpiif.code.toStdString() << " (" << cpiif.name.toStdString() << ") - PE: " << cpiif.specificMass << " kg/dm³\n";

        auto aggs = db.getAllAggregates();
        std::cout << "Agregados normativos cadastrados: " << aggs.size() << "\n";
        assert(aggs.size() >= 8);

        auto b1 = db.getAggregateByCode("B1");
        assert(!b1.code.isEmpty());
        std::cout << "Brita 1 verificada: DMC = " << b1.dmcMm << " mm, PE = " << b1.specificMass << " kg/dm³\n";

        auto slumps = db.getAllSlumpClasses();
        std::cout << "Classes de abatimento cadastradas: " << slumps.size() << "\n";
        assert(slumps.size() == 5);

        auto s100 = db.getSlumpByValue(120.0);
        assert(s100.slumpClass == "S100");
        std::cout << "Slump 120 mm classificado: " << s100.slumpClass.toStdString() << " (" << s100.placementMethod.toStdString() << ")\n";

        auto tolerances = db.getAllWeighingTolerances();
        std::cout << "Tolerâncias de pesagem (NBR 7212:2025): " << tolerances.size() << "\n";
        assert(tolerances.size() == 4);

        // Verificação DMC vs Cobrimento (NBR 6118)
        QString diag;
        bool okDmc = db.verifyDmcAgainstCover(19.0, 25.0, diag); // 19 <= 1.2 * 25 = 30 -> OK
        std::cout << "Verificacao DMC 19 mm vs Cobrimento 25 mm: " << (okDmc ? "CONFORME" : "NAO CONFORME") << " (" << diag.toStdString() << ")\n";
        assert(okDmc);

        bool failDmc = db.verifyDmcAgainstCover(38.0, 20.0, diag); // 38 > 1.2 * 20 = 24 -> NOT OK
        std::cout << "Verificacao DMC 38 mm vs Cobrimento 20 mm: " << (failDmc ? "CONFORME" : "NAO CONFORME") << " (" << diag.toStdString() << ")\n";
        assert(!failDmc);

        // Verificação de intervalos contínuos de Slump (NBR 8953:2015 Tabela 2)
        assert(db.getSlumpByValue(45.0).slumpClass == "S10");
        assert(db.getSlumpByValue(95.0).slumpClass == "S50");
        assert(db.getSlumpByValue(155.0).slumpClass == "S100");
        assert(db.getSlumpByValue(215.0).slumpClass == "S160");
        assert(db.getSlumpByValue(230.0).slumpClass == "S220");
    }

    // -------------------------------------------------------------
    // CENÁRIO 5: Planilha UFPR TC031 — Dados Reais Auditados e Corrigidos
    // -------------------------------------------------------------
    {
        std::cout << "\n>>> TESTANDO CENÁRIO 5: Planilha UFPR TC031 (Dados Reais Auditados)\n";
        ProjectData p;
        p.loadPresetPlanilhaUFPR();

        // Verifica cálculo da betonada piloto pela Lei dos Volumes Absolutos ('Memorial de cálculo'!I138..I360)
        // Traço piloto 1:5 (m=5.0), α0=40% (a0=1.4, b0=3.6), a/c0=0.50, γc=2.96, γa=2.60, γb=2.70
        auto pilot32 = MortarContentOptimizer::calculatePilotBatchFromVolume(
            32.515592515592516, 5.0, 40.0, 0.50, 2.96, 2.60, 2.70);
        std::cout << "Betonada piloto (32.5156 L - Referencia 0): mc=" << pilot32.cementMassKg
                  << " kg, mb=" << pilot32.gravelMassKg << " kg, mw=" << pilot32.waterMassKg << " kg\n";
        assert(std::abs(pilot32.cementMassKg - 12.00) < 0.01);
        assert(std::abs(pilot32.gravelMassKg - 43.20) < 0.01);
        assert(std::abs(pilot32.waterMassKg - 6.00) < 0.01);

        auto pilot29 = MortarContentOptimizer::calculatePilotBatchFromVolume(
            29.0, 5.0, 40.0, 0.50, 2.96, 2.60, 2.70);
        std::cout << "Betonada piloto (29.0 L - Resultados 1): mc=" << pilot29.cementMassKg
                  << " kg, mb=" << pilot29.gravelMassKg << " kg\n";
        assert(std::abs(pilot29.cementMassKg - 10.7026) < 0.01);
        assert(std::abs(pilot29.gravelMassKg - 38.5292) < 0.01);

        auto pilot90 = MortarContentOptimizer::calculatePilotBatchFromVolume(
            90.0, 5.0, 40.0, 0.50, 2.96, 2.60, 2.70);
        std::cout << "Betonada piloto (90.0 L - Resultados 2): mc=" << pilot90.cementMassKg
                  << " kg, mb=" << pilot90.gravelMassKg << " kg\n";
        assert(std::abs(pilot90.cementMassKg - 33.2148) < 0.01);
        assert(std::abs(pilot90.gravelMassKg - 119.5734) < 0.01);

        auto pilot100 = MortarContentOptimizer::calculatePilotBatchFromVolume(
            100.0, 5.0, 40.0, 0.50, 2.96, 2.60, 2.70);
        std::cout << "Betonada piloto (100.0 L - Resultados 3): mc=" << pilot100.cementMassKg
                  << " kg, mb=" << pilot100.gravelMassKg << " kg\n";
        assert(std::abs(pilot100.cementMassKg - 36.9054) < 0.01);
        assert(std::abs(pilot100.gravelMassKg - 132.8593) < 0.01);

        // Verifica tabela de variação de argamassa da Planilha UFPR ('Referência (0)'!B60:L74: m=5.0, mb=43.20 kg, mw=6.00 kg)
        auto ufprAlphaRows = MortarContentOptimizer::generateTable(5.0, 43.20, 6.00, 52.0);
        assert(ufprAlphaRows.size() == 15);
        assert(std::abs(ufprAlphaRows[0].totalCementMass - 12.00) < 0.01);
        assert(std::abs(ufprAlphaRows[0].totalSandMass - 16.80) < 0.01);
        assert(std::abs(ufprAlphaRows[6].alphaPercent - 52.0) < 0.01);
        assert(std::abs(ufprAlphaRows[6].totalCementMass - 15.00) < 0.01);
        assert(std::abs(ufprAlphaRows[6].totalSandMass - 31.80) < 0.01);
        assert(std::abs(ufprAlphaRows[6].incrementalSand - 3.00) < 0.01);
        assert(std::abs(ufprAlphaRows[6].incrementalCement - 0.60) < 0.01);

        MixDesignResult res = p.calculate();
        std::cout << "UFPR Analitico: fcj=" << res.fcj
                  << " MPa, a/c=" << res.targetWaterCement
                  << ", m=" << res.targetDryMixM
                  << ", C=" << res.targetCementConsumption << " kg/m3\n";
        assert(std::abs(res.fcj - 35.775) < 0.01);
        assert(std::abs(res.targetWaterCement - 0.411294) < 0.005);
        assert(std::abs(res.targetDryMixM - 5.508303) < 0.02);
        assert(std::abs(res.targetCementConsumption - 350.0) < 25.0);
        assert(res.compliance.isCompliant);

        // Verifica precificação inteligente do cimento (R$/kg, R$/saco 50kg, R$/t)
        assert(std::abs(MixDesignCalculator::getCementPricePerKg(0.42) - 0.42) < 1e-6);
        assert(std::abs(MixDesignCalculator::getCementPricePerKg(21.00) - 0.42) < 1e-6);
        assert(std::abs(MixDesignCalculator::getCementPricePerKg(420.00) - 0.42) < 1e-6);
    }

    // -------------------------------------------------------------
    // TESTES NORMATIVOS RIGOROSOS (NBR 7212:2025, NBR 6118:2023, NBR 12655:2022)
    // -------------------------------------------------------------
    {
        std::cout << "\n>>> TESTANDO REGRAS NORMATIVAS (NBR 7212:2025, NBR 6118:2023, NBR 12655:2022)\n";
        assert(std::abs(ConcreteStandards::BatchingTolerances::CementTolerancePercent - 1.0) < 1e-6);
        assert(std::abs(ConcreteStandards::BatchingTolerances::AggregateTolerancePercent - 3.0) < 1e-6);
        assert(std::abs(ConcreteStandards::BatchingTolerances::WaterTolerancePercent - 3.0) < 1e-6);
        assert(std::abs(ConcreteStandards::BatchingTolerances::AdmixtureTolerancePercent - 5.0) < 1e-6);

        // NBR 6118:2023 Tabela 7.2 (Nota a): CP = CA + 5 mm
        auto reqCA = ConcreteStandards::getDurabilityRequirement(EnvironmentalClass::CAA_II, StructuralType::ReinforcedConcrete);
        auto reqCP = ConcreteStandards::getDurabilityRequirement(EnvironmentalClass::CAA_II, StructuralType::PrestressedConcrete);
        assert(std::abs(reqCA.nominalCoverSlabs - 25.0) < 1e-6);
        assert(std::abs(reqCA.nominalCoverBeamsCols - 30.0) < 1e-6);
        assert(std::abs(reqCP.nominalCoverSlabs - 30.0) < 1e-6);
        assert(std::abs(reqCP.nominalCoverBeamsCols - 35.0) < 1e-6);

        // NBR 12655:2022 item 5.4.2 / 5.6.3.1: Condição B vedada para fck > 20 MPa
        auto compB_fail = ConcreteStandards::validateMixDesign(
            30.0, 0.50, 320.0, EnvironmentalClass::CAA_II, StructuralType::ReinforcedConcrete,
            100.0, 19.0, 30.0, PreparationCondition::ConditionB);
        assert(!compB_fail.isCompliant);

        // NBR 12655:2022 item 5.4.3 / 5.6.3.1: Condição C vedada para fck > 15 MPa
        auto compC_fail = ConcreteStandards::validateMixDesign(
            20.0, 0.60, 300.0, EnvironmentalClass::CAA_I, StructuralType::ReinforcedConcrete,
            100.0, 19.0, 25.0, PreparationCondition::ConditionC);
        assert(!compC_fail.isCompliant);

        // Round-trip de serialização JSON completa
        ProjectData orig;
        orig.loadPresetPlanilhaUFPR();
        orig.usePresetProjectName = false;
        orig.customProjectName = "Obra Ponte Estaiada C40";
        orig.engineerName = "Eng. Civil Fernando Nillsson Cidade";
        orig.institution = "UFPR - Engenharia Civil";
        orig.studentId = "GRR20182393";
        std::string jsonStr = orig.toJson();
        ProjectData loaded;
        bool okJson = loaded.fromJson(jsonStr);
        assert(okJson);
        assert(std::abs(loaded.pilotM - orig.pilotM) < 1e-6);
        assert(std::abs(loaded.pilotBatchGravelKg - orig.pilotBatchGravelKg) < 1e-4);
        assert(std::abs(loaded.baseGravelMassKg - orig.baseGravelMassKg) < 1e-4);
        assert(loaded.experimentalMixes.size() == orig.experimentalMixes.size());
        assert(loaded.usePresetProjectName == false);
        assert(loaded.customProjectName == "Obra Ponte Estaiada C40");
        assert(loaded.getDisplayProjectName(false) == "Obra Ponte Estaiada C40");
        assert(loaded.getDisplayProjectName(true) == "Obra Ponte Estaiada C40");

        // Verificação de tradução dinâmica em tempo de execução (Cenários 0 a 5 e Relatório Técnico)
        ProjectData dynProj;
        dynProj.loadPresetPlanilhaUFPR();
        assert(dynProj.currentPresetIndex == 5);
        assert(dynProj.usePresetProjectName == true);
        std::string ptName5 = dynProj.getDisplayProjectName(false);
        std::string enName5 = dynProj.getDisplayProjectName(true);
        assert(ptName5.find("Cenário 5: Planilha UFPR Original Corrigida") != std::string::npos);
        assert(enName5.find("Scenario 5: Corrected Original UFPR Spreadsheet") != std::string::npos);

        MixDesignResult dynRes = dynProj.calculate();
        std::string mdPt = dynProj.generateMarkdownReport(dynRes, false);
        std::string mdEn = dynProj.generateMarkdownReport(dynRes, true);
        assert(mdPt.find(ptName5) != std::string::npos);
        assert(mdEn.find(enName5) != std::string::npos);

        std::string htmlPt = dynProj.generateHtmlReport(dynRes, false, false);
        std::string htmlEn = dynProj.generateHtmlReport(dynRes, false, true);
        assert(htmlPt.find(ptName5) != std::string::npos);
        assert(htmlEn.find(enName5) != std::string::npos);

        // Validação rigorosa do símbolo monetário dinâmico por idioma (R$ em pt_BR e $ em en_US)
        assert(mdPt.find("R$ ") != std::string::npos);
        assert(mdEn.find("$ ") != std::string::npos);
        assert(htmlPt.find("R$ ") != std::string::npos);
        assert(htmlEn.find("$ ") != std::string::npos);
    }

    // =============================================================
    // FRENTE 1: ROBUSTEZ NUMÉRICA E REGRESSÕES (CENTRAMENTO & ESCALONAMENTO)
    // =============================================================
    {
        std::cout << "\n>>> FRENTE 1: TESTANDO ROBUSTEZ NUMÉRICA E REGRESSÕES MATEMÁTICAS\n";

        // 1.1 Regressão polinomial de 2º grau centrada e escalonada com dados reais
        std::vector<Point2D> ptsData = {{0.440, 40.5}, {0.520, 32.8}, {0.620, 25.2}};
        auto poly = RegressionEngine::fitPolynomial2(ptsData);
        assert(poly.isValid);
        assert(poly.rSquared > 0.99);
        assert(poly.minX == 0.440);
        assert(poly.maxX == 0.620);
        assert(poly.isInsideDomain(0.50));
        assert(!poly.isInsideDomain(0.30));
        assert(!poly.isInsideDomain(0.70));

        // Avaliação no ponto médio
        double yMid = poly.evaluate(0.520);
        assert(std::abs(yMid - 32.8) < 0.2);

        // 1.2 Tratamento robusto de dados degenerados / singulares (pontos idênticos em X)
        std::vector<Point2D> ptsSingular = {{0.50, 30.0}, {0.50, 31.0}, {0.50, 29.0}};
        auto polySingular = RegressionEngine::fitPolynomial2(ptsSingular);
        assert(!polySingular.isValid); // Deve sinalizar degenerescência sem crashar ou produzir NaN

        auto linearSingular = RegressionEngine::fitLinear(ptsSingular);
        assert(!linearSingular.isValid);

        // 1.3 Bloqueio de conformidade quando Abrams não encontra solução analítica válida
        ProjectData pFail;
        pFail.loadPresetReference0();
        // Força um fck impossivelmente alto (150 MPa) fora da curva experimental
        pFail.fck = 150.0;
        pFail.customSd = 4.0;
        MixDesignResult resFail = pFail.calculate();
        std::cout << "fck extremo (150 MPa): waterCementSolutionType = " 
                  << static_cast<int>(resFail.waterCementSolutionType)
                  << ", hasValidAnalyticalSolution = " << resFail.hasValidAnalyticalSolution
                  << ", isCompliant = " << resFail.compliance.isCompliant << "\n";
        assert(resFail.waterCementSolutionType == SolutionType::ExperimentalAverage ||
               resFail.waterCementSolutionType == SolutionType::NoSolution);
        assert(!resFail.hasValidAnalyticalSolution);
        assert(!resFail.compliance.isCompliant); // Bloqueio estrito de conformidade!
        assert(resFail.isExtrapolated);

        // Verifica que o relatório destaca o erro crítico
        std::string failMd = pFail.generateMarkdownReport(resFail, false);
        assert(failMd.find("MÉDIA EXPERIMENTAL") != std::string::npos || failMd.find("FALHA") != std::string::npos);

        std::cout << "Frente 1 aprovada: centramento, escalonamento, singularidades e bloqueio de conformidade validados!\n";
    }

    // =============================================================
    // FRENTE 2: TESTES NORMATIVOS DE CASOS-LIMITE (ABAIXO, NO LIMITE, ACIMA)
    // =============================================================
    {
        std::cout << "\n>>> FRENTE 2: TESTES DE LIMITES NORMATIVOS (ABAIXO, NO LIMITE, ACIMA)\n";

        // 2.1 Limites de relação água/cimento (CAA II, CA: limite = 0.60)
        // a) Imediatamente abaixo (0.599): Conforme
        auto vAc_pass = ConcreteStandards::validateMixDesign(
            25.0, 0.599, 285.0, EnvironmentalClass::CAA_II, StructuralType::ReinforcedConcrete,
            100.0, 19.0, 30.0, PreparationCondition::ConditionA);
        assert(vAc_pass.isCompliant);

        // b) Exatamente no limite (0.600): Conforme
        auto vAc_exact = ConcreteStandards::validateMixDesign(
            25.0, 0.600, 285.0, EnvironmentalClass::CAA_II, StructuralType::ReinforcedConcrete,
            100.0, 19.0, 30.0, PreparationCondition::ConditionA);
        assert(vAc_exact.isCompliant);

        // c) Imediatamente acima (0.601): Não Conforme
        auto vAc_fail = ConcreteStandards::validateMixDesign(
            25.0, 0.601, 285.0, EnvironmentalClass::CAA_II, StructuralType::ReinforcedConcrete,
            100.0, 19.0, 30.0, PreparationCondition::ConditionA);
        assert(!vAc_fail.isCompliant);

        // 2.2 Limites de Consumo de Cimento (CAA II: mínimo = 280 kg/m³)
        // a) 279 kg/m³: Não Conforme
        auto vC_fail = ConcreteStandards::validateMixDesign(
            25.0, 0.50, 279.0, EnvironmentalClass::CAA_II, StructuralType::ReinforcedConcrete,
            100.0, 19.0, 30.0, PreparationCondition::ConditionA);
        assert(!vC_fail.isCompliant);

        // b) 280 kg/m³: Conforme
        auto vC_exact = ConcreteStandards::validateMixDesign(
            25.0, 0.50, 280.0, EnvironmentalClass::CAA_II, StructuralType::ReinforcedConcrete,
            100.0, 19.0, 30.0, PreparationCondition::ConditionA);
        assert(vC_exact.isCompliant);

        // 2.3 Slump e tolerâncias de entrega (NBR 8953:2015 e NBR 7212:2025)
        // Slump 100 mm (Classe S100: tolerância de entrega = +- 20 mm => [80, 120] mm)
        auto vSlump_pass = ConcreteStandards::validateMixDesign(
            25.0, 0.50, 290.0, EnvironmentalClass::CAA_II, StructuralType::ReinforcedConcrete,
            100.0, 19.0, 30.0, PreparationCondition::ConditionA);
        assert(vSlump_pass.isCompliant);

        // Slump 0 ou negativo: Não Conforme
        auto vSlump_zero = ConcreteStandards::validateMixDesign(
            25.0, 0.50, 290.0, EnvironmentalClass::CAA_II, StructuralType::ReinforcedConcrete,
            0.0, 19.0, 30.0, PreparationCondition::ConditionA);
        assert(!vSlump_zero.isCompliant);

        // 2.4 Diferenciação de Cobrimento: Vigas/Pilares vs Lajes (NBR 6118:2023 item 7.4.7.6)
        // Em CAA I (Concreto Armado):
        // cnom_laje = 20 mm => 1.2 * cnom = 24.0 mm
        // cnom_viga = 25 mm => 1.2 * cnom = 30.0 mm
        // Se DMC = 25.0 mm (Brita 2):
        // Atende Vigas/Pilares (25.0 <= 30.0 mm), mas NÃO atende Lajes (25.0 > 24.0 mm)!
        ProjectData pDmc;
        pDmc.envClass = EnvironmentalClass::CAA_I;
        pDmc.structType = StructuralType::ReinforcedConcrete;
        pDmc.gravel1DmcMm = 25.0; // Brita 2
        pDmc.useGravelBlend = false;

        QString dmcDiagnostic;
        bool overallDmc = pDmc.checkDmcCoverCompliance(dmcDiagnostic);
        std::cout << "DMC = 25mm em CAA I: overall = " << overallDmc 
                  << ", Diagnostic: " << dmcDiagnostic.toStdString() << "\n";
        assert(!overallDmc); // Falha nas lajes!
        assert(dmcDiagnostic.contains("Lajes"));

        std::cout << "Frente 2 aprovada: limites normativos finos e distinção vigas vs lajes validados!\n";
    }

    // =============================================================
    // FRENTE 3: TESTES DE CONSISTÊNCIA FÍSICA E BALANÇO DE MASSAS
    // =============================================================
    {
        std::cout << "\n>>> FRENTE 3: TESTES DE CONSISTÊNCIA FÍSICA E BALANÇO DE MASSAS\n";

        ProjectData p;
        p.loadPresetReference0();
        MixDesignResult res = p.calculate();

        // 3.1 Balanço estrito de massa no metro cúbico (Massa Seca + Água Teórica == Massa Fresca Teórica)
        double dryTotal = res.perM3Analytical.cementKg + res.perM3Analytical.sandDryKg + 
                          res.perM3Analytical.gravelDryKg;
        double theoreticalWater = res.perM3Analytical.waterTheoreticalL;
        double theoreticalTotalFresh = dryTotal + theoreticalWater;

        // Massa úmida corrigida de areia e brita
        double sandMoistKg = res.perM3Analytical.sandWetKg;
        double gravelMoistKg = res.perM3Analytical.gravelWetKg;
        double effectiveWater = res.perM3Analytical.waterCorrectedL;
        double wetTotalFresh = res.perM3Analytical.cementKg + sandMoistKg + gravelMoistKg + effectiveWater;

        std::cout << "Balanço 1 m³: Teórico = " << theoreticalTotalFresh 
                  << " kg, Real Úmido = " << wetTotalFresh << " kg\n";
        assert(std::abs(theoreticalTotalFresh - wetTotalFresh) < 1e-4);
        assert(std::abs(res.perM3Analytical.totalMassKg - wetTotalFresh) < 1e-4);

        // 3.2 Conservação de água trazida pelos agregados
        double waterInSand = res.perM3Analytical.sandDryKg * (p.sand.moisturePercent / 100.0);
        double waterInGravel = res.perM3Analytical.gravelDryKg * (p.gravel1.moisturePercent / 100.0);
        double totalWaterFromAggregates = waterInSand + waterInGravel;
        assert(std::abs(res.perM3Analytical.waterTheoreticalL - (effectiveWater + totalWaterFromAggregates)) < 1e-4);

        // 3.3 Amassada para betoneira (proporcionalidade rigorosa ao volume)
        double ratio = (p.labMixerVolumeL / 1000.0);
        assert(std::abs(res.labBatch.cementKg - (res.perM3Analytical.cementKg * ratio)) < 1e-3);
        assert(std::abs(res.labBatch.sandWetKg - (res.perM3Analytical.sandWetKg * ratio)) < 1e-3);
        assert(std::abs(res.labBatch.gravelWetKg - (res.perM3Analytical.gravelWetKg * ratio)) < 1e-3);
        assert(std::abs(res.labBatch.waterCorrectedL - (res.perM3Analytical.waterCorrectedL * ratio)) < 1e-3);

        // 3.4 Padiola de 50 kg de cimento (Geometria 35 cm x 45 cm)
        double baseAreaDm2 = 3.5 * 4.5; // 15.75 dm² = 15.75 L por 10 cm de altura
        double expectedSandL = res.jobsiteBatch50kg.sandVolumeLiters;
        double expectedHeightCm = (expectedSandL / (baseAreaDm2 * res.jobsiteBatch50kg.sandPadiolaCount)) * 10.0;
        assert(std::abs(res.jobsiteBatch50kg.sandPadiolaHeightCm - expectedHeightCm) < 0.1);

        std::cout << "Frente 3 aprovada: balanço de massas e conservação de umidade verificados rigorosamente!\n";
    }

    // =============================================================
    // FRENTE 4: TESTES DE UNIDADES DE PREÇO EXPLÍCITAS
    // =============================================================
    {
        std::cout << "\n>>> FRENTE 4: TESTES DE CONVERSÃO EXPLÍCITA DE PREÇOS (PriceUnit)\n";

        // Cimento a R$ 40/sc (50 kg) => R$ 0.80/kg
        assert(std::abs(MixDesignCalculator::convertCementPriceToKg(40.0, PriceUnit::PerSack50kg) - 0.80) < 1e-6);
        // Cimento a R$ 800/t => R$ 0.80/kg
        assert(std::abs(MixDesignCalculator::convertCementPriceToKg(800.0, PriceUnit::PerTon) - 0.80) < 1e-6);
        // Cimento a R$ 0.80/kg => R$ 0.80/kg
        assert(std::abs(MixDesignCalculator::convertCementPriceToKg(0.80, PriceUnit::PerKg) - 0.80) < 1e-6);

        // Areia a R$ 74/m³ com massa unitária 1.48 kg/dm³ (= 1480 kg/m³) => 74 / 1480 = R$ 0.05/kg
        assert(std::abs(MixDesignCalculator::convertAggregatePriceToKg(74.0, PriceUnit::PerM3, 1.48) - 0.05) < 1e-6);
        // Areia a R$ 50/t => R$ 0.05/kg
        assert(std::abs(MixDesignCalculator::convertAggregatePriceToKg(50.0, PriceUnit::PerTon, 1.48) - 0.05) < 1e-6);
        // Areia a R$ 0.05/kg => R$ 0.05/kg
        assert(std::abs(MixDesignCalculator::convertAggregatePriceToKg(0.05, PriceUnit::PerKg, 1.48) - 0.05) < 1e-6);

        std::cout << "Frente 4 aprovada: unidades explícitas sem heurísticas frágeis validadas com sucesso!\n";
    }

    // =============================================================
    // FRENTE 5: TESTES DE RASTREABILIDADE E COERÊNCIA DE RELATÓRIOS
    // =============================================================
    {
        std::cout << "\n>>> FRENTE 5: TESTES DE RASTREABILIDADE E COERÊNCIA DE RELATÓRIOS\n";

        ProjectData p;
        p.loadPresetReference0();
        p.projectVolumeM3 = 50.0;
        MixDesignResult res = p.calculate();

        // 5.1 Escalonamento linear dos custos totais com o volume da obra
        assert(std::abs(res.projectCostAnalytical.totalCost - (res.costPerM3Analytical.totalCost * 50.0)) < 0.1);

        // 5.2 Verificação de tags e dados nos relatórios Markdown e HTML
        std::string md = p.generateMarkdownReport(res, false);
        assert(md.find("Diagnóstico de Resolução Matemática e Rastreabilidade") != std::string::npos);
        assert(md.find("Solução Analítica (Equações)") != std::string::npos);
        assert(md.find("Consumo de Materiais por Metro Cúbico") != std::string::npos);
        assert(md.find("STATUS: CONFORME AS NORMAS ABNT NBR 12655:2022") != std::string::npos);

        std::string html = p.generateHtmlReport(res, false, false);
        assert(html.find("Diagnóstico de Resolução Matemática e Rastreabilidade") != std::string::npos);
        assert(html.find("badge-pass") != std::string::npos);
        assert(html.find("fcj de Dosagem:") != std::string::npos);

        std::cout << "Frente 5 aprovada: relatórios exibem diagnósticos matemáticos e coerência dimensional completa!\n";
    }

    printSeparator();
    std::cout << "TODOS OS TESTES ANALITICOS E COMPARATIVOS PASSARAM COM SUCESSO!\n";
    printSeparator();
    return 0;
}
