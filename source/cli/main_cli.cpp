#include "source/core/ProjectData.hpp"
#include "source/core/MixDesignCalculator.hpp"
#include "source/core/ConcreteStandards.hpp"
#include "source/core/NormativeDatabase.hpp"
#include "source/core/RegressionEngine.hpp"
#include "source/language/GerenciadorTraducao.hpp"

#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <cmath>
#include <sstream>

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

using namespace ConcreteMix;

namespace {

const char* kCliVersion = "2026.10.26.0";

void printBanner() {
    std::cout << "========================================================================\n";
    std::cout << "  Concrete Mix Designer CLI — IPT / EPUSP Mix Design & NBR Compliance   \n";
    std::cout << "  Version " << kCliVersion << " | C++20 Engineering Calculation Engine           \n";
    std::cout << "========================================================================\n\n";
}

void printHelp() {
    printBanner();
    std::cout << "Usage:\n";
    std::cout << "  concrete-mix-designer-cli.exe <command> [options]\n\n";
    std::cout << "Commands:\n";
    std::cout << "  design                     Quick mix design from command-line parameters\n";
    std::cout << "  calculate <project.cmdproj> Calculate mix proportions from project file\n";
    std::cout << "  verify <project.cmdproj>   Audit standards compliance (NBR 12655, NBR 6118, etc.)\n";
    std::cout << "  report <project.cmdproj>   Generate Technical Report (Markdown or HTML)\n";
    std::cout << "  standards [category]       Query normative database (cements, aggregates, slump,\n";
    std::cout << "                             tolerances, durability, all)\n";
    std::cout << "  presets                    Manage built-in reference presets (--list, --run, --export)\n";
    std::cout << "  test                       Run automated internal engine verification test suite\n";
    std::cout << "  --version, -v              Show official version\n";
    std::cout << "  --help, -h                 Show this help message\n\n";
    std::cout << "Options for 'design':\n";
    std::cout << "  --fck <MPa>                Characteristic compressive strength (e.g. 25, 30, 40)\n";
    std::cout << "  --slump <mm>               Target slump cone value (e.g. 100, 120, 160)\n";
    std::cout << "  --caa <I|II|III|IV>        Environmental Aggressiveness Class (default: CAA II)\n";
    std::cout << "  --cond <A|B|C>             Batching condition per NBR 12655 (default: A)\n";
    std::cout << "  --sd <MPa>                 Custom dosage standard deviation override\n";
    std::cout << "  --struct <rc|pc>           Structural concrete type: rc (reinforced) or pc (prestressed)\n";
    std::cout << "  --alpha <%>                Mortar content percentage (default: 52.0)\n";
    std::cout << "  --mixer-vol <L>            Laboratory mixer volume in Liters (default: 30.0)\n";
    std::cout << "  --project-vol <m3>         Total jobsite volume in cubic meters (default: 100.0)\n";
    std::cout << "  --gravel-blend <0-100>     Percentage of gravel 2 in coarse blend (default: 0)\n\n";
    std::cout << "General Options:\n";
    std::cout << "  --format <text|json|markdown|html> Output format (default: text)\n";
    std::cout << "  --out <file>               Write output directly to specified file path\n";
    std::cout << "  --lang <pt|en>             Report/display language (default: pt)\n";
    std::cout << "  --dark                     Use dark theme for HTML report output\n";
    std::cout << "  --no-banner                Suppress header banner\n\n";
    std::cout << "Examples:\n";
    std::cout << "  concrete-mix-designer-cli.exe design --fck 30 --slump 120 --caa III\n";
    std::cout << "  concrete-mix-designer-cli.exe calculate \"projeto.cmdproj\" --format json\n";
    std::cout << "  concrete-mix-designer-cli.exe verify \"projeto.cmdproj\"\n";
    std::cout << "  concrete-mix-designer-cli.exe report \"projeto.cmdproj\" --format markdown --out relatorio.md\n";
    std::cout << "  concrete-mix-designer-cli.exe standards cements --format text\n";
    std::cout << "  concrete-mix-designer-cli.exe presets --export 1 c25_urbano.cmdproj\n";
    std::cout << "  concrete-mix-designer-cli.exe test\n";
}

bool saveStringToFile(const QString& path, const std::string& content) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        std::cerr << "[ERROR] Cannot open file for writing: " << path.toStdString() << "\n";
        return false;
    }
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << QString::fromUtf8(content.c_str());
    file.close();
    return true;
}

