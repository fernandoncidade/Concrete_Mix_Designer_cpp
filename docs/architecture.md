# Arquitetura do Sistema / System Architecture — Concrete Mix Designer

<p align="center">
  <b>Selecione o idioma / Select language:</b><br>
  <a href="#ptbr">🇧🇷 Português (BR)</a> | <a href="#enus">🇺🇸 English (US)</a>
</p>

---

## <a id="ptbr"></a>🇧🇷 Português (BR)

<details open>
<summary>Clique para expandir / recolher a arquitetura em português</summary><br>

# Concrete Mix Designer — Arquitetura de Sistema e Especificações Técnicas

**Versão:** `2026.11.22.0`  
**Data:** `22 de novembro de 2026`  
**Padrão:** C++23 | Qt 6.12.0 | SQLite 3 (`:memory:`) | Qt Charts  
**Plataforma Alvo:** Windows 11 / Windows 10 x64 (MSVC 14.52 + Windows SDK 10.0.28000.0 e MinGW-w64 GCC 15.1.0)  
**Autor:** Fernando Nillsson Cidade  

---

## 1. Filosofia Arquitetural e Visão Geral em Camadas

O **Concrete Mix Designer** (*Módulo de Dosagem de Concreto*) adota uma arquitetura modular estritamente desacoplada sobre **ISO C++23** e **Qt 6.12.0**, separando em bibliotecas estáticas reutilizáveis o núcleo analítico/normativo (`ConcreteMixCoreLib`), a interface gráfica e plotagem vetorial (`ConcreteMixGuiLib`), o aplicativo desktop (`ConcreteMixDesign.exe`), a ferramenta autônoma de linha de comando (`concrete-mix-designer-cli.exe`), os utilitários de manutenção (`mocks/`) e a suíte modular de engenharia de testes (`tests/`):

```text
+-------------------------------------------------------------------------+
|                        CAMADA DE APRESENTAÇÃO / CLI                     |
|     ConcreteMixDesign.exe (Qt 6.12.0)  |  concrete-mix-designer-cli.exe |
|  MainWindow | 7 Abas Especializadas | ManualDialog | SobreDialog (Z)    |
|  QuadChartWidget (4 Quadrantes) | IptUnifiedDiagramWidget (5º Gráfico)  |
+------------------------------------+------------------------------------+
                                     |
                                     v
+-------------------------------------------------------------------------+
|               BIBLIOTECA DE INTERFACE (ConcreteMixGuiLib)               |
|  TabProjectParams | TabMaterials | TabMortarContent | TabExperimental   |
|  TabFinalMix | TabCosts | TabTechnicalReport (PDF A4 / HTML5 / MD)      |
+------------------------------------+------------------------------------+
                                     |
                                     v
+-------------------------------------------------------------------------+
|                NÚCLEO ANALÍTICO & I18N (ConcreteMixCoreLib)             |
|  +-------------------------------+  +---------------------------------+ |
|  | Motor Matemático & Normativo  |  | Persistência, I18N & Utilitários| |
|  | - ConcreteStandards (ABNT NBR)|  | - ProjectData (.cmdproj/Reports)| |
|  | - AggregateOptimizer (NM 45)  |  | - NormativeDatabase (:memory:)  | |
|  | - MortarContentOptimizer (α)  |  | - GerenciadorTraducao (.ts/.qm) | |
|  | - RegressionEngine (OLS/Exp)  |  | - LogManager & PathUtils        | |
|  | - MixDesignCalculator         |  |                                 | |
|  +-------------------------------+  +---------------------------------+ |
+-------------------------------------------------------------------------+
```

---

## 2. Módulos da Camada Core (`source/core/`, `source/language/`, `source/utils/`)

