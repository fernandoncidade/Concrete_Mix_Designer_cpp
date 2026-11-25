#pragma once

#include "MixDesignCalculator.hpp"
#include <QString>
#include <string>
#include <vector>

namespace ConcreteMix {

class ProjectData {
public:
    ProjectData();

    // Metadados do projeto e controle de exibição no Relatório Técnico
    int currentPresetIndex = 1;
    bool usePresetProjectName = true;
    std::string customProjectName = "";
    std::string projectName = "Cenário 1: Concreto Estrutural C25 Urbano (CAA II - NBR 6118 / NBR 12655)";
    std::string engineerName = "Eng. Civil";
    std::string institution = "UFPR - Universidade Federal do Paraná";
    std::string studentId = "GRR20182393";
    std::string notes;

    static std::string getPresetName(int presetIndex, bool isEnglish);
    std::string getDisplayProjectName(bool isEnglish) const;
    std::string getDisplayEngineerName(bool isEnglish) const;
    std::string getDisplayInstitution(bool isEnglish) const;
    std::string getDisplayStudentId() const;

    // Propriedades dos materiais
    MaterialProperties cement{"Cimento Portland CP II-F 32", 2.96, 1.75, 0.0, 380.0}; // R$/t
    MaterialProperties sand{"Areia Natural Média", 2.60, 1.40, 4.0, 90.0};            // R$/m³
    MaterialProperties gravel1{"Brita 1 (DMC 19 mm)", 2.70, 1.45, 1.5, 70.0};         // R$/m³
    MaterialProperties gravel2{"Brita 2 (DMC 25 mm)", 2.70, 1.55, 1.5, 70.0};         // R$/m³
    MaterialProperties water{"Água de Amassamento", 1.00, 1.00, 0.0, 0.0};

    // Parâmetros de mistura de agregados graúdos
    bool useGravelBlend = false;
    double gravel2Percent = 70.0; // 70% B2 / 30% B1
    double gravel1Percent = 30.0;
    double gravel1DmcMm = 19.0;
    double gravel2DmcMm = 25.0;
    double baseGravelMassKg = 12.0;
    double containerVolumeL = 15.04;

    double getMaxCoarseDmc() const {
        return (useGravelBlend && gravel2Percent > 0.0) ? std::max(gravel1DmcMm, gravel2DmcMm) : gravel1DmcMm;
    }
    bool checkDmcCoverCompliance(QString &diag) const;

    // Parâmetros normativos e de projeto
    double fck = 30.0; // MPa
    PreparationCondition prepCondition = PreparationCondition::Arbitrary;
    double customSd = 3.5; // MPa (ou 4.0 para Condição A)
    EnvironmentalClass envClass = EnvironmentalClass::CAA_I;
    StructuralType structType = StructuralType::ReinforcedConcrete;
    double targetSlumpMm = 80.0;

    // Parâmetros do ensaio piloto de teor de argamassa (α)
    double pilotM = 5.0;
    double pilotBatchGravelKg = 38.53;
    double pilotBatchWaterKg = 5.35;
    double selectedAlphaPercent = 52.0;

    // Atualiza as massas de brita fixa e água do traço piloto pela Lei dos Volumes Absolutos
    void updatePilotBatchFromMixerVolume();

    // Volumes
    double projectVolumeM3 = 30.0;
    double labMixerVolumeL = 29.0;

    // Traços experimentais para o Diagrama IPT
    std::vector<ExperimentalMixRecord> experimentalMixes;

    // Leituras gráficas manuais (para comparação)
    double manualAc = 0.4285;
    double manualM = 5.80;
    double manualC = 332.0;

    // Executa o cálculo completo
    MixDesignResult calculate() const;

    // Carregamento de Predefinições Baseadas na Planilha Excel e Normas Técnicas
    void loadPreset(int presetIndex);
    void loadPresetUserCustom(); // Cenário 0: Personalizável pelo usuário (em branco)
    void loadPresetReference0();
    void loadPresetResultados1();
    void loadPresetResultados2();
    void loadPresetResultados3();
    void loadPresetPlanilhaUFPR(); // Cenário 5: Dados originais corrigidos da planilha UFPR (TC031)

    // Exportação em JSON e Relatório em Markdown / HTML
    std::string toJson() const;
    bool fromJson(const std::string& jsonStr);
    std::string generateMarkdownReport(const MixDesignResult& res, bool isEnglish = false) const;
    std::string generateHtmlReport(const MixDesignResult& res, bool isDarkMode = false, bool isEnglish = false) const;
};

} // namespace ConcreteMix
