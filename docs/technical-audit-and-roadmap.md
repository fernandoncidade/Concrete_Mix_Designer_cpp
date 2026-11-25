# Auditoria Técnica, Decisões Arquiteturais, Matriz V&V e Roadmap / Technical Audit, ADRs & Roadmap — Concrete Mix Designer

<p align="center">
  <b>Selecione o idioma / Select language:</b><br>
  <a href="#ptbr">🇧🇷 Português (BR)</a> | <a href="#enus">🇺🇸 English (US)</a>
</p>

---

## <a id="ptbr"></a>🇧🇷 Português (BR)

<details open>
<summary>Clique para expandir / recolher a auditoria técnica em português</summary><br>

# Concrete Mix Designer — Auditoria Técnica, ADRs, V&V e Roadmap

**Versão:** `2026.11.22.0`  
**Data:** `22 de novembro de 2026`  
**Autor:** Fernando Nillsson Cidade  

Este documento consolida a auditoria técnica de engenharia civil, decisões arquiteturais (ADRs), matrizes de integração, verificação e validação (V&V), conformidade normativa ABNT NBR, rastreabilidade e roadmap do **Concrete Mix Designer**.

---

## A. Inventário Funcional e Classificação por Módulo

| Recurso / Módulo | Escopo | Classificação Atual | Evidência Principal | Limitação / Observação |
|---|---|---|---|---|
| **Normas & Durabilidade (`ConcreteStandards`)** | $f_{cj}$, CAA I–IV, CA/CP, Condições A/B/C/Histórico, $D_{\max} \le 1{,}2 \cdot c_{\text{nom}}$, Slump | Implementado, Integrado, Validado, Testado, Documentado | `ConcreteStandards.*`, `test_StandardsAndNormativeDB` | Condição B rejeita $f_{ck} > 20\text{ MPa}$; Condição C rejeita $f_{ck} > 15\text{ MPa}$ |
| **Banco Relacional SQLite (`NormativeDatabase`)** | `:memory:` thread-safe (`QRecursiveMutex`), cimentos NBR 16697, agregados, tolerâncias NBR 7212 | Implementado, Integrado, Testado, Documentado | `NormativeDatabase.*`, `test_StandardsAndNormativeDB` | Dados normativos em RAM reconstruídos por processo |
| **Mistura Binária (`AggregateOptimizer`)** | Empacotamento granulométrico ($100\%$–$50\%$, passo $10\%$, 6 proporções, NBR 7211 / NM 45) | Implementado, Integrado, Validado, Testado, Documentado | `AggregateOptimizer.*`, `test_MathematicalCoreAndOptimizers` | Acopla duas graduações graúdas (Brita 1 + Brita 2) |
| **Teor de Argamassa (`MortarContentOptimizer`)** | Volumes absolutos da amassada piloto e progressão $\alpha$ ($40\%$–$68\%$) | Implementado, Integrado, Validado, Testado, Documentado | `MortarContentOptimizer.*`, `test_MathematicalCoreAndOptimizers` | Massa fixa de agregado graúdo na betoneira |
| **Regressões IPT (`RegressionEngine`)** | Abrams (exponencial + polinômio 2º grau), Lyse (linear), Molinari (volumes absolutos / polinômio) | Implementado, Integrado, Validado, Testado, Documentado | `RegressionEngine.*`, `test_MathematicalCoreAndOptimizers` | Degradamento rastreado por `SolutionType` em dados não monotônicos |
| **Fechamento, Umidade & Custos (`MixDesignCalculator`)** | $1\text{ m}^3$ seco/úmido, betoneira, padiolas ($35 \times 45\text{ cm}$), custos Analítico vs. Gráfico | Implementado, Integrado, Validado, Testado, Documentado | `MixDesignCalculator.*`, `verify_calculations`, `test_MathematicalCoreAndOptimizers` | Conservação estrita de massa $M_{\text{seca}} + A_{\text{teor}} \equiv M_{\text{úmida}} + A_{\text{corr}}$ |
| **Persistência & Memoriais (`ProjectData`)** | 6 cenários (`0..5`), JSON `.cmdproj`, Markdown, HTML5 (`escapeHtml`), PDF A4 (`QPdfWriter`) | Implementado, Integrado, Validado, Testado, Documentado | `ProjectData.*`, `test_ProjectDataSerializationAndReports` | Abertura direta `.cmdproj` via CLI (`calculate`/`verify`/`report`) |
| **Interface Desktop & Diagramas Qt Charts** | `MainWindow` (7 abas), `QuadChartWidget`, `IptUnifiedDiagramWidget` (5º gráfico), Modeless (`ManualDialog` 14 seções / `SobreDialog` 9 abas + `QComboBox` 11 licenças) | Implementado, Integrado, Testado, Documentado | `source/gui/*`, `test_GuiWorkflowsAndCharts` | Exportações de imagem/PDF na UI usam diálogos nativos |
| **I18N Runtime & Utilitários (`pt_BR` / `en_US`)** | `GerenciadorTraducao`, `.ts`/`.qm` (0 *unfinished*), `R$`/$, `LogManager` (`%LOCALAPPDATA%`) | Implementado, Integrado, Testado, Documentado | `GerenciadorTraducao.*`, `test_I18nUtilsAndPersistence` | Requer `.qm` em `locale/` junto ao executável |
| **CLI Headless (`concrete-mix-designer-cli`)** | `design`, `calculate`, `verify`, `report`, `standards`, `presets`, `test` | Implementado, Integrado, Testado, Documentado | `source/cli/main_cli.cpp`, `test_CliIntegration` | Sem dependência de `QtWidgets` |
| **Auditoria de Versões & Plugins Runtime** | `find_VersionEditor` (`--check`/`--apply`), `Qt6Sql/QSQLITE`, `qico`, `qmodernwindowsstyle`, `Charts`, `OpenGL` | Implementado, Integrado, Testado, Documentado | `mocks/find_VersionEditor.cpp`, `test_RuntimePluginsAndFrameworks`, `test_MocksAndAuditTools` | Cobertura validada em MSVC 14.52 e MinGW GCC 15.1.0 |