| Módulo (`hpp` / `cpp`) | Responsabilidade Técnica e Científica |
| :--- | :--- |
| [`NormativeDatabase`](../source/core/NormativeDatabase.hpp) | Inicializa e consulta o banco relacional **SQLite 3 em memória (`:memory:`)** com as tabelas normativas das normas **ABNT NBR 12655:2022**, **NBR 6118:2023**, **NBR 7211:2022**, **NBR 7212:2025**, **NBR 8953:2015** e **NBR 16697:2018**, protegido em concorrência por `QRecursiveMutex`. |
| [`ConcreteStandards`](../source/core/ConcreteStandards.hpp) | Determina o desvio-padrão $S_d$ (Condições A, B, C, histórico conhecido $\ge 2{,}0\text{ MPa}$ ou arbitrário), $f_{cj} = f_{ck} + 1{,}65 \cdot S_d$, limites de durabilidade por CAA I a IV ($a/c_{\max}$, $f_{ck,\min}$, $C_{\min}$), cobrimento nominal $c_{\text{nom}}$ (CA e CP $+5\text{ mm}$), verificação $D_{\max} \le 1{,}2 \cdot c_{\text{nom}}$ e tolerâncias de central (`±1%`, `±3%`, `±3%`, `±5%`). |
| [`AggregateOptimizer`](../source/core/AggregateOptimizer.hpp) | Calcula as 6 proporções experimentais da tabela de mistura binária de britas ($100\%$ a $50\%$, passo $10\%$) e determina o ponto ótimo de máxima massa unitária compactada (NBR 7211 / NBR NM 45). |
| [`MortarContentOptimizer`](../source/core/MortarContentOptimizer.hpp) | Calcula a amassada piloto pela Lei dos Volumes Absolutos e gera a progressão laboratorial de teor de argamassa seca $\alpha$ de $40\%$ a $68\%$ (passo $2\%$) mantendo fixa a massa de brita na betoneira. |
| [`RegressionEngine`](../source/core/RegressionEngine.hpp) | Ajusta por mínimos quadrados ordinários (OLS) em dupla precisão (`double` IEEE 754) a **Lei de Abrams Exponencial** ($f_{cj} = k_1 / k_2^{a/c}$), polinômios quadráticos (Abrams e Molinari) e **Lei Linear de Lyse** ($m = k_3 + k_4 \cdot (a/c)$), com cálculo de consumo teórico pelos volumes absolutos e proteções de domínio positivo. |
| [`MixDesignCalculator`](../source/core/MixDesignCalculator.hpp) | Executa fechamento volumétrico de $1\text{ m}^3$, correção rigorosa pela umidade superficial (com desconto exato de água livre), amassada para betoneira de laboratório, dimensionamento de padiolas ($35 \times 45\text{ cm}$) para $50\text{ kg}$ de cimento, rastreabilidade (`SolutionType`), e confronto de custos analítico vs. gráfico. |
| [`ProjectData`](../source/core/ProjectData.hpp) | Agrega o estado completo do estudo, gerencia os **6 cenários pré-configurados (`0..5`, incluindo Cenário 5 UFPR TC031)**, resolve metadados dinâmicos/manuais do relatório, adapta moeda (`R$` / `$`), serializa JSON `.cmdproj` e gera memoriais em Markdown e HTML5 (com `escapeHtml()`). |
| [`GerenciadorTraducao`](../source/language/GerenciadorTraducao.hpp) | Singleton de internacionalização em tempo de execução (`pt_BR` $\leftrightarrow$ `en_US`) carregando catálogos `.qm` via `QEvent::LanguageChange` e fornecendo símbolos monetários dinâmicos. |
| [`LogManager`](../source/utils/LogManager.hpp), [`CaminhoPersistenteUtils`](../source/utils/CaminhoPersistenteUtils.hpp), [`ApplicationPathUtils`](../source/utils/ApplicationPathUtils.hpp) | Gravação local segura de logs em `%LOCALAPPDATA%\ConcreteMixDesigner\logs\`, resolução de diretórios graváveis sem privilégios administrativos e localização de assets e ícones em builds locais ou instalados. |

---

## 3. Módulos da Camada GUI (`source/gui/`)

- [`MainWindow`](../source/gui/MainWindow.hpp): Janela principal com menus `Arquivo`, `Exibir`, `Configurações` (cenários `0..5`, dados do projeto `Ctrl+I`, submenu `Idioma`) e `Ajuda`, barra de status normativa e suporte automático aos temas Claro e Escuro do Windows 11 via DWM API (`DwmSetWindowAttribute`).
- **7 Abas Especializadas:** [`TabProjectParams`](../source/gui/TabProjectParams.hpp), [`TabMaterials`](../source/gui/TabMaterials.hpp), [`TabMortarContent`](../source/gui/TabMortarContent.hpp), [`TabExperimentalMixes`](../source/gui/TabExperimentalMixes.hpp) (com [`QuadChartWidget`](../source/gui/QuadChartWidget.hpp) + [`IptUnifiedDiagramWidget`](../source/gui/IptUnifiedDiagramWidget.hpp)), [`TabFinalMix`](../source/gui/TabFinalMix.hpp), [`TabCosts`](../source/gui/TabCosts.hpp) e [`TabTechnicalReport`](../source/gui/TabTechnicalReport.hpp) (exportação para Área de Transferência, `.md`, `.html` e `.pdf` A4).
- **Diálogos Modeless:** [`ManualDialog`](../source/gui/ManualDialog.hpp) (14 seções traduzidas dinamicamente), [`SobreDialog`](../source/gui/SobreDialog.hpp) (**9 abas** exibindo `ABOUT`, `History_APP`, `EULA`, `CLC`, `COPYRIGHT`, `NOTICES`, `PRIVACY_POLICY`, `RELEASE` e `LICENSES` com seletor `QComboBox` para 11 licenças completas) e diálogo de Normas Técnicas com pilha Z-Order independente.

---

## 4. Camada CLI, Empacotamento e Suíte de Testes (`source/cli/`, `library/`, `tests/`)

- [`source/cli/main_cli.cpp`](../source/cli/main_cli.cpp): Executável console autônomo `concrete-mix-designer-cli.exe` (`design`, `calc`, `verify`, `report`, `standards`, `presets`, `test`).
- [`source/windows_app_icon.rc.in`](../source/windows_app_icon.rc.in): Template de recurso Win32 RC que vincula `concrete_mix_design.ico` nos binários Windows.
- [`library/`](../library): Scripts Inno Setup unificado/dual (`Concrete_Mix_Designer.iss`, `_mingw.iss`, `_msvc.iss`), manifesto MSIX (`AppxManifest.xml`) e especificações (`Concrete_Mix_Designer_cpp.txt`) compatíveis com implantação local enxuta sem empacotamento de `vc_redist.x64.exe`.
- [`tests/`](../tests): Suíte modular abrangente com 9 alvos executáveis integrados via `CMakeLists.txt` e `CTest` em `build/build_suite_{msvc,mingw}/`, validando plugins/frameworks Qt, núcleo numérico, normas ABNT, JSON/PDF, tradução, GUI, CLI e auditoria de versões (`mocks/find_VersionEditor.cpp`).

</details>

---

## <a id="enus"></a>🇺🇸 English (US)

<details open>
<summary>Click to expand / collapse system architecture in English</summary><br>

# Concrete Mix Designer — System Architecture & Technical Specifications

**Version:** `2026.11.22.0`  
**Date:** `November 22, 2026`  
**Stack:** C++23 | Qt 6.12.0 | SQLite 3 (`:memory:`) | Qt Charts  
**Target Platform:** Windows 11 / Windows 10 x64 (MSVC 14.52 + Windows SDK 10.0.28000.0 & MinGW-w64 GCC 15.1.0)  
**Author:** Fernando Nillsson Cidade  

---

## 1. Architectural Philosophy & Layered Overview

**Concrete Mix Designer** adopts a strictly decoupled modular architecture built on **ISO C++23** and **Qt 6.12.0**, separating into reusable static libraries the analytical/normative core (`ConcreteMixCoreLib`), the graphical UI and vector charting layer (`ConcreteMixGuiLib`), the desktop application (`ConcreteMixDesign.exe`), the standalone command-line tool (`concrete-mix-designer-cli.exe`), maintenance utilities (`mocks/`), and the modular test engineering suite (`tests/`):

```text
+-------------------------------------------------------------------------+
|                       PRESENTATION & CLI LAYER                          |
|     ConcreteMixDesign.exe (Qt 6.12.0)  |  concrete-mix-designer-cli.exe |
|  MainWindow | 7 Specialized Tabs | ManualDialog | SobreDialog (Z-Order) |
|  QuadChartWidget (4 Quadrants) | IptUnifiedDiagramWidget (5th Unified)  |
+------------------------------------+------------------------------------+
                                     |
                                     v
