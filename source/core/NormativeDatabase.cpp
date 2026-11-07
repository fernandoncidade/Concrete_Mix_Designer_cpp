#include "NormativeDatabase.hpp"
#include "CaminhoPersistenteUtils.hpp"
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QVariant>
#include <QDebug>

namespace ConcreteMix {

NormativeDatabase& NormativeDatabase::instance() {
    static NormativeDatabase db;
    return db;
}

NormativeDatabase::NormativeDatabase() {
    m_connectionName = "ConcreteMixNormativeConnection";
    initialize();
}

NormativeDatabase::~NormativeDatabase() {
    QMutexLocker locker(&m_mutex);
    if (QSqlDatabase::contains(m_connectionName)) {
        {
            auto db = QSqlDatabase::database(m_connectionName);
            db.close();
        }
        QSqlDatabase::removeDatabase(m_connectionName);
    }
}

bool NormativeDatabase::initialize(const QString &dbPath) {
    QMutexLocker locker(&m_mutex);
    if (m_initialized) return true;

    QSqlDatabase db;
    if (QSqlDatabase::contains(m_connectionName)) {
        db = QSqlDatabase::database(m_connectionName);
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE", m_connectionName);
    }

    if (dbPath.isEmpty()) {
        QString baseDir = source::utils::obterCaminhoPersistente();
        QDir dir(baseDir);
        if (!dir.exists()) {
            dir.mkpath(".");
        }
        QString finalPath = dir.filePath("normative_database.db");
        db.setDatabaseName(finalPath);
    } else {
        db.setDatabaseName(dbPath);
    }

    if (!db.open()) {
        qWarning() << "Falha ao abrir banco de dados SQLite normativo:" << db.lastError().text();
        // Fallback para banco em memória
        db.setDatabaseName(":memory:");
        if (!db.open()) {
            qCritical() << "Falha critica ao abrir banco SQLite em memoria:" << db.lastError().text();
            return false;
        }
    }

    if (!createTables()) {
        return false;
    }

    if (!seedData()) {
        return false;
    }

    m_initialized = true;
    return true;
}

bool NormativeDatabase::createTables() {
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery q(db);

    // 1. Tabela de Tipos de Cimento (NBR 16697 / Mehta & Monteiro)
    if (!q.exec("CREATE TABLE IF NOT EXISTS cement_types ("
                "code TEXT PRIMARY KEY, "
                "name TEXT, "
                "standard TEXT, "
                "specific_mass REAL, "
                "unit_mass REAL, "
                "default_cost REAL, "
                "clinker_addition TEXT, "
                "hydration_heat TEXT, "
                "technical_notes TEXT)")) {
        qWarning() << "Erro ao criar tabela cement_types:" << q.lastError().text();
        return false;
    }

    // 2. Tabela de Agregados e DMC (NBR 7211:2022)
    if (!q.exec("CREATE TABLE IF NOT EXISTS aggregate_types ("
                "code TEXT PRIMARY KEY, "
                "commercial_name TEXT, "
                "aggregate_type TEXT, "
                "dmc_mm REAL, "
                "fineness_modulus REAL, "
                "specific_mass REAL, "
                "unit_mass REAL, "
                "default_moisture REAL, "
                "default_cost REAL, "
                "standard TEXT, "
                "granulometric_zone TEXT, "
                "technical_notes TEXT)")) {
        qWarning() << "Erro ao criar tabela aggregate_types:" << q.lastError().text();
        return false;
    }

    // 3. Tabela de Classes de Consistência e Slump (NBR 8953:2015)
    if (!q.exec("CREATE TABLE IF NOT EXISTS consistency_classes ("
                "slump_class TEXT PRIMARY KEY, "
                "min_slump REAL, "
                "max_slump REAL, "
                "placement_method TEXT, "
                "application TEXT, "
                "standard TEXT)")) {
        qWarning() << "Erro ao criar tabela consistency_classes:" << q.lastError().text();
        return false;
    }

    // 4. Tabela de Durabilidade e Agressividade Ambiental (NBR 6118:2023 / NBR 12655:2022)
    if (!q.exec("CREATE TABLE IF NOT EXISTS durability_requirements ("
                "caa_class TEXT PRIMARY KEY, "
                "aggressiveness TEXT, "
                "environment TEXT, "
                "deterioration_risk TEXT, "
                "max_ac_ca REAL, "
                "max_ac_cp REAL, "
                "min_class_ca TEXT, "
                "min_class_cp TEXT, "
                "min_fck_ca REAL, "
                "min_fck_cp REAL, "
                "min_cement_kg_m3 REAL, "
                "cover_slabs_mm REAL, "
                "cover_beams_cols_mm REAL, "
                "standard TEXT)")) {
        qWarning() << "Erro ao criar tabela durability_requirements:" << q.lastError().text();
        return false;
    }

    // 5. Tabela de Condições de Preparo (NBR 12655:2022 Tabela 7)
    if (!q.exec("CREATE TABLE IF NOT EXISTS preparation_conditions ("
                "condition_code TEXT PRIMARY KEY, "
                "condition_name TEXT, "
                "sd_mpa REAL, "
                "standard TEXT, "
                "batching_method TEXT, "
                "max_allowed_class TEXT)")) {
        qWarning() << "Erro ao criar tabela preparation_conditions:" << q.lastError().text();
        return false;
    }

    // 6. Tabela de Tolerâncias de Pesagem (NBR 7212:2025)
    if (!q.exec("CREATE TABLE IF NOT EXISTS weighing_tolerances ("
                "component TEXT PRIMARY KEY, "
                "tolerance_percent REAL, "
                "standard TEXT, "
                "equipment TEXT)")) {
        qWarning() << "Erro ao criar tabela weighing_tolerances:" << q.lastError().text();
        return false;
    }

    // 7. Tabela de Cenários da Planilha Excel
    if (!q.exec("CREATE TABLE IF NOT EXISTS scenario_presets ("
                "id INTEGER PRIMARY KEY, "
                "title TEXT, "
                "sheet_name TEXT, "
                "fck REAL, "
                "sd REAL, "
                "prep_condition TEXT, "
                "env_class TEXT, "
                "struct_type TEXT, "
                "target_slump_mm REAL, "
                "selected_alpha REAL, "
                "mixer_volume_l REAL, "
                "project_volume_m3 REAL, "
                "manual_ac REAL, "
                "manual_m REAL, "
                "manual_c REAL, "
                "notes TEXT)")) {
        qWarning() << "Erro ao criar tabela scenario_presets:" << q.lastError().text();
        return false;
    }

    return true;
}

bool NormativeDatabase::seedData() {
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery q(db);

    // 1. Popula Cimentos Brasileiros (NBR 16697 / Mehta & Monteiro)
    q.exec("SELECT COUNT(*) FROM cement_types");
    if (q.next() && q.value(0).toInt() == 0) {
        struct CementSeed {
            const char* code;
            const char* name;
            const char* standard;
            double spec;
            double unit;
            double cost;
            const char* add;
            const char* heat;
            const char* notes;
        } cements[] = {
            {"CP I", "Cimento Portland Comum", "ABNT NBR 16697:2018", 3.12, 1.15, 42.0, "Clínquer + Gesso (100% puro)", "Elevado", "Uso geral sem sulfatos."},
            {"CP I-S", "Cimento Portland Comum com Adição", "ABNT NBR 16697:2018", 3.10, 1.15, 40.0, "Clínquer + 1% a 5% de material pozolânico ou escória", "Moderado", "Uso geral estrutural convencional."},
            {"CP II-E", "Cimento Portland Composto com Escória", "ABNT NBR 16697:2018", 3.05, 1.12, 38.0, "Clínquer + 6% a 34% de escória granulada de alto-forno", "Baixo a moderado", "Menor calor de hidratação, boa durabilidade marinha."},
            {"CP II-Z", "Cimento Portland Composto com Pozolana", "ABNT NBR 16697:2018", 3.00, 1.10, 38.0, "Clínquer + 6% a 14% de material pozolânico", "Baixo", "Maior impermeabilidade, combate à RAA e sulfatos."},
            {"CP II-F", "Cimento Portland Composto com Fíler", "ABNT NBR 16697:2018", 3.10, 1.10, 38.0, "Clínquer + 6% a 10% de material carbonático (calcário)", "Moderado", "Excelente trabalhabilidade e acabamento superficial."},
            {"CP II-F32", "Cimento Portland Composto com Fíler (Classe 32)", "ABNT NBR 16697:2018", 2.96, 1.75, 38.0, "Clínquer + 6% a 10% de fíler calcário", "Moderado", "Referência padrão adotada na planilha de ensaios UFPR."},
            {"CP III", "Cimento Portland de Alto-Forno", "ABNT NBR 16697:2018", 2.98, 1.08, 37.0, "Clínquer + 35% a 70% de escória granulada", "Muito Baixo", "Obras maciças, barragens e agressividade severa por sulfatos."},
            {"CP IV", "Cimento Portland Pozolânico", "ABNT NBR 16697:2018", 2.95, 1.05, 37.0, "Clínquer + 15% a 50% de material pozolânico", "Muito Baixo", "Ambientes litorâneos, esgotos e água ácida doce."},
            {"CP V-ARI", "Cimento de Alta Resistência Inicial", "ABNT NBR 16697:2018", 3.15, 1.20, 46.0, "Clínquer nobre com moagem ultrafina", "Elevadíssimo", "Pré-moldados, protensão rápida e desforma acelerada."},
            {"RS", "Cimento Portland Resistente a Sulfatos", "ABNT NBR 16697:2018", 3.05, 1.10, 44.0, "C3A < 8% no clínquer", "Baixo", "Estações de esgoto, água do mar e solos com sulfatos."},
            {"BC", "Cimento de Baixo Calor de Hidratação", "ABNT NBR 16697:2018", 3.00, 1.08, 43.0, "Liberação ≤ 260 J/g aos 7 dias", "Mínimo", "Concreto massa para evitar fissuração térmica volumétrica."}
        };

        db.transaction();
        QSqlQuery qi(db);
        qi.prepare("INSERT INTO cement_types VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");
        for (const auto& c : cements) {
            qi.addBindValue(c.code);
            qi.addBindValue(c.name);
            qi.addBindValue(c.standard);
            qi.addBindValue(c.spec);
            qi.addBindValue(c.unit);
            qi.addBindValue(c.cost);
            qi.addBindValue(c.add);
            qi.addBindValue(c.heat);
            qi.addBindValue(c.notes);
            qi.exec();
        }
        db.commit();
    }

    // 2. Popula Agregados e DMC (NBR 7211:2022)
    q.exec("SELECT COUNT(*) FROM aggregate_types");
    if (q.next() && q.value(0).toInt() == 0) {
        struct AggSeed {
            const char* code;
            const char* name;
            const char* type;
            double dmc;
            double mf;
            double spec;
            double unit;
            double moist;
            double cost;
            const char* std;
            const char* zone;
            const char* notes;
        } aggs[] = {
            {"AREIA_FINA", "Areia Natural Fina", "MIUDO", 2.4, 1.80, 2.60, 1.40, 5.0, 95.0, "ABNT NBR 7211:2022", "Zona Utilizável Inferior", "Aumenta demanda de água de amassamento."},
            {"AREIA_MED", "Areia Natural Média", "MIUDO", 4.75, 2.50, 2.40, 1.45, 4.0, 90.0, "ABNT NBR 7211:2022", "Zona Ótima (NBR 7211)", "Granulometria ideal para traço equilibrado."},
            {"AREIA_GROSSA", "Areia Natural Grossa", "MIUDO", 4.75, 3.20, 2.65, 1.50, 3.0, 85.0, "ABNT NBR 7211:2022", "Zona Utilizável Superior", "Boa economia de pasta, verificar aspereza."},
            {"BRITA_0", "Brita 0 (Pedrisco)", "GRAUDO", 12.5, 5.80, 2.70, 1.40, 1.5, 75.0, "ABNT NBR 7211:2022", "Faixa 4,75/12,5", "Elementos esbeltos, telhas e blocos de concreto."},
            {"BRITA_1", "Brita 1", "GRAUDO", 19.0, 6.80, 2.70, 1.55, 1.5, 70.0, "ABNT NBR 7211:2022", "Faixa 9,5/19", "Agregado mais comum para concreto estrutural (vigas/pilares/lajes)."},
            {"BRITA_2", "Brita 2", "GRAUDO", 25.0, 7.50, 2.70, 1.689, 1.5, 70.0, "ABNT NBR 7211:2022", "Faixa 19/25", "Estruturas pesadas, fundações e pisos industriais."},
            {"BRITA_3", "Brita 3", "GRAUDO", 38.0, 8.20, 2.70, 1.50, 1.0, 65.0, "ABNT NBR 7211:2022", "Faixa 25/38", "Concreto ciclópico, bases de pavimentos e blocos espessos."},
            {"BRITA_4", "Brita 4 (Pedra de mão)", "GRAUDO", 76.0, 9.00, 2.70, 1.45, 0.5, 60.0, "ABNT NBR 7211:2022", "Faixa 38/76", "Obras de contenção e barragens."}
        };

        db.transaction();
        QSqlQuery qi(db);
        qi.prepare("INSERT INTO aggregate_types VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        for (const auto& a : aggs) {
            qi.addBindValue(a.code);
            qi.addBindValue(a.name);
            qi.addBindValue(a.type);
            qi.addBindValue(a.dmc);
            qi.addBindValue(a.mf);
            qi.addBindValue(a.spec);
            qi.addBindValue(a.unit);
            qi.addBindValue(a.moist);
            qi.addBindValue(a.cost);
            qi.addBindValue(a.std);
            qi.addBindValue(a.zone);
            qi.addBindValue(a.notes);
            qi.exec();
        }
        db.commit();
    }

    // 3. Popula Classes de Slump (NBR 8953:2015 Tabela 2)
    {
        struct SlumpSeed {
            const char* sc;
            double minS;
            double maxS;
            const char* method;
            const char* app;
            const char* std;
        } slumps[] = {
            {"S10", 10.0, 50.0, "Vibroprensado ou extrusado", "Tubos, blocos e pavimentos intertravados", "ABNT NBR 8953:2015"},
            {"S50", 50.0, 100.0, "Vibração normal convencional", "Pavimentos rodoviários e elementos de fundação direta", "ABNT NBR 8953:2015"},
            {"S100", 100.0, 160.0, "Lançamento convencional ou bombeado leve", "Elementos estruturais usuais (vigas, pilares e lajes)", "ABNT NBR 8953:2015"},
            {"S160", 160.0, 220.0, "Bombeado em grandes alturas ou peças esbeltas", "Paredes de concreto, caixões e pilares densamente armados", "ABNT NBR 8953:2015"},
            {"S220", 220.0, 300.0, "Auto-adensável (fluido com aditivo superplastificante)", "Elementos de altíssima densidade de armadura e estacas hélice", "ABNT NBR 8953:2015"}
        };

        db.transaction();
        QSqlQuery qi(db);
        qi.prepare("INSERT OR REPLACE INTO consistency_classes VALUES (?, ?, ?, ?, ?, ?)");
        for (const auto& s : slumps) {
            qi.addBindValue(s.sc);
            qi.addBindValue(s.minS);
            qi.addBindValue(s.maxS);
            qi.addBindValue(s.method);
            qi.addBindValue(s.app);
            qi.addBindValue(s.std);
            qi.exec();
        }
        db.commit();
    }

    // 4. Popula Durabilidade (NBR 6118 / NBR 12655)
    {
        struct DurSeed {
            const char* caa;
            const char* agg;
            const char* env;
            const char* risk;
            double acCa;
            double acCp;
            const char* clCa;
            const char* clCp;
            double fckCa;
            double fckCp;
            double cMin;
            double covSlab;
            double covBeam;
            const char* std;
        } durs[] = {
            {"CAA I", "Fraca", "Rural / Submerso em água doce", "Insignificante", 0.65, 0.60, "C20", "C25", 20.0, 25.0, 260.0, 20.0, 25.0, "ABNT NBR 6118:2023 / ABNT NBR 12655:2022"},
            {"CAA II", "Moderada", "Urbano comum com carbonatação", "Pequeno", 0.60, 0.55, "C25", "C30", 25.0, 30.0, 280.0, 25.0, 30.0, "ABNT NBR 6118:2023 / ABNT NBR 12655:2022"},
            {"CAA III", "Forte", "Marinho ou industrial moderado", "Grande", 0.55, 0.50, "C30", "C35", 30.0, 35.0, 320.0, 35.0, 40.0, "ABNT NBR 6118:2023 / ABNT NBR 12655:2022"},
            {"CAA IV", "Muito Forte", "Respingos de maré ou químicos severos", "Elevado", 0.45, 0.45, "C40", "C40", 40.0, 40.0, 360.0, 45.0, 50.0, "ABNT NBR 6118:2023 / ABNT NBR 12655:2022"}
        };

        db.transaction();
        QSqlQuery qi(db);
        qi.prepare("INSERT OR REPLACE INTO durability_requirements VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        for (const auto& d : durs) {
            qi.addBindValue(d.caa);
            qi.addBindValue(d.agg);
            qi.addBindValue(d.env);
            qi.addBindValue(d.risk);
            qi.addBindValue(d.acCa);
            qi.addBindValue(d.acCp);
            qi.addBindValue(d.clCa);
            qi.addBindValue(d.clCp);
            qi.addBindValue(d.fckCa);
            qi.addBindValue(d.fckCp);
            qi.addBindValue(d.cMin);
            qi.addBindValue(d.covSlab);
            qi.addBindValue(d.covBeam);
            qi.addBindValue(d.std);
            qi.exec();
        }
        db.commit();
    }

    // 5. Popula Condições de Preparo (NBR 12655:2022 itens 5.4 e 5.6.3.1)
    {
        struct PrepSeed {
            const char* code;
            const char* name;
            double sd;
            const char* std;
            const char* method;
            const char* limit;
        } preps[] = {
            {"A", "Condição A - Central / Massa Total", 4.0, "ABNT NBR 12655:2022", "Cimento e agregados medidos em massa; água medida em massa ou volume e corrigida em função da umidade dos agregados.", "Todas as classes (C10 a C100)"},
            {"B", "Condição B - Canteiro Controlado", 5.5, "ABNT NBR 12655:2022", "Cimento medido em massa; água medida em massa ou volume (com correção da umidade do agregado miúdo); agregados medidos em massa combinada com volume.", "Classes C10 a C20"},
            {"C", "Condição C - Canteiro Empírico", 7.0, "ABNT NBR 12655:2022", "Cimento medido em massa; agregados medidos em volume; água medida em volume e corrigida pela estimativa da umidade e da consistência.", "Exclusivo para classes C10 e C15"},
            {"HISTORICO", "Desvio Conhecido Histórico", 2.0, "ABNT NBR 12655:2022", "Desvio-padrão calculado em ensaios prévios de produção com no mínimo 20 resultados consecutivos em 30 dias (n ≥ 20).", "Permite Sd customizado (≥ 2,0 MPa)"},
            {"ARBITRARIO", "Laboratório / Arbitrário", 3.5, "Procedimento Helene & Terzian (IPT)", "Desvio adotado para estudo acadêmico experimental ou condição específica em betoneira de laboratório.", "Estudos laboratoriais controlados"}
        };

        db.transaction();
        QSqlQuery qi(db);
        qi.prepare("INSERT OR REPLACE INTO preparation_conditions VALUES (?, ?, ?, ?, ?, ?)");
        for (const auto& p : preps) {
            qi.addBindValue(p.code);
            qi.addBindValue(p.name);
            qi.addBindValue(p.sd);
            qi.addBindValue(p.std);
            qi.addBindValue(p.method);
            qi.addBindValue(p.limit);
            qi.exec();
        }
        db.commit();
    }

    // 6. Popula Tolerâncias de Pesagem (ABNT NBR 7212:2024 / versão corrigida 2025, item 5.5)
    {
        struct TolSeed {
            const char* comp;
            double tol;
            const char* std;
            const char* equip;
        } tols[] = {
            {"Cimento e Adições Minerais", 1.0, "ABNT NBR 7212:2025 (itens 5.5.2 e 5.5.5)", "Balança dosadora de cimento e adições (±1%)"},
            {"Agregados (Areia e Britas)", 3.0, "ABNT NBR 7212:2025 (item 5.5.1)", "Balança dosadora de agregados na massa acumulada (±3%)"},
            {"Água de Amassamento", 3.0, "ABNT NBR 7212:2025 (item 5.5.3)", "Hidrômetro ou balança dosadora na quantidade total (±3%)"},
            {"Aditivos Químicos", 5.0, "ABNT NBR 7212:2025 (item 5.5.4)", "Dispensador dosador volumétrico ou gravimétrico (±5%)"}
        };

        db.transaction();
        QSqlQuery qi(db);
        qi.prepare("INSERT OR REPLACE INTO weighing_tolerances VALUES (?, ?, ?, ?)");
        for (const auto& t : tols) {
            qi.addBindValue(t.comp);
            qi.addBindValue(t.tol);
            qi.addBindValue(t.std);
            qi.addBindValue(t.equip);
            qi.exec();
        }
        db.commit();
    }

    // 7. Popula Cenários Presets da Planilha Excel
    q.exec("SELECT COUNT(*) FROM scenario_presets");
    if (q.next() && q.value(0).toInt() == 0) {
        struct ScenSeed {
            int id;
            const char* title;
            const char* sheet;
            double fck;
            double sd;
            const char* cond;
            const char* env;
            const char* st;
            double slump;
            double alpha;
            double mixVol;
            double projVol;
            double mAc;
            double mM;
            double mC;
            const char* notes;
        } scens[] = {
            {0, "Cenário 1: Concreto C25 Urbano (CAA II)", "NBR 6118 / NBR 12655", 25.0, 4.0, "A", "CAA II", "CA", 100.0, 52.0, 29.0, 30.0, 0.534, 5.08, 360.0, "Concreto convencional para edifícios urbanos. CP II-F-32, Brita 1 (DMC 19mm), fcj 31,60 MPa."},
            {1, "Cenário 2: Concreto C30 Marinho (CAA III)", "NBR 6118 / NBR 12655", 30.0, 4.0, "A", "CAA III", "CA", 120.0, 50.0, 90.0, 50.0, 0.486, 4.79, 382.4, "Concreto de alta durabilidade marinha/industrial. CP III-40 RS, Britas 1+2 (DMC 25mm), fcj 36,60 MPa."},
            {2, "Cenário 3: Concreto C40 Alto Desempenho (CAA IV)", "NBR 6118 / NBR 8953", 40.0, 4.0, "A", "CAA IV", "CA", 160.0, 48.0, 100.0, 100.0, 0.428, 4.30, 423.7, "Concreto de alto desempenho para respingos de maré e pilares esbeltos. CP V-ARI, Brita Basáltica (DMC 19mm), fcj 46,60 MPa."},
            {3, "Cenário 4: Concreto C20 Fundações (Condição B)", "NBR 12655 / NBR 6118", 20.0, 5.5, "B", "CAA I", "CA", 80.0, 54.0, 90.0, 40.0, 0.575, 5.34, 343.0, "Concreto para sapatas e blocos com controle em canteiro (Condição B). CP II-Z-32, Brita 2 (DMC 25mm), fcj 29,08 MPa."}
        };


        db.transaction();
        QSqlQuery qi(db);
        qi.prepare("INSERT INTO scenario_presets VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        for (const auto& s : scens) {
            qi.addBindValue(s.id);
            qi.addBindValue(s.title);
            qi.addBindValue(s.sheet);
            qi.addBindValue(s.fck);
            qi.addBindValue(s.sd);
            qi.addBindValue(s.cond);
            qi.addBindValue(s.env);
            qi.addBindValue(s.st);
            qi.addBindValue(s.slump);
            qi.addBindValue(s.alpha);
            qi.addBindValue(s.mixVol);
            qi.addBindValue(s.projVol);
            qi.addBindValue(s.mAc);
            qi.addBindValue(s.mM);
            qi.addBindValue(s.mC);
            qi.addBindValue(s.notes);
            qi.exec();
        }
        db.commit();
    }

    return true;
}

QVector<CementNormativeRecord> NormativeDatabase::getAllCementTypes() const {
    QMutexLocker locker(&m_mutex);
    QVector<CementNormativeRecord> list;
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery q("SELECT code, name, standard, specific_mass, unit_mass, default_cost, clinker_addition, hydration_heat, technical_notes FROM cement_types", db);
    while (q.next()) {
        CementNormativeRecord c;
        c.code = q.value(0).toString();
        c.name = q.value(1).toString();
        c.standard = q.value(2).toString();
        c.specificMass = q.value(3).toDouble();
        c.unitMass = q.value(4).toDouble();
        c.defaultCostPerBag = q.value(5).toDouble();
        c.clinkerAddition = q.value(6).toString();
        c.hydrationHeat = q.value(7).toString();
        c.technicalNotes = q.value(8).toString();
        list.append(c);
    }
    return list;
}

CementNormativeRecord NormativeDatabase::getCementByCode(const QString &code) const {
    QMutexLocker locker(&m_mutex);
    QString c = code.trimmed().toUpper();
    if (c == "CPII-F" || c == "CPIIF") c = "CP II-F";
    else if (c == "CPII-F32" || c == "CPIIF32" || c == "CP II-F 32") c = "CP II-F32";
    else if (c == "CPIII" || c == "CP-III") c = "CP III";
    else if (c == "CPIV" || c == "CP-IV") c = "CP IV";
    else if (c == "CPV" || c == "CPV-ARI" || c == "CP-V") c = "CP V-ARI";

    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery q(db);
    q.prepare("SELECT code, name, standard, specific_mass, unit_mass, default_cost, clinker_addition, hydration_heat, technical_notes FROM cement_types WHERE UPPER(code) = ? OR UPPER(code) LIKE ? OR UPPER(name) LIKE ?");
    q.addBindValue(c);
    q.addBindValue("%" + c + "%");
    q.addBindValue("%" + c + "%");
    if (q.exec() && q.next()) {
        CementNormativeRecord rec;
        rec.code = q.value(0).toString();
        rec.name = q.value(1).toString();
        rec.standard = q.value(2).toString();
        rec.specificMass = q.value(3).toDouble();
        rec.unitMass = q.value(4).toDouble();
        rec.defaultCostPerBag = q.value(5).toDouble();
        rec.clinkerAddition = q.value(6).toString();
        rec.hydrationHeat = q.value(7).toString();
        rec.technicalNotes = q.value(8).toString();
        return rec;
    }
    return CementNormativeRecord();
}

QVector<AggregateNormativeRecord> NormativeDatabase::getAllAggregates() const {
    QMutexLocker locker(&m_mutex);
    QVector<AggregateNormativeRecord> list;
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery q("SELECT code, commercial_name, aggregate_type, dmc_mm, fineness_modulus, specific_mass, unit_mass, default_moisture, default_cost, standard, granulometric_zone, technical_notes FROM aggregate_types", db);
    while (q.next()) {
        AggregateNormativeRecord a;
        a.code = q.value(0).toString();
        a.commercialName = q.value(1).toString();
        a.aggregateType = q.value(2).toString();
        a.dmcMm = q.value(3).toDouble();
        a.finenessModulus = q.value(4).toDouble();
        a.specificMass = q.value(5).toDouble();
        a.unitMass = q.value(6).toDouble();
        a.defaultMoisture = q.value(7).toDouble();
        a.defaultCostPerM3 = q.value(8).toDouble();
        a.standard = q.value(9).toString();
        a.granulometricZone = q.value(10).toString();
        a.technicalNotes = q.value(11).toString();
        list.append(a);
    }
    return list;
}

AggregateNormativeRecord NormativeDatabase::getAggregateByCode(const QString &code) const {
    QMutexLocker locker(&m_mutex);
    QString c = code.trimmed().toUpper();
    if (c == "B0" || c == "BRITA 0") c = "BRITA_0";
    else if (c == "B1" || c == "BRITA 1" || c.contains("BRITA 1")) c = "BRITA_1";
    else if (c == "B2" || c == "BRITA 2" || c.contains("BRITA 2")) c = "BRITA_2";
    else if (c == "B3" || c == "BRITA 3" || c.contains("BRITA 3")) c = "BRITA_3";
    else if (c == "B4" || c == "BRITA 4" || c.contains("BRITA 4")) c = "BRITA_4";
    else if (c == "AREIA" || c == "AREIA_MEDIA" || c.contains("MÉDIA") || c.contains("MEDIA")) c = "AREIA_MED";
    else if (c.contains("FINA")) c = "AREIA_FINA";
    else if (c.contains("GROSSA")) c = "AREIA_GROSSA";

    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery q(db);
    q.prepare("SELECT code, commercial_name, aggregate_type, dmc_mm, fineness_modulus, specific_mass, unit_mass, default_moisture, default_cost, standard, granulometric_zone, technical_notes FROM aggregate_types WHERE UPPER(code) = ? OR UPPER(code) LIKE ? OR UPPER(commercial_name) LIKE ?");
    q.addBindValue(c);
    q.addBindValue("%" + c + "%");
    q.addBindValue("%" + c + "%");
    if (q.exec() && q.next()) {
        AggregateNormativeRecord a;
        a.code = q.value(0).toString();
        a.commercialName = q.value(1).toString();
        a.aggregateType = q.value(2).toString();
        a.dmcMm = q.value(3).toDouble();
        a.finenessModulus = q.value(4).toDouble();
        a.specificMass = q.value(5).toDouble();
        a.unitMass = q.value(6).toDouble();
        a.defaultMoisture = q.value(7).toDouble();
        a.defaultCostPerM3 = q.value(8).toDouble();
        a.standard = q.value(9).toString();
        a.granulometricZone = q.value(10).toString();
        a.technicalNotes = q.value(11).toString();
        return a;
    }
    return AggregateNormativeRecord();
}

QVector<SlumpNormativeRecord> NormativeDatabase::getAllSlumpClasses() const {
    QMutexLocker locker(&m_mutex);
    QVector<SlumpNormativeRecord> list;
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery q("SELECT slump_class, min_slump, max_slump, placement_method, application, standard FROM consistency_classes ORDER BY min_slump ASC", db);
    while (q.next()) {
        SlumpNormativeRecord s;
        s.slumpClass = q.value(0).toString();
        s.minSlumpMm = q.value(1).toDouble();
        s.maxSlumpMm = q.value(2).toDouble();
        s.placementMethod = q.value(3).toString();
        s.application = q.value(4).toString();
        s.standard = q.value(5).toString();
        list.append(s);
    }
    return list;
}

SlumpNormativeRecord NormativeDatabase::getSlumpByValue(double slumpMm) const {
    QMutexLocker locker(&m_mutex);
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery q(db);
    q.prepare("SELECT slump_class, min_slump, max_slump, placement_method, application, standard "
              "FROM consistency_classes "
              "WHERE (? >= min_slump AND ? < max_slump) OR (slump_class = 'S220' AND ? >= 220.0) "
              "ORDER BY min_slump ASC LIMIT 1");
    q.addBindValue(slumpMm);
    q.addBindValue(slumpMm);
    q.addBindValue(slumpMm);
    if (q.exec() && q.next()) {
        SlumpNormativeRecord s;
        s.slumpClass = q.value(0).toString();
        s.minSlumpMm = q.value(1).toDouble();
        s.maxSlumpMm = q.value(2).toDouble();
        s.placementMethod = q.value(3).toString();
        s.application = q.value(4).toString();
        s.standard = q.value(5).toString();
        return s;
    }
    return SlumpNormativeRecord();
}

QVector<DurabilityNormativeRecord> NormativeDatabase::getAllDurabilityRequirements() const {
    QMutexLocker locker(&m_mutex);
    QVector<DurabilityNormativeRecord> list;
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery q("SELECT caa_class, aggressiveness, environment, deterioration_risk, max_ac_ca, max_ac_cp, min_class_ca, min_class_cp, min_fck_ca, min_fck_cp, min_cement_kg_m3, cover_slabs_mm, cover_beams_cols_mm, standard FROM durability_requirements", db);
    while (q.next()) {
        DurabilityNormativeRecord d;
        d.caaClass = q.value(0).toString();
        d.aggressiveness = q.value(1).toString();
        d.environment = q.value(2).toString();
        d.deteriorationRisk = q.value(3).toString();
        d.maxAcCa = q.value(4).toDouble();
        d.maxAcCp = q.value(5).toDouble();
        d.minClassCa = q.value(6).toString();
        d.minClassCp = q.value(7).toString();
        d.minFckCa = q.value(8).toDouble();
        d.minFckCp = q.value(9).toDouble();
        d.minCementKgM3 = q.value(10).toDouble();
        d.coverSlabsMm = q.value(11).toDouble();
        d.coverBeamsColsMm = q.value(12).toDouble();
        d.standard = q.value(13).toString();
        list.append(d);
    }
    return list;
}

DurabilityNormativeRecord NormativeDatabase::getDurabilityRequirement(const QString &caa) const {
    QMutexLocker locker(&m_mutex);
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery q(db);
    q.prepare("SELECT caa_class, aggressiveness, environment, deterioration_risk, max_ac_ca, max_ac_cp, min_class_ca, min_class_cp, min_fck_ca, min_fck_cp, min_cement_kg_m3, cover_slabs_mm, cover_beams_cols_mm, standard FROM durability_requirements WHERE caa_class = ?");
    q.addBindValue(caa);
    if (q.exec() && q.next()) {
        DurabilityNormativeRecord d;
        d.caaClass = q.value(0).toString();
        d.aggressiveness = q.value(1).toString();
        d.environment = q.value(2).toString();
        d.deteriorationRisk = q.value(3).toString();
        d.maxAcCa = q.value(4).toDouble();
        d.maxAcCp = q.value(5).toDouble();
        d.minClassCa = q.value(6).toString();
        d.minClassCp = q.value(7).toString();
        d.minFckCa = q.value(8).toDouble();
        d.minFckCp = q.value(9).toDouble();
        d.minCementKgM3 = q.value(10).toDouble();
        d.coverSlabsMm = q.value(11).toDouble();
        d.coverBeamsColsMm = q.value(12).toDouble();
        d.standard = q.value(13).toString();
        return d;
    }
    return DurabilityNormativeRecord();
}

QVector<PreparationNormativeRecord> NormativeDatabase::getAllPreparationConditions() const {
    QMutexLocker locker(&m_mutex);
    QVector<PreparationNormativeRecord> list;
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery q("SELECT condition_code, condition_name, sd_mpa, standard, batching_method, max_allowed_class FROM preparation_conditions", db);
    while (q.next()) {
        PreparationNormativeRecord p;
        p.conditionCode = q.value(0).toString();
        p.conditionName = q.value(1).toString();
        p.sdMpa = q.value(2).toDouble();
        p.standard = q.value(3).toString();
        p.batchingMethod = q.value(4).toString();
        p.maxAllowedClass = q.value(5).toString();
        list.append(p);
    }
    return list;
}

PreparationNormativeRecord NormativeDatabase::getPreparationCondition(const QString &code) const {
    QMutexLocker locker(&m_mutex);
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery q(db);
    q.prepare("SELECT condition_code, condition_name, sd_mpa, standard, batching_method, max_allowed_class FROM preparation_conditions WHERE condition_code = ?");
    q.addBindValue(code);
    if (q.exec() && q.next()) {
        PreparationNormativeRecord p;
        p.conditionCode = q.value(0).toString();
        p.conditionName = q.value(1).toString();
        p.sdMpa = q.value(2).toDouble();
        p.standard = q.value(3).toString();
        p.batchingMethod = q.value(4).toString();
        p.maxAllowedClass = q.value(5).toString();
        return p;
    }
    return PreparationNormativeRecord();
}

QVector<WeighingToleranceRecord> NormativeDatabase::getAllWeighingTolerances() const {
    QMutexLocker locker(&m_mutex);
    QVector<WeighingToleranceRecord> list;
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery q("SELECT component, tolerance_percent, standard, equipment FROM weighing_tolerances", db);
    while (q.next()) {
        WeighingToleranceRecord w;
        w.component = q.value(0).toString();
        w.tolerancePercent = q.value(1).toDouble();
        w.standard = q.value(2).toString();
        w.equipment = q.value(3).toString();
        list.append(w);
    }
    return list;
}

QVector<ScenarioPresetRecord> NormativeDatabase::getAllScenarios() const {
    QMutexLocker locker(&m_mutex);
    QVector<ScenarioPresetRecord> list;
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery q("SELECT id, title, sheet_name, fck, sd, prep_condition, env_class, struct_type, target_slump_mm, selected_alpha, mixer_volume_l, project_volume_m3, manual_ac, manual_m, manual_c, notes FROM scenario_presets ORDER BY id ASC", db);
    while (q.next()) {
        ScenarioPresetRecord s;
        s.id = q.value(0).toInt();
        s.title = q.value(1).toString();
        s.sheetName = q.value(2).toString();
        s.fck = q.value(3).toDouble();
        s.sd = q.value(4).toDouble();
        s.prepCondition = q.value(5).toString();
        s.envClass = q.value(6).toString();
        s.structType = q.value(7).toString();
        s.targetSlumpMm = q.value(8).toDouble();
        s.selectedAlpha = q.value(9).toDouble();
        s.mixerVolumeL = q.value(10).toDouble();
        s.projectVolumeM3 = q.value(11).toDouble();
        s.manualAc = q.value(12).toDouble();
        s.manualM = q.value(13).toDouble();
        s.manualC = q.value(14).toDouble();
        s.notes = q.value(15).toString();
        list.append(s);
    }
    return list;
}

ScenarioPresetRecord NormativeDatabase::getScenarioById(int id) const {
    QMutexLocker locker(&m_mutex);
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery q(db);
    q.prepare("SELECT id, title, sheet_name, fck, sd, prep_condition, env_class, struct_type, target_slump_mm, selected_alpha, mixer_volume_l, project_volume_m3, manual_ac, manual_m, manual_c, notes FROM scenario_presets WHERE id = ?");
    q.addBindValue(id);
    if (q.exec() && q.next()) {
        ScenarioPresetRecord s;
        s.id = q.value(0).toInt();
        s.title = q.value(1).toString();
        s.sheetName = q.value(2).toString();
        s.fck = q.value(3).toDouble();
        s.sd = q.value(4).toDouble();
        s.prepCondition = q.value(5).toString();
        s.envClass = q.value(6).toString();
        s.structType = q.value(7).toString();
        s.targetSlumpMm = q.value(8).toDouble();
        s.selectedAlpha = q.value(9).toDouble();
        s.mixerVolumeL = q.value(10).toDouble();
        s.projectVolumeM3 = q.value(11).toDouble();
        s.manualAc = q.value(12).toDouble();
        s.manualM = q.value(13).toDouble();
        s.manualC = q.value(14).toDouble();
        s.notes = q.value(15).toString();
        return s;
    }
    return ScenarioPresetRecord();
}

bool NormativeDatabase::verifyDmcAgainstCover(double dmcMm, double coverMm, QString &diagnostic) const {
    QMutexLocker locker(&m_mutex);
    // ABNT NBR 6118:2023 item 7.4.7.6:
    // A dimensão máxima característica do agregado graúdo não deve superar 1,2 vezes o cobrimento nominal
    double maxDmc = 1.2 * coverMm;
    if (dmcMm <= maxDmc) {
        diagnostic = QString("OK: DMC (%1 mm) ≤ 1,2 · cnom (%2 mm)").arg(dmcMm, 0, 'f', 1).arg(maxDmc, 0, 'f', 1);
        return true;
    } else {
        diagnostic = QString("ALERTA: DMC (%1 mm) > 1,2 · cnom (%2 mm). Risco de bicheiras/brocas na armadura.").arg(dmcMm, 0, 'f', 1).arg(maxDmc, 0, 'f', 1);
        return false;
    }
}

} // namespace ConcreteMix
