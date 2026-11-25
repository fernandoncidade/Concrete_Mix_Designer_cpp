#include "TestFramework.hpp"

#include "../source/core/ProjectData.hpp"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QGuiApplication>
#include <QMarginsF>
#include <QPageLayout>
#include <QPageSize>
#include <QPdfWriter>
#include <QTemporaryDir>
#include <QTextDocument>

#include <string>

using namespace ConcreteMix;

int main(int argc, char* argv[]) {
    if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM")) {
        qputenv("QT_QPA_PLATFORM", "offscreen");
    }
    QGuiApplication app(argc, argv);

    concrete_tests::TestRunner runner("ProjectDataSerializationAndReports");

    runner.add("AllSixPresetsCalculateWithoutErrors", []() {
        // Preset 0 (Personalizável pelo usuário) inicia limpo (fck == 0.0) e gera HTML aguardando dados
        {
            ProjectData customProj;
            customProj.loadPreset(0);
            CMD_ASSERT(customProj.currentPresetIndex == 0);
            CMD_ASSERT_NEAR(customProj.fck, 0.0, 1e-9);
            const MixDesignResult blankRes = customProj.calculate();
            const std::string blankHtml = customProj.generateHtmlReport(blankRes, false, true);
            CMD_ASSERT_CONTAINS(blankHtml, "Waiting for User Input Data");
        }

        // Presets 1..5 calculam estudos completos de dosagem e consumo por m³
        for (int preset = 1; preset <= 5; ++preset) {
            ProjectData proj;
            proj.loadPreset(preset);
            CMD_ASSERT(proj.currentPresetIndex == preset);
            CMD_ASSERT(!ProjectData::getPresetName(preset, false).empty());
            CMD_ASSERT(!ProjectData::getPresetName(preset, true).empty());
            CMD_ASSERT(!proj.getDisplayProjectName(false).empty());
            CMD_ASSERT(!proj.getDisplayProjectName(true).empty());

            const MixDesignResult res = proj.calculate();
            CMD_ASSERT(res.fcj > 0.0);
            CMD_ASSERT(res.perM3Analytical.cementKg > 0.0);
            CMD_ASSERT(res.perM3Analytical.waterCorrectedL > 0.0);
        }
    });

    runner.add("JsonRoundTripFidelityAndMalformedRejection", []() {
        ProjectData orig;
        orig.loadPreset(2);
        orig.usePresetProjectName = false;
        orig.customProjectName = "Estudo Rodoviario Lote 4";
        orig.projectName = orig.customProjectName;
        orig.engineerName = "Eng. Teste QA";
        orig.institution = "UFPR LAB";
        orig.studentId = "CREA-PR-0001";
        orig.fck = 38.5;
        orig.targetSlumpMm = 140.0;
        orig.useGravelBlend = true;
        orig.gravel2Percent = 62.5;
        orig.projectVolumeM3 = 85.0;

        const std::string json = orig.toJson();
        CMD_ASSERT(!json.empty());

        ProjectData restored;
        CMD_ASSERT(restored.fromJson(json));
        CMD_ASSERT(restored.usePresetProjectName == false);
        CMD_ASSERT(restored.customProjectName == orig.customProjectName);
        CMD_ASSERT(restored.engineerName == orig.engineerName);
        CMD_ASSERT(restored.institution == orig.institution);
        CMD_ASSERT(restored.studentId == orig.studentId);
        CMD_ASSERT_NEAR(restored.fck, 38.5, 1e-6);
        CMD_ASSERT_NEAR(restored.targetSlumpMm, 140.0, 1e-6);
        CMD_ASSERT(restored.useGravelBlend);
        CMD_ASSERT_NEAR(restored.gravel2Percent, 62.5, 1e-6);
        CMD_ASSERT_NEAR(restored.projectVolumeM3, 85.0, 1e-6);
        CMD_ASSERT(restored.experimentalMixes.size() == orig.experimentalMixes.size());

        ProjectData invalid;
        CMD_ASSERT(!invalid.fromJson("{not-valid-json"));
        CMD_ASSERT(!invalid.fromJson(""));
    });

    runner.add("BilingualMarkdownAndHtmlReportsWithXssSanitization", []() {
        ProjectData proj;
        proj.loadPreset(5);
        proj.usePresetProjectName = false;
        proj.customProjectName = "Ponte <script>alert('xss')</script> Sul";
        proj.projectName = proj.customProjectName;
        proj.engineerName = "Eng. <img src=x onerror=alert(1)>";
        proj.institution = "Instituto & Teste";
        proj.notes = "Nota <b>teste</b>";
        const MixDesignResult res = proj.calculate();

        const std::string mdPt = proj.generateMarkdownReport(res, false);
        const std::string mdEn = proj.generateMarkdownReport(res, true);
        CMD_ASSERT_CONTAINS(mdPt, "R$");
        CMD_ASSERT_CONTAINS(mdEn, "$");
        CMD_ASSERT(mdPt.size() > 500 && mdEn.size() > 500);

        for (bool dark : {false, true}) {
            for (bool en : {false, true}) {
                const std::string html = proj.generateHtmlReport(res, dark, en);
                CMD_ASSERT_CONTAINS(html, "<html");
                CMD_ASSERT(html.find("<script>alert") == std::string::npos);
                CMD_ASSERT(html.find("<img src=x") == std::string::npos);
                CMD_ASSERT_CONTAINS(html, "&lt;script&gt;");
                CMD_ASSERT_CONTAINS(html, "&lt;img src=x");
            }
        }
    });

    runner.add("PdfReportExportViaQPdfWriter", []() {
        QTemporaryDir tempDir;
        CMD_ASSERT(tempDir.isValid());
        const QString pdfPath = tempDir.filePath(QStringLiteral("memorial_test.pdf"));

        ProjectData proj;
        proj.loadPreset(1);
        const MixDesignResult res = proj.calculate();
        const QString html = QString::fromStdString(proj.generateHtmlReport(res, false, false));

        QPdfWriter writer(pdfPath);
        writer.setPageSize(QPageSize(QPageSize::A4));
        writer.setPageOrientation(QPageLayout::Portrait);
        writer.setPageMargins(QMarginsF(15, 15, 15, 15), QPageLayout::Millimeter);
        writer.setResolution(300);

        QTextDocument doc;
        doc.setHtml(html);
        doc.print(&writer);

        QFile pdfFile(pdfPath);
        CMD_ASSERT(pdfFile.exists());
        CMD_ASSERT(pdfFile.size() > 1024);
        CMD_ASSERT(pdfFile.open(QIODevice::ReadOnly));
        const QByteArray header = pdfFile.read(5);
        CMD_ASSERT(header == QByteArray("%PDF-"));
    });

    return runner.runAll();
}