QJsonObject mixResultToJson(const MixDesignResult& res, const ProjectData& proj) {
    QJsonObject obj;
    obj["fck"] = proj.fck;
    obj["fcj"] = res.fcj;
    obj["sd"] = res.sd;
    obj["targetWaterCement"] = res.targetWaterCement;
    obj["targetDryMixM"] = res.targetDryMixM;
    obj["targetCementConsumption"] = res.targetCementConsumption;
    obj["sandRatioA"] = res.sandRatioA;
    obj["gravelRatioP"] = res.gravelRatioP;

    QJsonObject abramsObj;
    abramsObj["equation"] = QString::fromStdString(res.abramsPoly.toEquationString("fcj", "w/c"));
    abramsObj["rSquared"] = res.abramsPoly.rSquared;
    obj["abrams"] = abramsObj;

    QJsonObject lyseObj;
    lyseObj["equation"] = QString::fromStdString(res.lyseLinear.toEquationString("m", "w/c"));
    lyseObj["rSquared"] = res.lyseLinear.rSquared;
    obj["lyse"] = lyseObj;

    QJsonObject molinariObj;
    molinariObj["equation"] = QString::fromStdString(res.molinariPoly.toEquationString("m", "C"));
    molinariObj["rSquared"] = res.molinariPoly.rSquared;
    obj["molinari"] = molinariObj;

    QJsonObject perM3;
    perM3["cementKg"] = res.perM3Analytical.cementKg;
    perM3["sandDryKg"] = res.perM3Analytical.sandDryKg;
    perM3["sandWetKg"] = res.perM3Analytical.sandWetKg;
    perM3["gravelDryKg"] = res.perM3Analytical.gravelDryKg;
    perM3["gravelWetKg"] = res.perM3Analytical.gravelWetKg;
    perM3["gravel1WetKg"] = res.perM3Analytical.gravel1WetKg;
    perM3["gravel2WetKg"] = res.perM3Analytical.gravel2WetKg;
    perM3["waterCorrectedL"] = res.perM3Analytical.waterCorrectedL;
    perM3["totalMassKg"] = res.perM3Analytical.totalMassKg;
    obj["quantitiesPerM3"] = perM3;

    QJsonObject jobsite;
    jobsite["sandPadiolaCount"] = res.jobsiteBatch50kg.sandPadiolaCount;
    jobsite["sandPadiolaHeightCm"] = res.jobsiteBatch50kg.sandPadiolaHeightCm;
    jobsite["gravelPadiolaCount"] = res.jobsiteBatch50kg.gravelPadiolaCount;
    jobsite["gravelPadiolaHeightCm"] = res.jobsiteBatch50kg.gravelPadiolaHeightCm;
    jobsite["waterLiters"] = res.jobsiteBatch50kg.waterLiters;
    obj["jobsite50kgBatch"] = jobsite;

    QJsonObject costs;
    costs["costPerM3Analytical"] = res.costPerM3Analytical.totalCost;
    costs["costPerM3Graphical"] = res.costPerM3Graphical.totalCost;
    costs["projectCostAnalytical"] = res.projectCostAnalytical.totalCost;
    costs["totalCementBags"] = res.totalCementBagsAnalytical;
    obj["costs"] = costs;

    QJsonObject compliance;
    compliance["isCompliant"] = res.compliance.isCompliant;
    QJsonArray passedArr;
    for (const auto& chk : res.compliance.passedChecks) passedArr.append(QString::fromStdString(chk));
    compliance["passedChecks"] = passedArr;
    QJsonArray warnArr;
    for (const auto& w : res.compliance.warnings) warnArr.append(QString::fromStdString(w));
    compliance["warnings"] = warnArr;
    QJsonArray errArr;
    for (const auto& e : res.compliance.errors) errArr.append(QString::fromStdString(e));
    compliance["errors"] = errArr;
    obj["compliance"] = compliance;

    return obj;
}