---

## B. Decisões Arquiteturais (ADRs)

- **ADR-001 — Núcleo Desacoplado (`ConcreteMixCoreLib`) Livre de `QtWidgets`:** Separa cálculos, regressões OLS, normas e relatórios em biblioteca estática ligada apenas a `Qt6::Core` e `Qt6::Sql`, permitindo reutilização idêntica na GUI, na CLI console (`concrete-mix-designer-cli.exe`) e nos executáveis de teste.
- **ADR-002 — Banco Normativo SQLite em Memória (`:memory:`) Protegido por `QRecursiveMutex`:** Evita arquivos `.db` vulneráveis a adulteração no diretório do usuário e garante acesso determinístico em paralelo entre a GUI e threads de teste.
- **ADR-003 — Auditoria Algorítmica da Planilha UFPR TC031 (Cenário 5):** Isola variáveis independentes de entrada frente a células calculadas em precisão dupla IEEE 754 (`double`), elimina inversão de preços entre areia e brita e oferece o módulo de confrontação Analítico vs. Gráfico (Aba 6).
- **ADR-004 — Implantações Externas Limpas Sem `vc_redist.x64.exe`:** A distribuição MSVC carrega localmente as 5 DLLs exatas `Microsoft.VC145.CRT` (`msvcp140*.dll`, `vcruntime140*.dll`, ~1,1 MB) e a MinGW carrega as 3 DLLs do GCC 15.1.0 (`C:\Qt\Tools\mingw1510_64\bin`), reduzindo o instalador MSVC Inno Setup de ~29 MB para ~12 MB.
- **ADR-005 — Duplo Licenciamento Transparente (`GNU AGPLv3` + Liberdade em `source/assets/` + `EULA Oficial`):** Concilia código-fonte auditável na comunidade com distribuição gratuita assinada na Microsoft Store e política de privacidade independente (`100% Local-First`).

---

## C. Matriz de Benchmarks Quantitativos (V&V)

