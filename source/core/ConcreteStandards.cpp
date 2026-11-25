#include "ConcreteStandards.hpp"
#include <cmath>
#include <sstream>
#include <iomanip>

namespace ConcreteMix {

std::string ConcreteStandards::getEnvironmentalClassName(EnvironmentalClass c) {
    switch (c) {
        case EnvironmentalClass::CAA_I:   return "Classe I (Fraca)";
        case EnvironmentalClass::CAA_II:  return "Classe II (Moderada)";
        case EnvironmentalClass::CAA_III: return "Classe III (Forte)";
        case EnvironmentalClass::CAA_IV:  return "Classe IV (Muito Forte)";
    }
    return "Classe I";
}

std::string ConcreteStandards::getEnvironmentalClassDescription(EnvironmentalClass c) {
    switch (c) {
        case EnvironmentalClass::CAA_I:
            return "Ambiente rural ou submerso em água doce. Pouco agressivo às armaduras.";
        case EnvironmentalClass::CAA_II:
            return "Ambiente urbano comum. Agressividade média por carbonatação do concreto.";
        case EnvironmentalClass::CAA_III:
            return "Ambiente marinho ou industrial com névoa salina ou gases ácidos moderados.";
        case EnvironmentalClass::CAA_IV:
            return "Respingos de maré, indústrias químicas severas, tanques industriais e galvanoplastia.";
    }
    return "";
}

std::string ConcreteStandards::getDeteriorationRisk(EnvironmentalClass c) {
    switch (c) {
        case EnvironmentalClass::CAA_I:   return "Insignificante";
        case EnvironmentalClass::CAA_II:  return "Pequeno";
        case EnvironmentalClass::CAA_III: return "Grande";
        case EnvironmentalClass::CAA_IV:  return "Elevado";
    }
    return "Insignificante";
}

DurabilityRequirement ConcreteStandards::getDurabilityRequirement(EnvironmentalClass envClass, StructuralType structType) {
    DurabilityRequirement req;
    req.envClass = envClass;
    req.structType = structType;
    req.envDescription = getEnvironmentalClassDescription(envClass);
    req.riskDescription = getDeteriorationRisk(envClass);

    // ABNT NBR 6118:2023 Tabela 7.2 Nota a: Para concreto protendido (CP), o cobrimento nominal
    // de cabos, bainhas e cordoalhas é acrescido em 5 mm em relação ao concreto armado (CA).
    const double cpCoverBonus = (structType == StructuralType::PrestressedConcrete) ? 5.0 : 0.0;

    switch (envClass) {
        case EnvironmentalClass::CAA_I:
            req.maxWaterCementRatio = (structType == StructuralType::ReinforcedConcrete) ? 0.65 : 0.60;
            req.minConcreteClass = (structType == StructuralType::ReinforcedConcrete) ? "C20" : "C25";
            req.minFck = (structType == StructuralType::ReinforcedConcrete) ? 20.0 : 25.0;
            req.minCementConsumption = 260.0;
            req.nominalCoverSlabs = 20.0 + cpCoverBonus;
            req.nominalCoverBeamsCols = 25.0 + cpCoverBonus;
            break;
        case EnvironmentalClass::CAA_II:
            req.maxWaterCementRatio = (structType == StructuralType::ReinforcedConcrete) ? 0.60 : 0.55;
            req.minConcreteClass = (structType == StructuralType::ReinforcedConcrete) ? "C25" : "C30";
            req.minFck = (structType == StructuralType::ReinforcedConcrete) ? 25.0 : 30.0;
            req.minCementConsumption = 280.0;
            req.nominalCoverSlabs = 25.0 + cpCoverBonus;
            req.nominalCoverBeamsCols = 30.0 + cpCoverBonus;
            break;
        case EnvironmentalClass::CAA_III:
            req.maxWaterCementRatio = (structType == StructuralType::ReinforcedConcrete) ? 0.55 : 0.50;
            req.minConcreteClass = (structType == StructuralType::ReinforcedConcrete) ? "C30" : "C35";
            req.minFck = (structType == StructuralType::ReinforcedConcrete) ? 30.0 : 35.0;
            req.minCementConsumption = 320.0;
            req.nominalCoverSlabs = 35.0 + cpCoverBonus;
            req.nominalCoverBeamsCols = 40.0 + cpCoverBonus;
            break;
        case EnvironmentalClass::CAA_IV:
            req.maxWaterCementRatio = 0.45;
            req.minConcreteClass = "C40";
            req.minFck = 40.0;
            req.minCementConsumption = 360.0;
            req.nominalCoverSlabs = 45.0 + cpCoverBonus;
            req.nominalCoverBeamsCols = 50.0 + cpCoverBonus;
            break;
    }
    return req;
}

double ConcreteStandards::getStandardDeviation(PreparationCondition cond, double customSd) {
    switch (cond) {
        case PreparationCondition::ConditionA: return 4.0;
        case PreparationCondition::ConditionB: return 5.5;
        case PreparationCondition::ConditionC: return 7.0;
        case PreparationCondition::KnownSd:    return std::max(2.0, customSd);
        case PreparationCondition::Arbitrary:  return std::max(0.1, customSd);
    }
    return 4.0;
}

std::string ConcreteStandards::getPreparationConditionDescription(PreparationCondition cond) {
    switch (cond) {
        case PreparationCondition::ConditionA:
            return "Condição A (Sd = 4,0 MPa): Aplicável a todas as classes. Cimento e agregados medidos em massa; água corrigida em função da umidade.";
        case PreparationCondition::ConditionB:
            return "Condição B (Sd = 5,5 MPa): Aplicável às classes C10 a C20. Cimento em massa; água em massa ou volume; agregados em massa combinada com volume.";
        case PreparationCondition::ConditionC:
            return "Condição C (Sd = 7,0 MPa): Aplicável apenas às classes C10 e C15. Cimento em massa; agregados em volume; água em volume com estimativa de umidade.";
        case PreparationCondition::KnownSd:
            return "Desvio-Padrão Conhecido: Determinado com no mínimo 20 resultados consecutivos em 30 dias (Sd >= 2,0 MPa).";
        case PreparationCondition::Arbitrary:
            return "Laboratório / Arbitrário: Desvio-padrão de controle experimental específico.";
    }
    return "";
}

double ConcreteStandards::calculateDosageStrength(double fck, double sd) {
    return fck + 1.65 * sd;
}

std::vector<std::string> ConcreteStandards::getStandardStrengthClasses() {
    return {"C20", "C25", "C30", "C35", "C40", "C45", "C50", "C55", "C60", "C70", "C80", "C90", "C100"};
}

double ConcreteStandards::getFckFromClass(const std::string& className) {
    if (className.size() > 1 && (className[0] == 'C' || className[0] == 'c')) {
        try {
            return std::stod(className.substr(1));
        } catch (...) {}
    }
    return 30.0;
}

SlumpClass ConcreteStandards::classifySlump(double slumpMm) {
    if (slumpMm < 50.0) return SlumpClass::S10;
    if (slumpMm < 100.0) return SlumpClass::S50;
    if (slumpMm < 160.0) return SlumpClass::S100;
    if (slumpMm < 220.0) return SlumpClass::S160;
    return SlumpClass::S220;
}

std::string ConcreteStandards::getSlumpClassName(SlumpClass sc) {
    switch (sc) {
        case SlumpClass::S10:  return "S10 (10 <= A < 50 mm)";
        case SlumpClass::S50:  return "S50 (50 <= A < 100 mm)";
        case SlumpClass::S100: return "S100 (100 <= A < 160 mm)";
        case SlumpClass::S160: return "S160 (160 <= A < 220 mm)";
        case SlumpClass::S220: return "S220 (>= 220 mm)";
    }
    return "S100";
}

std::string ConcreteStandards::getSlumpApplication(SlumpClass sc) {
    switch (sc) {
        case SlumpClass::S10:  return "Concreto extrusado, vibroprensado ou centrifugado.";
        case SlumpClass::S50:  return "Pavimentos de concreto e elementos de fundações diretas.";
        case SlumpClass::S100: return "Elementos estruturais usuais com lançamento convencional.";
        case SlumpClass::S160: return "Elementos estruturais esbeltos com concreto bombeado.";
        case SlumpClass::S220: return "Elementos com altíssima taxa de armadura ou auto-adensável.";
    }
    return "";
}

double ConcreteStandards::getCoarseDMC(CoarseAggregateType type) {
    switch (type) {
        case CoarseAggregateType::Brita0: return 12.5; // ou 9.5 mm
        case CoarseAggregateType::Brita1: return 19.0;
        case CoarseAggregateType::Brita2: return 25.0; // ou 31.5 mm
        case CoarseAggregateType::Brita3: return 50.0;
        case CoarseAggregateType::Custom: return 19.0;
    }
    return 19.0;
}

std::string ConcreteStandards::getCoarseName(CoarseAggregateType type) {
    switch (type) {
        case CoarseAggregateType::Brita0: return "Brita 0 (DMC 9,5 a 12,5 mm)";
        case CoarseAggregateType::Brita1: return "Brita 1 (DMC 19 mm)";
        case CoarseAggregateType::Brita2: return "Brita 2 (DMC 25 mm)";
        case CoarseAggregateType::Brita3: return "Brita 3 (DMC 50 mm)";
        case CoarseAggregateType::Custom: return "Personalizada";
    }
    return "Brita 1";
}

TechnicalComplianceResult ConcreteStandards::validateMixDesign(
    double targetFck,
    double targetWaterCement,
    double calculatedCementConsumption,
    EnvironmentalClass envClass,
    StructuralType structType,
    double slumpMm,
    double dmcMm,
    double coverMm,
    PreparationCondition prepCond
) {
    TechnicalComplianceResult res;
    DurabilityRequirement req = getDurabilityRequirement(envClass, structType);

    // 1. Verificação fck mínimo (NBR 12655:2022 / NBR 6118:2023)
    if (targetFck < req.minFck) {
        std::ostringstream ss;
        ss << "fck de projeto (" << targetFck << " MPa) é inferior ao mínimo normativo de " 
           << req.minFck << " MPa (" << req.minConcreteClass << ") para " << getEnvironmentalClassName(envClass)
           << " conforme ABNT NBR 12655:2022 / NBR 6118:2023.";
        res.errors.push_back(ss.str());
        res.isCompliant = false;
    } else {
        std::ostringstream ss;
        ss << "fck de projeto (" << targetFck << " MPa) atende à classe mínima " << req.minConcreteClass << " (" << req.minFck << " MPa).";
        res.passedChecks.push_back(ss.str());
    }

    // 2. Verificação a/c máximo (NBR 12655:2022 Tabela 2 / NBR 6118:2023 Tabela 7.1)
    if (targetWaterCement > req.maxWaterCementRatio + 1e-4) {
        std::ostringstream ss;
        ss << "Relação água/cimento obtida (" << std::fixed << std::setprecision(3) << targetWaterCement
           << ") é superior ao limite máximo normativo de " << req.maxWaterCementRatio
           << " para " << getEnvironmentalClassName(envClass)
           << ". Risco de elevada permeabilidade capilar e penetração de agentes agressivos (Mehta & Monteiro).";
        res.errors.push_back(ss.str());
        res.isCompliant = false;
    } else {
        std::ostringstream ss;
        ss << "Relação água/cimento (" << std::fixed << std::setprecision(3) << targetWaterCement
           << ") atende ao limite máximo normativo de " << req.maxWaterCementRatio << ".";
        res.passedChecks.push_back(ss.str());
    }

    // 3. Verificação consumo mínimo de cimento (NBR 12655 Tabela 2)
    if (calculatedCementConsumption < req.minCementConsumption - 1e-2) {
        std::ostringstream ss;
        ss << "Consumo de cimento calculado (" << std::fixed << std::setprecision(1) << calculatedCementConsumption
           << " kg/m³) é inferior ao mínimo normativo de " << req.minCementConsumption
           << " kg/m³ exigido pela NBR 12655 Tabela 2 para " << getEnvironmentalClassName(envClass) << ".";
        res.warnings.push_back(ss.str());
        res.isCompliant = false;
    } else {
        std::ostringstream ss;
        ss << "Consumo de cimento (" << std::fixed << std::setprecision(1) << calculatedCementConsumption
           << " kg/m³) atende ao mínimo normativo de " << req.minCementConsumption << " kg/m³.";
        res.passedChecks.push_back(ss.str());
    }

    // 4. Verificação da Condição de Preparo x Classe de Resistência (ABNT NBR 12655:2022 itens 5.4 e 5.6.3.1)
    if (prepCond == PreparationCondition::ConditionB && targetFck > 20.0 + 1e-4) {
        std::ostringstream ss;
        ss << "Condição B de preparo (Sd = 5,5 MPa) é permitida apenas para concretos das classes C10 a C20 (fck <= 20 MPa) conforme ABNT NBR 12655:2022 (itens 5.4.2 e 5.6.3.1). Para fck = "
           << std::fixed << std::setprecision(1) << targetFck << " MPa, exige-se a Condição A.";
        res.errors.push_back(ss.str());
        res.isCompliant = false;
    } else if (prepCond == PreparationCondition::ConditionC && targetFck > 15.0 + 1e-4) {
        std::ostringstream ss;
        ss << "Condição C de preparo (Sd = 7,0 MPa) é permitida apenas para concretos das classes C10 e C15 (fck <= 15 MPa) conforme ABNT NBR 12655:2022 (itens 5.4.3 e 5.6.3.1).";
        res.errors.push_back(ss.str());
        res.isCompliant = false;
    } else {
        res.passedChecks.push_back("Condição de preparo e desvio-padrão adotado atendem aos limites de classe da ABNT NBR 12655:2022.");
    }

    // 5. Verificação de Consistência e Abatimento (ABNT NBR 8953:2015 Tabela 2 e NBR 7212:2025 item 5.8)
    if (slumpMm < 10.0) {
        std::ostringstream ss;
        ss << "Abatimento especificado (" << std::fixed << std::setprecision(0) << slumpMm
           << " mm) é inferior ao limite inferior da NBR 8953 (10 mm - Classe S10). Mistura excessivamente seca para concreto plástico.";
        res.errors.push_back(ss.str());
        res.isCompliant = false;
    } else {
        SlumpClass sc = classifySlump(slumpMm);
        double slumpTol = (slumpMm < 50.0) ? 10.0 : ((slumpMm <= 90.0) ? 20.0 : 30.0);
        std::ostringstream ss;
        ss << "Consistência NBR 8953: Classe " << getSlumpClassName(sc) << " (Slump " << std::fixed << std::setprecision(0) << slumpMm
           << " mm). Tolerância de recebimento NBR 7212:2025: ±" << slumpTol << " mm ("
           << (slumpMm - slumpTol) << " mm a " << (slumpMm + slumpTol) << " mm).";
        res.passedChecks.push_back(ss.str());

        if (slumpMm < 50.0) {
            res.warnings.push_back("Consistência S10 (< 50 mm): Concreto seco/firme, adequado para pavimentos e vibroprensagem. Desaconselhado para bombeamento convencional ou peças delgadas densamente armadas.");
        } else if (slumpMm >= 220.0) {
            res.warnings.push_back("Consistência S220 (≥ 220 mm): Concreto muito fluido. Exige aditivo superplastificante e controle estrito de exsudação e segregação (NBR 7212:2025).");
        }
    }

    // 6. Verificação DMC x Cobrimento Nominal (ABNT NBR 6118:2023 item 7.4.7.6)
    // O dmc não deve superar 1,2 x cobrimento nominal
    double coverBeams = (coverMm > 0.0) ? coverMm : req.nominalCoverBeamsCols;
    double coverSlabs = req.nominalCoverSlabs;

    if (coverBeams > 0.0 && dmcMm > 1.2 * coverBeams) {
        std::ostringstream ss;
        ss << "A Dimensão Máxima Característica do agregado graúdo (" << dmcMm 
           << " mm) ultrapassa 1,2 vezes o cobrimento nominal de vigas/pilares (" << std::fixed << std::setprecision(1) << (1.2 * coverBeams)
           << " mm). Risco de segregação e bicheiras (ABNT NBR 6118:2023 item 7.4.7.6).";
        res.warnings.push_back(ss.str());
    } else if (coverBeams > 0.0) {
        std::ostringstream ss;
        ss << "DMC do agregado (" << dmcMm << " mm) atende ao critério geométrico para vigas/pilares (≤ 1,2 · cnom = "
           << std::fixed << std::setprecision(1) << (1.2 * coverBeams) << " mm conforme NBR 6118).";
        res.passedChecks.push_back(ss.str());
    }

    if (coverSlabs > 0.0 && dmcMm > 1.2 * coverSlabs) {
        std::ostringstream ss;
        ss << "Nota geométrica para lajes: DMC (" << dmcMm << " mm) supera 1,2 × cnom de lajes ("
           << std::fixed << std::setprecision(1) << (1.2 * coverSlabs) << " mm). Recomenda-se Brita 0/1 para elementos de laje nesta classe.";
        res.warnings.push_back(ss.str());
    } else if (coverSlabs > 0.0) {
        std::ostringstream ss;
        ss << "DMC do agregado (" << dmcMm << " mm) atende ao critério para lajes (≤ 1,2 · cnom = "
           << std::fixed << std::setprecision(1) << (1.2 * coverSlabs) << " mm).";
        res.passedChecks.push_back(ss.str());
    }

    // Recomendações técnicas baseadas em Mehta & Monteiro
    if (targetWaterCement > 0.50) {
        res.recommendations.push_back(
            "Mehta & Monteiro: Concretos com a/c > 0,50 apresentam rede de poros capilares contínua e interconectada, aumentando a taxa de difusão iônica e carbonatação."
        );
    }
    if (calculatedCementConsumption > 450.0) {
        res.recommendations.push_back(
            "Mehta & Monteiro: Consumos de cimento acima de 450 kg/m³ aumentam o risco de fissuração por retração térmica e retração autógena. Considere o uso de plastificantes/superplastificantes para reduzir o volume de pasta."
        );
    }

    // Rastreabilidade do escopo
    res.passedChecks.push_back(
        "Escopo da verificação: parâmetros normativos de durabilidade, dosagem, consistência e geometria de agregados (ABNT NBR 12655, NBR 6118, NBR 8953 e NBR 7212)."
    );

    return res;
}

} // namespace ConcreteMix
