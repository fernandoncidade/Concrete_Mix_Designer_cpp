#pragma once

#include <string>
#include <vector>
#include <string_view>
#include <optional>

namespace ConcreteMix {

// ABNT NBR 6118:2023 / NBR 12655:2022 - Classes de Agressividade Ambiental (CAA)
enum class EnvironmentalClass {
    CAA_I,   // Fraca: Rural, Submersa
    CAA_II,  // Moderada: Urbana
    CAA_III, // Forte: Marinha, Industrial
    CAA_IV   // Muito Forte: Respingos de maré, Industrial agressiva
};

// Tipo de Concreto Estrutural
enum class StructuralType {
    ReinforcedConcrete, // CA: Concreto Armado
    PrestressedConcrete // CP: Concreto Protendido
};

// ABNT NBR 12655:2022 - Condições de Preparo e Desvio-Padrão
enum class PreparationCondition {
    ConditionA,  // Sd = 4.0 MPa (Medição em massa, água em massa/volume dosada com correção)
    ConditionB,  // Sd = 5.5 MPa (Cimento em massa, agregados massa combinada com volume)
    ConditionC,  // Sd = 7.0 MPa (Agregados em volume, água com estimativa de consistência)
    KnownSd,     // Sd histórico conhecido (Sd >= 2.0 MPa com >= 20 corpos de prova)
    Arbitrary    // Personalizado de laboratório (ex: Sd = 3.5 MPa)
};

// ABNT NBR 8953:2015 - Classes de Consistência (Abatimento / Slump)
enum class SlumpClass {
    S10,  // 10 mm <= Slump < 50 mm (Extrusado, vibroprensado, centrifugado)
    S50,  // 50 mm <= Slump < 100 mm (Pavimentos, fundações, elementos correntes)
    S100, // 100 mm <= Slump < 160 mm (Elementos estruturais usuais, bombeamento comum)
    S160, // 160 mm <= Slump < 220 mm (Elementos esbeltos, bombeado longo)
    S220  // Slump >= 220 mm (Alta taxa de armadura, peças complexas)
};

// Tipos de Brita / Agregado Graúdo (NBR 7211)
enum class CoarseAggregateType {
    Brita0, // DMC 9.5 mm / 12.5 mm
    Brita1, // DMC 19.0 mm
    Brita2, // DMC 25.0 mm ou 31.5 mm
    Brita3, // DMC 50.0 mm
    Custom
};

// Requisitos Normativos de Durabilidade segundo NBR 12655:2022 e NBR 6118:2023
struct DurabilityRequirement {
    EnvironmentalClass envClass;
    StructuralType structType;
    double maxWaterCementRatio;   // a/c máximo permitido
    std::string minConcreteClass; // Classe mínima NBR 8953 (ex: C20, C25, C30, C40)
    double minFck;                // fck mínimo em MPa
    double minCementConsumption;  // Consumo mínimo de cimento em kg/m³
    double nominalCoverSlabs;     // Cobrimento nominal para lajes (mm)
    double nominalCoverBeamsCols; // Cobrimento nominal para vigas/pilares (mm)
    std::string envDescription;
    std::string riskDescription;
};

// Validação Técnica e Avisos de Conformidade Normativa
struct TechnicalComplianceResult {
    bool isCompliant = true;
    std::vector<std::string> passedChecks;
    std::vector<std::string> warnings;
    std::vector<std::string> errors;
    std::vector<std::string> recommendations;
};

class ConcreteStandards {
public:
    // Informações da Classe de Agressividade (NBR 12655 Tab. 1 / NBR 6118 Tab. 6.1)
    static std::string getEnvironmentalClassName(EnvironmentalClass c);
    static std::string getEnvironmentalClassDescription(EnvironmentalClass c);
    static std::string getDeteriorationRisk(EnvironmentalClass c);

    // Requisitos de Qualidade (NBR 12655 Tab. 2 / NBR 6118 Tab. 7.1)
    static DurabilityRequirement getDurabilityRequirement(EnvironmentalClass envClass, StructuralType structType);

    // Desvio-Padrão por Condição de Preparo (NBR 12655 Tab. 7)
    static double getStandardDeviation(PreparationCondition cond, double customSd = 4.0);
    static std::string getPreparationConditionDescription(PreparationCondition cond);

    // Cálculo da Resistência de Dosagem: fcj = fck + 1.65 * Sd (NBR 12655 Eq. 5.6.3)
    static double calculateDosageStrength(double fck, double sd);

    // Classes de Resistência NBR 8953 (Grupo I e II)
    static std::vector<std::string> getStandardStrengthClasses();
    static double getFckFromClass(const std::string& className);

    // Classes de Consistência NBR 8953
    static SlumpClass classifySlump(double slumpMm);
    static std::string getSlumpClassName(SlumpClass sc);
    static std::string getSlumpApplication(SlumpClass sc);

    // Agregados NBR 7211
    static double getCoarseDMC(CoarseAggregateType type);
    static std::string getCoarseName(CoarseAggregateType type);

    // Tolerâncias de Pesagem em Central (ABNT NBR 7212:2024 / versão corrigida 2025, item 5.5)
    struct BatchingTolerances {
        static constexpr double CementTolerancePercent = 1.0;     // item 5.5.2 e 5.5.5 (±1%)
        static constexpr double AggregateTolerancePercent = 3.0;  // item 5.5.1 (±3% na massa acumulada)
        static constexpr double WaterTolerancePercent = 3.0;      // item 5.5.3 (±3% na quantidade total)
        static constexpr double AdmixtureTolerancePercent = 5.0;  // item 5.5.4 (±5% da quantidade prevista)
    };

    // Validação de Rigor Técnico frente às Normas e Mehta & Monteiro
    static TechnicalComplianceResult validateMixDesign(
        double targetFck,
        double targetWaterCement,
        double calculatedCementConsumption,
        EnvironmentalClass envClass,
        StructuralType structType,
        double slumpMm,
        double dmcMm,
        double coverMm,
        PreparationCondition prepCond = PreparationCondition::ConditionA
    );
};

} // namespace ConcreteMix