void printTextSummary(const MixDesignResult& res, const ProjectData& proj, bool isEnglish) {
    const std::string cur = isEnglish ? "$ " : "R$ ";
    std::cout << std::fixed << std::setprecision(2);
    if (isEnglish) {
        std::cout << "------------------------------------------------------------------------\n";
        std::cout << "  CALCULATION RESULTS: IPT / EPUSP CONCRETE MIX DESIGN                  \n";
        std::cout << "------------------------------------------------------------------------\n";
        std::cout << "  - Target Mean Strength (fcj):       " << res.fcj << " MPa (fck = " << proj.fck << " MPa, Sd = " << res.sd << " MPa)\n";
        std::cout << "  - Target Water/Cement Ratio (w/c):   " << std::setprecision(4) << res.targetWaterCement << "\n";
        std::cout << "  - Total Dry Mix Ratio (1:m):        1 : " << std::setprecision(2) << res.targetDryMixM << "\n";
        std::cout << "  - Dry Unit Mix (1:a:p):             1 : " << res.sandRatioA << " : " << res.gravelRatioP << "\n";
        std::cout << "  - Cement Content (C):               " << std::setprecision(1) << res.targetCementConsumption << " kg/m³\n";
        std::cout << "  - Equations:\n";
        std::cout << "    * Abrams (fcj vs w/c): " << res.abramsPoly.toEquationString("fcj", "w/c") << "\n";
        std::cout << "    * Lyse (m vs w/c):     " << res.lyseLinear.toEquationString("m", "w/c") << "\n";
        std::cout << "    * Molinari (m vs C):   " << res.molinariPoly.toEquationString("m", "C") << "\n";
        std::cout << "  - Materials per 1 m³ of Concrete:\n";
        std::cout << "    * Cement:        " << std::setprecision(1) << res.perM3Analytical.cementKg << " kg\n";
        std::cout << "    * Sand (wet):    " << res.perM3Analytical.sandWetKg << " kg (moisture: " << proj.sand.moisturePercent << "%)\n";
        if (proj.useGravelBlend && proj.gravel2Percent > 0.0) {
            std::cout << "    * Gravel 1 (wet): " << res.perM3Analytical.gravel1WetKg << " kg\n";
            std::cout << "    * Gravel 2 (wet): " << res.perM3Analytical.gravel2WetKg << " kg\n";
        } else {
            std::cout << "    * Gravel (wet):  " << res.perM3Analytical.gravelWetKg << " kg\n";
        }
        std::cout << "    * Effective Water: " << res.perM3Analytical.waterCorrectedL << " L\n";
        std::cout << "    * Total Mass:      " << res.perM3Analytical.totalMassKg << " kg/m³\n";
        std::cout << "  - Jobsite Measuring Boxes (50 kg Bag, Base 35x45 cm):\n";
        std::cout << "    * Sand:   " << res.jobsiteBatch50kg.sandPadiolaCount << " box(es) with height " << res.jobsiteBatch50kg.sandPadiolaHeightCm << " cm\n";
        std::cout << "    * Gravel: " << res.jobsiteBatch50kg.gravelPadiolaCount << " box(es) with height " << res.jobsiteBatch50kg.gravelPadiolaHeightCm << " cm\n";
        std::cout << "    * Water:  " << res.jobsiteBatch50kg.waterLiters << " Liters\n";
        std::cout << "  - Economic Analysis:\n";
        std::cout << "    * Cost per m³:     " << cur << res.costPerM3Analytical.totalCost << "/m³\n";
        std::cout << "    * Total Cost (" << std::setprecision(0) << proj.projectVolumeM3 << " m³): " << cur << std::setprecision(2) << res.projectCostAnalytical.totalCost << "\n";
        std::cout << "  - Normative Standards Compliance:\n";
        if (res.compliance.isCompliant) {
            std::cout << "    [✔ COMPLIANT] All ABNT NBR 12655 and NBR 6118 requirements met.\n";
        } else {
            std::cout << "    [⚠ NON-COMPLIANT / PENDING REQUIREMENTS]:\n";
            for (const auto& err : res.compliance.errors) {
                std::cout << "      - [x] " << err << "\n";
            }
        }
    } else {
        std::cout << "------------------------------------------------------------------------\n";
        std::cout << "  RESULTADOS DO ESTUDO DE DOSAGEM: MÉTODO IPT / EPUSP                   \n";
        std::cout << "------------------------------------------------------------------------\n";
        std::cout << "  - Resistência Média (fcj):          " << res.fcj << " MPa (fck = " << proj.fck << " MPa, Sd = " << res.sd << " MPa)\n";
        std::cout << "  - Relação Água/Cimento (a/c):       " << std::setprecision(4) << res.targetWaterCement << "\n";
        std::cout << "  - Traço Seco Total (1:m):           1 : " << std::setprecision(2) << res.targetDryMixM << "\n";
        std::cout << "  - Traço Unitário em Massa (1:a:p):  1 : " << res.sandRatioA << " : " << res.gravelRatioP << "\n";
        std::cout << "  - Consumo de Cimento (C):           " << std::setprecision(1) << res.targetCementConsumption << " kg/m³\n";
        std::cout << "  - Equações Ajustadas:\n";
        std::cout << "    * Abrams (fcj x a/c): " << res.abramsPoly.toEquationString("fcj", "a/c") << "\n";
        std::cout << "    * Lyse (m x a/c):     " << res.lyseLinear.toEquationString("m", "a/c") << "\n";
        std::cout << "    * Molinari (m x C):   " << res.molinariPoly.toEquationString("m", "C") << "\n";
        std::cout << "  - Consumo de Materiais por 1 m³ de Concreto:\n";
        std::cout << "    * Cimento:       " << std::setprecision(1) << res.perM3Analytical.cementKg << " kg\n";
        std::cout << "    * Areia Úmida:   " << res.perM3Analytical.sandWetKg << " kg (umidade: " << proj.sand.moisturePercent << "%)\n";
        if (proj.useGravelBlend && proj.gravel2Percent > 0.0) {
            std::cout << "    * Brita 1 Úmida: " << res.perM3Analytical.gravel1WetKg << " kg\n";
            std::cout << "    * Brita 2 Úmida: " << res.perM3Analytical.gravel2WetKg << " kg\n";
        } else {
            std::cout << "    * Brita Úmida:   " << res.perM3Analytical.gravelWetKg << " kg\n";
        }
        std::cout << "    * Água Efetiva:  " << res.perM3Analytical.waterCorrectedL << " L\n";
        std::cout << "    * Massa Total:   " << res.perM3Analytical.totalMassKg << " kg/m³\n";
        std::cout << "  - Padiolas de Canteiro (Saco 50 kg, Base 35x45 cm):\n";
        std::cout << "    * Areia: " << res.jobsiteBatch50kg.sandPadiolaCount << " padiola(s) com altura " << res.jobsiteBatch50kg.sandPadiolaHeightCm << " cm\n";
        std::cout << "    * Brita: " << res.jobsiteBatch50kg.gravelPadiolaCount << " padiola(s) com altura " << res.jobsiteBatch50kg.gravelPadiolaHeightCm << " cm\n";
        std::cout << "    * Água:  " << res.jobsiteBatch50kg.waterLiters << " Litros\n";
        std::cout << "  - Análise Econômica:\n";
        std::cout << "    * Custo por m³:     " << cur << res.costPerM3Analytical.totalCost << "/m³\n";
        std::cout << "    * Custo Total Obra (" << std::setprecision(0) << proj.projectVolumeM3 << " m³): " << cur << std::setprecision(2) << res.projectCostAnalytical.totalCost << "\n";
        std::cout << "  - Conformidade Normativa:\n";
        if (res.compliance.isCompliant) {
            std::cout << "    [✔ CONFORME] Atende integralmente às normas ABNT NBR 12655 e NBR 6118.\n";
        } else {
            std::cout << "    [⚠ NÃO CONFORME / PENDÊNCIAS IDENTIFICADAS]:\n";
            for (const auto& err : res.compliance.errors) {
                std::cout << "      - [x] " << err << "\n";
            }
        }
    }
    std::cout << "------------------------------------------------------------------------\n";
}