+-------------------------------------------------------------------------+
|                   GUI INTERFACE LIBRARY (ConcreteMixGuiLib)             |
|  TabProjectParams | TabMaterials | TabMortarContent | TabExperimental   |
|  TabFinalMix | TabCosts | TabTechnicalReport (A4 PDF / HTML5 / MD)      |
+------------------------------------+------------------------------------+
                                     |
                                     v
+-------------------------------------------------------------------------+
|               ANALYTICAL & I18N CORE (ConcreteMixCoreLib)               |
|  +-------------------------------+  +---------------------------------+ |
|  | Mathematical & Normative Core |  | Persistence, I18N & Utilities   | |
|  | - ConcreteStandards (ABNT NBR)|  | - ProjectData (.cmdproj/Reports)| |
|  | - AggregateOptimizer (NM 45)  |  | - NormativeDatabase (:memory:)  | |
|  | - MortarContentOptimizer (α)  |  | - GerenciadorTraducao (.ts/.qm) | |
|  | - RegressionEngine (OLS/Exp)  |  | - LogManager & PathUtils        | |
|  | - MixDesignCalculator         |  |                                 | |
|  +-------------------------------+  +---------------------------------+ |
+-------------------------------------------------------------------------+
```

---

## 2. Core Layer Modules (`source/core/`, `source/language/`, `source/utils/`)

| Module (`hpp` / `cpp`) | Technical and Scientific Responsibility |
| :--- | :--- |
| [`NormativeDatabase`](../source/core/NormativeDatabase.hpp) | Initializes and queries the embedded **in-memory SQLite 3 database (`:memory:`)** storing tables from **ABNT NBR 12655:2022**, **NBR 6118:2023**, **NBR 7211:2022**, **NBR 7212:2025**, **NBR 8953:2015**, and **NBR 16697:2018**, synchronized across threads via `QRecursiveMutex`. |
| [`ConcreteStandards`](../source/core/ConcreteStandards.hpp) | Computes dosage standard deviation $S_d$ (Conditions A, B, C, known historical $\ge 2.0\text{ MPa}$, or arbitrary), $f_{cj} = f_{ck} + 1.65 \cdot S_d$, durability limits by CAA I–IV (max $w/c$, min $f_{ck}$, min cement), nominal cover $c_{\text{nom}}$ (RC and PC $+5\text{ mm}$), $D_{\max} \le 1.2 \cdot c_{\text{nom}}$, and plant weighing tolerances (`±1%`, `±3%`, `±3%`, `±5%`). |
| [`AggregateOptimizer`](../source/core/AggregateOptimizer.hpp) | Computes the 6 binary coarse-aggregate blend proportions ($100\%$ to $50\%$, $10\%$ increments) and identifies the maximum compacted bulk density point (NBR 7211 / NBR NM 45). |
| [`MortarContentOptimizer`](../source/core/MortarContentOptimizer.hpp) | Computes the initial pilot batch via the Law of Absolute Volumes and generates the laboratory mortar content $\alpha$ table ($40\%$ to $68\%$, $2\%$ step) with fixed coarse aggregate mass in the mixer. |
| [`RegressionEngine`](../source/core/RegressionEngine.hpp) | Performs IEEE 754 `double` ordinary least-squares fitting for **Abrams' Exponential Law** ($f_{cj} = k_1 / k_2^{w/c}$), quadratic polynomials (Abrams and Molinari), and **Lyse's Linear Law** ($m = k_3 + k_4 \cdot (w/c)$), alongside absolute-volume cement consumption and positive-domain guards. |
| [`MixDesignCalculator`](../source/core/MixDesignCalculator.hpp) | Performs $1\text{ m}^3$ volumetric closure, field surface-moisture mass/water correction, lab mixer batching, jobsite volume-box (*padiola*, $35 \times 45\text{ cm}$) sizing per $50\text{ kg}$ cement bag, solution diagnostics (`SolutionType`), and analytical vs. graphical cost comparison. |
| [`ProjectData`](../source/core/ProjectData.hpp) | Holds complete study state, manages the **6 built-in presets (`0..5`, including Scenario 5 UFPR TC031)**, resolves dynamic/manual report headers, adapts currency symbols (`$` / `R$`), serializes `.cmdproj` JSON, and generates Markdown and HTML5 reports (with `escapeHtml()`). |
| [`GerenciadorTraducao`](../source/language/GerenciadorTraducao.hpp) | Runtime localization singleton (`pt_BR` $\leftrightarrow$ `en_US`) loading compiled `.qm` catalogs via `QEvent::LanguageChange` and supplying adaptive currency symbols. |
| [`LogManager`](../source/utils/LogManager.hpp), [`CaminhoPersistenteUtils`](../source/utils/CaminhoPersistenteUtils.hpp), [`ApplicationPathUtils`](../source/utils/ApplicationPathUtils.hpp) | Unprivileged local logging under `%LOCALAPPDATA%\ConcreteMixDesigner\logs\`, writable per-user configuration directory resolution, and runtime asset/icon lookup. |

---

## 3. GUI Layer Modules (`source/gui/`)

- [`MainWindow`](../source/gui/MainWindow.hpp): Main application window featuring `File`, `View`, `Settings` (scenarios `0..5`, project metadata `Ctrl+I`, `Language` submenu), and `Help` menus, normative status bar, and automatic Windows 11 Light/Dark theme integration via DWM API.
- **7 Workspace Tabs:** [`TabProjectParams`](../source/gui/TabProjectParams.hpp), [`TabMaterials`](../source/gui/TabMaterials.hpp), [`TabMortarContent`](../source/gui/TabMortarContent.hpp), [`TabExperimentalMixes`](../source/gui/TabExperimentalMixes.hpp) (hosting [`QuadChartWidget`](../source/gui/QuadChartWidget.hpp) + [`IptUnifiedDiagramWidget`](../source/gui/IptUnifiedDiagramWidget.hpp)), [`TabFinalMix`](../source/gui/TabFinalMix.hpp), [`TabCosts`](../source/gui/TabCosts.hpp), and [`TabTechnicalReport`](../source/gui/TabTechnicalReport.hpp) (exporting to Clipboard, `.md`, `.html`, and A4 `.pdf`).
- **Modeless Dialogs:** [`ManualDialog`](../source/gui/ManualDialog.hpp) (14 dynamically translated sections), [`SobreDialog`](../source/gui/SobreDialog.hpp) (**9 tabs** displaying `ABOUT`, `History_APP`, `EULA`, `CLC`, `COPYRIGHT`, `NOTICES`, `PRIVACY_POLICY`, `RELEASE`, and `LICENSES` via a `QComboBox` for 11 complete licenses), and the Technical Standards dialog with independent Z-Order stacking.

---

## 4. CLI, Packaging & Comprehensive Test Suite (`source/cli/`, `library/`, `tests/`)

- [`source/cli/main_cli.cpp`](../source/cli/main_cli.cpp): Standalone console executable `concrete-mix-designer-cli.exe` (`design`, `calc`, `verify`, `report`, `standards`, `presets`, `test`).
- [`source/windows_app_icon.rc.in`](../source/windows_app_icon.rc.in): Win32 RC template embedding `concrete_mix_design.ico` into Windows executables.
- [`library/`](../library): Unified and toolchain-specific Inno Setup scripts (`Concrete_Mix_Designer.iss`, `_mingw.iss`, `_msvc.iss`), MSIX manifest (`AppxManifest.xml`), and release prompts (`Concrete_Mix_Designer_cpp.txt`) engineered for lean app-local deployment without bundling `vc_redist.x64.exe`.
- [`tests/`](../tests): Comprehensive modular test suite of 9 CTest executables built in `build/build_suite_{msvc,mingw}/`, verifying Qt plugins/frameworks, numerical/mathematical solvers, ABNT NBR rules, JSON/PDF reports, runtime i18n, GUI/charts interactions, CLI workflows, and version auditing (`mocks/find_VersionEditor.cpp`).

</details>
