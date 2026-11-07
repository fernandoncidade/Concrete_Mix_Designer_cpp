#include "ProjectData.hpp"
#include "NormativeDatabase.hpp"
#include "MortarContentOptimizer.hpp"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <sstream>
#include <iomanip>
#include <cmath>

namespace ConcreteMix {

bool ProjectData::checkDmcCoverCompliance(QString &diag) const {
    auto req = ConcreteStandards::getDurabilityRequirement(envClass, structType);
    double coverBeams = req.nominalCoverBeamsCols;
    double coverSlabs = req.nominalCoverSlabs;
    double dmc = getMaxCoarseDmc();

    QString diagBeams, diagSlabs;
    bool okBeams = NormativeDatabase::instance().verifyDmcAgainstCover(dmc, coverBeams, diagBeams);
    bool okSlabs = NormativeDatabase::instance().verifyDmcAgainstCover(dmc, coverSlabs, diagSlabs);

    if (okBeams && okSlabs) {
        diag = QString("Vigas/Pilares (cnom %1 mm): OK | Lajes (cnom %2 mm): OK (DMC = %3 mm ≤ 1,2·cnom)")
               .arg(coverBeams, 0, 'f', 0).arg(coverSlabs, 0, 'f', 0).arg(dmc, 0, 'f', 1);
        return true;
    } else if (okBeams && !okSlabs) {
        diag = QString("Vigas/Pilares: OK (%1 mm ≤ %2 mm) | Lajes: ALERTA (DMC %1 mm > 1,2·cnom %3 mm - NBR 6118:2023)")
               .arg(dmc, 0, 'f', 1).arg(1.2 * coverBeams, 0, 'f', 1).arg(1.2 * coverSlabs, 0, 'f', 1);
        return false; // Conformidade global exige atendimento integral a todos os elementos
    } else {
        diag = QString("NÃO CONFORME: DMC (%1 mm) > 1,2·cnom de Vigas/Pilares (%2 mm) e Lajes (%3 mm)")
               .arg(dmc, 0, 'f', 1).arg(1.2 * coverBeams, 0, 'f', 1).arg(1.2 * coverSlabs, 0, 'f', 1);
        return false;
    }
}

void ProjectData::updatePilotBatchFromMixerVolume() {
    if (labMixerVolumeL <= 0.0 || pilotM <= 0.0) {
        pilotBatchGravelKg = 0.0;
        pilotBatchWaterKg = 0.0;
        return;
    }
    double effectiveGravelSpec = gravel1.specificMass;
    if (useGravelBlend && gravel2Percent > 0.0) {
        double f2 = gravel2Percent / 100.0;
        double f1 = 1.0 - f2;
        effectiveGravelSpec = (f2 * gravel2.specificMass) + (f1 * gravel1.specificMass);
    }
    auto pb = MortarContentOptimizer::calculatePilotBatchFromVolume(
        labMixerVolumeL,
        pilotM,
        40.0,
        0.50,
        cement.specificMass,
        sand.specificMass,
        effectiveGravelSpec
    );
    pilotBatchGravelKg = pb.gravelMassKg;
    pilotBatchWaterKg = pb.waterMassKg;
}

ProjectData::ProjectData() {
    loadPresetReference0();
}

MixDesignResult ProjectData::calculate() const {
    return MixDesignCalculator::calculate(
        fck,
        prepCondition,
        customSd,
        envClass,
        structType,
        targetSlumpMm,
        selectedAlphaPercent,
        useGravelBlend,
        gravel2Percent,
        cement,
        sand,
        gravel1,
        gravel2,
        experimentalMixes,
        projectVolumeM3,
        labMixerVolumeL,
        manualAc,
        manualM,
        manualC,
        gravel1DmcMm,
        gravel2DmcMm
    );
}

std::string ProjectData::getPresetName(int presetIndex, bool isEnglish) {
    switch (presetIndex) {
        case 0:
            return isEnglish
                ? "Scenario 0: Customizable by user"
                : "Cenário 0: Personalizável pelo usuário";
        case 1:
            return isEnglish
                ? "Scenario 1: Urban C25 Concrete (CAA II) - CP II-F-32, Slump 100mm, Jobsite 30 m³"
                : "Cenário 1: Concreto C25 Urbano (CAA II) - CP II-F-32, Slump 100mm, Obra 30 m³";
        case 2:
            return isEnglish
                ? "Scenario 2: Marine C30 Concrete (CAA III) - CP III-40 RS, Gravel 1+2, Jobsite 50 m³"
                : "Cenário 2: Concreto C30 Marinho (CAA III) - CP III-40 RS, Britas 1+2, Obra 50 m³";
        case 3:
            return isEnglish
                ? "Scenario 3: High-Performance C40 Concrete (CAA IV) - CP V-ARI, Slump 160mm, Jobsite 100 m³"
                : "Cenário 3: Concreto C40 Alto Desempenho (CAA IV) - CP V-ARI, Slump 160mm, Obra 100 m³";
        case 4:
            return isEnglish
                ? "Scenario 4: Foundations C20 Concrete (CAA I - Condition B) - CP II-Z-32, Gravel 2, Jobsite 40 m³"
                : "Cenário 4: Concreto C20 Fundações (CAA I - Condição B) - CP II-Z-32, Brita 2, Obra 40 m³";
        case 5:
            return isEnglish
                ? "Scenario 5: Corrected Original UFPR Spreadsheet (TC031 - C30, Sd 3.5 MPa, α 52%, Jobsite 30 m³)"
                : "Cenário 5: Planilha UFPR Original Corrigida (TC031 - C30, Sd 3,5 MPa, α 52%, Obra 30 m³)";
        default:
            return isEnglish
                ? "Concrete Mix Designer"
                : "Módulo de Dosagem de Concreto";
    }
}

std::string ProjectData::getDisplayProjectName(bool isEnglish) const {
    if (usePresetProjectName) {
        return getPresetName(currentPresetIndex, isEnglish);
    }
    std::string raw = !customProjectName.empty() ? customProjectName : projectName;
    if (raw.empty()) {
        return isEnglish ? "Custom Project - Concrete Mix Designer" : "Projeto Personalizado - Módulo de Dosagem de Concreto";
    }
    // Se o texto corresponder a algum dos cenários padrão, traduzir dinamicamente também
    for (int i = 0; i <= 5; ++i) {
        if (raw == getPresetName(i, false) || raw == getPresetName(i, true)) {
            return getPresetName(i, isEnglish);
        }
    }
    if (raw.find("Cenário 0") == 0 || raw.find("Scenario 0") == 0) return getPresetName(0, isEnglish);
    if (raw.find("Cenário 1") == 0 || raw.find("Scenario 1") == 0) return getPresetName(1, isEnglish);
    if (raw.find("Cenário 2") == 0 || raw.find("Scenario 2") == 0) return getPresetName(2, isEnglish);
    if (raw.find("Cenário 3") == 0 || raw.find("Scenario 3") == 0) return getPresetName(3, isEnglish);
    if (raw.find("Cenário 4") == 0 || raw.find("Scenario 4") == 0) return getPresetName(4, isEnglish);
    if (raw.find("Cenário 5") == 0 || raw.find("Scenario 5") == 0) return getPresetName(5, isEnglish);
    if (raw == "Dosagem Experimental de Concreto" || raw == "Experimental Concrete Mix Design" ||
        raw == "Módulo de Dosagem de Concreto" || raw == "Concrete Mix Designer") {
        return isEnglish ? "Concrete Mix Designer" : "Módulo de Dosagem de Concreto";
    }
    return raw;
}

std::string ProjectData::getDisplayEngineerName(bool isEnglish) const {
    if (engineerName.empty()) {
        return "—";
    }
    if (isEnglish && (engineerName == "Eng. Civil" || engineerName == "Engenheiro Civil")) {
        return "Civil Engineer";
    }
    if (!isEnglish && engineerName == "Civil Engineer") {
        return "Eng. Civil";
    }
    return engineerName;
}

std::string ProjectData::getDisplayInstitution(bool isEnglish) const {
    if (institution.empty()) {
        return "—";
    }
    if (isEnglish && institution == "UFPR - Universidade Federal do Paraná") {
        return "UFPR - Federal University of Paraná";
    }
    if (!isEnglish && institution == "UFPR - Federal University of Paraná") {
        return "UFPR - Universidade Federal do Paraná";
    }
    return institution;
}

std::string ProjectData::getDisplayStudentId() const {
    return studentId.empty() ? "—" : studentId;
}

void ProjectData::loadPresetUserCustom() {
    currentPresetIndex = 0;
    projectName = usePresetProjectName ? getPresetName(0, false) : customProjectName;
    notes = "";

    fck = 0.0;
    prepCondition = PreparationCondition::Arbitrary;
    customSd = 0.0;
    envClass = EnvironmentalClass::CAA_I;
    structType = StructuralType::ReinforcedConcrete;
    targetSlumpMm = 0.0;
    pilotM = 0.0;
    pilotBatchGravelKg = 0.0;
    pilotBatchWaterKg = 0.0;
    selectedAlphaPercent = 0.0;

    projectVolumeM3 = 0.0;
    labMixerVolumeL = 0.0;

    cement = {"", 0.0, 0.0, 0.0, 0.0};
    sand = {"", 0.0, 0.0, 0.0, 0.0};
    gravel1 = {"", 0.0, 0.0, 0.0, 0.0};
    gravel2 = {"", 0.0, 0.0, 0.0, 0.0};
    gravel1DmcMm = 0.0;
    gravel2DmcMm = 0.0;
    baseGravelMassKg = 12.0;
    containerVolumeL = 15.04;

    useGravelBlend = false;
    gravel2Percent = 0.0;
    gravel1Percent = 0.0;

    experimentalMixes = {
        {"", 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
        {"", 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
        {"", 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}
    };

    manualAc = 0.0;
    manualM = 0.0;
    manualC = 0.0;
}

void ProjectData::loadPreset(int presetIndex) {
    switch (presetIndex) {
    case 0:
        loadPresetUserCustom();
        break;
    case 1:
        loadPresetReference0();
        break;
    case 2:
        loadPresetResultados1();
        break;
    case 3:
        loadPresetResultados2();
        break;
    case 4:
        loadPresetResultados3();
        break;
    case 5:
        loadPresetPlanilhaUFPR();
        break;
    default:
        break;
    }
}

void ProjectData::loadPresetReference0() {
    currentPresetIndex = 1;
    projectName = usePresetProjectName ? getPresetName(1, false) : customProjectName;
    notes = "Edifício residencial/comercial em ambiente urbano comum (CAA II). fck = 25 MPa, Condição A (Sd = 4,0 MPa), fcj = 31,60 MPa. Slump 100 mm (S100 - NBR 8953). CP II-F-32 e Brita 1 (DMC = 19 mm <= 1,2 * cnom = 30 mm).";
    fck = 25.0;
    prepCondition = PreparationCondition::ConditionA;
    customSd = 4.0;
    envClass = EnvironmentalClass::CAA_II;
    structType = StructuralType::ReinforcedConcrete;
    targetSlumpMm = 100.0;
    pilotM = 5.0;
    selectedAlphaPercent = 52.0;

    projectVolumeM3 = 30.0;
    labMixerVolumeL = 29.0;

    cement = {"CP II-F-32", 3.11, 1.15, 0.0, 40.0};
    sand = {"Areia Média Natural", 2.62, 1.48, 3.5, 90.0};
    gravel1 = {"Brita 1 Granítica", 2.68, 1.45, 1.0, 80.0};
    gravel2 = {"Brita 2 Granítica", 2.70, 1.48, 1.0, 75.0};
    gravel1DmcMm = 19.0;
    gravel2DmcMm = 25.0;
    baseGravelMassKg = 12.0;
    containerVolumeL = 15.04;

    useGravelBlend = false;
    gravel2Percent = 70.0;
    gravel1Percent = 30.0;

    experimentalMixes = {
        {"Traço 1 (Rico 1:4.0)", 4.0, 0.440, 40.5, 100.0, 2.390, 2390.0 / 5.44},
        {"Traço 2 (Médio 1:5.0)", 5.0, 0.520, 32.8, 100.0, 2.380, 2380.0 / 6.52},
        {"Traço 3 (Pobre 1:6.0)", 6.0, 0.620, 25.2, 100.0, 2.360, 2360.0 / 7.62}
    };

    manualAc = 0.534;
    manualM = 5.08;
    manualC = 360.0;

    updatePilotBatchFromMixerVolume();
}

void ProjectData::loadPresetResultados1() {
    currentPresetIndex = 2;
    projectName = usePresetProjectName ? getPresetName(2, false) : customProjectName;
    notes = "Estrutura marítima/industrial com alta durabilidade (CAA III). fck = 30 MPa, Condição A (Sd = 4,0 MPa), fcj = 36,60 MPa. Slump 120 mm (S100). CP III-40 RS com empacotamento granular otimizado de Brita 1 (30%) + Brita 2 (70%).";
    fck = 30.0;
    prepCondition = PreparationCondition::ConditionA;
    customSd = 4.0;
    envClass = EnvironmentalClass::CAA_III;
    structType = StructuralType::ReinforcedConcrete;
    targetSlumpMm = 120.0;
    pilotM = 5.0;
    selectedAlphaPercent = 50.0;

    projectVolumeM3 = 50.0;
    labMixerVolumeL = 90.0;

    cement = {"CP III-40 RS", 3.00, 1.18, 0.0, 42.0};
    sand = {"Areia Média Natural", 2.62, 1.48, 3.0, 90.0};
    gravel1 = {"Brita 1 Granítica", 2.68, 1.45, 1.0, 80.0};
    gravel2 = {"Brita 2 Granítica", 2.70, 1.50, 1.0, 75.0};
    gravel1DmcMm = 19.0;
    gravel2DmcMm = 25.0;
    baseGravelMassKg = 12.0;
    containerVolumeL = 15.04;

    useGravelBlend = true;
    gravel1Percent = 30.0;
    gravel2Percent = 70.0;

    experimentalMixes = {
        {"Traço 1 (Rico 1:3.8)", 3.8, 0.400, 45.8, 120.0, 2.420, 2420.0 / 5.20},
        {"Traço 2 (Médio 1:4.8)", 4.8, 0.480, 37.2, 120.0, 2.400, 2400.0 / 6.28},
        {"Traço 3 (Pobre 1:5.8)", 5.8, 0.580, 28.5, 120.0, 2.380, 2380.0 / 7.38}
    };

    manualAc = 0.486;
    manualM = 4.79;
    manualC = 382.4;

    updatePilotBatchFromMixerVolume();
}

void ProjectData::loadPresetResultados2() {
    currentPresetIndex = 3;
    projectName = usePresetProjectName ? getPresetName(3, false) : customProjectName;
    notes = "Concreto de alto desempenho para respingos de maré e pilares esbeltos (CAA IV). fck = 40 MPa, Condição A (Sd = 4,0 MPa), fcj = 46,60 MPa. Slump 160 mm (S160 fluido com plastificante). Cimento CP V-ARI e Brita 1 Basáltica.";
    fck = 40.0;
    prepCondition = PreparationCondition::ConditionA;
    customSd = 4.0;
    envClass = EnvironmentalClass::CAA_IV;
    structType = StructuralType::ReinforcedConcrete;
    targetSlumpMm = 160.0;
    pilotM = 5.0;
    selectedAlphaPercent = 48.0;

    projectVolumeM3 = 100.0;
    labMixerVolumeL = 100.0;

    cement = {"CP V-ARI", 3.12, 1.12, 0.0, 45.0};
    sand = {"Areia Fina/Média Quartzosa", 2.63, 1.47, 3.0, 95.0};
    gravel1 = {"Brita 1 Basáltica", 2.85, 1.52, 0.8, 85.0};
    gravel2 = {"Brita 2 Basáltica", 2.88, 1.55, 0.8, 80.0};
    gravel1DmcMm = 19.0;
    gravel2DmcMm = 25.0;
    baseGravelMassKg = 12.0;
    containerVolumeL = 15.04;

    useGravelBlend = false;

    experimentalMixes = {
        {"Traço 1 (Rico 1:3.2)", 3.2, 0.340, 56.0, 160.0, 2.450, 2450.0 / 4.54},
        {"Traço 2 (Médio 1:4.2)", 4.2, 0.420, 47.5, 160.0, 2.430, 2430.0 / 5.62},
        {"Traço 3 (Pobre 1:5.2)", 5.2, 0.500, 38.0, 160.0, 2.400, 2400.0 / 6.70}
    };

    manualAc = 0.428;
    manualM = 4.30;
    manualC = 423.7;

    updatePilotBatchFromMixerVolume();
}

void ProjectData::loadPresetResultados3() {
    currentPresetIndex = 4;
    projectName = usePresetProjectName ? getPresetName(4, false) : customProjectName;
    notes = "Concreto para sapatas e blocos de fundação com controle no canteiro (Condição B). fck = 20 MPa, Condição B (Sd = 5,5 MPa), fcj = 29,08 MPa. Slump 80 mm (S50). Cimento CP II-Z-32 e Brita 2 (DMC = 25 mm).";
    fck = 20.0;
    prepCondition = PreparationCondition::ConditionB;
    customSd = 5.5;
    envClass = EnvironmentalClass::CAA_I;
    structType = StructuralType::ReinforcedConcrete;
    targetSlumpMm = 80.0;
    pilotM = 5.0;
    selectedAlphaPercent = 54.0;

    projectVolumeM3 = 40.0;
    labMixerVolumeL = 90.0;

    cement = {"CP II-Z-32", 3.00, 1.16, 0.0, 39.0};
    sand = {"Areia Média Natural", 2.62, 1.48, 4.0, 85.0};
    gravel1 = {"Brita 2 Granítica", 2.68, 1.46, 1.2, 75.0};
    gravel2 = {"Brita 1 Granítica", 2.68, 1.45, 1.0, 80.0};
    gravel1DmcMm = 25.0;
    gravel2DmcMm = 19.0;
    baseGravelMassKg = 12.0;
    containerVolumeL = 15.04;

    useGravelBlend = false;

    experimentalMixes = {
        {"Traço 1 (Rico 1:4.2)", 4.2, 0.480, 36.0, 80.0, 2.380, 2380.0 / 5.68},
        {"Traço 2 (Médio 1:5.2)", 5.2, 0.560, 30.2, 80.0, 2.370, 2370.0 / 6.76},
        {"Traço 3 (Pobre 1:6.2)", 6.2, 0.650, 23.5, 80.0, 2.350, 2350.0 / 7.85}
    };

    manualAc = 0.575;
    manualM = 5.34;
    manualC = 343.0;

    updatePilotBatchFromMixerVolume();
}

void ProjectData::loadPresetPlanilhaUFPR() {
    currentPresetIndex = 5;
    projectName = usePresetProjectName ? getPresetName(5, false) : customProjectName;
    notes = "Reprodução integral e corrigida do estudo experimental da planilha TC031 (UFPR - GRR20182393, aba 'Referência (0)' e 'Memorial de cálculo'). fck = 30 MPa, Sd = 3,5 MPa (fcj = 35,775 MPa), α = 52%, m = {4,0; 5,0; 6,0}, Cimento CP II-F-32 (R$ 380/t), Areia Média (R$ 90/m³, h=4%), Brita 1 (R$ 70/m³, h=1,5%).";
    fck = 30.0;
    prepCondition = PreparationCondition::Arbitrary;
    customSd = 3.5;
    envClass = EnvironmentalClass::CAA_I;
    structType = StructuralType::ReinforcedConcrete;
    targetSlumpMm = 80.0;
    pilotM = 5.0;
    selectedAlphaPercent = 52.0;

    projectVolumeM3 = 30.0;
    labMixerVolumeL = 32.515592515592516;

    cement = {"CP II-F-32", 2.96, 1.75, 0.0, 380.0}; // R$ 380,00 / tonelada (R$ 0,38/kg)
    sand = {"Areia Natural Média", 2.60, 1.40, 4.0, 90.0};
    gravel1 = {"Brita 1 (DMC 19 mm)", 2.70, 1.45, 1.5, 70.0};
    gravel2 = {"Brita 2 (DMC 25 mm)", 2.70, 1.55, 1.5, 70.0};
    gravel1DmcMm = 19.0;
    gravel2DmcMm = 25.0;
    baseGravelMassKg = 12.0;
    containerVolumeL = 15.04;

    useGravelBlend = false;
    gravel2Percent = 70.0;
    gravel1Percent = 30.0;

    // Dados experimentais exatos da aba 'Referência (0)' (B80:N104) e 'Memorial de cálculo' (RESULTADOS 0)
    // m_b fixa = 43,20 kg (α = 52%):
    // Traço 1 (m=4,0): c = 43,20/2,40 = 18,00 kg, mw = 5,90 kg => a/c = 0,327778, fcj = 45,2 MPa, rho = 2,41 kg/dm³, C = 452,3462 kg/m³
    // Traço 2 (m=5,0): c = 43,20/2,88 = 15,00 kg, mw = 5,60 kg => a/c = 0,373333, fcj = 39,9 MPa, rho = 2,39 kg/dm³, C = 375,0000 kg/m³
    // Traço 3 (m=6,0): c = 43,20/3,36 = 12,8571 kg, mw = 5,70 kg => a/c = 0,443333, fcj = 32,5 MPa, rho = 2,40 kg/dm³, C = 322,4362 kg/m³
    const double ac1 = 5.90 / 18.00;
    const double ac2 = 5.60 / 15.00;
    const double ac3 = 5.70 / (43.20 / 3.36);
    experimentalMixes = {
        {"Traço 1 (Rico 1:4.0)", 4.0, ac1, 45.2, 80.0, 2.410, 2410.0 / (5.0 + ac1)},
        {"Traço 2 (Piloto 1:5.0)", 5.0, ac2, 39.9, 80.0, 2.390, 2390.0 / (6.0 + ac2)},
        {"Traço 3 (Pobre 1:6.0)", 6.0, ac3, 32.5, 80.0, 2.400, 2400.0 / (7.0 + ac3)}
    };

    manualAc = 0.4285;
    manualM = 5.80;
    manualC = 332.0;

    updatePilotBatchFromMixerVolume();
}

static std::string translateEnvClass(EnvironmentalClass c, bool isEnglish) {
    if (!isEnglish) return ConcreteStandards::getEnvironmentalClassName(c);
    switch (c) {
        case EnvironmentalClass::CAA_I:   return "Class I (Weak)";
        case EnvironmentalClass::CAA_II:  return "Class II (Moderate)";
        case EnvironmentalClass::CAA_III: return "Class III (Strong)";
        case EnvironmentalClass::CAA_IV:  return "Class IV (Very Strong)";
    }
    return "Class I (Weak)";
}

static std::string translateCheck(const std::string& msg, bool isEnglish) {
    if (!isEnglish) return msg;

    // 1. fck mínimo (NBR 12655 / NBR 6118)
    if (msg.find("atende à classe mínima") != std::string::npos) {
        size_t p1 = msg.find('(');
        size_t p2 = msg.find(')');
        size_t p3 = msg.find("mínima ");
        if (p1 != std::string::npos && p2 != std::string::npos && p3 != std::string::npos) {
            std::string fckVal = msg.substr(p1, p2 - p1 + 1);
            std::string rest = msg.substr(p3 + 7);
            return "Design fck " + fckVal + " meets minimum concrete class " + rest;
        }
        return "Design fck meets minimum normative concrete class requirement.";
    }
    if ((msg.find("fck de projeto") != std::string::npos || msg.find("fck") != std::string::npos) &&
        (msg.find("é inferior ao mínimo") != std::string::npos || msg.find("inferior ao mínimo") != std::string::npos)) {
        return "Design fck is below the normative minimum required for the selected Environmental Aggressiveness Class per ABNT NBR 12655:2022 / NBR 6118:2023.";
    }

    // 2. Relação água/cimento (w/c)
    if (msg.find("Relação água/cimento") != std::string::npos && msg.find("atende ao limite") != std::string::npos) {
        size_t p1 = msg.find('(');
        size_t p2 = msg.find(')');
        size_t p3 = msg.find("normativo de ");
        if (p1 != std::string::npos && p2 != std::string::npos && p3 != std::string::npos) {
            std::string acVal = msg.substr(p1, p2 - p1 + 1);
            std::string lim = msg.substr(p3 + 13);
            return "Water/cement ratio " + acVal + " meets normative maximum limit of " + lim;
        }
        return "Water/cement ratio meets normative maximum limit.";
    }
    if (msg.find("Relação água/cimento") != std::string::npos && msg.find("é superior ao limite") != std::string::npos) {
        return "Calculated water/cement ratio exceeds the normative maximum limit for the selected Environmental Aggressiveness Class. Risk of high capillary permeability and aggressive agent ingress (Mehta & Monteiro).";
    }

    // 3. Consumo de cimento
    if (msg.find("Consumo de cimento") != std::string::npos && msg.find("atende ao mínimo") != std::string::npos) {
        size_t p1 = msg.find('(');
        size_t p2 = msg.find(')');
        size_t p3 = msg.find("normativo de ");
        if (p1 != std::string::npos && p2 != std::string::npos && p3 != std::string::npos) {
            std::string cVal = msg.substr(p1, p2 - p1 + 1);
            std::string lim = msg.substr(p3 + 13);
            return "Cement consumption " + cVal + " meets normative minimum requirement of " + lim;
        }
        return "Cement consumption meets normative minimum requirement.";
    }
    if (msg.find("Consumo de cimento calculado") != std::string::npos && msg.find("é inferior ao mínimo") != std::string::npos) {
        return "Calculated cement consumption is below the normative minimum required by ABNT NBR 12655:2022 Table 2 for the selected Environmental Aggressiveness Class.";
    }

    // 4. Condição de preparo
    if (msg.find("Condição de preparo e desvio-padrão") != std::string::npos ||
        msg.find("desvio-padrão adotado atendem") != std::string::npos) {
        return "Batching preparation condition and adopted standard deviation comply with class limits of ABNT NBR 12655:2022.";
    }
    if (msg.find("Condição B de preparo") != std::string::npos) {
        return "Preparation Condition B (Sd = 5.5 MPa) is only permitted for concrete classes C10 to C20 (fck <= 20 MPa) per ABNT NBR 12655:2022 (items 5.4.2 and 5.6.3.1). Condition A is required.";
    }
    if (msg.find("Condição C de preparo") != std::string::npos) {
        return "Preparation Condition C (Sd = 7.0 MPa) is only permitted for concrete classes C10 and C15 (fck <= 15 MPa) per ABNT NBR 12655:2022 (items 5.4.3 and 5.6.3.1).";
    }

    // 5. Consistência e Abatimento (Slump)
    if (msg.find("Abatimento especificado") != std::string::npos && msg.find("inferior ao limite inferior") != std::string::npos) {
        return "Specified slump is below the lower limit of NBR 8953 (10 mm - Class S10). Excessively dry mix for plastic concrete.";
    }
    if (msg.find("Consistência NBR 8953:") != std::string::npos || msg.find("consistência NBR 8953:") != std::string::npos) {
        std::string res = msg;
        size_t p = res.find("Consistência NBR 8953: Classe ");
        if (p != std::string::npos) {
            res.replace(p, 30, "NBR 8953 Consistency: Class ");
        }
        p = res.find("Tolerância de recebimento NBR 7212:2025: ");
        if (p != std::string::npos) {
            res.replace(p, 41, "NBR 7212:2025 Acceptance Tolerance: ");
        }
        p = res.find(" a ");
        if (p != std::string::npos) {
            res.replace(p, 3, " to ");
        }
        return res;
    }
    if (msg.find("Consistência S10") != std::string::npos) {
        return "S10 Consistency (< 50 mm): Dry/stiff concrete, suitable for pavements and roller-compacted/extruded elements. Not recommended for conventional pumping or slender densely reinforced elements.";
    }
    if (msg.find("Consistência S220") != std::string::npos) {
        return "S220 Consistency (≥ 220 mm): Highly fluid concrete. Requires superplasticizer admixture and strict control of bleeding and segregation (NBR 7212:2025).";
    }
    if (msg.find("Abatimento (slump)") != std::string::npos || msg.find("consistência") != std::string::npos || msg.find("Consistência") != std::string::npos) {
        return "Slump consistency and tolerance classified per ABNT NBR 8953:2015 and NBR 7212:2025.";
    }

    // 6. DMC e Cobrimento Nominal (NBR 6118)
    if (msg.find("ultrapassa 1,2 vezes o cobrimento nominal") != std::string::npos) {
        return "Maximum characteristic coarse aggregate size (DMC) exceeds 1.2 times nominal cover for beams/columns (ABNT NBR 6118:2023 item 7.4.7.6). Risk of segregation and honeycombing.";
    }
    if (msg.find("atende ao critério geométrico para vigas/pilares") != std::string::npos) {
        std::string res = msg;
        size_t p = res.find("DMC do agregado");
        if (p != std::string::npos) res.replace(p, 15, "Coarse aggregate DMC");
        p = res.find("atende ao critério geométrico para vigas/pilares");
        if (p != std::string::npos) res.replace(p, 48, "meets geometric criterion for beams/columns");
        p = res.find("conforme NBR 6118");
        if (p != std::string::npos) res.replace(p, 17, "per NBR 6118");
        return res;
    }
    if (msg.find("supera 1,2 × cnom de lajes") != std::string::npos || msg.find("Nota geométrica para lajes") != std::string::npos) {
        return "Geometric note for slabs: Coarse aggregate DMC exceeds 1.2 × cnom of slabs. Coarse aggregate 0/1 is recommended for slab elements in this class.";
    }
    if (msg.find("atende ao critério para lajes") != std::string::npos) {
        std::string res = msg;
        size_t p = res.find("DMC do agregado");
        if (p != std::string::npos) res.replace(p, 15, "Coarse aggregate DMC");
        p = res.find("atende ao critério para lajes");
        if (p != std::string::npos) res.replace(p, 29, "meets geometric criterion for slabs");
        return res;
    }

    // 7. Literatura Especializada (Mehta & Monteiro)
    if (msg.find("a/c > 0,50") != std::string::npos || msg.find("a/c > 0.50") != std::string::npos) {
        return "Mehta & Monteiro: Concretes with w/c > 0.50 exhibit a continuous and interconnected capillary pore network, increasing ionic diffusion and carbonation rate.";
    }
    if (msg.find("acima de 450") != std::string::npos || msg.find("acima de 450 kg/m³") != std::string::npos) {
        return "Mehta & Monteiro: Cement consumption above 450 kg/m³ increases thermal and autogenous shrinkage cracking risk. Consider plasticizers/superplasticizers to reduce paste volume.";
    }

    // 8. Escopo e Outros
    if (msg.find("Escopo da verificação:") != std::string::npos) {
        return "Scope of verification: normative durability, mix design, consistency/workability, and aggregate geometry parameters (ABNT NBR 12655, NBR 6118, NBR 8953, and NBR 7212).";
    }
    if (msg.find("Extrapolação Matemática") != std::string::npos) {
        return "Mathematical Extrapolation Warning: Target mean strength fcj is outside the experimental test domain. Results represent mathematical extrapolation.";
    }
    if (msg.find("Regressão de Abrams não convergiu") != std::string::npos) {
        return "Abrams regression did not converge to an exact analytical root. Result uses an experimental average fallback and is flagged as NON-COMPLIANT.";
    }

    return msg;
}

std::string ProjectData::generateMarkdownReport(const MixDesignResult& res, bool isEnglish) const {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(3);

    const std::string dispProj = getDisplayProjectName(isEnglish);
    const std::string dispEng = getDisplayEngineerName(isEnglish);
    const std::string dispInst = getDisplayInstitution(isEnglish);
    const std::string dispId = getDisplayStudentId();

    if (fck <= 0.0 || res.targetWaterCement <= 0.0) {
        if (isEnglish) {
            ss << "# TECHNICAL REPORT & EXPERIMENTAL CONCRETE MIX DESIGN\n\n"
               << "**Project:** " << dispProj << "  \n"
               << "**Institution:** " << dispInst << " | **ID / Code:** " << dispId << "  \n"
               << "**Responsible:** " << dispEng << "  \n\n"
               << "---\n\n"
               << "> **Status:** Waiting for user input data.\n\n"
               << "Please fill in the project parameters, materials, and experimental mixes in the application tabs to generate the complete calculation report.\n";
        } else {
            ss << "# MEMORIAL DE CÁLCULO E ESTUDO DE DOSAGEM EXPERIMENTAL\n\n"
               << "**Projeto:** " << dispProj << "  \n"
               << "**Instituição:** " << dispInst << " | **Matrícula / ID:** " << dispId << "  \n"
               << "**Responsável:** " << dispEng << "  \n\n"
               << "---\n\n"
               << "> **Status:** Aguardando preenchimento dos dados de entrada.\n\n"
               << "Por favor, preencha os parâmetros de projeto, características dos materiais e traços experimentais nas abas do aplicativo para gerar o memorial de cálculo completo.\n";
        }
        return ss.str();
    }

    if (isEnglish) {
        ss << "# TECHNICAL REPORT & EXPERIMENTAL CONCRETE MIX DESIGN\n";
        ss << "## IPT / EPUSP Method and Standards (ABNT NBR 12655, NBR 6118, NBR 7211, NBR 8953, NBR 7212)\n\n";

        ss << "**Project:** " << dispProj << "  \n";
        ss << "**Institution:** " << dispInst << " | **ID / Code:** " << dispId << "  \n";
        ss << "**Responsible:** " << dispEng << "  \n\n";

        ss << "---\n\n";
        ss << "### 1. Design Parameters and Durability Requirements\n\n";
        ss << "| Parameter | Input Value | Unit | Normative Reference |\n";
        ss << "| :--- | :--- | :--- | :--- |\n";
        ss << "| Characteristic Compressive Strength ($f_{ck}$) | " << fck << " | MPa | Structural Design (NBR 6118) |\n";
        ss << "| Dosage Standard Deviation ($S_d$) | " << res.sd << " | MPa | NBR 12655:2022 Table 7 |\n";
        ss << "| **Target Mean Strength ($f_{cj}$)** | **" << res.fcj << "** | **MPa** | **$f_{cj} = f_{ck} + 1.65 S_d$** |\n";
        ss << "| Environmental Aggressiveness Class | " << translateEnvClass(envClass, true) << " | - | NBR 12655 Tab. 1 / NBR 6118 Tab. 6.1 |\n";
        ss << "| Structural Type | " << (structType == StructuralType::ReinforcedConcrete ? "Reinforced Concrete (RC)" : "Prestressed Concrete (PC)") << " | - | - |\n";
        ss << "| Slump Cone Workability (Slump) | " << targetSlumpMm << " | mm | NBR 8953:2015 |\n";
        ss << "| Adopted Mortar Content ($\\alpha$) | " << selectedAlphaPercent << " | % | IPT / EPUSP Method |\n\n";

        ss << "### 2. Fitted Experimental Mix Equations (4-Quadrant Diagram)\n\n";
        ss << "- **1st Quadrant - Abrams' Law ($f_{cj}$ vs. $w/c$):**  \n  `" << res.abramsPoly.toEquationString("fcj", "w/c") << "`  \n";
        ss << "- **2nd Quadrant - Lyse's Law ($m$ vs. $w/c$):**  \n  `" << res.lyseLinear.toEquationString("m", "w/c") << "`  \n";
        ss << "- **3rd Quadrant - Molinari's Law ($m$ vs. $C$):**  \n  `" << res.molinariPoly.toEquationString("m", "C") << "`  \n\n";

        ss << "#### 2.1. Mathematical Diagnostics & Solution Traceability\n\n";
        ss << "| Item | Mathematical Status | Diagnostic Observation |\n";
        ss << "| :--- | :--- | :--- |\n";
        ss << "| Water/Cement ($w/c$) Resolution | "
           << (res.waterCementSolutionType == SolutionType::AnalyticalExact ? "**Exact Analytical** (Quadratic Root)" :
               res.waterCementSolutionType == SolutionType::ExponentialFallback ? "*Exponential Fallback*" :
               res.waterCementSolutionType == SolutionType::ExperimentalAverage ? "❌ **AVERAGE FALLBACK (Non-Compliant)**" : "None")
           << " | "
           << (res.waterCementSolutionType == SolutionType::AnalyticalExact ? "Strict convergence via Abrams quadratic equation." :
               res.waterCementSolutionType == SolutionType::ExponentialFallback ? "Secondary exponential model used; approximate solution." :
               "Abrams equation did not yield a valid root. Fallback average used; compliance is blocked.")
           << " |\n";
        ss << "| Cement Content ($C$) Resolution | "
           << (res.cementSolutionType == SolutionType::AnalyticalExact ? "**Exact Analytical** (Molinari)" : "❌ **AVERAGE FALLBACK**")
           << " | "
           << (res.cementSolutionType == SolutionType::AnalyticalExact ? "Strict root obtained from Molinari equation." : "Average cement consumption adopted.")
           << " |\n";
        ss << "| Experimental Domain ($f_{cj}$) | "
           << (res.isExtrapolated ? "⚠️ **Extrapolated**" : "✅ **Interpolated**")
           << " | Target $f_{cj} = " << res.fcj << "$ MPa is "
           << (res.isExtrapolated ? "outside experimental test bounds. Results represent mathematical extrapolation." : "within experimental test bounds.")
           << " |\n\n";

        ss << "### 3. Optimal Mix Proportion Results\n\n";
        ss << "| Variable | Analytical Solution (Equations) | Graphical Solution (Manual Reading) | Difference ($\\Delta$) |\n";
        ss << "| :--- | :--- | :--- | :--- |\n";
        ss << "| Water/Cement Ratio ($w/c$) | **" << res.targetWaterCement << "** | " << res.graphicalWaterCement << " | " << std::abs(res.targetWaterCement - res.graphicalWaterCement) << " |\n";
        ss << "| Total Dry Mix Ratio ($m$) | **" << res.targetDryMixM << "** | " << res.graphicalDryMixM << " | " << std::abs(res.targetDryMixM - res.graphicalDryMixM) << " |\n";
        ss << "| Cement Content ($C$) | **" << res.targetCementConsumption << " kg/m³** | " << res.graphicalCementConsumption << " kg/m³ | " << std::abs(res.targetCementConsumption - res.graphicalCementConsumption) << " kg/m³ |\n";
        ss << "| Dry Mix Proportion by Mass | **1 : " << res.sandRatioA << " : " << res.gravelRatioP << "** | 1 : " << res.graphSandRatioA << " : " << res.graphGravelRatioP << " | - |\n\n";

        const double cementVolM3 = (cement.unitMass > 0.0) ? (res.perM3Analytical.cementKg / (cement.unitMass * 1000.0)) : 0.0;
        ss << "### 4. Material Consumption per Cubic Meter of Concrete ($1 \\text{ m}^3$)\n\n";
        ss << "| Material | Dry Mass (kg) | Moisture $h$ (%) | Corrected Wet Mass (kg) | Wet Volume ($m^3$) |\n";
        ss << "| :--- | :--- | :--- | :--- | :--- |\n";
        ss << "| Cement | " << res.perM3Analytical.cementKg << " | - | " << res.perM3Analytical.cementKg << " | " << cementVolM3 << " |\n";
        ss << "| Medium Sand | " << res.perM3Analytical.sandDryKg << " | " << sand.moisturePercent << " | " << res.perM3Analytical.sandWetKg << " | " << res.perM3Analytical.sandVolumeM3 << " |\n";
        if (useGravelBlend && gravel2Percent > 0.0) {
            ss << "| Coarse Aggregate 1 (" << gravel1Percent << "%) | " << res.perM3Analytical.gravel1DryKg << " | " << gravel1.moisturePercent << " | " << res.perM3Analytical.gravel1WetKg << " | " << res.perM3Analytical.gravel1VolumeM3 << " |\n";
            ss << "| Coarse Aggregate 2 (" << gravel2Percent << "%) | " << res.perM3Analytical.gravel2DryKg << " | " << gravel2.moisturePercent << " | " << res.perM3Analytical.gravel2WetKg << " | " << res.perM3Analytical.gravel2VolumeM3 << " |\n";
        } else {
            ss << "| Coarse Aggregate (Gravel) | " << res.perM3Analytical.gravelDryKg << " | " << gravel1.moisturePercent << " | " << res.perM3Analytical.gravelWetKg << " | " << res.perM3Analytical.gravelVolumeM3 << " |\n";
        }
        ss << "| Mixing Water | " << res.perM3Analytical.waterTheoreticalL << " | - | **" << res.perM3Analytical.waterCorrectedL << " L** | " << (res.perM3Analytical.waterCorrectedL / 1000.0) << " |\n";
        ss << "| **Total Concrete Mass** | **" << (res.perM3Analytical.cementKg + res.perM3Analytical.sandDryKg + res.perM3Analytical.gravelDryKg + res.perM3Analytical.waterTheoreticalL) << " kg** | - | **" << res.perM3Analytical.totalMassKg << " kg** | **1.000 $m^3$** |\n\n";

        ss << "### 5. Mixer Batching & Jobsite Measuring Boxes (1 Bag of 50 kg)\n\n";
        ss << "- **Laboratory Mixer Batch (" << res.labMixerVolumeLiters << " Liters):**  \n";
        ss << "  - Cement: `" << res.labBatch.cementKg << " kg`  \n";
        ss << "  - Wet Sand: `" << res.labBatch.sandWetKg << " kg`  \n";
        ss << "  - Wet Gravel: `" << res.labBatch.gravelWetKg << " kg`  \n";
        ss << "  - Effective Water: `" << res.labBatch.waterCorrectedL << " L`  \n\n";

        ss << "- **Jobsite Measuring Boxes (Base 35 cm x 45 cm):**  \n";
        ss << "  - Sand: " << res.jobsiteBatch50kg.sandPadiolaCount << " box(es) with height `" << std::setprecision(1) << res.jobsiteBatch50kg.sandPadiolaHeightCm << " cm` (" << std::setprecision(2) << res.jobsiteBatch50kg.sandVolumeLiters << " L)  \n";
        ss << "  - Gravel: " << res.jobsiteBatch50kg.gravelPadiolaCount << " box(es) with height `" << std::setprecision(1) << res.jobsiteBatch50kg.gravelPadiolaHeightCm << " cm` (" << std::setprecision(2) << res.jobsiteBatch50kg.gravelVolumeLiters << " L)  \n";
        ss << "  - Water to add: `" << std::setprecision(2) << res.jobsiteBatch50kg.waterLiters << " L` per 50 kg bag of cement.  \n\n";

        const std::string curEn = "$";
        ss << "### 6. Economic Analysis & Cost Comparison (" << projectVolumeM3 << " m³ Project)\n\n";
        ss << "| Item | Analytical (Equations) | Graphical Solution | Difference |\n";
        ss << "| :--- | :--- | :--- | :--- |\n";
        ss << "| Cost per Cubic Meter | " << curEn << " " << std::setprecision(2) << res.costPerM3Analytical.totalCost << " | " << curEn << " " << res.costPerM3Graphical.totalCost << " | " << curEn << " " << res.costDiffPerM3 << "/m³ |\n";
        ss << "| **Total Project Cost (" << projectVolumeM3 << " m³)** | **" << curEn << " " << res.projectCostAnalytical.totalCost << "** | **" << curEn << " " << res.projectCostGraphical.totalCost << "** | **" << curEn << " " << res.projectCostDiff << "** |\n";
        ss << "| Total Cement Bags (50 kg) | " << res.totalCementBagsAnalytical << " bags | " << res.totalCementBagsGraphical << " bags | " << res.diffCementBags << " bags |\n\n";

        ss << "### 7. Technical Rigor & Standards Verification\n\n";
        if (res.compliance.isCompliant) {
            ss << "> **STATUS: COMPLIANT WITH ABNT NBR 12655:2022 AND NBR 6118:2023 STANDARDS.**\n\n";
        } else {
            ss << "> **STATUS: ATTENTION - PENDING REQUIREMENTS OR NON-COMPLIANCES IDENTIFIED.**\n\n";
        }

        ss << "**Verifications Completed Successfully:**\n";
        for (const auto& chk : res.compliance.passedChecks) {
            ss << "- [x] " << translateCheck(chk, true) << "\n";
        }

        if (!res.compliance.warnings.empty()) {
            ss << "\n**Technical Warnings:**\n";
            for (const auto& w : res.compliance.warnings) {
                ss << "- [!] " << translateCheck(w, true) << "\n";
            }
        }

        if (!res.compliance.errors.empty()) {
            ss << "\n**Critical Non-Compliances:**\n";
            for (const auto& e : res.compliance.errors) {
                ss << "- [x] " << translateCheck(e, true) << "\n";
            }
        }

        if (!res.compliance.recommendations.empty()) {
            ss << "\n**Recommendations & Technical Literature (Mehta & Monteiro):**\n";
            for (const auto& rec : res.compliance.recommendations) {
                ss << "- *Note:* " << translateCheck(rec, true) << "\n";
            }
        }
    } else {
        ss << "# MEMORIAL DE CÁLCULO E ESTUDO DE DOSAGEM EXPERIMENTAL\n";
        ss << "## Método IPT / EPUSP e Normas Brasileiras (ABNT NBR 12655, NBR 6118, NBR 7211, NBR 8953, NBR 7212)\n\n";

        ss << "**Projeto:** " << dispProj << "  \n";
        ss << "**Instituição:** " << dispInst << " | **Matrícula / ID:** " << dispId << "  \n";
        ss << "**Responsável:** " << dispEng << "  \n\n";

        ss << "---\n\n";
        ss << "### 1. Parâmetros de Projeto e Requisitos de Durabilidade\n\n";
        ss << "| Parâmetro | Valor de Entrada | Unidade | Referência Normativa |\n";
        ss << "| :--- | :--- | :--- | :--- |\n";
        ss << "| Resistência Característica ($f_{ck}$) | " << fck << " | MPa | Projeto Estrutural (NBR 6118) |\n";
        ss << "| Desvio-Padrão da Dosagem ($S_d$) | " << res.sd << " | MPa | NBR 12655:2022 Tabela 7 |\n";
        ss << "| **Resistência de Dosagem ($f_{cj}$)** | **" << res.fcj << "** | **MPa** | **$f_{cj} = f_{ck} + 1,65 S_d$** |\n";
        ss << "| Classe de Agressividade Ambiental | " << ConcreteStandards::getEnvironmentalClassName(envClass) << " | - | NBR 12655 Tab. 1 / NBR 6118 Tab. 6.1 |\n";
        ss << "| Tipo Estrutural | " << (structType == StructuralType::ReinforcedConcrete ? "Concreto Armado (CA)" : "Concreto Protendido (CP)") << " | - | - |\n";
        ss << "| Abatimento de Tronco de Cone (Slump) | " << targetSlumpMm << " | mm | NBR 8953:2015 |\n";
        ss << "| Teor de Argamassa Adotado ($\\alpha$) | " << selectedAlphaPercent << " | % | Método IPT/EPUSP |\n\n";

        ss << "### 2. Equações Experimentais de Dosagem Ajustadas (Diagrama de 4 Quadrantes)\n\n";
        ss << "- **1º Quadrante - Lei de Abrams ($f_{cj}$ x $a/c$):**  \n  `" << res.abramsPoly.toEquationString("fcj", "a/c") << "`  \n";
        ss << "- **2º Quadrante - Lei de Lyse ($m$ x $a/c$):**  \n  `" << res.lyseLinear.toEquationString("m", "a/c") << "`  \n";
        ss << "- **3º Quadrante - Lei de Molinari ($m$ x $C$):**  \n  `" << res.molinariPoly.toEquationString("m", "C") << "`  \n\n";

        ss << "#### 2.1. Diagnóstico de Resolução Matemática e Rastreabilidade\n\n";
        ss << "| Item | Status Matemático | Observação do Diagnóstico |\n";
        ss << "| :--- | :--- | :--- |\n";
        ss << "| Resolução de Água/Cimento ($a/c$) | "
           << (res.waterCementSolutionType == SolutionType::AnalyticalExact ? "**Analítica Exata** (Raiz de Abrams)" :
               res.waterCementSolutionType == SolutionType::ExponentialFallback ? "*Ajuste Exponencial*" :
               res.waterCementSolutionType == SolutionType::ExperimentalAverage ? "❌ **MÉDIA EXPERIMENTAL (Não Conforme)**" : "Nenhum")
           << " | "
           << (res.waterCementSolutionType == SolutionType::AnalyticalExact ? "Convergência matemática estrita via raiz da equação de Abrams." :
               res.waterCementSolutionType == SolutionType::ExponentialFallback ? "Ajuste secundário exponencial adotado; solução aproximada." :
               "A regressão não convergiu para uma raiz válida. Traço utiliza média experimental; conformidade normativa bloqueada.")
           << " |\n";
        ss << "| Resolução de Consumo ($C$) | "
           << (res.cementSolutionType == SolutionType::AnalyticalExact ? "**Analítica Exata** (Molinari)" : "❌ **MÉDIA EXPERIMENTAL**")
           << " | "
           << (res.cementSolutionType == SolutionType::AnalyticalExact ? "Raiz estrita obtida da equação de Molinari." : "Média dos ensaios experimentais adotada.")
           << " |\n";
        ss << "| Domínio Experimental ($f_{cj}$) | "
           << (res.isExtrapolated ? "⚠️ **Extrapolação**" : "✅ **Interpolação**")
           << " | $f_{cj} = " << res.fcj << "$ MPa está "
           << (res.isExtrapolated ? "fora do intervalo experimental ensaiado. Traço constitui extrapolação matemática." : "dentro do intervalo experimental ensaiado.")
           << " |\n\n";

        ss << "### 3. Resultados do Traço Ótimo de Dosagem\n\n";
        ss << "| Variável | Solução Analítica (Equações) | Solução Gráfica (Leitura Manual) | Variação ($\\Delta$) |\n";
        ss << "| :--- | :--- | :--- | :--- |\n";
        ss << "| Relação Água/Cimento ($a/c$) | **" << res.targetWaterCement << "** | " << res.graphicalWaterCement << " | " << std::abs(res.targetWaterCement - res.graphicalWaterCement) << " |\n";
        ss << "| Traço Seco Total ($m$) | **" << res.targetDryMixM << "** | " << res.graphicalDryMixM << " | " << std::abs(res.targetDryMixM - res.graphicalDryMixM) << " |\n";
        ss << "| Consumo de Cimento ($C$) | **" << res.targetCementConsumption << " kg/m³** | " << res.graphicalCementConsumption << " kg/m³ | " << std::abs(res.targetCementConsumption - res.graphicalCementConsumption) << " kg/m³ |\n";
        ss << "| Traço Unitário Seco em Massa | **1 : " << res.sandRatioA << " : " << res.gravelRatioP << "** | 1 : " << res.graphSandRatioA << " : " << res.graphGravelRatioP << " | - |\n\n";

        const double cementVolM3 = (cement.unitMass > 0.0) ? (res.perM3Analytical.cementKg / (cement.unitMass * 1000.0)) : 0.0;
        ss << "### 4. Consumo de Materiais por Metro Cúbico de Concreto ($1 \\text{ m}^3$)\n\n";
        ss << "| Material | Massa Seca (kg) | Umidade $h$ (%) | Massa Úmida Corrigida (kg) | Volume Úmido ($m^3$) |\n";
        ss << "| :--- | :--- | :--- | :--- | :--- |\n";
        ss << "| Cimento | " << res.perM3Analytical.cementKg << " | - | " << res.perM3Analytical.cementKg << " | " << cementVolM3 << " |\n";
        ss << "| Areia Média | " << res.perM3Analytical.sandDryKg << " | " << sand.moisturePercent << " | " << res.perM3Analytical.sandWetKg << " | " << res.perM3Analytical.sandVolumeM3 << " |\n";
        if (useGravelBlend && gravel2Percent > 0.0) {
            ss << "| Brita 1 (" << gravel1Percent << "%) | " << res.perM3Analytical.gravel1DryKg << " | " << gravel1.moisturePercent << " | " << res.perM3Analytical.gravel1WetKg << " | " << res.perM3Analytical.gravel1VolumeM3 << " |\n";
            ss << "| Brita 2 (" << gravel2Percent << "%) | " << res.perM3Analytical.gravel2DryKg << " | " << gravel2.moisturePercent << " | " << res.perM3Analytical.gravel2WetKg << " | " << res.perM3Analytical.gravel2VolumeM3 << " |\n";
        } else {
            ss << "| Brita | " << res.perM3Analytical.gravelDryKg << " | " << gravel1.moisturePercent << " | " << res.perM3Analytical.gravelWetKg << " | " << res.perM3Analytical.gravelVolumeM3 << " |\n";
        }
        ss << "| Água de Amassamento | " << res.perM3Analytical.waterTheoreticalL << " | - | **" << res.perM3Analytical.waterCorrectedL << " L** | " << (res.perM3Analytical.waterCorrectedL / 1000.0) << " |\n";
        ss << "| **Massa Total do Concreto** | **" << (res.perM3Analytical.cementKg + res.perM3Analytical.sandDryKg + res.perM3Analytical.gravelDryKg + res.perM3Analytical.waterTheoreticalL) << " kg** | - | **" << res.perM3Analytical.totalMassKg << " kg** | **1,000 $m^3$** |\n\n";

        ss << "### 5. Dimensionamento para Betoneira e Padiolas de Canteiro (1 Saco de 50 kg)\n\n";
        ss << "- **Amassada para Betoneira de Laboratório (" << res.labMixerVolumeLiters << " Litros):**  \n";
        ss << "  - Cimento: `" << res.labBatch.cementKg << " kg`  \n";
        ss << "  - Areia Úmida: `" << res.labBatch.sandWetKg << " kg`  \n";
        ss << "  - Brita Úmida: `" << res.labBatch.gravelWetKg << " kg`  \n";
        ss << "  - Água Efetiva: `" << res.labBatch.waterCorrectedL << " L`  \n\n";

        ss << "- **Padiolas para Canteiro (Base 35 cm x 45 cm):**  \n";
        ss << "  - Areia: " << res.jobsiteBatch50kg.sandPadiolaCount << " padiola(s) com altura de `" << std::setprecision(1) << res.jobsiteBatch50kg.sandPadiolaHeightCm << " cm` (" << std::setprecision(2) << res.jobsiteBatch50kg.sandVolumeLiters << " L)  \n";
        ss << "  - Brita: " << res.jobsiteBatch50kg.gravelPadiolaCount << " padiola(s) com altura de `" << std::setprecision(1) << res.jobsiteBatch50kg.gravelPadiolaHeightCm << " cm` (" << std::setprecision(2) << res.jobsiteBatch50kg.gravelVolumeLiters << " L)  \n";
        ss << "  - Água a adicionar: `" << std::setprecision(2) << res.jobsiteBatch50kg.waterLiters << " L` por saco de cimento.  \n\n";

        ss << "### 6. Análise Econômica e Diferença de Custos (Obra de " << projectVolumeM3 << " m³)\n\n";
        ss << "| Item | Analítico (Equações) | Leitura Gráfica | Diferença |\n";
        ss << "| :--- | :--- | :--- | :--- |\n";
        ss << "| Custo por Metro Cúbico | R$ " << std::setprecision(2) << res.costPerM3Analytical.totalCost << " | R$ " << res.costPerM3Graphical.totalCost << " | R$ " << res.costDiffPerM3 << "/m³ |\n";
        ss << "| **Custo Total da Obra (" << projectVolumeM3 << " m³)** | **R$ " << res.projectCostAnalytical.totalCost << "** | **R$ " << res.projectCostGraphical.totalCost << "** | **R$ " << res.projectCostDiff << "** |\n";
        ss << "| Total de Sacos de Cimento (50 kg) | " << res.totalCementBagsAnalytical << " sacos | " << res.totalCementBagsGraphical << " sacos | " << res.diffCementBags << " sacos |\n\n";

        ss << "### 7. Verificação de Rigor Técnico e Normas\n\n";
        if (res.compliance.isCompliant) {
            ss << "> **STATUS: CONFORME AS NORMAS ABNT NBR 12655:2022 E NBR 6118:2023.**\n\n";
        } else {
            ss << "> **STATUS: ATENÇÃO - PENDÊNCIAS OU NÃO CONFORMIDADES IDENTIFICADAS.**\n\n";
        }

        ss << "**Verificações Concluídas com Sucesso:**\n";
        for (const auto& chk : res.compliance.passedChecks) {
            ss << "- [x] " << chk << "\n";
        }

        if (!res.compliance.warnings.empty()) {
            ss << "\n**Alertas Técnicos:**\n";
            for (const auto& w : res.compliance.warnings) {
                ss << "- [!] " << w << "\n";
            }
        }

        if (!res.compliance.errors.empty()) {
            ss << "\n**Inconformidades Críticas:**\n";
            for (const auto& e : res.compliance.errors) {
                ss << "- [x] " << e << "\n";
            }
        }

        if (!res.compliance.recommendations.empty()) {
            ss << "\n**Recomendações e Literatura Técnica (Mehta & Monteiro):**\n";
            for (const auto& rec : res.compliance.recommendations) {
                ss << "- *Nota:* " << rec << "\n";
            }
        }
    }

    return ss.str();
}

std::string ProjectData::generateHtmlReport(const MixDesignResult& res, bool isDarkMode, bool isEnglish) const {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2);

    ss << "<!DOCTYPE html>\n<html><head><meta charset='utf-8'>\n"
       << "<title>" << (isEnglish ? "Technical Report - Concrete Mix Designer" : "Memorial de Cálculo - Módulo de Dosagem de Concreto") << "</title>\n"
       << "<style>\n";

    if (isDarkMode) {
        ss << "  body { font-family: 'Segoe UI', Arial, Helvetica, sans-serif; font-size: 13px; color: #f0f6fc; line-height: 1.5; margin: 20px; background-color: #1e1e1e; }\n"
           << "  .header-box { border: 2px solid #388bfd; background-color: #252b33; padding: 16px 20px; border-radius: 6px; margin-bottom: 20px; }\n"
           << "  .header-title { font-size: 20px; font-weight: bold; color: #58a6ff; margin: 0 0 4px 0; text-transform: uppercase; letter-spacing: 0.5px; }\n"
           << "  .header-sub { font-size: 13px; color: #8b949e; font-weight: 600; margin-bottom: 12px; }\n"
           << "  .meta-table { width: 100%; border: none; margin: 0; }\n"
           << "  .meta-table td { border: none; padding: 3px 0; font-size: 12px; color: #c9d1d9; }\n"
           << "  .meta-label { font-weight: bold; color: #79c0ff; width: 18%; }\n"
           << "  .meta-val { width: 32%; }\n"
           << "  h2 { font-size: 15px; color: #79c0ff; border-bottom: 2px solid #30363d; padding-bottom: 5px; margin-top: 24px; margin-bottom: 12px; }\n"
           << "  h3 { font-size: 13px; color: #7ee787; margin-top: 14px; margin-bottom: 6px; }\n"
           << "  table.data-table { width: 100%; border-collapse: collapse; margin-top: 8px; margin-bottom: 16px; font-size: 12px; }\n"
           << "  table.data-table th { background-color: #21262d; color: #f0f6fc; font-weight: bold; border: 1px solid #30363d; padding: 7px 10px; text-align: left; }\n"
           << "  table.data-table td { border: 1px solid #30363d; padding: 6px 10px; vertical-align: middle; color: #c9d1d9; }\n"
           << "  table.data-table tr:nth-child(even) { background-color: #161b22; }\n"
           << "  .summary-card { background-color: #161b22; border: 1px solid #30363d; border-radius: 4px; padding: 10px 14px; margin: 12px 0; color: #f0f6fc; }\n"
           << "  .eq-box { background-color: #161b22; border: 1px solid #30363d; border-left: 4px solid #58a6ff; padding: 8px 12px; margin: 6px 0; font-family: Consolas, monospace; font-size: 12px; color: #79c0ff; }\n"
           << "  .badge-pass { background-color: #1b4729; color: #7ee787; border: 1px solid #238636; padding: 4px 8px; font-weight: bold; border-radius: 3px; display: inline-block; }\n"
           << "  .badge-fail { background-color: #49181c; color: #ff7b72; border: 1px solid #da3633; padding: 4px 8px; font-weight: bold; border-radius: 3px; display: inline-block; }\n";
    } else {
        ss << "  body { font-family: 'Segoe UI', Arial, Helvetica, sans-serif; font-size: 13px; color: #212529; line-height: 1.5; margin: 20px; background-color: #ffffff; }\n"
           << "  .header-box { border: 2px solid #0056b3; background-color: #f4f8fc; padding: 16px 20px; border-radius: 6px; margin-bottom: 20px; }\n"
           << "  .header-title { font-size: 20px; font-weight: bold; color: #003366; margin: 0 0 4px 0; text-transform: uppercase; letter-spacing: 0.5px; }\n"
           << "  .header-sub { font-size: 13px; color: #495057; font-weight: 600; margin-bottom: 12px; }\n"
           << "  .meta-table { width: 100%; border: none; margin: 0; }\n"
           << "  .meta-table td { border: none; padding: 3px 0; font-size: 12px; color: #333333; }\n"
           << "  .meta-label { font-weight: bold; color: #004085; width: 18%; }\n"
           << "  .meta-val { width: 32%; }\n"
           << "  h2 { font-size: 15px; color: #004085; border-bottom: 2px solid #b8daff; padding-bottom: 5px; margin-top: 24px; margin-bottom: 12px; }\n"
           << "  h3 { font-size: 13px; color: #155724; margin-top: 14px; margin-bottom: 6px; }\n"
           << "  table.data-table { width: 100%; border-collapse: collapse; margin-top: 8px; margin-bottom: 16px; font-size: 12px; }\n"
           << "  table.data-table th { background-color: #e9ecef; color: #212529; font-weight: bold; border: 1px solid #ced4da; padding: 7px 10px; text-align: left; }\n"
           << "  table.data-table td { border: 1px solid #dee2e6; padding: 6px 10px; vertical-align: middle; }\n"
           << "  table.data-table tr:nth-child(even) { background-color: #f8f9fa; }\n"
           << "  .summary-card { background-color: #e8f4fd; border: 1px solid #b8daff; border-radius: 4px; padding: 10px 14px; margin: 12px 0; }\n"
           << "  .eq-box { background-color: #f8f9fa; border: 1px solid #dee2e6; border-left: 4px solid #007acc; padding: 8px 12px; margin: 6px 0; font-family: Consolas, monospace; font-size: 12px; }\n"
           << "  .badge-pass { background-color: #d4edda; color: #155724; border: 1px solid #c3e6cb; padding: 4px 8px; font-weight: bold; border-radius: 3px; display: inline-block; }\n"
           << "  .badge-fail { background-color: #f8d7da; color: #721c24; border: 1px solid #f5c6cb; padding: 4px 8px; font-weight: bold; border-radius: 3px; display: inline-block; }\n";
    }

    ss << "  ul { margin: 6px 0 12px 20px; padding: 0; }\n"
       << "  li { margin-bottom: 4px; }\n"
       << "</style></head><body>\n";

    const std::string dispProj = getDisplayProjectName(isEnglish);
    const std::string dispEng = getDisplayEngineerName(isEnglish);
    const std::string dispInst = getDisplayInstitution(isEnglish);
    const std::string dispId = getDisplayStudentId();

    // 1. Cabeçalho Oficial
    ss << "<div class='header-box'>\n"
       << "  <div class='header-title'>" << (isEnglish ? "TECHNICAL REPORT & EXPERIMENTAL CONCRETE MIX DESIGN" : "MEMORIAL DE CÁLCULO E ESTUDO DE DOSAGEM EXPERIMENTAL") << "</div>\n"
       << "  <div class='header-sub'>" << (isEnglish ? "IPT / EPUSP Method and Standards (ABNT NBR 12655:2022, NBR 6118:2023, NBR 7211:2022, NBR 8953:2015, NBR 7212:2025)" : "Método IPT / EPUSP e Normas Brasileiras (ABNT NBR 12655:2022, NBR 6118:2023, NBR 7211:2022, NBR 8953:2015, NBR 7212:2025)") << "</div>\n"
       << "  <table class='meta-table'>\n"
       << "    <tr>\n"
       << "      <td class='meta-label'>" << (isEnglish ? "Project:" : "Projeto:") << "</td><td class='meta-val'><b>" << dispProj << "</b></td>\n"
       << "      <td class='meta-label'>" << (isEnglish ? "Responsible:" : "Responsável:") << "</td><td class='meta-val'><b>" << dispEng << "</b></td>\n"
       << "    </tr>\n"
       << "    <tr>\n"
       << "      <td class='meta-label'>" << (isEnglish ? "Institution:" : "Instituição:") << "</td><td class='meta-val'>" << dispInst << "</td>\n"
       << "      <td class='meta-label'>" << (isEnglish ? "ID / Code:" : "Matrícula / ID:") << "</td><td class='meta-val'>" << dispId << "</td>\n"
       << "    </tr>\n"
       << "  </table>\n"
       << "</div>\n";

    if (fck <= 0.0 || res.targetWaterCement <= 0.0) {
        ss << "<div class='summary-card' style='text-align: center; padding: 40px;'>\n"
           << "  <h3 style='font-size: 16px; margin-bottom: 12px;'>" << (isEnglish ? "⏳ Waiting for User Input Data" : "⏳ Aguardando Preenchimento dos Dados de Entrada") << "</h3>\n"
           << "  <p style='color: " << (isDarkMode ? "#8b949e" : "#555555") << ";'>" << (isEnglish ? "Please enter the project parameters, materials, and experimental mixes in the application tabs to generate the complete calculation report." : "Insira os parâmetros de projeto, características dos materiais e traços experimentais nas abas correspondentes para gerar o memorial de cálculo completo.") << "</p>\n"
           << "</div>\n"
           << "<hr style='margin-top: 30px; border: 0; border-top: 1px solid " << (isDarkMode ? "#30363d" : "#ced4da") << ";'>\n"
           << "<p style='font-size: 11px; color: #6c757d; text-align: center;'>" << (isEnglish ? "Report generated by Concrete Mix Designer (C++20) | IPT / EPUSP Method | ABNT Standards Compliance" : "Relatório emitido pelo Módulo de Dosagem de Concreto (C++20) | Método IPT / EPUSP | Conformidade Normativa ABNT") << "</p>\n"
           << "</body></html>\n";
        return ss.str();
    }

    // Destaques do Traço de Projeto
    ss << "<div class='summary-card'>\n"
       << "  <b>" << (isEnglish ? "DESIGN MIX HIGHLIGHTS:" : "DESTAQUES DO TRAÇO DE PROJETO:") << "</b><br>\n"
       << "  <b>" << (isEnglish ? "Dry Unit Mix by Mass:" : "Traço Unitário Seco em Massa:") << "</b> 1 : " << std::setprecision(2) << res.sandRatioA << " : " << res.gravelRatioP
       << " (m = " << res.targetDryMixM << ") &nbsp;|&nbsp; "
       << "  <b>" << (isEnglish ? "w/c:" : "a/c:") << "</b> " << std::setprecision(3) << res.targetWaterCement << " &nbsp;|&nbsp; "
       << "  <b>" << (isEnglish ? "Cement Content:" : "Consumo de Cimento:") << "</b> " << std::setprecision(1) << res.targetCementConsumption << " kg/m³ &nbsp;|&nbsp; "
       << "  <b>" << (isEnglish ? "Target fcj:" : "fcj de Dosagem:") << "</b> " << std::setprecision(2) << res.fcj << " MPa &nbsp;|&nbsp; "
       << "  <b>Status:</b> " << (res.compliance.isCompliant ? (isEnglish ? "<span class='badge-pass'>✔ NBR 12655 COMPLIANT</span>" : "<span class='badge-pass'>✔ CONFORME NBR 12655</span>")
                                                        : (isEnglish ? "<span class='badge-fail'>⚠ ATTENTION / NON-COMPLIANT</span>" : "<span class='badge-fail'>⚠ ATENÇÃO / NÃO CONFORME</span>")) << "\n"
       << "</div>\n";

    // 1. Parâmetros de Projeto
    ss << "<h2>" << (isEnglish ? "1. Design Parameters and Durability Requirements" : "1. Parâmetros de Projeto e Requisitos de Durabilidade") << "</h2>\n"
       << "<table class='data-table'>\n"
       << "  <tr><th>" << (isEnglish ? "Parameter" : "Parâmetro") << "</th><th>" << (isEnglish ? "Adopted Value" : "Valor Adotado") << "</th><th>" << (isEnglish ? "Unit" : "Unidade") << "</th><th>" << (isEnglish ? "Normative Reference" : "Referência Normativa") << "</th></tr>\n"
       << "  <tr><td>" << (isEnglish ? "Characteristic Compressive Strength (fck)" : "Resistência Característica à Compressão (fck)") << "</td><td><b>" << std::setprecision(1) << fck << "</b></td><td>MPa</td><td>" << (isEnglish ? "Structural Design (NBR 6118)" : "Projeto Estrutural (NBR 6118)") << "</td></tr>\n"
       << "  <tr><td>" << (isEnglish ? "Dosage Standard Deviation (Sd)" : "Desvio-Padrão da Dosagem (Sd)") << "</td><td>" << std::setprecision(1) << res.sd << "</td><td>MPa</td><td>" << (isEnglish ? "NBR 12655:2022 Table 7" : "NBR 12655:2022 Tabela 7") << "</td></tr>\n"
       << "  <tr><td><b>" << (isEnglish ? "Target Mean Strength (fcj)" : "Resistência Média de Dosagem (fcj)") << "</b></td><td><b>" << std::setprecision(2) << res.fcj << "</b></td><td><b>MPa</b></td><td><b>" << (isEnglish ? "fcj = fck + 1.65 * Sd" : "fcj = fck + 1,65 * Sd") << "</b></td></tr>\n"
       << "  <tr><td>" << (isEnglish ? "Environmental Aggressiveness Class (CAA)" : "Classe de Agressividade Ambiental (CAA)") << "</td><td>" << translateEnvClass(envClass, isEnglish) << "</td><td>-</td><td>NBR 12655 Tab. 1 / NBR 6118 Tab. 6.1</td></tr>\n"
       << "  <tr><td>" << (isEnglish ? "Structural Type" : "Tipo Estrutural") << "</td><td>" << (structType == StructuralType::ReinforcedConcrete ? (isEnglish ? "Reinforced Concrete (RC)" : "Concreto Armado (CA)") : (isEnglish ? "Prestressed Concrete (PC)" : "Concreto Protendido (CP)")) << "</td><td>-</td><td>NBR 6118:2023</td></tr>\n"
       << "  <tr><td>" << (isEnglish ? "Slump Cone Workability (Slump)" : "Abatimento de Tronco de Cone (Slump)") << "</td><td>" << std::setprecision(0) << targetSlumpMm << "</td><td>mm</td><td>NBR 8953:2015</td></tr>\n"
       << "  <tr><td>" << (isEnglish ? "Adopted Mortar Content (&alpha;)" : "Teor de Argamassa Adotado (&alpha;)") << "</td><td>" << std::setprecision(1) << selectedAlphaPercent << "</td><td>%</td><td>" << (isEnglish ? "IPT / EPUSP Method" : "Método IPT / EPUSP") << "</td></tr>\n"
       << "</table>\n";

    // 2. Equações Matemáticas Ajustadas
    ss << "<h2>" << (isEnglish ? "2. Fitted Mathematical Equations (IPT 4-Quadrant Diagram)" : "2. Equações Matemáticas Ajustadas (Diagrama de 4 Quadrantes IPT)") << "</h2>\n"
       << "<div class='eq-box'><b>" << (isEnglish ? "1st Quadrant - Abrams' Law (fcj vs. w/c):" : "1º Quadrante - Lei de Abrams (fcj x a/c):") << "</b><br>"
       << res.abramsPoly.toEquationString("fcj", isEnglish ? "w/c" : "a/c") << "</div>\n"
       << "<div class='eq-box'><b>" << (isEnglish ? "2nd Quadrant - Lyse's Law (m vs. w/c):" : "2º Quadrante - Lei de Lyse (m x a/c):") << "</b><br>"
       << res.lyseLinear.toEquationString("m", isEnglish ? "w/c" : "a/c") << "</div>\n"
       << "<div class='eq-box'><b>" << (isEnglish ? "3rd Quadrant - Molinari's Law (m vs. C):" : "3º Quadrante - Lei de Molinari (m x C):") << "</b><br>"
       << res.molinariPoly.toEquationString("m", "C") << "</div>\n";

    // 2.1 Diagnóstico de Resolução Matemática
    ss << "<h3>" << (isEnglish ? "2.1. Mathematical Diagnostics & Solution Traceability" : "2.1. Diagnóstico de Resolução Matemática e Rastreabilidade") << "</h3>\n"
       << "<table class='data-table'>\n"
       << "  <tr><th>" << (isEnglish ? "Parameter / Model" : "Parâmetro / Modelo") << "</th><th>" << (isEnglish ? "Resolution Type" : "Tipo de Resolução") << "</th><th>" << (isEnglish ? "Status & Diagnostic Observations" : "Status e Observações do Diagnóstico") << "</th></tr>\n"
       << "  <tr><td>" << (isEnglish ? "Water/Cement Ratio (w/c)" : "Relação Água/Cimento (a/c)") << "</td><td>"
       << (res.waterCementSolutionType == SolutionType::AnalyticalExact ? "<span class='badge-pass'>✔ " + std::string(isEnglish ? "Exact Analytical" : "Analítica Exata") + "</span>" :
           res.waterCementSolutionType == SolutionType::ExponentialFallback ? "<span style='color:#b8860b; font-weight:bold;'>⚠ " + std::string(isEnglish ? "Exponential Fallback" : "Ajuste Exponencial") + "</span>" :
           "<span class='badge-fail'>✖ " + std::string(isEnglish ? "Experimental Average (Non-Compliant)" : "Média dos Ensaios (Não Conforme)") + "</span>")
       << "</td><td>"
       << (res.waterCementSolutionType == SolutionType::AnalyticalExact ? (isEnglish ? "Strict quadratic positive root converged." : "Raiz positiva da equação de Abrams convergida com sucesso.") :
           res.waterCementSolutionType == SolutionType::ExponentialFallback ? (isEnglish ? "Approximated via secondary exponential fit." : "Aproximação obtida por modelo secundário exponencial.") :
           (isEnglish ? "Abrams equation did not yield a valid root. Average fallback used; compliance blocked." : "Regressão de Abrams falhou em obter raiz válida. Utilizada média experimental; conformidade bloqueada."))
       << "</td></tr>\n"
       << "  <tr><td>" << (isEnglish ? "Cement Content (C)" : "Consumo de Cimento (C)") << "</td><td>"
       << (res.cementSolutionType == SolutionType::AnalyticalExact ? "<span class='badge-pass'>✔ " + std::string(isEnglish ? "Exact Analytical" : "Analítica Exata") + "</span>" :
           "<span class='badge-fail'>✖ " + std::string(isEnglish ? "Experimental Average" : "Média dos Ensaios") + "</span>")
       << "</td><td>"
       << (res.cementSolutionType == SolutionType::AnalyticalExact ? (isEnglish ? "Strict root obtained from Molinari equation." : "Raiz estrita obtida da equação de Molinari.") :
           (isEnglish ? "Calculated via experimental average." : "Calculado pela média dos ensaios experimentais."))
       << "</td></tr>\n"
       << "  <tr><td>" << (isEnglish ? "Experimental Domain" : "Domínio Experimental") << "</td><td>"
       << (res.isExtrapolated ? "<span class='badge-fail'>⚠ " + std::string(isEnglish ? "Extrapolated" : "Extrapolação") + "</span>" :
                                "<span class='badge-pass'>✔ " + std::string(isEnglish ? "Interpolated" : "Interpolação") + "</span>")
       << "</td><td>"
       << (res.isExtrapolated ? (isEnglish ? "Target fcj is outside experimental test bounds. Results represent mathematical extrapolation." : "fcj de projeto está fora dos limites dos ensaios. Traço constitui extrapolação matemática.") :
                                (isEnglish ? "Target fcj is within experimental test bounds." : "fcj de projeto está contida dentro dos limites dos ensaios experimentais."))
       << "</td></tr>\n"
       << "</table>\n";

    // 3. Resultados do Traço Ótimo
    ss << "<h2>" << (isEnglish ? "3. Optimal Mix Proportion Results" : "3. Resultados do Traço Ótimo de Dosagem") << "</h2>\n"
       << "<table class='data-table'>\n"
       << "  <tr><th>" << (isEnglish ? "Design Variable" : "Variável de Dosagem") << "</th><th>" << (isEnglish ? "Analytical Solution (Equations)" : "Solução Analítica (Equações)") << "</th><th>" << (isEnglish ? "Graphical Solution (Manual Reading)" : "Leitura Gráfica (Ábaco Manual)") << "</th><th>" << (isEnglish ? "Difference (|&Delta;|)" : "Diferença (|&Delta;|)") << "</th></tr>\n"
       << "  <tr><td>" << (isEnglish ? "Water/Cement Ratio (w/c)" : "Relação Água/Cimento (a/c)") << "</td><td><b>" << std::setprecision(4) << res.targetWaterCement << "</b></td><td>" << std::setprecision(4) << res.graphicalWaterCement << "</td><td>" << std::setprecision(4) << std::abs(res.targetWaterCement - res.graphicalWaterCement) << "</td></tr>\n"
       << "  <tr><td>" << (isEnglish ? "Total Dry Mix Ratio m (1:m)" : "Traço Seco Total m (1:m)") << "</td><td><b>" << std::setprecision(2) << res.targetDryMixM << "</b></td><td>" << std::setprecision(2) << res.graphicalDryMixM << "</td><td>" << std::setprecision(2) << std::abs(res.targetDryMixM - res.graphicalDryMixM) << "</td></tr>\n"
       << "  <tr><td>" << (isEnglish ? "Cement Content C" : "Consumo de Cimento C") << "</td><td><b>" << std::setprecision(1) << res.targetCementConsumption << " kg/m³</b></td><td>" << std::setprecision(1) << res.graphicalCementConsumption << " kg/m³</td><td>" << std::setprecision(1) << std::abs(res.targetCementConsumption - res.graphicalCementConsumption) << " kg/m³</td></tr>\n"
       << "  <tr><td>" << (isEnglish ? "Dry Mix Proportion by Mass" : "Traço Unitário em Massa Seca") << "</td><td><b>1 : " << std::setprecision(2) << res.sandRatioA << " : " << res.gravelRatioP << "</b></td><td>1 : " << res.graphSandRatioA << " : " << res.graphGravelRatioP << "</td><td>-</td></tr>\n"
       << "</table>\n";

    const double cementVolM3 = (cement.unitMass > 0.0) ? (res.perM3Analytical.cementKg / (cement.unitMass * 1000.0)) : 0.0;
    const double cementBagVolL = (cement.unitMass > 0.0) ? (50.0 / cement.unitMass) : 0.0;

    // 4. Consumo de Materiais por 1 m³
    ss << "<h2>" << (isEnglish ? "4. Material Consumption per Cubic Meter of Concrete (1 m³)" : "4. Consumo de Materiais por Metro Cúbico de Concreto (1 m³)") << "</h2>\n"
       << "<table class='data-table'>\n"
       << "  <tr><th>" << (isEnglish ? "Constituent Material" : "Material Constituinte") << "</th><th>" << (isEnglish ? "Dry Mass (kg)" : "Massa Seca (kg)") << "</th><th>" << (isEnglish ? "Moisture h (%)" : "Umidade h (%)") << "</th><th>" << (isEnglish ? "Corrected Wet Mass (kg)" : "Massa Úmida Corrigida (kg)") << "</th><th>" << (isEnglish ? "Wet Volume (m³ or L)" : "Volume Úmido (m³ ou L)") << "</th></tr>\n"
       << "  <tr><td>" << (isEnglish ? "Portland Cement" : "Cimento Portland") << "</td><td>" << std::setprecision(2) << res.perM3Analytical.cementKg << "</td><td>-</td><td>" << res.perM3Analytical.cementKg << "</td><td>" << std::setprecision(3) << cementVolM3 << " m³</td></tr>\n"
       << "  <tr><td>" << (isEnglish ? "Medium Natural Sand" : "Areia Natural Média") << "</td><td>" << std::setprecision(2) << res.perM3Analytical.sandDryKg << "</td><td>" << std::setprecision(1) << sand.moisturePercent << "%</td><td>" << std::setprecision(2) << res.perM3Analytical.sandWetKg << "</td><td>" << std::setprecision(3) << res.perM3Analytical.sandVolumeM3 << " m³</td></tr>\n";
    if (useGravelBlend && gravel2Percent > 0.0) {
        ss << "  <tr><td>" << (isEnglish ? "Coarse Aggregate 1 (" : "Brita 1 (") << std::setprecision(0) << gravel1Percent << "%)</td><td>" << std::setprecision(2) << res.perM3Analytical.gravel1DryKg << "</td><td>" << std::setprecision(1) << gravel1.moisturePercent << "%</td><td>" << std::setprecision(2) << res.perM3Analytical.gravel1WetKg << "</td><td>" << std::setprecision(3) << res.perM3Analytical.gravel1VolumeM3 << " m³</td></tr>\n"
           << "  <tr><td>" << (isEnglish ? "Coarse Aggregate 2 (" : "Brita 2 (") << std::setprecision(0) << gravel2Percent << "%)</td><td>" << std::setprecision(2) << res.perM3Analytical.gravel2DryKg << "</td><td>" << std::setprecision(1) << gravel2.moisturePercent << "%</td><td>" << std::setprecision(2) << res.perM3Analytical.gravel2WetKg << "</td><td>" << std::setprecision(3) << res.perM3Analytical.gravel2VolumeM3 << " m³</td></tr>\n";
    } else {
        ss << "  <tr><td>" << (isEnglish ? "Coarse Aggregate (Gravel)" : "Agregado Graúdo (Brita)") << "</td><td>" << std::setprecision(2) << res.perM3Analytical.gravelDryKg << "</td><td>" << std::setprecision(1) << gravel1.moisturePercent << "%</td><td>" << std::setprecision(2) << res.perM3Analytical.gravelWetKg << "</td><td>" << std::setprecision(3) << res.perM3Analytical.gravelVolumeM3 << " m³</td></tr>\n";
    }
    ss << "  <tr><td>" << (isEnglish ? "Mixing Water" : "Água de Amassamento") << "</td><td>" << std::setprecision(2) << res.perM3Analytical.waterTheoreticalL << " L</td><td>-</td><td><b>" << res.perM3Analytical.waterCorrectedL << " L</b></td><td>" << std::setprecision(3) << (res.perM3Analytical.waterCorrectedL / 1000.0) << " m³</td></tr>\n"
       << "  <tr style='font-weight: bold; background-color: " << (isDarkMode ? "#21262d" : "#e9ecef") << ";'><td>" << (isEnglish ? "Total Concrete Mass" : "Massa Total do Concreto") << "</td><td>" << std::setprecision(2) << (res.perM3Analytical.cementKg + res.perM3Analytical.sandDryKg + res.perM3Analytical.gravelDryKg + res.perM3Analytical.waterTheoreticalL) << " kg</td><td>-</td><td>" << res.perM3Analytical.totalMassKg << " kg</td><td>" << (isEnglish ? "1.000 m³" : "1,000 m³") << "</td></tr>\n"
       << "</table>\n";

    // 5. Dimensionamento para Betoneira e Canteiro
    ss << "<h2>" << (isEnglish ? "5. Mixer Batching & Jobsite Measuring Boxes" : "5. Dimensionamento de Betoneira e Padiolas de Canteiro") << "</h2>\n"
       << "<h3>" << (isEnglish ? "Laboratory Batch (Mixer Volume = " : "Amassada de Laboratório (Volume do Misturador = ") << std::setprecision(1) << res.labMixerVolumeLiters << (isEnglish ? " Liters)" : " Litros)") << "</h3>\n"
       << "<table class='data-table'>\n"
       << "  <tr><th>" << (isEnglish ? "Material" : "Material") << "</th><th>" << (isEnglish ? "Dry Mass (kg)" : "Massa Seca (kg)") << "</th><th>" << (isEnglish ? "Effective Weight to Batch (kg)" : "Massa Efetiva a Pesar (kg)") << "</th></tr>\n"
       << "  <tr><td>" << (isEnglish ? "Cement" : "Cimento") << "</td><td>" << std::setprecision(3) << res.labBatch.cementKg << "</td><td><b>" << res.labBatch.cementKg << " kg</b></td></tr>\n"
       << "  <tr><td>" << (isEnglish ? "Sand" : "Areia") << "</td><td>" << res.labBatch.sandDryKg << "</td><td><b>" << res.labBatch.sandWetKg << " kg</b></td></tr>\n";
    if (useGravelBlend && gravel2Percent > 0.0) {
        ss << "  <tr><td>" << (isEnglish ? "Gravel 1" : "Brita 1") << "</td><td>" << res.labBatch.gravel1DryKg << "</td><td><b>" << res.labBatch.gravel1WetKg << " kg</b></td></tr>\n"
           << "  <tr><td>" << (isEnglish ? "Gravel 2" : "Brita 2") << "</td><td>" << res.labBatch.gravel2DryKg << "</td><td><b>" << res.labBatch.gravel2WetKg << " kg</b></td></tr>\n";
    } else {
        ss << "  <tr><td>" << (isEnglish ? "Gravel" : "Brita") << "</td><td>" << res.labBatch.gravelDryKg << "</td><td><b>" << res.labBatch.gravelWetKg << " kg</b></td></tr>\n";
    }
    ss << "  <tr><td>" << (isEnglish ? "Effective Water" : "Água Efetiva") << "</td><td>" << res.labBatch.waterTheoreticalL << " L</td><td><b>" << res.labBatch.waterCorrectedL << " L</b></td></tr>\n"
       << "</table>\n"
       << "<h3>" << (isEnglish ? "Jobsite Batching (Basis: 1 Bag of 50 kg Cement)" : "Dosagem em Canteiro de Obras (Base: 1 Saco de 50 kg de Cimento)") << "</h3>\n"
       << "<table class='data-table'>\n"
       << "  <tr><th>" << (isEnglish ? "Component" : "Componente") << "</th><th>" << (isEnglish ? "Wet Mass (kg)" : "Massa Úmida (kg)") << "</th><th>" << (isEnglish ? "Bulk Volume (L)" : "Volume Aparente (L)") << "</th><th>" << (isEnglish ? "Measuring Box Sizing (35 cm x 45 cm)" : "Dimensionamento em Padiolas (35 cm x 45 cm)") << "</th></tr>\n"
       << "  <tr><td>" << (isEnglish ? "Cement" : "Cimento") << "</td><td>" << (isEnglish ? "50.00" : "50,00") << "</td><td>" << std::setprecision(1) << cementBagVolL << "</td><td>" << (isEnglish ? "1 sealed 50 kg bag" : "1 saco fechado de 50 kg") << "</td></tr>\n"
       << "  <tr><td>" << (isEnglish ? "Wet Sand" : "Areia Úmida") << "</td><td>" << std::setprecision(2) << res.jobsiteBatch50kg.sandWetKg << "</td><td>" << std::setprecision(1) << res.jobsiteBatch50kg.sandVolumeLiters << "</td><td>" << res.jobsiteBatch50kg.sandPadiolaCount << (isEnglish ? " box(es) with h = " : " padiola(s) com h = ") << std::setprecision(1) << res.jobsiteBatch50kg.sandPadiolaHeightCm << " cm</td></tr>\n"
       << "  <tr><td>" << (isEnglish ? "Wet Gravel" : "Brita Úmida") << "</td><td>" << std::setprecision(2) << res.jobsiteBatch50kg.gravelWetKg << "</td><td>" << std::setprecision(1) << res.jobsiteBatch50kg.gravelVolumeLiters << "</td><td>" << res.jobsiteBatch50kg.gravelPadiolaCount << (isEnglish ? " box(es) with h = " : " padiola(s) com h = ") << std::setprecision(1) << res.jobsiteBatch50kg.gravelPadiolaHeightCm << " cm</td></tr>\n"
       << "  <tr><td>" << (isEnglish ? "Water to Add" : "Água a Adicionar") << "</td><td>" << std::setprecision(1) << res.jobsiteBatch50kg.waterLiters << "</td><td>" << res.jobsiteBatch50kg.waterLiters << "</td><td><b>" << (isEnglish ? "Add exactly " : "Adicionar exatamente ") << res.jobsiteBatch50kg.waterLiters << (isEnglish ? " Liters" : " Litros") << "</b></td></tr>\n"
       << "</table>\n";

    // 6. Análise Econômica
    const std::string curHtml = isEnglish ? "$" : "R$";
    ss << "<h2>" << (isEnglish ? "6. Economic Analysis & Cost Comparison (" : "6. Análise Econômica e Diferença de Custos (Obra de ") << std::setprecision(1) << projectVolumeM3 << " m³)</h2>\n"
       << "<table class='data-table'>\n"
       << "  <tr><th>" << (isEnglish ? "Evaluated Item" : "Item Avaliado") << "</th><th>" << (isEnglish ? "Analytical Solution" : "Solução Analítica") << "</th><th>" << (isEnglish ? "Graphical Solution" : "Solução Gráfica") << "</th><th>" << (isEnglish ? "Difference (|&Delta;|)" : "Diferença (|&Delta;|)") << "</th></tr>\n"
       << "  <tr><td>" << (isEnglish ? "Cost per Cubic Meter of Concrete" : "Custo por Metro Cúbico de Concreto") << "</td><td>" << curHtml << " " << std::setprecision(2) << res.costPerM3Analytical.totalCost << "</td><td>" << curHtml << " " << res.costPerM3Graphical.totalCost << "</td><td>" << curHtml << " " << res.costDiffPerM3 << " / m³</td></tr>\n"
       << "  <tr><td><b>" << (isEnglish ? "Total Project Cost (" : "Custo Total da Obra (") << std::setprecision(0) << projectVolumeM3 << " m³)</b></td><td><b>" << curHtml << " " << std::setprecision(2) << res.projectCostAnalytical.totalCost << "</b></td><td><b>" << curHtml << " " << res.projectCostGraphical.totalCost << "</b></td><td><b>" << curHtml << " " << res.projectCostDiff << "</b></td></tr>\n"
       << "  <tr><td>" << (isEnglish ? "Total Cement Bags (50 kg)" : "Total de Sacos de Cimento (50 kg)") << "</td><td>" << res.totalCementBagsAnalytical << (isEnglish ? " bags" : " sacos") << "</td><td>" << res.totalCementBagsGraphical << (isEnglish ? " bags" : " sacos") << "</td><td>" << res.diffCementBags << (isEnglish ? " bags" : " sacos") << "</td></tr>\n"
       << "</table>\n";

    // 7. Rigor Técnico e Normas
    ss << "<h2>" << (isEnglish ? "7. Technical Rigor & Standards Compliance (NBR)" : "7. Verificação de Rigor Técnico e Conformidade Normativa (NBR)") << "</h2>\n";
    if (res.compliance.isCompliant) {
        ss << "<div class='badge-pass'>" << (isEnglish ? "✔ STATUS: COMPLIANT WITH ABNT NBR 12655:2022 AND NBR 6118:2023" : "✔ STATUS: CONFORME AS NORMAS ABNT NBR 12655:2022 E NBR 6118:2023") << "</div>\n";
    } else {
        ss << "<div class='badge-fail'>" << (isEnglish ? "⚠ STATUS: ATTENTION - NORMATIVE REQUIREMENTS PENDING" : "⚠ STATUS: ATENÇÃO - REQUISITOS NORMATIVOS PENDENTES") << "</div>\n";
    }

    ss << "<p><b>" << (isEnglish ? "Safety and Durability Verifications Passed Successfully:" : "Verificações de Segurança e Durabilidade Concluídas com Sucesso:") << "</b></p><ul>\n";
    for (const auto& chk : res.compliance.passedChecks) {
        ss << "<li>✔ " << translateCheck(chk, isEnglish) << "</li>\n";
    }
    ss << "</ul>\n";

    if (!res.compliance.warnings.empty()) {
        ss << "<p><b>" << (isEnglish ? "Technical Warnings Identified:" : "Alertas Técnicos Identificados:") << "</b></p><ul>\n";
        for (const auto& w : res.compliance.warnings) {
            ss << "<li style='color: #856404;'>⚠ " << translateCheck(w, isEnglish) << "</li>\n";
        }
        ss << "</ul>\n";
    }

    if (!res.compliance.errors.empty()) {
        ss << "<p><b>" << (isEnglish ? "Critical Non-Compliances:" : "Inconformidades Críticas:") << "</b></p><ul>\n";
        for (const auto& e : res.compliance.errors) {
            ss << "<li style='color: #721c24;'>✖ " << translateCheck(e, isEnglish) << "</li>\n";
        }
        ss << "</ul>\n";
    }

    if (!res.compliance.recommendations.empty()) {
        ss << "<p><b>" << (isEnglish ? "Recommendations & Specialized Literature (Mehta & Monteiro):" : "Recomendações e Literatura Especializada (Mehta & Monteiro):") << "</b></p><ul>\n";
        for (const auto& rec : res.compliance.recommendations) {
            ss << "<li><i>" << (isEnglish ? "Note: " : "Nota: ") << "</i> " << translateCheck(rec, isEnglish) << "</li>\n";
        }
        ss << "</ul>\n";
    }

    ss << "<hr style='margin-top: 30px; border: 0; border-top: 1px solid #ced4da;'>\n"
       << "<p style='font-size: 11px; color: #6c757d; text-align: center;'>" << (isEnglish ? "Report generated by Concrete Mix Designer (C++20) | IPT / EPUSP Method | ABNT Standards Compliance" : "Relatório emitido pelo Módulo de Dosagem de Concreto (C++20) | Método IPT / EPUSP | Conformidade Normativa ABNT") << "</p>\n"
       << "</body></html>\n";

    return ss.str();
}

std::string ProjectData::toJson() const {
    auto matToJson = [](const MaterialProperties& m) {
        QJsonObject obj;
        obj["name"] = QString::fromStdString(m.name);
        obj["specificMass"] = m.specificMass;
        obj["unitMass"] = m.unitMass;
        obj["moisturePercent"] = m.moisturePercent;
        obj["costPerUnit"] = m.costPerUnit;
        obj["costUnit"] = static_cast<int>(m.costUnit);
        return obj;
    };

    QJsonObject root;
    root["currentPresetIndex"] = currentPresetIndex;
    root["usePresetProjectName"] = usePresetProjectName;
    root["customProjectName"] = QString::fromStdString(customProjectName);
    root["projectName"] = QString::fromStdString(projectName);
    root["engineerName"] = QString::fromStdString(engineerName);
    root["institution"] = QString::fromStdString(institution);
    root["studentId"] = QString::fromStdString(studentId);
    root["notes"] = QString::fromStdString(notes);

    root["cement"] = matToJson(cement);
    root["sand"] = matToJson(sand);
    root["gravel1"] = matToJson(gravel1);
    root["gravel2"] = matToJson(gravel2);
    root["water"] = matToJson(water);

    root["useGravelBlend"] = useGravelBlend;
    root["gravel2Percent"] = gravel2Percent;
    root["gravel1Percent"] = gravel1Percent;
    root["gravel1DmcMm"] = gravel1DmcMm;
    root["gravel2DmcMm"] = gravel2DmcMm;
    root["baseGravelMassKg"] = baseGravelMassKg;
    root["containerVolumeL"] = containerVolumeL;

    root["fck"] = fck;
    root["prepCondition"] = static_cast<int>(prepCondition);
    root["customSd"] = customSd;
    root["envClass"] = static_cast<int>(envClass);
    root["structType"] = static_cast<int>(structType);
    root["targetSlumpMm"] = targetSlumpMm;

    root["pilotM"] = pilotM;
    root["pilotBatchGravelKg"] = pilotBatchGravelKg;
    root["pilotBatchWaterKg"] = pilotBatchWaterKg;
    root["selectedAlphaPercent"] = selectedAlphaPercent;

    root["projectVolumeM3"] = projectVolumeM3;
    root["labMixerVolumeL"] = labMixerVolumeL;

    root["manualAc"] = manualAc;
    root["manualM"] = manualM;
    root["manualC"] = manualC;

    QJsonArray mixesArr;
    for (const auto& mix : experimentalMixes) {
        QJsonObject mObj;
        mObj["label"] = QString::fromStdString(mix.label);
        mObj["dryRatioM"] = mix.dryRatioM;
        mObj["waterCementRatio"] = mix.waterCementRatio;
        mObj["compressiveStrength28d"] = mix.compressiveStrength28d;
        mObj["slumpMm"] = mix.slumpMm;
        mObj["freshDensity"] = mix.freshDensity;
        mObj["cementConsumption"] = mix.cementConsumption;
        mixesArr.append(mObj);
    }
    root["experimentalMixes"] = mixesArr;

    QJsonDocument doc(root);
    return doc.toJson(QJsonDocument::Indented).toStdString();
}

bool ProjectData::fromJson(const std::string& jsonStr) {
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(jsonStr), &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
        return false;
    }

    QJsonObject root = doc.object();
    auto matFromJson = [](const QJsonObject& obj, MaterialProperties& m) {
        if (obj.contains("name")) m.name = obj["name"].toString().toStdString();
        if (obj.contains("specificMass")) m.specificMass = obj["specificMass"].toDouble();
        if (obj.contains("unitMass")) m.unitMass = obj["unitMass"].toDouble();
        if (obj.contains("moisturePercent")) m.moisturePercent = obj["moisturePercent"].toDouble();
        if (obj.contains("costPerUnit")) m.costPerUnit = obj["costPerUnit"].toDouble();
        if (obj.contains("costUnit")) m.costUnit = static_cast<PriceUnit>(obj["costUnit"].toInt());
    };

    if (root.contains("currentPresetIndex")) currentPresetIndex = root["currentPresetIndex"].toInt();
    if (root.contains("usePresetProjectName")) usePresetProjectName = root["usePresetProjectName"].toBool();
    if (root.contains("customProjectName")) customProjectName = root["customProjectName"].toString().toStdString();
    if (root.contains("projectName")) projectName = root["projectName"].toString().toStdString();
    if (root.contains("engineerName")) engineerName = root["engineerName"].toString().toStdString();
    if (root.contains("institution")) institution = root["institution"].toString().toStdString();
    if (root.contains("studentId")) studentId = root["studentId"].toString().toStdString();
    if (root.contains("notes")) notes = root["notes"].toString().toStdString();

    if (root.contains("cement") && root["cement"].isObject()) matFromJson(root["cement"].toObject(), cement);
    if (root.contains("sand") && root["sand"].isObject()) matFromJson(root["sand"].toObject(), sand);
    if (root.contains("gravel1") && root["gravel1"].isObject()) matFromJson(root["gravel1"].toObject(), gravel1);
    if (root.contains("gravel2") && root["gravel2"].isObject()) matFromJson(root["gravel2"].toObject(), gravel2);
    if (root.contains("water") && root["water"].isObject()) matFromJson(root["water"].toObject(), water);

    if (root.contains("useGravelBlend")) useGravelBlend = root["useGravelBlend"].toBool();
    if (root.contains("gravel2Percent")) gravel2Percent = root["gravel2Percent"].toDouble();
    if (root.contains("gravel1Percent")) gravel1Percent = root["gravel1Percent"].toDouble();
    if (root.contains("gravel1DmcMm")) gravel1DmcMm = root["gravel1DmcMm"].toDouble();
    if (root.contains("gravel2DmcMm")) gravel2DmcMm = root["gravel2DmcMm"].toDouble();
    if (root.contains("baseGravelMassKg")) baseGravelMassKg = root["baseGravelMassKg"].toDouble();
    if (root.contains("containerVolumeL")) containerVolumeL = root["containerVolumeL"].toDouble();

    if (root.contains("fck")) fck = root["fck"].toDouble();
    if (root.contains("prepCondition")) prepCondition = static_cast<PreparationCondition>(root["prepCondition"].toInt());
    if (root.contains("customSd")) customSd = root["customSd"].toDouble();
    if (root.contains("envClass")) envClass = static_cast<EnvironmentalClass>(root["envClass"].toInt());
    if (root.contains("structType")) structType = static_cast<StructuralType>(root["structType"].toInt());
    if (root.contains("targetSlumpMm")) targetSlumpMm = root["targetSlumpMm"].toDouble();

    if (root.contains("pilotM")) pilotM = root["pilotM"].toDouble();
    if (root.contains("pilotBatchGravelKg")) pilotBatchGravelKg = root["pilotBatchGravelKg"].toDouble();
    if (root.contains("pilotBatchWaterKg")) pilotBatchWaterKg = root["pilotBatchWaterKg"].toDouble();
    if (root.contains("selectedAlphaPercent")) selectedAlphaPercent = root["selectedAlphaPercent"].toDouble();

    if (root.contains("projectVolumeM3")) projectVolumeM3 = root["projectVolumeM3"].toDouble();
    if (root.contains("labMixerVolumeL")) labMixerVolumeL = root["labMixerVolumeL"].toDouble();

    if (root.contains("manualAc")) manualAc = root["manualAc"].toDouble();
    if (root.contains("manualM")) manualM = root["manualM"].toDouble();
    if (root.contains("manualC")) manualC = root["manualC"].toDouble();

    if (root.contains("experimentalMixes") && root["experimentalMixes"].isArray()) {
        experimentalMixes.clear();
        QJsonArray arr = root["experimentalMixes"].toArray();
        for (const auto& val : arr) {
            if (!val.isObject()) continue;
            QJsonObject mObj = val.toObject();
            ExperimentalMixRecord mix;
            mix.label = mObj["label"].toString().toStdString();
            mix.dryRatioM = mObj["dryRatioM"].toDouble();
            mix.waterCementRatio = mObj["waterCementRatio"].toDouble();
            mix.compressiveStrength28d = mObj["compressiveStrength28d"].toDouble();
            mix.slumpMm = mObj["slumpMm"].toDouble();
            mix.freshDensity = mObj["freshDensity"].toDouble();
            mix.cementConsumption = mObj["cementConsumption"].toDouble();
            experimentalMixes.push_back(mix);
        }
    }

    return true;
}

} // namespace ConcreteMix
