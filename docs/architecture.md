# Arquitetura de Software / Software Architecture — Concrete Mix Design C++

[Português (Brasil)](#português-brasil) | [English (United States)](#english-united-states)

---

## Português (Brasil)

### 1. Visão Geral da Arquitetura em Camadas

O **Concrete Mix Design C++ (v2026.11.11.0)** adota uma arquitetura modular estritamente desacoplada em três camadas principais sobre o padrão **ISO C++20** e o framework **Qt 6.11.1**:

1. **Camada de Domínio, Cálculo Analítico e Banco de Dados Normativo (`source/core/`):**
   Implementa toda a física, estatística, regressões numéricas (Leis de Abrams, Lyse e Molinari), otimização de empacotamento granular, sequência experimental de teor de argamassa ($\alpha$), fechamento volumétrico de $1\text{ m}^3$, dimensionamento de amassadas e padiolas, geração de relatórios (Markdown/HTML) e o banco de dados relacional **SQLite 3 embarcado em memória (`:memory:`)** (concebido exclusivamente para fins de estudo acadêmico, exigindo conferência manual obrigatória e validação em ferramentas consolidadas no mercado antes de qualquer decisão em produção).
2. **Camada de Internacionalização Dinâmica (`source/language/`):**
   Gerencia o carregamento em tempo de execução dos catálogos binários compilados (`.qm`) para Português (`pt_BR`) e Inglês (`en_US`) via `GerenciadorTraducao` (Singleton reativo conectado ao evento `QEvent::LanguageChange`), acionado pelo submenu `Configurações > Idioma`.
3. **Camada de Interface Gráfica do Usuário e Renderização Vetorial (`source/gui/`):**
   Orquestra a janela principal (`MainWindow`), o menu **Configurações (`Ctrl+I`)** com o seletor dos 6 cenários de engenharia (`Cenário 0` a `Cenário 5`) e o submenu **Idioma**, as 7 abas de trabalho especializadas, o renderizador vetorial de 4 quadrantes (`QuadChartWidget`), o 5º diagrama unificado (`IptUnifiedDiagramWidget`) e os diálogos institucionais modeless (`ManualDialog` e `SobreDialog`).
4. **Camada de Interface de Linha de Comando — CLI (`source/cli/`):**
   Utilitário de console standalone (`concrete-mix-designer-cli.exe`) que orquestra diretamente o motor de cálculo (`source/core/`) sem dependências gráficas (`QtWidgets`), oferecendo comandos automatizados para dosagem, cálculo, auditoria normativa, consulta ao banco de normas e emissão de laudos técnicos em múltiplos formatos.

---

### 2. Módulos da Camada Core (`source/core/`)

| Módulo (`hpp` / `cpp`) | Responsabilidade Técnica e Científica |
| :--- | :--- |
| [`NormativeDatabase`](../source/core/NormativeDatabase.hpp) | Inicializa e consulta o banco SQLite 3 em memória (`:memory:`) com as tabelas normativas das normas **ABNT NBR 12655:2022**, **NBR 6118:2023**, **NBR 7211:2022**, **NBR 7212:2025** e **NBR 8953:2015**, com sincronização multithreading via `QRecursiveMutex`. |
| [`ConcreteStandards`](../source/core/ConcreteStandards.hpp) | Calcula o desvio-padrão $S_d$ (Condições A, B, C, histórico ou personalizado), a resistência de dosagem $f_{cj} = f_{ck} + 1,65 \cdot S_d$, os limites de durabilidade por CAA (I a IV) e a validação de conformidade normativa (`ComplianceCheck`). |
| [`AggregateOptimizer`](../source/core/AggregateOptimizer.hpp) | Calcula as 11 composições experimentais de mistura binária de Brita 2 e Brita 1 ($0\%$ a $100\%$) e determina o ponto de máximo empacotamento granular (NBR 7211 / NM 45). |
| [`MortarContentOptimizer`](../source/core/MortarContentOptimizer.hpp) | Simula a sequência laboratorial de adições de areia e cimento para teores de argamassa $\alpha$ de $40\%$ a $68\%$ (passo de $2\%$) mantendo massa fixa de brita na betoneira. |
| [`RegressionEngine`](../source/core/RegressionEngine.hpp) | Executa regressões por mínimos quadrados ordinários (OLS) em dupla precisão (`double`) para a **Lei de Abrams** ($f_{cj} = k_1 / k_2^{(a/c)}$), **Lei de Lyse** ($m = k_3 + k_4 \cdot (a/c)$) e **Lei de Molinari** ($C = 1000 / (k_5 + k_6 \cdot m)$), com proteções numéricas contra divisão por zero. |
| [`MixDesignCalculator`](../source/core/MixDesignCalculator.hpp) | Calcula o traço unitário seco e úmido, o consumo de materiais por $1\text{ m}^3$, a amassada da betoneira de laboratório, as padiolas de canteiro ($35 \times 45\text{ cm}$) para 1 saco de $50\text{ kg}$ de cimento e a análise comparativa de custos (Analítico vs. Gráfico). |
| [`ProjectData`](../source/core/ProjectData.hpp) | Agrega o estado completo do estudo, gerencia os 6 cenários pré-configurados (`Cenários 0 a 5`), resolve os metadados dinâmicos/manuais do cabeçalho (`Projeto:`, `Responsável:`, `Instituição:`, `Matrícula / ID:`), adapta dinamicamente o símbolo de moeda (R$ / $), serializa em JSON e gera o relatório técnico bilíngue em Markdown e HTML/PDF. |

---

### 3. Módulos da Camada GUI (`source/gui/`)

- [`MainWindow`](../source/gui/MainWindow.hpp): Janela principal com barra de menus (`Arquivo`, `Exibir`, `Configurações` — integrando o submenu `Configurações > Idioma` e o seletor dos 6 cenários — e `Ajuda`), diálogo modal de **Configurações — Dados do Projeto e Identificação** (persistido via `QSettings`) e barra de status normativa.
- [`TabProjectParams`](../source/gui/TabProjectParams.hpp): **Aba 1 (`1. Parâmetros de Projeto`)**.
- [`TabMaterials`](../source/gui/TabMaterials.hpp): **Aba 2 (`2. Materiais & Britas`)**.
- [`TabMortarContent`](../source/gui/TabMortarContent.hpp): **Aba 3 (`3. Teor de Argamassa (α)`)**.
- [`TabExperimentalMixes`](../source/gui/TabExperimentalMixes.hpp), [`QuadChartWidget`](../source/gui/QuadChartWidget.hpp) e [`IptUnifiedDiagramWidget`](../source/gui/IptUnifiedDiagramWidget.hpp): **Aba 4 (`4. Traços & Diagrama IPT`)**, fornecendo os quadrantes individuais, o diagrama de 4 quadrantes e a 5ª vista unificada (Abrams, Lyse e Molinari combinados com réguas limpas e sem sobreposições).
- [`TabFinalMix`](../source/gui/TabFinalMix.hpp): **Aba 5 (`5. Traço Final & Amassadas`)**.
- [`TabCosts`](../source/gui/TabCosts.hpp): **Aba 6 (`6. Análise de Custos`)**.
- [`TabTechnicalReport`](../source/gui/TabTechnicalReport.hpp): **Aba 7 (`7. Relatório Técnico`)**.
- [`ManualDialog`](../source/gui/ManualDialog.hpp) e [`SobreDialog`](../source/gui/SobreDialog.hpp): Diálogos institucionais de documentação integrada e conformidade legal Microsoft Store / GitHub.

---

### 4. Camada de Linha de Comando (`source/cli/`)

- [`main_cli.cpp`](../source/cli/main_cli.cpp): Implementa o analisador de opções e comandos `design`, `calc`, `verify`, `report`, `standards`, `presets` e `test`. Utiliza saída formatada colorida (ANSI), suporte bilíngue (`--lang pt|en`), temas claro e escuro (`--dark`) e códigos de saída padronizados (0 para sucesso, 1 para erro de entrada, 2 para não conformidade normativa).

---

## English (United States)

### 1. Layered Architecture Overview

**Concrete Mix Design C++ (v2026.11.11.0)** adopts a strictly decoupled three-tier modular architecture built on the **ISO C++20** standard and the **Qt 6.11.1** framework:

1. **Domain, Analytical Calculation, and Normative Database Layer (`source/core/`):**
   Implements all physics, statistics, numerical regressions (Abrams', Lyse's, and Molinari's Laws), granular packing optimization, pilot mortar content ($\alpha$) experimental sequence, $1\text{ m}^3$ volumetric closure, batch and volume-box (*padiola*) sizing, report generation (Markdown/HTML), and the **embedded in-memory SQLite 3 relational database (`:memory:`)** (conceived exclusively for academic study purposes, requiring mandatory manual verification and validation in consolidated market tools prior to any production decision).
2. **Dynamic Internationalization Layer (`source/language/`):**
   Manages runtime loading of compiled binary translation catalogs (`.qm`) for Portuguese (`pt_BR`) and English (`en_US`) via `GerenciadorTraducao` (reactive Singleton bound to `QEvent::LanguageChange`), triggered by the `Settings > Language` submenu.
3. **Graphical User Interface and Vector Rendering Layer (`source/gui/`):**
   Orchestrates the main window (`MainWindow`), the **Settings (`Ctrl+I`)** menu featuring the 6 engineering scenarios (`Scenario 0` to `Scenario 5`) and the **Language** submenu, the 7 specialized workspace tabs, the 4-quadrant vector chart renderer (`QuadChartWidget`), the 5th unified diagram widget (`IptUnifiedDiagramWidget`), and the institutional modeless dialogs (`ManualDialog` and `SobreDialog`).
4. **Command Line Interface Layer — CLI (`source/cli/`):**
   Standalone console utility (`concrete-mix-designer-cli.exe`) that directly orchestrates the calculation engine (`source/core/`) without GUI dependencies (`QtWidgets`), providing automated mix design, volumetric closure, normative auditing, standards database queries, and multi-format report exports.

---

### 2. Core Layer Modules (`source/core/`)

| Module (`hpp` / `cpp`) | Technical and Scientific Responsibility |
| :--- | :--- |
| [`NormativeDatabase`](../source/core/NormativeDatabase.hpp) | Initializes and queries the in-memory SQLite 3 database (`:memory:`) containing normative tables from **ABNT NBR 12655:2022**, **NBR 6118:2023**, **NBR 7211:2022**, **NBR 7212:2025**, and **NBR 8953:2015**, guarded by multithreaded synchronization (`QRecursiveMutex`). |
| [`ConcreteStandards`](../source/core/ConcreteStandards.hpp) | Computes standard deviation $S_d$ (Conditions A, B, C, historical, or custom), target mix design strength $f_{cj} = f_{ck} + 1.65 \cdot S_d$, durability limits by CAA (I to IV), and normative compliance checks (`ComplianceCheck`). |
| [`AggregateOptimizer`](../source/core/AggregateOptimizer.hpp) | Computes the 11 experimental binary blend compositions of Coarse Aggregate 2 and Coarse Aggregate 1 ($0\%$ to $100\%$) and identifies the maximum granular packing point (NBR 7211 / NM 45). |
| [`MortarContentOptimizer`](../source/core/MortarContentOptimizer.hpp) | Simulates the laboratory sequence of sand and cement additions for dry mortar contents $\alpha$ from $40\%$ to $68\%$ ($2\%$ step) while keeping a fixed coarse aggregate mass in the mixer. |
| [`RegressionEngine`](../source/core/RegressionEngine.hpp) | Performs double-precision (`double`) ordinary least squares (OLS) regressions for **Abrams' Law** ($f_{cj} = k_1 / k_2^{(w/c)}$), **Lyse's Law** ($m = k_3 + k_4 \cdot (w/c)$), and **Molinari's Law** ($C = 1000 / (k_5 + k_6 \cdot m)$), computing $R^2$ coefficients with zero-division protections. |
| [`MixDesignCalculator`](../source/core/MixDesignCalculator.hpp) | Computes dry and wet unit mixes, material consumption per $1\text{ m}^3$, laboratory mixer batch masses, jobsite volume boxes (*padiolas*, $35 \times 45\text{ cm}$) per $50\text{ kg}$ cement bag, and comparative cost analysis (Analytical vs. Graphical). |
| [`ProjectData`](../source/core/ProjectData.hpp) | Aggregates the complete study state, manages the 6 pre-configured scenarios (`Scenarios 0 to 5`), resolves dynamic/manual header metadata (`Project:`, `Engineer:`, `Institution:`, `Registration / ID:`), adapts the currency symbol dynamically ($ / R$), serializes to JSON, and generates the bilingual technical report in Markdown and HTML/PDF. |

---

### 3. GUI Layer Modules (`source/gui/`)

- [`MainWindow`](../source/gui/MainWindow.hpp): Main application window featuring the menu bar (`File`, `View`, `Settings` — integrating the `Settings > Language` submenu and the 6-scenario selector — and `Help`), the **Settings — Project Data and Identification** modal dialog (persisted via `QSettings`), and the normative status bar.
- [`TabProjectParams`](../source/gui/TabProjectParams.hpp): **Tab 1 (`1. Project Parameters`)**.
- [`TabMaterials`](../source/gui/TabMaterials.hpp): **Tab 2 (`2. Materials & Coarse Aggregates`)**.
- [`TabMortarContent`](../source/gui/TabMortarContent.hpp): **Tab 3 (`3. Mortar Content (α)`)**.
- [`TabExperimentalMixes`](../source/gui/TabExperimentalMixes.hpp), [`QuadChartWidget`](../source/gui/QuadChartWidget.hpp), and [`IptUnifiedDiagramWidget`](../source/gui/IptUnifiedDiagramWidget.hpp): **Tab 4 (`4. Mixes & IPT Diagram`)**, providing individual quadrants, the 4-quadrant diagram, and the 5th unified view combining Abrams, Lyse, and Molinari curves with high-contrast grid lines.
- [`TabFinalMix`](../source/gui/TabFinalMix.hpp): **Tab 5 (`5. Final Mix & Batches`)**.
- [`TabCosts`](../source/gui/TabCosts.hpp): **Tab 6 (`6. Cost Analysis`)**.
- [`TabTechnicalReport`](../source/gui/TabTechnicalReport.hpp): **Tab 7 (`7. Technical Report`)**.
- [`ManualDialog`](../source/gui/ManualDialog.hpp) and [`SobreDialog`](../source/gui/SobreDialog.hpp): Built-in institutional documentation and Microsoft Store / GitHub legal compliance dialogs.

---

### 4. CLI Layer Modules (`source/cli/`)

- [`main_cli.cpp`](../source/cli/main_cli.cpp): Command line parser and dispatcher for `design`, `calc`, `verify`, `report`, `standards`, `presets`, and `test` commands. Supports ANSI color formatting, bilingual output (`--lang pt|en`), dark theme styling (`--dark`), and standardized exit codes (0 for success, 1 for input error, 2 for normative non-compliance).