int handleOutput(const MixDesignResult& res, const ProjectData& proj, const QString& format, const QString& outFile, bool isEnglish, bool isDark) {
    if (format == QStringLiteral("json")) {
        QJsonObject root = mixResultToJson(res, proj);
        QJsonDocument doc(root);
        std::string jsonStr = doc.toJson(QJsonDocument::Indented).toStdString();
        if (!outFile.isEmpty()) {
            if (saveStringToFile(outFile, jsonStr)) {
                std::cout << "[+] JSON results successfully written to: " << outFile.toStdString() << "\n";
                return 0;
            }
            return 2;
        }
        std::cout << jsonStr << std::endl;
        return res.compliance.isCompliant ? 0 : 1;
    } else if (format == QStringLiteral("markdown") || format == QStringLiteral("md")) {
        std::string mdStr = proj.generateMarkdownReport(res, isEnglish);
        if (!outFile.isEmpty()) {
            if (saveStringToFile(outFile, mdStr)) {
                std::cout << "[+] Markdown report successfully written to: " << outFile.toStdString() << "\n";
                return 0;
            }
            return 2;
        }
        std::cout << mdStr << std::endl;
        return res.compliance.isCompliant ? 0 : 1;
    } else if (format == QStringLiteral("html")) {
        std::string htmlStr = proj.generateHtmlReport(res, isDark, isEnglish);
        if (!outFile.isEmpty()) {
            if (saveStringToFile(outFile, htmlStr)) {
                std::cout << "[+] HTML report successfully written to: " << outFile.toStdString() << "\n";
                return 0;
            }
            return 2;
        }
        std::cout << htmlStr << std::endl;
        return res.compliance.isCompliant ? 0 : 1;
    } else {
        printTextSummary(res, proj, isEnglish);
        if (!outFile.isEmpty()) {
            std::ostringstream ss;
            // Generate standard text report to file
            ss << proj.generateMarkdownReport(res, isEnglish);
            saveStringToFile(outFile, ss.str());
            std::cout << "[+] Text report saved to: " << outFile.toStdString() << "\n";
        }
        return res.compliance.isCompliant ? 0 : 1;
    }
}

} // namespace