| Cenário / Benchmark | Dados de Entrada Principais | Suíte Verificadora | Critério Verificado |
|---|---|---|---|
| **Cenário 0 — Campos Zerados** | $f_{ck}=0$, $S_d=0$, `experimentalMixes` vazios | `verify_calculations`, `test_ProjectDataSerializationAndReports` | Zero `NaN`/`Inf`, sem divisão por zero, relatórios seguros |
| **Cenário 1 — C25 Urbano (CAA II)** | $f_{ck}=25\text{ MPa}$, Cond. A ($S_d=4{,}0$), Brita 1 | `verify_calculations`, `test_StandardsAndNormativeDB` | $f_{cj}=31{,}60\text{ MPa}$, $R^2_{\text{Abrams}}>0{,}99$, `isCompliant == true` |
| **Cenário 5 — Planilha UFPR TC031** | $f_{ck}=30\text{ MPa}$, $S_d=3{,}5\text{ MPa}$, $\alpha=52\%$, $80\%$ B2 / $20\%$ B1, $29\text{ L}$, $30\text{ m}^3$ | `verify_calculations`, `test_MathematicalCoreAndOptimizers` | $f_{cj}=35{,}775\text{ MPa}$, conservação estrita de massa + água livre, fechamento $1{,}000\text{ m}^3$ |
| **Conservação Massa / Umidade** | $h_{\text{areia}} = 4\%$, $h_{\text{brita}} = 1{,}5\%$ | `test_MathematicalCoreAndOptimizers` | $|(M_{\text{seca}} + A_{\text{teor}}) - (M_{\text{úmida}} + A_{\text{corr}})| < 10^{-9}\text{ kg}$ |

---

## D. Limitações Conhecidas & Roadmap

1. **Escopo Experimental & Uso Auxiliar ("AS IS"):** Os testes automatizados da suíte em `tests/` validam os cenários numéricos e normativos programados, **sem certificar o software para produção definitiva** (sujeito a limitações, falhas humanas e possíveis erros no código-fonte, matemática e física dos materiais; conferência manual e ensaios conforme ABNT NBR 5738/5739 são obrigatórios).
2. **Roadmap P0–P4:**
   - **P0 (Concluído v2026.11.22.0):** C++23 / Qt 6.12.0, MSVC 14.52 + MinGW GCC 15.1.0, distribuições limpas sem `vc_redist.x64.exe`, suíte modular de 9 alvos CTest, documentação completa e duplo licenciamento.
   - **P1–P4:** Ver [`docs/suggested-issues.md`](suggested-issues.md) (curvas granulométricas NBR 17054, aditivos químicos NBR 11768 / adições pozolânicas, ações diretas `.cmdproj` no menu Arquivo e cartas estatísticas $f_{ck,\text{est}}$).

</details>

---

## <a id="enus"></a>🇺🇸 English (US)

<details open>
<summary>Click to expand / collapse technical audit in English</summary><br>

# Concrete Mix Designer — Technical Audit, ADRs, V&V & Roadmap

**Version:** `2026.11.22.0`  
**Date:** `November 22, 2026`  
**Author:** Fernando Nillsson Cidade  

This document consolidates the civil engineering technical audit, Architectural Decision Records (ADRs), integration and V&V matrices, ABNT NBR code compliance, traceability, and roadmap of **Concrete Mix Designer**.

---

## A. Functional Inventory by Module

