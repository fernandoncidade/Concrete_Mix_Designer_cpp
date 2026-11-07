#include "ManualDialog.hpp"
#include "source/language/GerenciadorTraducao.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>
#include <QSplitter>
#include <QListWidget>
#include <QTextBrowser>
#include <QPushButton>
#include <QIcon>
#include <QFile>
#include <QCoreApplication>
#include <QTextCursor>
#include <QTextDocument>
#include <QScrollBar>
#include <QEvent>
#include <QPalette>
#include <QUrl>

namespace ConcreteMix::Gui {

ManualDialog::ManualDialog(QWidget* parent)
    : QDialog(parent)
{
    setupUi();
    retranslateUi();
}

void ManualDialog::setupUi()
{
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint |
                   Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    setWindowModality(Qt::NonModal);
    setModal(false);
    setAttribute(Qt::WA_DeleteOnClose, true);
    resize(950, 680);
    setMinimumSize(750, 500);

    QIcon appIcon;
    const QStringList iconPaths = {
        QCoreApplication::applicationDirPath() + "/assets/icons/concrete_mix_design.ico",
        QCoreApplication::applicationDirPath() + "/source/assets/icons/concrete_mix_design.ico",
        "source/assets/icons/concrete_mix_design.ico",
        "assets/icons/concrete_mix_design.ico"
    };
    for (const auto& p : iconPaths) {
        if (QFile::exists(p)) {
            appIcon = QIcon(p);
            break;
        }
    }
    setWindowIcon(appIcon);


    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(8);

    // Search bar layout
    auto* searchLayout = new QHBoxLayout();
    searchLayout->setSpacing(6);

    searchLabel_ = new QLabel(this);
    searchLayout->addWidget(searchLabel_);

    searchEdit_ = new QLineEdit(this);
    searchEdit_->setClearButtonEnabled(true);
    searchLayout->addWidget(searchEdit_, 1);

    searchPrevBtn_ = new QToolButton(this);
    searchPrevBtn_->setArrowType(Qt::LeftArrow);
    searchPrevBtn_->setEnabled(false);
    searchLayout->addWidget(searchPrevBtn_);

    searchNextBtn_ = new QToolButton(this);
    searchNextBtn_->setArrowType(Qt::RightArrow);
    searchNextBtn_->setEnabled(false);
    searchLayout->addWidget(searchNextBtn_);

    mainLayout->addLayout(searchLayout);

    // Splitter with TOC list and Text Browser
    splitter_ = new QSplitter(Qt::Horizontal, this);

    tocList_ = new QListWidget(splitter_);
    tocList_->setMinimumWidth(220);
    tocList_->setMaximumWidth(320);

    browser_ = new QTextBrowser(splitter_);
    browser_->setOpenExternalLinks(true);
    browser_->setReadOnly(true);

    splitter_->addWidget(tocList_);
    splitter_->addWidget(browser_);
    splitter_->setStretchFactor(0, 0);
    splitter_->setStretchFactor(1, 1);

    mainLayout->addWidget(splitter_, 1);

    // Bottom close button
    auto* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch(1);

    closeButton_ = new QPushButton(this);
    closeButton_->setAutoDefault(false);
    closeButton_->setDefault(false);
    buttonLayout->addWidget(closeButton_);

    mainLayout->addLayout(buttonLayout);

    // Connections
    connect(closeButton_, &QPushButton::clicked, this, &QDialog::close);
    connect(tocList_, &QListWidget::itemClicked, this, &ManualDialog::onTocItemClicked);
    connect(searchEdit_, &QLineEdit::textChanged, this, &ManualDialog::onSearchTextChanged);
    connect(searchEdit_, &QLineEdit::returnPressed, this, &ManualDialog::onSearchNext);
    connect(searchNextBtn_, &QToolButton::clicked, this, &ManualDialog::onSearchNext);
    connect(searchPrevBtn_, &QToolButton::clicked, this, &ManualDialog::onSearchPrevious);
}

void ManualDialog::changeEvent(QEvent* event)
{
    if (event != nullptr && event->type() == QEvent::LanguageChange) {
        retranslateUi();
    } else if (event != nullptr && (event->type() == QEvent::PaletteChange ||
                                    event->type() == QEvent::ApplicationPaletteChange ||
                                    event->type() == QEvent::ThemeChange)) {
        loadManualContent();
    }
    QDialog::changeEvent(event);
}

void ManualDialog::retranslateUi()
{
    bool isEn = (Language::GerenciadorTraducao::instance().codigoIdiomaAtual() == QStringLiteral("en_US"));

    setWindowTitle(isEn ? QStringLiteral("User Manual — Concrete Mix Designer")
                        : QStringLiteral("Manual do Usuário — Módulo de Dosagem de Concreto"));

    searchLabel_->setText(isEn ? QStringLiteral("Search in Manual:") : QStringLiteral("Pesquisar no Manual:"));
    searchEdit_->setPlaceholderText(isEn ? QStringLiteral("Search topic, standard, formula...")
                                         : QStringLiteral("Pesquise por tópico, norma, fórmula..."));
    searchPrevBtn_->setToolTip(isEn ? QStringLiteral("Previous match") : QStringLiteral("Ocorrência anterior"));
    searchNextBtn_->setToolTip(isEn ? QStringLiteral("Next match") : QStringLiteral("Próxima ocorrência"));
    closeButton_->setText(isEn ? QStringLiteral("Close") : QStringLiteral("Fechar"));

    loadManualContent();
}

void ManualDialog::loadManualContent()
{
    const bool isEn = (Language::GerenciadorTraducao::instance().codigoIdiomaAtual() == QStringLiteral("en_US"));
    const bool isDark = (palette().color(QPalette::Window).lightness() < 128);
    const int savedTocRow = tocList_->currentRow();
    const int savedScroll = browser_->verticalScrollBar() ? browser_->verticalScrollBar()->value() : 0;

    tocList_->clear();
    sectionAnchors_.clear();

    struct TocEntry {
        QString title;
        QString anchor;
    };

    QVector<TocEntry> entries;
    if (isEn) {
        entries = {
            {QStringLiteral("1. Overview & Settings Menu"), QStringLiteral("sec1")},
            {QStringLiteral("2. Fundamental Empirical Laws"), QStringLiteral("sec2")},
            {QStringLiteral("3. Tab 1: Project Parameters"), QStringLiteral("sec3")},
            {QStringLiteral("4. Tab 2: Materials & Aggregate Blend"), QStringLiteral("sec4")},
            {QStringLiteral("5. Tab 3: Mortar Content (α)"), QStringLiteral("sec5")},
            {QStringLiteral("6. Tab 4: Trial Mixes & IPT Diagram"), QStringLiteral("sec6")},
            {QStringLiteral("7. Tab 5: Final Mix & Batching"), QStringLiteral("sec7")},
            {QStringLiteral("8. Tab 6: Cost Analysis"), QStringLiteral("sec8")},
            {QStringLiteral("9. Tab 7: Technical Report & Export"), QStringLiteral("sec9")},
            {QStringLiteral("10. Standards & Normative Rules"), QStringLiteral("sec10")},
            {QStringLiteral("11. Dual Compilation (MinGW / MSVC)"), QStringLiteral("sec11")},
            {QStringLiteral("12. Troubleshooting & FAQ"), QStringLiteral("sec12")},
            {QStringLiteral("13. Command Line Interface (CLI)"), QStringLiteral("sec13")}
        };
    } else {
        entries = {
            {QStringLiteral("1. Visão Geral e Menu Configurações"), QStringLiteral("sec1")},
            {QStringLiteral("2. Leis Fundamentais da Dosagem"), QStringLiteral("sec2")},
            {QStringLiteral("3. Aba 1: Parâmetros de Projeto"), QStringLiteral("sec3")},
            {QStringLiteral("4. Aba 2: Materiais e Mistura de Britas"), QStringLiteral("sec4")},
            {QStringLiteral("5. Aba 3: Teor de Argamassa (α)"), QStringLiteral("sec5")},
            {QStringLiteral("6. Aba 4: Traços e Diagrama IPT"), QStringLiteral("sec6")},
            {QStringLiteral("7. Aba 5: Traço Final e Amassadas"), QStringLiteral("sec7")},
            {QStringLiteral("8. Aba 6: Análise de Custos"), QStringLiteral("sec8")},
            {QStringLiteral("9. Aba 7: Relatório Técnico e Exportação"), QStringLiteral("sec9")},
            {QStringLiteral("10. Normas Técnicas e Critérios"), QStringLiteral("sec10")},
            {QStringLiteral("11. Compilação Dual (MinGW / MSVC)"), QStringLiteral("sec11")},
            {QStringLiteral("12. Resolução de Problemas (FAQ)"), QStringLiteral("sec12")},
            {QStringLiteral("13. Interface de Linha de Comando (CLI)"), QStringLiteral("sec13")}
        };
    }

    for (const auto& entry : entries) {
        auto* item = new QListWidgetItem(entry.title, tocList_);
        item->setData(Qt::UserRole, entry.anchor);
        sectionAnchors_[entry.title] = entry.anchor;
    }

    const QString styleSheet = isDark
        ? QStringLiteral(
            "body { font-family: sans-serif; line-height: 1.5; padding: 10px; color: #e8e8e8; background-color: #2d2d2d; }"
            "h2 { color: #65bcff; border-bottom: 2px solid #3d8bd4; padding-bottom: 4px; margin-top: 24px; }"
            "h3 { color: #82c8ff; margin-top: 16px; }"
            "code { background-color: #3c3c3c; color: #9cdcfe; padding: 2px 5px; border-radius: 3px; font-family: Consolas, monospace; }"
            "table { border-collapse: collapse; width: 100%; margin: 12px 0; }"
            "th, td { border: 1px solid #505050; padding: 6px 10px; text-align: left; color: #e8e8e8; }"
            "th { background-color: #383838; color: #ffffff; }"
          )
        : QStringLiteral(
            "body { font-family: sans-serif; line-height: 1.5; padding: 10px; color: #1a1a1a; background-color: #ffffff; }"
            "h2 { color: #005a9e; border-bottom: 2px solid #005a9e; padding-bottom: 4px; margin-top: 24px; }"
            "h3 { color: #2b579a; margin-top: 16px; }"
            "code { background-color: #eef2f6; color: #0b4f8a; padding: 2px 5px; border-radius: 3px; font-family: Consolas, monospace; }"
            "table { border-collapse: collapse; width: 100%; margin: 12px 0; }"
            "th, td { border: 1px solid #cccccc; padding: 6px 10px; text-align: left; color: #1a1a1a; }"
            "th { background-color: #e8eef4; color: #1a1a1a; }"
          );

    QString html;
    if (isEn) {
        html = QStringLiteral("<html><head><style>") + styleSheet + QStringLiteral("</style></head><body>"

            "<h2 id='sec1'>1. Overview &amp; Settings Menu</h2>"
            "<p><b>Concrete Mix Designer</b> is an analytical civil engineering platform implementing the "
            "celebrated <b>IPT / EPUSP Method (Helene &amp; Terzian, 1992)</b> for experimental structural concrete proportioning. "
            "Use the <b>Settings (&amp;Configurações)</b> menu in the top menu bar to select the <b>Spreadsheet Scenario</b> "
            "(from the dropdown list) and configure header identification fields (<code>Ctrl+I</code>: "
            "<b>Project:</b>, <b>Responsible:</b>, <b>Institution:</b>, and <b>ID / Code:</b>), choosing whether <b>Project:</b> "
            "automatically reports the selected <b>Spreadsheet Scenario</b> (with dynamic runtime translation) or uses manual custom text. "
            "Auxiliary windows (Manual, Technical Standards, About) operate as <b>Modeless Dialogs</b> with independent <b>Z-Order</b> alongside the main GUI.</p>"

            "<h2 id='sec2'>2. Fundamental Empirical Laws</h2>"
            "<ul>"
            "<li><b>Abrams' Law (1st Quadrant):</b> <code>fcj = k1 / (k2 ^ (w/c))</code> and 2nd-degree polynomial regression — Compressive strength as a decreasing function of the water/cement ratio.</li>"
            "<li><b>Lyse's Law (2nd Quadrant):</b> <code>m = k3 + k4 * (w/c)</code> — Linear relationship between total dry aggregate/cement ratio (m) and water/cement ratio for a fixed slump.</li>"
            "<li><b>Molinari's Law (3rd Quadrant):</b> <code>C = 1000 / (1/dc + a/da + p/db + (w/c)/1.0)</code> — Cement consumption per cubic meter of compacted fresh concrete.</li>"
            "</ul>"

            "<h2 id='sec3'>3. Tab 1: Project Parameters</h2>"
            "<p>Define characteristic compressive strength (<b>fck</b>, e.g., 20 to 100 MPa), preparation condition (Condition A: Sd=4.0 MPa, B: Sd=5.5 MPa for C10–C20, C: Sd=7.0 MPa for C10–C15, Known Historical Sd ≥ 2.0 MPa, or Laboratory/Arbitrary per ABNT NBR 12655:2022), "
            "target dosage strength <code>fcj = fck + 1.65 * Sd</code>, target slump (NBR 8953:2015), structural type (Reinforced Concrete CA or Prestressed Concrete CP, with +5 mm cover per NBR 6118:2023 Table 7.2), and Environmental Aggressiveness Class (CAA I to IV).</p>"

            "<h2 id='sec4'>4. Tab 2: Materials &amp; Aggregate Blend</h2>"
            "<p>Register specific mass (kg/dm³), bulk unit mass (kg/dm³), field surface moisture (h%), and unit costs for Portland cement ($/bag, $/t, or $/kg), fine aggregate (sand), and coarse aggregates (Gravel 1 &amp; Gravel 2). "
            "Includes the binary coarse aggregate packing optimization table (NBR 7211:2022 / NBR NM 45) to determine the maximum compacted bulk unit mass (e.g., 70% Gravel 2 / 30% Gravel 1).</p>"

            "<h2 id='sec5'>5. Tab 3: Mortar Content (α)</h2>"
            "<p>Simulates the experimental pilot mix procedure (<code>m = 5.0</code>) varying dry mortar content <code>α = (1 + a) / (1 + m)</code> from 40% to 68% in 2% steps, keeping coarse aggregate mass fixed in the mixer.</p>"

            "<h2 id='sec6'>6. Tab 4: Trial Mixes &amp; IPT Diagrams</h2>"
            "<p>Enter data from the 3 experimental trial mixes (Rich, Pilot/Intermediate, and Lean) tested at the target slump. "
            "The interactive diagram selector provides 6 comprehensive visualization modes:</p>"
            "<ul>"
            "<li><b>Complete 4-Quadrant Diagram (IPT):</b> Multi-quadrant overview with coordinate axes.</li>"
            "<li><b>1st Quad: Abrams (fcj x w/c):</b> Hyperbolic curve fitting with normative durability threshold lines.</li>"
            "<li><b>2nd Quad: Lyse (m x w/c):</b> Linear regression of aggregate-to-cement ratio at constant slump.</li>"
            "<li><b>3rd Quad: Molinari (m x C):</b> Hyperbolic cement consumption curve.</li>"
            "<li><b>4th Quad: Consumption &amp; Cost:</b> Concrete cost per m³ vs. compressive strength.</li>"
            "<li><b>5th Abrams/Lyse/Molinari (IPT):</b> Unified Cartesian composition strictly matching the official ABNT NBR standard layout with clean non-gradient background, high-density axis grid lines, and high-resolution export.</li>"
            "</ul>"

            "<h2 id='sec7'>7. Tab 5: Final Mix &amp; Batching</h2>"
            "<p>Computes the unit dry proportion <code>1 : a : p</code>, moisture-corrected wet proportion <code>1 : a_wet : p_wet</code>, material quantities for 1 m³ of concrete, "
            "laboratory mixer batch weights, and jobsite wooden measuring boxes ('padiolas', 35 cm × 45 cm base) for a 50 kg bag of cement.</p>"

            "<h2 id='sec8'>8. Tab 6: Cost Analysis</h2>"
            "<p>Compares exact analytical regression solutions against manual graphical chart readings, computing cost per m³ ($/m³), total project cost, and 50 kg cement bag differences.</p>"

            "<h2 id='sec9'>9. Tab 7: Technical Report &amp; Export</h2>"
            "<p>Generates a complete, dynamically translated engineering calculation report with header metadata, equations, mix proportions, cost tables, and normative verification. "
            "Automatically switches currency symbols between Brazilian Real (<b>R$</b>) and US Dollar (<b>$</b>) depending on selected language. "
            "Supports clipboard copy and export to PDF, HTML, and Markdown.</p>"

            "<h2 id='sec10'>10. Standards &amp; Normative Rules</h2>"
            "<p>Enforces strict compliance with: <b>ABNT NBR 12655:2022</b>, <b>ABNT NBR 6118:2023</b>, <b>ABNT NBR 7211:2022</b>, <b>ABNT NBR 7212:2025</b> (central plant weighing tolerances item 5.5: cement ±1%, aggregates ±3%, water ±3%, admixtures ±5%), and <b>ABNT NBR 8953:2015</b>.</p>"

            "<h2 id='sec11'>11. Dual Compilation (MinGW / MSVC)</h2>"
            "<p>Supports compilation on both <b>MinGW 64-bit</b> and <b>MSVC 64-bit</b>. Automated <code>windeployqt</code> deployment co-locates all required Qt6 DLLs to ensure standalone execution.</p>"

            "<h2 id='sec12'>12. Troubleshooting &amp; FAQ</h2>"
            "<p><b>Curve fitting warning?</b> Ensure at least 3 experimental mixes are entered with monotonically decreasing strength as w/c increases.<br>"
            "<b>How to customize report header or change scenario?</b> Open the <b>Settings (&amp;Configurações)</b> menu to select the <i>Spreadsheet Scenario</i> or click <i>Project Data &amp; Identification (Ctrl+I)</i>.</p>"

            "<h2 id='sec13'>13. Command Line Interface (CLI)</h2>"
            "<p>The project includes <code>concrete-mix-designer-cli.exe</code> for terminal automation, batch simulations, and CI/CD pipelines:</p>"
            "<ul>"
            "<li><code>design</code>: Automated mix design from fck, slump, and environmental class.</li>"
            "<li><code>calc</code>: Analytical volumetric closure for given m and w/c.</li>"
            "<li><code>verify</code>: Compliance audit against ABNT NBR 12655, 6118, 8953.</li>"
            "<li><code>report</code>: Generate technical reports (<code>--format text|json|markdown|html</code>).</li>"
            "<li><code>standards</code>: Query normative SQLite tables for cements, aggregates, slump.</li>"
            "<li><code>presets</code>: Run built-in standard industry mix scenarios (0 to 5).</li>"
            "<li><code>test</code>: Execute internal calculation and normative regression self-test.</li>"
            "</ul>"

            "</body></html>"
        );
    } else {
        html = QStringLiteral("<html><head><style>") + styleSheet + QStringLiteral("</style></head><body>"

            "<h2 id='sec1'>1. Visão Geral e Menu Configurações</h2>"
            "<p>O <b>Módulo de Dosagem de Concreto</b> é uma plataforma de engenharia civil de alta precisão dedicada à dosagem experimental de concreto estrutural "
            "pelo consagrado <b>Método IPT / EPUSP (Helene &amp; Terzian, 1992)</b>. "
            "Utilize o menu <b>Configurações</b> na barra de menus superior para selecionar o <b>Cenário da Planilha:</b> "
            "(na lista suspensa associada) e configurar os dados de identificação do cabeçalho (<code>Ctrl+I</code>: "
            "<b>Projeto:</b>, <b>Responsável:</b>, <b>Instituição:</b> e <b>Matrícula / ID:</b>), escolhendo se o campo <b>Projeto:</b> "
            "deve reportar automaticamente o <b>Cenário da Planilha</b> selecionado (com tradução dinâmica em tempo de execução) ou ser editado manualmente. "
            "As janelas auxiliares (Manual, Normas Técnicas, Sobre) operam como <b>Janelas Modeless</b> com <b>Z-Order (Ordem no Eixo Z)</b> independente junto à GUI principal.</p>"

            "<h2 id='sec2'>2. Leis Fundamentais da Dosagem</h2>"
            "<ul>"
            "<li><b>Lei de Abrams (1º Quadrante):</b> <code>fcj = k1 / (k2 ^ (a/c))</code> e regressão polinomial de 2º grau — Resistência à compressão em função da relação água/cimento (a/c).</li>"
            "<li><b>Lei de Lyse (2º Quadrante):</b> <code>m = k3 + k4 * (a/c)</code> — Relação agregados secos/cimento (m) em função da relação água/cimento para abatimento constante.</li>"
            "<li><b>Lei de Molinari (3º Quadrante):</b> <code>C = 1000 / (1/dc + a/da + p/db + (a/c)/1,0)</code> — Consumo de cimento por metro cúbico de concreto fresco adensado.</li>"
            "</ul>"

            "<h2 id='sec3'>3. Aba 1: Parâmetros de Projeto</h2>"
            "<p>Configuração da resistência característica (<b>fck</b>, de 20 a 100 MPa), condição de preparo (Condição A: Sd=4,0 MPa, Condição B: Sd=5,5 MPa para C10–C20, Condição C: Sd=7,0 MPa para C10–C15, Desvio Histórico Conhecido Sd ≥ 2,0 MPa ou Laboratório/Arbitrário conforme ABNT NBR 12655:2022), "
            "resistência de dosagem <code>fcj = fck + 1,65 * Sd</code>, abatimento alvo (NBR 8953:2015), tipo estrutural (Concreto Armado CA ou Concreto Protendido CP, com acréscimo de +5 mm no cobrimento pela NBR 6118:2023 Tabela 7.2) e Classe de Agressividade Ambiental (CAA I a IV).</p>"

            "<h2 id='sec4'>4. Aba 2: Materiais e Mistura de Britas</h2>"
            "<p>Cadastro das propriedades físicas: massa específica real (kg/dm³), massa unitária aparente (kg/dm³), umidade superficial de campo (h%) e custos unitários para cimento Portland (R$/saco, R$/t ou R$/kg), areia e britas 1 e 2. "
            "Inclui a tabela de otimização da mistura binária de britas (NBR 7211:2022 / NBR NM 45) para determinação da máxima massa unitária compactada (ex.: 70% Brita 2 / 30% Brita 1).</p>"

            "<h2 id='sec5'>5. Aba 3: Teor de Argamassa (α)</h2>"
            "<p>Simula o ensaio experimental do traço piloto (<code>m = 5,0</code>) variando o teor de argamassa seca <code>α = (1 + a) / (1 + m)</code> de 40% a 68% em passos de 2%, mantendo a massa de brita constante na betoneira.</p>"

            "<h2 id='sec6'>6. Aba 4: Traços e Diagramas IPT</h2>"
            "<p>Inserção dos dados dos 3 traços experimentais (Rico, Piloto/Médio e Pobre) ensaiados no abatimento alvo. "
            "O seletor de gráficos interativo disponibiliza 6 modos de visualização:</p>"
            "<ul>"
            "<li><b>Diagrama Completo de 4 Quadrantes (IPT):</b> Visão geral multi-quadrante integrada.</li>"
            "<li><b>1º Quad: Abrams (fcj x a/c):</b> Curva hiperbólica com demarcação do limite normativo de durabilidade.</li>"
            "<li><b>2º Quad: Lyse (m x a/c):</b> Regressão linear da proporção aglomerante/agregados a abatimento constante.</li>"
            "<li><b>3º Quad: Molinari (m x C):</b> Curva hiperbólica de consumo de cimento por metro cúbico.</li>"
            "<li><b>4º Quad: Consumo e Custo:</b> Correlação de custo por m³ com a resistência à compressão.</li>"
            "<li><b>5° Abrams/Lyse/Molinari (IPT):</b> Composição cartesiana unificada dos 4 quadrantes clássicos do IPT em estrita conformidade com o padrão visual das NBRs, com fundo sólido limpo sem gradientes, linhas de grade de alta densidade e exportação de alta resolução.</li>"
            "</ul>"

            "<h2 id='sec7'>7. Aba 5: Traço Final e Amassadas</h2>"
            "<p>Apresenta o traço unitário seco <code>1 : a : p</code>, o traço úmido corrigido <code>1 : a_um : p_um</code>, o consumo de materiais para 1 m³ de concreto, "
            "a receita da amassada para o misturador de laboratório e o dimensionamento de padiolas de madeira (base 35 cm × 45 cm) para 1 saco de 50 kg de cimento.</p>"

            "<h2 id='sec8'>8. Aba 6: Análise de Custos</h2>"
            "<p>Compara a solução analítica exata das equações de regressão com a leitura manual em ábaco gráfico, calculando o custo por m³ (R$/m³), o custo total da obra e a diferença em sacos de cimento de 50 kg.</p>"

            "<h2 id='sec9'>9. Aba 7: Relatório Técnico e Exportação</h2>"
            "<p>Gera o memorial de cálculo completo com tradução dinâmica em tempo de execução, incluindo cabeçalho de identificação, equações ajustadas, consumos, custos e verificação normativa. "
            "Conta com alternância dinâmica do símbolo da moeda (<b>R$</b> em português e <b>$</b> em inglês) em todas as tabelas econômicas. "
            "Permite copiar ou exportar em PDF, HTML e Markdown.</p>"

            "<h2 id='sec10'>10. Normas Técnicas e Critérios</h2>"
            "<p>Atendimento estrito às normas da ABNT: <b>NBR 12655:2022</b>, <b>NBR 6118:2023</b>, <b>NBR 7211:2022</b>, <b>NBR 7212:2025</b> (tolerâncias de pesagem em central item 5.5: cimento ±1%, agregados ±3%, água ±3%, aditivos ±5%) e <b>NBR 8953:2015</b>.</p>"

            "<h2 id='sec11'>11. Compilação Dual (MinGW / MSVC)</h2>"
            "<p>Suporte completo a builds em <b>MinGW 64-bit</b> e <b>MSVC 64-bit</b>. Automação com <code>windeployqt</code> garante que todas as DLLs do Qt6 acompanhem os binários.</p>"

            "<h2 id='sec12'>12. Resolução de Problemas (FAQ)</h2>"
            "<p><b>Alerta no ajuste de curva?</b> Certifique-se de que inseriu 3 traços experimentais com relação a/c crescente e resistência decrescente.<br>"
            "<b>Como trocar o cenário da planilha ou personalizar o cabeçalho do relatório?</b> Acesse o menu <b>Configurações</b> para escolher o <i>Cenário da Planilha</i> ou clique em <i>Dados do Projeto e Identificação (Ctrl+I)</i>.</p>"

            "<h2 id='sec13'>13. Interface de Linha de Comando (CLI)</h2>"
            "<p>O projeto acompanha o executável de console <code>concrete-mix-designer-cli.exe</code> para automações em terminal, simulações em lote e esteiras CI/CD:</p>"
            "<ul>"
            "<li><code>design</code>: Dosagem automatizada a partir de fck, slump e classe ambiental.</li>"
            "<li><code>calc</code>: Fechamento volumétrico analítico para m e a/c fornecidos.</li>"
            "<li><code>verify</code>: Auditoria de conformidade com as normas ABNT NBR 12655, 6118, 8953.</li>"
            "<li><code>report</code>: Emissão de relatórios técnicos (<code>--format text|json|markdown|html</code>).</li>"
            "<li><code>standards</code>: Consulta a tabelas normativas SQLite de cimentos, agregados, slump.</li>"
            "<li><code>presets</code>: Execução dos cenários de referência padronizados da indústria (0 a 5).</li>"
            "<li><code>test</code>: Bateria interna de autodiagnóstico de regressões analíticas e conformidade.</li>"
            "</ul>"

            "</body></html>"
        );
    }

    browser_->setHtml(html);
    if (savedTocRow >= 0 && savedTocRow < tocList_->count()) {
        tocList_->setCurrentRow(savedTocRow);
    }
    if (browser_->verticalScrollBar() && savedScroll > 0) {
        browser_->verticalScrollBar()->setValue(savedScroll);
    }
}

void ManualDialog::onTocItemClicked(QListWidgetItem* item)
{
    if (!item) return;

    QString anchor = item->data(Qt::UserRole).toString();
    if (!anchor.isEmpty()) {
        browser_->scrollToAnchor(anchor);
    }
}

void ManualDialog::onSearchTextChanged(const QString& text)
{
    bool hasText = !text.trimmed().isEmpty();
    searchPrevBtn_->setEnabled(hasText);
    searchNextBtn_->setEnabled(hasText);

    if (hasText) {
        performSearch(1);
    }
}

void ManualDialog::onSearchNext()
{
    performSearch(1);
}

void ManualDialog::onSearchPrevious()
{
    performSearch(-1);
}

void ManualDialog::performSearch(int direction)
{
    QString text = searchEdit_->text().trimmed();
    if (text.isEmpty()) return;

    QTextDocument::FindFlags flags;
    if (direction < 0) {
        flags |= QTextDocument::FindBackward;
    }

    bool found = browser_->find(text, flags);
    if (!found) {
        QTextCursor cursor = browser_->textCursor();
        if (direction < 0) {
            cursor.movePosition(QTextCursor::End);
        } else {
            cursor.movePosition(QTextCursor::Start);
        }
        browser_->setTextCursor(cursor);
        browser_->find(text, flags);
    }
}

} // namespace ConcreteMix::Gui
