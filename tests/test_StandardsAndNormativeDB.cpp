#include "TestFramework.hpp"

#include "../source/core/ConcreteStandards.hpp"
#include "../source/core/NormativeDatabase.hpp"

#include <QCoreApplication>
#include <atomic>
#include <thread>
#include <vector>

using namespace ConcreteMix;

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    concrete_tests::TestRunner runner("StandardsAndNormativeDB");

    runner.add("NormativeDatabaseInitializationAndCatalogCounts", []() {
        auto& db = NormativeDatabase::instance();
        CMD_ASSERT(db.initialize());
        CMD_ASSERT(db.isInitialized());

        const auto cements = db.getAllCementTypes();
        const auto aggregates = db.getAllAggregates();
        const auto slumps = db.getAllSlumpClasses();
        const auto durabilities = db.getAllDurabilityRequirements();
        const auto preps = db.getAllPreparationConditions();
        const auto tolerances = db.getAllWeighingTolerances();
        const auto scenarios = db.getAllScenarios();

        CMD_ASSERT(cements.size() >= 6);
        CMD_ASSERT(aggregates.size() >= 4);
        CMD_ASSERT(slumps.size() == 5);
        CMD_ASSERT(durabilities.size() == 4);
        CMD_ASSERT(preps.size() >= 3);
        CMD_ASSERT(tolerances.size() == 4);
        CMD_ASSERT(scenarios.size() >= 4);
    });

    runner.add("NormativeDatabaseTypedLookupsAndDmcVsCover", []() {
        auto& db = NormativeDatabase::instance();
        CMD_ASSERT(db.initialize());

        const auto cpii = db.getCementByCode(QStringLiteral("CP II-F32"));
        CMD_ASSERT(!cpii.code.isEmpty());
        CMD_ASSERT(cpii.specificMass > 2.5 && cpii.specificMass < 3.5);

        const auto cpv = db.getCementByCode(QStringLiteral("CP V-ARI"));
        CMD_ASSERT(!cpv.code.isEmpty());

        const auto slump100 = db.getSlumpByValue(100.0);
        CMD_ASSERT(slump100.slumpClass == QStringLiteral("S100"));

        const auto caa3 = db.getDurabilityRequirement(QStringLiteral("CAA III"));
        CMD_ASSERT(caa3.caaClass == QStringLiteral("CAA III"));
        CMD_ASSERT_NEAR(caa3.maxAcCa, 0.55, 1e-6);
        CMD_ASSERT_NEAR(caa3.maxAcCp, 0.50, 1e-6);

        QString diagBad;
        // ABNT NBR 6118:2023 Item 18.3.2.2 -> DMC <= 1.2 * c_nom (25 <= 1.2*20 = 24 is false)
        CMD_ASSERT(!db.verifyDmcAgainstCover(25.0, 20.0, diagBad));
        CMD_ASSERT(!diagBad.isEmpty());

        QString diagGood;
        CMD_ASSERT(db.verifyDmcAgainstCover(19.0, 25.0, diagGood));
    });

    runner.add("NormativeDatabaseReentrantAndMultiThreadMemoryConnections", []() {
        auto& db = NormativeDatabase::instance();
        CMD_ASSERT(db.initialize());

        // 1. Consultas repetidas na thread proprietária exercitam o QRecursiveMutex sem recriar conexão
        for (int i = 0; i < 20; ++i) {
            const auto c = db.getAllCementTypes();
            const auto d = db.getAllDurabilityRequirements();
            CMD_ASSERT(!c.isEmpty());
            CMD_ASSERT(d.size() == 4);
        }

        // 2. Conexões SQLite :memory: independentes por thread respeitando afinidade de thread do Qt SQL
        std::atomic<int> failures{0};
        std::vector<std::thread> workers;
        for (int t = 0; t < 4; ++t) {
            workers.emplace_back([&, t]() {
                const QString conn = QStringLiteral("worker_mem_db_%1").arg(t);
                {
                    QSqlDatabase wdb = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), conn);
                    wdb.setDatabaseName(QStringLiteral(":memory:"));
                    if (!wdb.open()) {
                        ++failures;
                    } else {
                        QSqlQuery q(wdb);
                        if (!q.exec(QStringLiteral("SELECT 1 + 1")) || !q.next() || q.value(0).toInt() != 2) {
                            ++failures;
                        }
                        wdb.close();
                    }
                }
                QSqlDatabase::removeDatabase(conn);
            });
        }
        for (auto& w : workers) {
            w.join();
        }
        CMD_ASSERT(failures.load() == 0);
    });

    runner.add("ConcreteStandardsDurabilityStrengthAndSlump", []() {
        const auto ca2 = ConcreteStandards::getDurabilityRequirement(
            EnvironmentalClass::CAA_II, StructuralType::ReinforcedConcrete);
        const auto cp2 = ConcreteStandards::getDurabilityRequirement(
            EnvironmentalClass::CAA_II, StructuralType::PrestressedConcrete);

        CMD_ASSERT_NEAR(ca2.maxWaterCementRatio, 0.60, 1e-6);
        CMD_ASSERT_NEAR(cp2.maxWaterCementRatio, 0.55, 1e-6);
        CMD_ASSERT(cp2.minFck >= ca2.minFck);
        CMD_ASSERT(cp2.nominalCoverBeamsCols >= ca2.nominalCoverBeamsCols);

        CMD_ASSERT_NEAR(ConcreteStandards::getStandardDeviation(PreparationCondition::ConditionA), 4.0, 1e-6);
        CMD_ASSERT_NEAR(ConcreteStandards::getStandardDeviation(PreparationCondition::ConditionB), 5.5, 1e-6);
        CMD_ASSERT_NEAR(ConcreteStandards::getStandardDeviation(PreparationCondition::ConditionC), 7.0, 1e-6);
        CMD_ASSERT_NEAR(ConcreteStandards::getStandardDeviation(PreparationCondition::KnownSd, 2.5), 2.5, 1e-6);
        CMD_ASSERT_NEAR(ConcreteStandards::getStandardDeviation(PreparationCondition::KnownSd, 1.0), 2.0, 1e-6);

        CMD_ASSERT_NEAR(ConcreteStandards::calculateDosageStrength(30.0, 4.0), 36.6, 1e-9);
        CMD_ASSERT_NEAR(ConcreteStandards::getFckFromClass("C35"), 35.0, 1e-6);
        CMD_ASSERT(!ConcreteStandards::getStandardStrengthClasses().empty());

        CMD_ASSERT(ConcreteStandards::classifySlump(20.0) == SlumpClass::S10);
        CMD_ASSERT(ConcreteStandards::classifySlump(75.0) == SlumpClass::S50);
        CMD_ASSERT(ConcreteStandards::classifySlump(120.0) == SlumpClass::S100);
        CMD_ASSERT(ConcreteStandards::classifySlump(180.0) == SlumpClass::S160);
        CMD_ASSERT(ConcreteStandards::classifySlump(230.0) == SlumpClass::S220);

        CMD_ASSERT_NEAR(ConcreteStandards::getCoarseDMC(CoarseAggregateType::Brita1), 19.0, 1e-6);
        CMD_ASSERT_NEAR(ConcreteStandards::BatchingTolerances::CementTolerancePercent, 1.0, 1e-9);
        CMD_ASSERT_NEAR(ConcreteStandards::BatchingTolerances::AggregateTolerancePercent, 3.0, 1e-9);
    });

    runner.add("ConcreteStandardsValidateMixDesignPassAndReject", []() {
        const auto okRes = ConcreteStandards::validateMixDesign(
            30.0, 0.50, 360.0,
            EnvironmentalClass::CAA_II, StructuralType::ReinforcedConcrete,
            100.0, 19.0, 30.0, PreparationCondition::ConditionA);
        CMD_ASSERT(okRes.isCompliant);
        CMD_ASSERT(okRes.errors.empty());
        CMD_ASSERT(!okRes.passedChecks.empty());

        // CAA IV exige fck >= 40 MPa, a/c <= 0.45, C >= 360 kg/m³, e Condição C veta concreto >= C25
        const auto badRes = ConcreteStandards::validateMixDesign(
            25.0, 0.62, 260.0,
            EnvironmentalClass::CAA_IV, StructuralType::ReinforcedConcrete,
            100.0, 32.0, 20.0, PreparationCondition::ConditionC);
        CMD_ASSERT(!badRes.isCompliant);
        CMD_ASSERT(badRes.errors.size() >= 3);
    });

    return runner.runAll();
}