| Feature / Module | Scope | Status | Primary Evidence | Limitation / Note |
|---|---|---|---|---|
| **Standards & Durability (`ConcreteStandards`)** | $f_{cj}$, CAA I–IV, RC/PC, Conditions A/B/C/Known, $D_{\max} \le 1.2 \cdot c_{\text{nom}}$, Slump | Implemented, Integrated, Validated, Tested, Documented | `ConcreteStandards.*`, `test_StandardsAndNormativeDB` | Cond. B rejects $f_{ck} > 20\text{ MPa}$; Cond. C rejects $f_{ck} > 15\text{ MPa}$ |
| **In-Memory SQLite DB (`NormativeDatabase`)** | Thread-safe `:memory:` (`QRecursiveMutex`), NBR 16697 cements, aggregates, NBR 7212 tolerances | Implemented, Integrated, Tested, Documented | `NormativeDatabase.*`, `test_StandardsAndNormativeDB` | Normative tables live in volatile RAM rebuilt per process |
| **Binary Blend (`AggregateOptimizer`)** | Granular packing ($100\%$–$50\%$, $10\%$ step, 6 proportions, NBR 7211 / NM 45) | Implemented, Integrated, Validated, Tested, Documented | `AggregateOptimizer.*`, `test_MathematicalCoreAndOptimizers` | Models binary blends of Gravel 1 + Gravel 2 |
| **Mortar Content (`MortarContentOptimizer`)** | Pilot batch absolute volumes & $\alpha$ progression ($40\%$–$68\%$) | Implemented, Integrated, Validated, Tested, Documented | `MortarContentOptimizer.*`, `test_MathematicalCoreAndOptimizers` | Assumes fixed coarse-aggregate mass inside mixer |
| **IPT Regressions (`RegressionEngine`)** | Abrams (exponential + 2nd-degree poly), Lyse (linear), Molinari (absolute volumes / poly) | Implemented, Integrated, Validated, Tested, Documented | `RegressionEngine.*`, `test_MathematicalCoreAndOptimizers` | Explicit `SolutionType` degradation on non-monotonic data |
| **Closure, Moisture & Costs (`MixDesignCalculator`)** | Dry/wet $1\text{ m}^3$, mixer batch, padiolas ($35 \times 45\text{ cm}$), Analytical vs. Graphical costs | Implemented, Integrated, Validated, Tested, Documented | `MixDesignCalculator.*`, `verify_calculations`, `test_MathematicalCoreAndOptimizers` | Strict conservation $M_{\text{dry}} + W_{\text{theor}} \equiv M_{\text{wet}} + W_{\text{corr}}$ |
| **Persistence & Reports (`ProjectData`)** | 6 scenarios (`0..5`), `.cmdproj` JSON, Markdown, HTML5 (`escapeHtml`), A4 PDF (`QPdfWriter`) | Implemented, Integrated, Validated, Tested, Documented | `ProjectData.*`, `test_ProjectDataSerializationAndReports` | Direct `.cmdproj` processing supported via CLI |
| **Desktop GUI & Qt Charts Diagrams** | `MainWindow` (7 tabs), `QuadChartWidget`, `IptUnifiedDiagramWidget` (5th view), Modeless (`ManualDialog` 14 sections / `SobreDialog` 9 tabs + `QComboBox` 11 licenses) | Implemented, Integrated, Tested, Documented | `source/gui/*`, `test_GuiWorkflowsAndCharts` | Interactive UI image/PDF export uses standard Windows dialogs |
| **Runtime I18N & Utils (`pt_BR` / `en_US`)** | `GerenciadorTraducao`, `.ts`/`.qm` (0 unfinished), `R$`/$, `LogManager` (`%LOCALAPPDATA%`) | Implemented, Integrated, Tested, Documented | `GerenciadorTraducao.*`, `test_I18nUtilsAndPersistence` | Expects `.qm` catalogs under `locale/` alongside binary |
| **Headless CLI (`concrete-mix-designer-cli`)** | `design`, `calculate`, `verify`, `report`, `standards`, `presets`, `test` | Implemented, Integrated, Tested, Documented | `source/cli/main_cli.cpp`, `test_CliIntegration` | Free of `QtWidgets` dependencies |
| **Audit Tools & Qt Runtime Plugins** | `find_VersionEditor` (`--check`/`--apply`), `Qt6Sql/QSQLITE`, `qico`, `qmodernwindowsstyle`, `Charts`, `OpenGL` | Implemented, Integrated, Tested, Documented | `mocks/find_VersionEditor.cpp`, `test_RuntimePluginsAndFrameworks`, `test_MocksAndAuditTools` | Verified under both MSVC 14.52 and MinGW GCC 15.1.0 |

---

## B. Architectural Decision Records (ADRs)