int main(int argc, char *argv[])
{
#if defined(_WIN32)
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("ConcreteMixDesignerCLI"));
    QCoreApplication::setApplicationVersion(QStringLiteral("2026.10.26.0"));

    NormativeDatabase::instance().initialize();

    if (argc < 2) {
        printHelp();
        return 0;
    }

    const QString cmd = QString::fromLocal8Bit(argv[1]).trimmed().toLower();

    if (cmd == QStringLiteral("--version") || cmd == QStringLiteral("-v") || cmd == QStringLiteral("version")) {
        std::cout << "Concrete Mix Designer CLI v" << kCliVersion << " (C++20)\n";
        return 0;
    }

    if (cmd == QStringLiteral("--help") || cmd == QStringLiteral("-h") || cmd == QStringLiteral("help")) {
        printHelp();
        return 0;
    }

    // Argument extraction
    QString format = QStringLiteral("text");
    QString outFile;
    bool isEnglish = false;
    bool isDark = false;
    bool noBanner = false;

    // Design options
    double fck = 25.0;
    double slump = 100.0;
    QString caaStr = QStringLiteral("CAA II");
    QString condStr = QStringLiteral("A");
    double customSd = 0.0;
    QString structStr = QStringLiteral("rc");
    double alpha = 52.0;
    double mixerVol = 30.0;
    double projectVol = 100.0;
    double gravelBlend = 0.0;
    QString projectName;
    QString engineerName;
    QString institution;
    QString studentId;
    QString projectFile;
    int presetId = 1;

    for (int i = 2; i < argc; ++i) {
        QString arg = QString::fromLocal8Bit(argv[i]);
        if (arg == QStringLiteral("--format") && i + 1 < argc) {
            format = QString::fromLocal8Bit(argv[++i]).toLower();
        } else if (arg == QStringLiteral("--out") && i + 1 < argc) {
            outFile = QString::fromLocal8Bit(argv[++i]);
        } else if (arg == QStringLiteral("--lang") && i + 1 < argc) {
            QString l = QString::fromLocal8Bit(argv[++i]).toLower();
            isEnglish = (l == QStringLiteral("en") || l == QStringLiteral("en_us") || l == QStringLiteral("english"));
        } else if (arg == QStringLiteral("--dark")) {
            isDark = true;
        } else if (arg == QStringLiteral("--no-banner")) {
            noBanner = true;
        } else if (arg == QStringLiteral("--project") && i + 1 < argc) {
            projectName = QString::fromLocal8Bit(argv[++i]);
        } else if (arg == QStringLiteral("--engineer") && i + 1 < argc) {
            engineerName = QString::fromLocal8Bit(argv[++i]);
        } else if (arg == QStringLiteral("--institution") && i + 1 < argc) {
            institution = QString::fromLocal8Bit(argv[++i]);
        } else if (arg == QStringLiteral("--student-id") && i + 1 < argc) {
            studentId = QString::fromLocal8Bit(argv[++i]);
        } else if (arg == QStringLiteral("--preset") && i + 1 < argc) {
            presetId = QString::fromLocal8Bit(argv[++i]).toInt();
        } else if (arg == QStringLiteral("--file") && i + 1 < argc) {
            projectFile = QString::fromLocal8Bit(argv[++i]);
        } else if (arg == QStringLiteral("--fck") && i + 1 < argc) {
            fck = QString::fromLocal8Bit(argv[++i]).toDouble();
        } else if (arg == QStringLiteral("--slump") && i + 1 < argc) {
            slump = QString::fromLocal8Bit(argv[++i]).toDouble();
        } else if (arg == QStringLiteral("--caa") && i + 1 < argc) {
            caaStr = QString::fromLocal8Bit(argv[++i]).toUpper();
            if (!caaStr.startsWith(QStringLiteral("CAA "))) caaStr = QStringLiteral("CAA ") + caaStr;
        } else if (arg == QStringLiteral("--cond") && i + 1 < argc) {
            condStr = QString::fromLocal8Bit(argv[++i]).toUpper();
        } else if (arg == QStringLiteral("--sd") && i + 1 < argc) {
            customSd = QString::fromLocal8Bit(argv[++i]).toDouble();
        } else if (arg == QStringLiteral("--struct") && i + 1 < argc) {
            structStr = QString::fromLocal8Bit(argv[++i]).toLower();
        } else if (arg == QStringLiteral("--alpha") && i + 1 < argc) {
            alpha = QString::fromLocal8Bit(argv[++i]).toDouble();
        } else if (arg == QStringLiteral("--mixer-vol") && i + 1 < argc) {
            mixerVol = QString::fromLocal8Bit(argv[++i]).toDouble();
        } else if (arg == QStringLiteral("--project-vol") && i + 1 < argc) {
            projectVol = QString::fromLocal8Bit(argv[++i]).toDouble();
        } else if (arg == QStringLiteral("--gravel-blend") && i + 1 < argc) {
            gravelBlend = QString::fromLocal8Bit(argv[++i]).toDouble();
        } else if (projectFile.isEmpty() && !arg.startsWith(QStringLiteral("--")) &&
                   (QFile::exists(arg) || arg.endsWith(QStringLiteral(".cmdproj")) || arg.endsWith(QStringLiteral(".json")))) {
            projectFile = arg;
        }
    }

    if (!noBanner && format != QStringLiteral("json")) {
        printBanner();
    }

    // -------------------------------------------------------------------------
    // Command: design
    // -------------------------------------------------------------------------
    if (cmd == QStringLiteral("design")) {
        ProjectData proj;
        proj.loadPreset(presetId); // Baseline materials and curves
        if (!projectName.isEmpty()) {
            proj.projectName = projectName.toStdString();
            proj.customProjectName = projectName.toStdString();
            proj.usePresetProjectName = false;
        }
        if (!engineerName.isEmpty()) proj.engineerName = engineerName.toStdString();
        if (!institution.isEmpty()) proj.institution = institution.toStdString();
        if (!studentId.isEmpty()) proj.studentId = studentId.toStdString();

        proj.fck = fck;
        proj.targetSlumpMm = slump;
        proj.selectedAlphaPercent = alpha;
        proj.labMixerVolumeL = mixerVol;
        proj.projectVolumeM3 = projectVol;
        proj.gravel2Percent = gravelBlend;
        proj.useGravelBlend = (gravelBlend > 0.0);

        if (caaStr == QStringLiteral("CAA I")) proj.envClass = EnvironmentalClass::CAA_I;
        else if (caaStr == QStringLiteral("CAA III")) proj.envClass = EnvironmentalClass::CAA_III;
        else if (caaStr == QStringLiteral("CAA IV")) proj.envClass = EnvironmentalClass::CAA_IV;
        else proj.envClass = EnvironmentalClass::CAA_II;

        if (condStr == QStringLiteral("B")) proj.prepCondition = PreparationCondition::ConditionB;
        else if (condStr == QStringLiteral("C")) proj.prepCondition = PreparationCondition::ConditionC;
        else proj.prepCondition = PreparationCondition::ConditionA;

        if (customSd > 0.0) proj.customSd = customSd;
        proj.structType = (structStr == QStringLiteral("pc")) ? StructuralType::PrestressedConcrete : StructuralType::ReinforcedConcrete;

        MixDesignResult res = proj.calculate();
        return handleOutput(res, proj, format, outFile, isEnglish, isDark);
    }

    // -------------------------------------------------------------------------
    // Command: calculate / calc
    // -------------------------------------------------------------------------
    if (cmd == QStringLiteral("calculate") || cmd == QStringLiteral("calc")) {
        if (projectFile.isEmpty()) {
            std::cerr << "[ERROR] Missing required <project.cmdproj> argument for calculate command.\n";
            std::cerr << "Run 'concrete-mix-designer-cli.exe --help' for usage.\n";
            return 3;
        }

        QFile file(projectFile);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            std::cerr << "[ERROR] Cannot open project file: " << projectFile.toStdString() << "\n";
            return 3;
        }
        std::string jsonStr = file.readAll().toStdString();
        file.close();

        ProjectData proj;
        if (!proj.fromJson(jsonStr)) {
            std::cerr << "[ERROR] Invalid or corrupted project file format in: " << projectFile.toStdString() << "\n";
            return 2;
        }

        MixDesignResult res = proj.calculate();
        return handleOutput(res, proj, format, outFile, isEnglish, isDark);
    }

    // -------------------------------------------------------------------------
    // Command: verify / audit
    // -------------------------------------------------------------------------
    if (cmd == QStringLiteral("verify") || cmd == QStringLiteral("audit")) {
        ProjectData proj;
        if (!projectFile.isEmpty()) {
            QFile file(projectFile);
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                std::cerr << "[ERROR] Cannot open project file: " << projectFile.toStdString() << "\n";
                return 3;
            }
            std::string jsonStr = file.readAll().toStdString();
            file.close();
            if (!proj.fromJson(jsonStr)) {
                std::cerr << "[ERROR] Invalid project JSON.\n";
                return 2;
            }
        } else {
            proj.loadPreset(1);
            proj.fck = fck;
            proj.targetSlumpMm = slump;
        }

        MixDesignResult res = proj.calculate();
        if (format == QStringLiteral("json")) {
            QJsonObject obj;
            obj["isCompliant"] = res.compliance.isCompliant;
            QJsonArray p;
            for (const auto& s : res.compliance.passedChecks) p.append(QString::fromStdString(s));
            obj["passedChecks"] = p;
            QJsonArray w;
            for (const auto& s : res.compliance.warnings) w.append(QString::fromStdString(s));
            obj["warnings"] = w;
            QJsonArray e;
            for (const auto& s : res.compliance.errors) e.append(QString::fromStdString(s));
            obj["errors"] = e;
            QJsonDocument doc(obj);
            std::cout << doc.toJson(QJsonDocument::Indented).toStdString() << std::endl;
            return res.compliance.isCompliant ? 0 : 1;
        }

        std::cout << "========================================================================\n";
        std::cout << "  NORMATIVE STANDARDS AUDIT REPORT (ABNT NBR 12655 / NBR 6118 / NBR 7212)\n";
        std::cout << "========================================================================\n";
        std::cout << "Passed Verifications (" << res.compliance.passedChecks.size() << "):\n";
        for (const auto& p : res.compliance.passedChecks) {
            std::cout << "  [✔] " << p << "\n";
        }
        if (!res.compliance.warnings.empty()) {
            std::cout << "\nTechnical Warnings (" << res.compliance.warnings.size() << "):\n";
            for (const auto& w : res.compliance.warnings) {
                std::cout << "  [!] " << w << "\n";
            }
        }
        if (!res.compliance.errors.empty()) {
            std::cout << "\nCritical Inconformities (" << res.compliance.errors.size() << "):\n";
            for (const auto& e : res.compliance.errors) {
                std::cout << "  [✖] " << e << "\n";
            }
        }
        std::cout << "------------------------------------------------------------------------\n";
        std::cout << "Final Audit Result: " << (res.compliance.isCompliant ? "COMPLIANT (APPROVED)" : "NON-COMPLIANT (ATTENTION REQUIRED)") << "\n";
        std::cout << "========================================================================\n";
        return res.compliance.isCompliant ? 0 : 1;
    }

    // -------------------------------------------------------------------------
    // Command: report
    // -------------------------------------------------------------------------
    if (cmd == QStringLiteral("report")) {
        ProjectData proj;
        if (!projectFile.isEmpty()) {
            QFile file(projectFile);
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                std::cerr << "[ERROR] Cannot open project file: " << projectFile.toStdString() << "\n";
                return 3;
            }
            std::string jsonStr = file.readAll().toStdString();
            file.close();
            if (!proj.fromJson(jsonStr)) {
                std::cerr << "[ERROR] Invalid project JSON.\n";
                return 2;
            }
        } else {
            proj.loadPreset(presetId);
        }

        if (!projectName.isEmpty()) {
            proj.projectName = projectName.toStdString();
            proj.customProjectName = projectName.toStdString();
            proj.usePresetProjectName = false;
        }
        if (!engineerName.isEmpty()) proj.engineerName = engineerName.toStdString();
        if (!institution.isEmpty()) proj.institution = institution.toStdString();
        if (!studentId.isEmpty()) proj.studentId = studentId.toStdString();
        if (fck > 0.0) proj.fck = fck;
        if (slump > 0.0) proj.targetSlumpMm = slump;
        if (alpha > 0.0) proj.selectedAlphaPercent = alpha;
        if (projectVol > 0.0) proj.projectVolumeM3 = projectVol;
        if (mixerVol > 0.0) proj.labMixerVolumeL = mixerVol;
        if (caaStr == QStringLiteral("CAA I")) proj.envClass = EnvironmentalClass::CAA_I;
        else if (caaStr == QStringLiteral("CAA III")) proj.envClass = EnvironmentalClass::CAA_III;
        else if (caaStr == QStringLiteral("CAA IV")) proj.envClass = EnvironmentalClass::CAA_IV;
        else if (caaStr == QStringLiteral("CAA II")) proj.envClass = EnvironmentalClass::CAA_II;

        MixDesignResult res = proj.calculate();
        if (format == QStringLiteral("html")) {
            std::string html = proj.generateHtmlReport(res, isDark, isEnglish);
            if (!outFile.isEmpty()) {
                saveStringToFile(outFile, html);
                std::cout << "[+] HTML report written to: " << outFile.toStdString() << "\n";
                return 0;
            }
            std::cout << html << std::endl;
        } else {
            std::string md = proj.generateMarkdownReport(res, isEnglish);
            if (!outFile.isEmpty()) {
                saveStringToFile(outFile, md);
                std::cout << "[+] Markdown report written to: " << outFile.toStdString() << "\n";
                return 0;
            }
            std::cout << md << std::endl;
        }
        return 0;
    }

    // -------------------------------------------------------------------------
    // Command: standards
    // -------------------------------------------------------------------------
    if (cmd == QStringLiteral("standards")) {
        QString sub = (argc > 2 && !QString::fromLocal8Bit(argv[2]).startsWith(QStringLiteral("--")))
                      ? QString::fromLocal8Bit(argv[2]).toLower() : QStringLiteral("all");

        if (format == QStringLiteral("json")) {
            QJsonObject root;
            if (sub == QStringLiteral("cements") || sub == QStringLiteral("all")) {
                QJsonArray arr;
                for (const auto& c : NormativeDatabase::instance().getAllCementTypes()) {
                    QJsonObject o;
                    o["code"] = c.code;
                    o["name"] = c.name;
                    o["standard"] = c.standard;
                    o["specificMass"] = c.specificMass;
                    o["unitMass"] = c.unitMass;
                    arr.append(o);
                }
                root["cements"] = arr;
            }
            if (sub == QStringLiteral("aggregates") || sub == QStringLiteral("all")) {
                QJsonArray arr;
                for (const auto& a : NormativeDatabase::instance().getAllAggregates()) {
                    QJsonObject o;
                    o["code"] = a.code;
                    o["commercialName"] = a.commercialName;
                    o["dmcMm"] = a.dmcMm;
                    o["specificMass"] = a.specificMass;
                    arr.append(o);
                }
                root["aggregates"] = arr;
            }
            if (sub == QStringLiteral("slump") || sub == QStringLiteral("all")) {
                QJsonArray arr;
                for (const auto& s : NormativeDatabase::instance().getAllSlumpClasses()) {
                    QJsonObject o;
                    o["class"] = s.slumpClass;
                    o["minSlump"] = s.minSlumpMm;
                    o["maxSlump"] = s.maxSlumpMm;
                    o["application"] = s.application;
                    arr.append(o);
                }
                root["slumpClasses"] = arr;
            }
            if (sub == QStringLiteral("tolerances") || sub == QStringLiteral("all")) {
                QJsonArray arr;
                for (const auto& t : NormativeDatabase::instance().getAllWeighingTolerances()) {
                    QJsonObject o;
                    o["component"] = t.component;
                    o["tolerancePercent"] = t.tolerancePercent;
                    o["standard"] = t.standard;
                    arr.append(o);
                }
                root["batchingTolerances"] = arr;
            }
            QJsonDocument doc(root);
            std::cout << doc.toJson(QJsonDocument::Indented).toStdString() << std::endl;
            return 0;
        }

        if (sub == QStringLiteral("cements") || sub == QStringLiteral("all")) {
            std::cout << "--- CEMENT TYPES (ABNT NBR 16697) ---\n";
            for (const auto& c : NormativeDatabase::instance().getAllCementTypes()) {
                std::cout << "  * " << c.code.toStdString() << " - " << c.name.toStdString()
                          << " | Density: " << c.specificMass << " kg/dm³ | " << c.standard.toStdString() << "\n";
            }
            std::cout << "\n";
        }
        if (sub == QStringLiteral("aggregates") || sub == QStringLiteral("all")) {
            std::cout << "--- NORMATIVE AGGREGATES (ABNT NBR 7211:2022) ---\n";
            for (const auto& a : NormativeDatabase::instance().getAllAggregates()) {
                std::cout << "  * " << a.code.toStdString() << " (" << a.commercialName.toStdString()
                          << ") | DMC: " << a.dmcMm << " mm | Density: " << a.specificMass << " kg/dm³\n";
            }
            std::cout << "\n";
        }
        if (sub == QStringLiteral("slump") || sub == QStringLiteral("all")) {
            std::cout << "--- SLUMP CONSISTENCY CLASSES (ABNT NBR 8953) ---\n";
            for (const auto& s : NormativeDatabase::instance().getAllSlumpClasses()) {
                std::cout << "  * " << s.slumpClass.toStdString() << ": " << s.minSlumpMm << " to " << s.maxSlumpMm
                          << " mm (" << s.placementMethod.toStdString() << ")\n";
            }
            std::cout << "\n";
        }
        if (sub == QStringLiteral("tolerances") || sub == QStringLiteral("all")) {
            std::cout << "--- BATCHING WEIGHING TOLERANCES (ABNT NBR 7212:2025 Item 5.5) ---\n";
            for (const auto& t : NormativeDatabase::instance().getAllWeighingTolerances()) {
                std::cout << "  * " << t.component.toStdString() << ": ±" << t.tolerancePercent << "% (" << t.standard.toStdString() << ")\n";
            }
            std::cout << "\n";
        }
        return 0;
    }

    // -------------------------------------------------------------------------
    // Command: presets
    // -------------------------------------------------------------------------
    if (cmd == QStringLiteral("presets")) {
        bool listOnly = true;
        int exportId = -1;
        int runId = -1;
        QString exportPath;

        for (int i = 2; i < argc; ++i) {
            QString arg = QString::fromLocal8Bit(argv[i]);
            if (arg == QStringLiteral("--list")) {
                listOnly = true;
            } else if (arg == QStringLiteral("--export") && i + 2 < argc) {
                exportId = QString::fromLocal8Bit(argv[++i]).toInt();
                exportPath = QString::fromLocal8Bit(argv[++i]);
                listOnly = false;
            } else if (arg == QStringLiteral("--run") && i + 1 < argc) {
                runId = QString::fromLocal8Bit(argv[++i]).toInt();
                listOnly = false;
            }
        }

        if (exportId >= 0 && !exportPath.isEmpty()) {
            ProjectData proj;
            proj.loadPreset(exportId);
            saveStringToFile(exportPath, proj.toJson());
            std::cout << "[+] Preset " << exportId << " exported to: " << exportPath.toStdString() << "\n";
            return 0;
        }

        if (runId >= 0) {
            ProjectData proj;
            proj.loadPreset(runId);
            MixDesignResult res = proj.calculate();
            return handleOutput(res, proj, format, outFile, isEnglish, isDark);
        }

        std::cout << "Available Built-in Presets:\n";
        std::cout << "  [0] Cenário 0: Personalizável pelo usuário (Valores em branco)\n";
        std::cout << "  [1] Cenário 1: Concreto Estrutural C25 Urbano (CAA II) - fck 25 MPa\n";
        std::cout << "  [2] Cenário 2: Concreto Estrutural C30 Marinho (CAA III) - fck 30 MPa\n";
        std::cout << "  [3] Cenário 3: Concreto C40 Alto Desempenho (CAA IV) - fck 40 MPa\n";
        std::cout << "  [4] Cenário 4: Concreto C20 Fundações (Condição B) - fck 20 MPa\n";
        std::cout << "  [5] Cenário 5: Planilha UFPR TC031 Original Auditada - fck 25 MPa\n\n";
        std::cout << "Use 'concrete-mix-designer-cli.exe presets --run <id>' or '--export <id> <file.cmdproj>'\n";
        return 0;
    }

    // -------------------------------------------------------------------------
    // Command: test
    // -------------------------------------------------------------------------
    if (cmd == QStringLiteral("test")) {
        std::cout << "[*] Executing Internal Calculation & Normative Audit Self-Test...\n\n";
        ProjectData p;
        p.loadPreset(1);
        MixDesignResult r1 = p.calculate();
        if (std::abs(r1.fcj - 31.60) > 0.05) {
            std::cerr << "[FAIL] Scenario 1 fcj error: got " << r1.fcj << ", expected 31.60\n";
            return 1;
        }
        std::cout << "[PASS] Scenario 1 fcj = " << r1.fcj << " MPa\n";

        p.loadPreset(2);
        MixDesignResult r2 = p.calculate();
        if (std::abs(r2.fcj - 36.60) > 0.05) {
            std::cerr << "[FAIL] Scenario 2 fcj error\n";
            return 1;
        }
        std::cout << "[PASS] Scenario 2 fcj = " << r2.fcj << " MPa\n";

        p.loadPreset(5);
        MixDesignResult r5 = p.calculate();
        if (std::abs(r5.fcj - 35.775) > 0.05) {
            std::cerr << "[FAIL] Scenario 5 (UFPR) fcj error\n";
            return 1;
        }
        std::cout << "[PASS] Scenario 5 (UFPR) fcj = " << r5.fcj << " MPa\n";

        // Test mass balance
        double theoreticalTotal = r5.perM3Analytical.cementKg + r5.perM3Analytical.sandDryKg +
                                  r5.perM3Analytical.gravelDryKg + r5.perM3Analytical.waterTheoreticalL;
        if (std::abs(theoreticalTotal - r5.perM3Analytical.totalMassKg) > 1e-4) {
            std::cerr << "[FAIL] Mass balance error\n";
            return 1;
        }
        std::cout << "[PASS] Mass balance closure: " << r5.perM3Analytical.totalMassKg << " kg/m³ exact\n";

        // Test currency strings in reports
        std::string mdPt = p.generateMarkdownReport(r5, false);
        std::string mdEn = p.generateMarkdownReport(r5, true);
        if (mdPt.find("R$ ") == std::string::npos || mdEn.find("$ ") == std::string::npos) {
            std::cerr << "[FAIL] Currency translation report error\n";
            return 1;
        }
        std::cout << "[PASS] Currency translation: pt_BR uses R$, en_US uses $\n";

        std::cout << "\n[✔] ALL INTERNAL ENGINE AND NORMATIVE SELF-TESTS PASSED SUCCESSFULLY!\n";
        return 0;
    }

    std::cerr << "[ERROR] Unknown command: " << cmd.toStdString() << "\n";
    printHelp();
    return 3;
}
