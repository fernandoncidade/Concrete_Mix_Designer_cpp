#pragma once

#include <QString>
#include <QVector>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QRecursiveMutex>
#include <QMutexLocker>


namespace ConcreteMix {

struct CementNormativeRecord {
    QString code;               // Ex: "CP II-F", "CP V-ARI"
    QString name;               // Ex: "Cimento Portland Composto com Fíler"
    QString standard;           // Ex: "ABNT NBR 16697:2018"
    double specificMass;        // kg/dm³ (g/cm³)
    double unitMass;            // kg/dm³
    double defaultCostPerBag;   // R$/saco 50 kg
    QString clinkerAddition;    // Ex: "Clínquer + 6% a 10% de material carbonático"
    QString hydrationHeat;      // Ex: "Médio a moderado"
    QString technicalNotes;     // Mehta & Monteiro / NBR
};

struct AggregateNormativeRecord {
    QString code;               // Ex: "B1", "B2", "AREIA_MED"
    QString commercialName;     // Ex: "Brita 1 (Pedrisco)", "Areia Natural Média"
    QString aggregateType;      // "GRAUDO" ou "MIUDO"
    double dmcMm;               // Dimensão Máxima Característica (mm)
    double finenessModulus;     // Módulo de finura (MF)
    double specificMass;        // kg/dm³
    double unitMass;            // kg/dm³
    double defaultMoisture;     // %
    double defaultCostPerM3;    // R$/m³
    QString standard;           // Ex: "ABNT NBR 7211:2022"
    QString granulometricZone;  // "Zona Ótima", "Zona Utilizável", "Faixa 9,5/19"
    QString technicalNotes;     // Requisitos normativos
};

struct SlumpNormativeRecord {
    QString slumpClass;         // Ex: "S10", "S50", "S100", "S160", "S220"
    double minSlumpMm;          // mm
    double maxSlumpMm;          // mm
    QString placementMethod;    // Ex: "Convencional", "Bombeado", "Auto-adensável"
    QString application;        // Ex: "Pavimentos", "Elementos esbeltos"
    QString standard;           // "ABNT NBR 8953:2015"
};

struct DurabilityNormativeRecord {
    QString caaClass;           // "CAA I", "CAA II", "CAA III", "CAA IV"
    QString aggressiveness;     // "Fraca", "Moderada", "Forte", "Muito Forte"
    QString environment;        // "Rural / Submerso", "Urbano", "Marinho", "Industrial severo"
    QString deteriorationRisk;  // "Insignificante", "Pequeno", "Grande", "Elevado"
    double maxAcCa;             // a/c máx Concreto Armado
    double maxAcCp;             // a/c máx Concreto Protendido
    QString minClassCa;         // "C20", "C25", "C30", "C40"
    QString minClassCp;         // "C25", "C30", "C35", "C40"
    double minFckCa;            // MPa
    double minFckCp;            // MPa
    double minCementKgM3;       // kg/m³
    double coverSlabsMm;        // Cobrimento lajes (mm)
    double coverBeamsColsMm;    // Cobrimento vigas/pilares (mm)
    QString standard;           // "ABNT NBR 6118:2023 / ABNT NBR 12655:2022"
};

struct PreparationNormativeRecord {
    QString conditionCode;      // "A", "B", "C", "HISTORICO", "ARBITRARIO"
    QString conditionName;      // Ex: "Condição A - Central / Massa Total"
    double sdMpa;               // Desvio-padrão (MPa)
    QString standard;           // "ABNT NBR 12655:2022 Tabela 7"
    QString batchingMethod;     // "Cimento e agregados medidos em massa..."
    QString maxAllowedClass;    // "Sem restrição" ou "Até C20 (Condição C)"
};

struct WeighingToleranceRecord {
    QString component;          // "Cimento e Adições", "Agregados", "Água", "Aditivos"
    double tolerancePercent;    // ±1%, ±2%, ±1%, ±3%
    QString standard;           // "ABNT NBR 7212:2025"
    QString equipment;          // "Balança dosadora estática ou dinâmica"
};

struct ScenarioPresetRecord {
    int id;                     // 0, 1, 2, 3
    QString title;
    QString sheetName;
    double fck;
    double sd;
    QString prepCondition;
    QString envClass;
    QString structType;
    double targetSlumpMm;
    double selectedAlpha;
    double mixerVolumeL;
    double projectVolumeM3;
    double manualAc;
    double manualM;
    double manualC;
    QString notes;
};

class NormativeDatabase {
public:
    static NormativeDatabase& instance();

    bool initialize(const QString &dbPath = QString());
    bool isInitialized() const { return m_initialized; }

    // Consultas normativas tipadas
    QVector<CementNormativeRecord> getAllCementTypes() const;
    CementNormativeRecord getCementByCode(const QString &code) const;

    QVector<AggregateNormativeRecord> getAllAggregates() const;
    AggregateNormativeRecord getAggregateByCode(const QString &code) const;

    QVector<SlumpNormativeRecord> getAllSlumpClasses() const;
    SlumpNormativeRecord getSlumpByValue(double slumpMm) const;

    QVector<DurabilityNormativeRecord> getAllDurabilityRequirements() const;
    DurabilityNormativeRecord getDurabilityRequirement(const QString &caa) const;

    QVector<PreparationNormativeRecord> getAllPreparationConditions() const;
    PreparationNormativeRecord getPreparationCondition(const QString &code) const;

    QVector<WeighingToleranceRecord> getAllWeighingTolerances() const;

    QVector<ScenarioPresetRecord> getAllScenarios() const;
    ScenarioPresetRecord getScenarioById(int id) const;

    // Verificação normativa cruzada (NBR 6118 cobrimento vs DMC)
    bool verifyDmcAgainstCover(double dmcMm, double coverMm, QString &diagnostic) const;

private:
    NormativeDatabase();
    ~NormativeDatabase();

    bool createTables();
    bool seedData();

    bool m_initialized = false;
    QString m_connectionName;
    mutable QRecursiveMutex m_mutex;
};

} // namespace ConcreteMix