- **ADR-001 — `QtWidgets`-Free Static Core (`ConcreteMixCoreLib`):** Separates proportioning physics, OLS fitting, normative checks, and reports into a shared static library linked against `Qt6::Core` and `Qt6::Sql`, shared identically by GUI, console CLI, and test executables.
- **ADR-002 — Thread-Safe In-Memory SQLite (`:memory:` + `QRecursiveMutex`):** Avoids tamper-prone external `.db` assets and ensures concurrent read safety across UI and multithreaded test suites.
- **ADR-003 — UFPR TC031 Spreadsheet Audit (Scenario 5):** Separates independent inputs from calculated cells in IEEE 754 `double`, fixes inverted sand/gravel unit prices, and provides Analytical vs. Graphical economic comparison (Tab 6).
- **ADR-004 — Lean App-Local Deployments Without `vc_redist.x64.exe`:** MSVC deployment co-locates the 5 exact `Microsoft.VC145.CRT` DLLs (~1.1 MB) and MinGW co-locates 3 GCC 15.1.0 runtime DLLs, cutting the MSVC Inno Setup installer from ~29 MB to ~12 MB.
- **ADR-005 — Transparent Dual Licensing (`GNU AGPLv3` + Freedom in `source/assets/` + `Official EULA`):** Balances auditable open-source availability on GitHub with free digitally signed Microsoft Store distribution under an independent local-first Privacy Policy.

---

## C. Quantitative Benchmark Verification Matrix (V&V)

| Scenario / Benchmark | Core Input Parameters | Verifying Test Target | Verified Criterion |
|---|---|---|---|
| **Scenario 0 — Zeroed Fields** | $f_{ck}=0$, $S_d=0$, empty `experimentalMixes` | `verify_calculations`, `test_ProjectDataSerializationAndReports` | Zero `NaN`/`Inf`, no zero-division, safe HTML/MD output |
| **Scenario 1 — C25 Urban (CAA II)** | $f_{ck}=25\text{ MPa}$, Cond. A ($S_d=4.0$), Gravel 1 | `verify_calculations`, `test_StandardsAndNormativeDB` | $f_{cj}=31.60\text{ MPa}$, $R^2_{\text{Abrams}}>0.99$, `isCompliant == true` |
| **Scenario 5 — UFPR TC031 Audited** | $f_{ck}=30\text{ MPa}$, $S_d=3.5\text{ MPa}$, $\alpha=52\%$, $80\%$ G2 / $20\%$ G1, $29\text{ L}$, $30\text{ m}^3$ | `verify_calculations`, `test_MathematicalCoreAndOptimizers` | $f_{cj}=35.775\text{ MPa}$, exact mass + free-water conservation, $1.000\text{ m}^3$ closure |
| **Moisture & Mass Conservation** | $h_{\text{sand}} = 4\%$, $h_{\text{gravel}} = 1.5\%$ | `test_MathematicalCoreAndOptimizers` | $|(M_{\text{dry}} + W_{\text{theor}}) - (M_{\text{wet}} + W_{\text{corr}})| < 10^{-9}\text{ kg}$ |

---

## D. Known Limitations & Roadmap

1. **Experimental Scope & Free Auxiliary Use ("AS IS"):** Passing all 9 CTest targets in `tests/` verifies implemented mathematical and normative logic **and does not certify the software for definitive production use** (subject to limitations, human flaws, and possible source-code defects; manual verification and physical lab testing per ABNT NBR 5738/5739 remain mandatory).
2. **Roadmap P0–P4:**
   - **P0 (Delivered v2026.11.22.0):** C++23 / Qt 6.12.0, MSVC 14.52 + MinGW GCC 15.1.0, ultra-clean app-local deployments without `vc_redist.x64.exe`, 9-target modular test suite, complete OmniFrame3D-grade docs, and dual licensing.
   - **P1–P4:** Tracked in [`docs/suggested-issues.md`](suggested-issues.md) (NBR 17054 grading curves, NBR 11768 admixtures & pozzolans, File menu `.cmdproj` persistence, and $f_{ck,\text{est}}$ control charts).

</details>
