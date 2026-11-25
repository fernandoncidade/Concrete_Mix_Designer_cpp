# Referência da Linha de Comando e Build / CLI & Build Reference — Concrete Mix Designer

<p align="center">
  <b>Selecione o idioma / Select language:</b><br>
  <a href="#ptbr">🇧🇷 Português (BR)</a> | <a href="#enus">🇺🇸 English (US)</a>
</p>

---

## <a id="ptbr"></a>🇧🇷 Português (BR)

<details open>
<summary>Clique para expandir / recolher a referência da CLI em português</summary><br>

# Concrete Mix Designer — Referência de Linha de Comando (CLI), Build e Testes

**Versão:** `2026.11.22.0`  
**Data:** `22 de novembro de 2026`  
**Autor:** Fernando Nillsson Cidade  

---

> [!WARNING]
> **Escopo Auxiliar da CLI (`concrete-mix-designer-cli.exe`):** A ferramenta de linha de comando, suas saídas JSON, a auditoria `verify` e os relatórios gerados (`report`) utilizam exatamente o mesmo motor computacional auxiliar gratuito da interface gráfica, o qual **ainda não está maduro o suficiente para produção ou uso profissional definitivo**. Devido a limitações, falhas humanas e eventuais erros no código-fonte (abrangendo tanto a matemática aplicada quanto a física/tecnologia do concreto envolvidas), **todos os resultados devem ser conferidos manualmente, comparados com ferramentas consolidadas no mercado e validados em laboratório (ABNT NBR 5738 e NBR 5739)**. Caso o usuário decida utilizá-los exclusivamente em produção, todas as consequências serão de sua inteira e exclusiva responsabilidade ([`licensing.md`](licensing.md#ptbr) e [EULA Oficial](../source/assets/EULA/EULA_pt_BR.txt)).

---

## 1. Executáveis e Alvos Gerados pelo Projeto

O sistema de build CMake (CMake 4.5.1 + Ninja 1.13.2 + C++23 + Qt 6.12.0) gera os seguintes executáveis nativos `x64`:

1. **`ConcreteMixDesign.exe` (Aplicativo Gráfico Principal — `WIN32_EXECUTABLE`):**
   Inicializa o banco de dados normativo SQLite em memória (`NormativeDatabase`), carrega preferências em `QSettings("AppsCpp", "ConcreteMixDesign")`, grava logs locais em `%LOCALAPPDATA%\ConcreteMixDesigner\logs\` e abre a interface de 7 abas + diagramas IPT.
2. **`concrete-mix-designer-cli.exe` (Utilitário Standalone de Linha de Comando):**
   Console nativo independente para automatizar dosagens IPT/EPUSP, fechamento volumétrico de $1\text{ m}^3$, auditorias ABNT NBR, consulta ao SQLite normativo e geração de memoriais (`text`, `json`, `markdown`, `html`). Quando empacotado em MSIX (`library/AppxManifest.xml`), registra alias `concrete-mix-designer-cli.exe` e `concretemixdesign.exe`.
3. **`mocks/` (`find_VersionEditor.exe`, `CompileTranslationsTerminal.exe`, `CompileTranslationsGUI.exe`):**
   Ferramentas auxiliares para auditoria/sincronização de versões/datas em todo o repositório (`--check` / `--apply`) e compilação dos catálogos de tradução `.ts` $\to$ `.qm`.
4. **Suíte Modular de Testes CTest em `tests/` (`build/build_suite_{msvc,mingw}/`):**
   **9 alvos independentes CTest** (`verify_calculations`, `test_RuntimePluginsAndFrameworks`, `test_StandardsAndNormativeDB`, `test_MathematicalCoreAndOptimizers`, `test_ProjectDataSerializationAndReports`, `test_I18nUtilsAndPersistence`, `test_GuiWorkflowsAndCharts`, `test_CliIntegration`, `test_MocksAndAuditTools`).

---

## 2. Manual da CLI (`concrete-mix-designer-cli.exe`)

### 2.1. Sintaxe Geral
```powershell
concrete-mix-designer-cli.exe [comando] [opcoes...]
```

### 2.2. Opções Globais
| Opção | Descrição | Valores / Padrão |
| :--- | :--- | :--- |
| `-h`, `--help` | Exibe ajuda e sintaxe completa dos comandos. | — |
| `-v`, `--version` | Exibe a versão oficial do binário (`2026.11.22.0`) e do motor. | — |
| `--lang <pt\|en>` | Idioma de saída de textos e relatórios. | `pt` (padrão) ou `en` |
| `--format <formato>` | Formato estruturado da resposta. | `text` (padrão), `json`, `markdown`, `html` |
| `--out <arquivo>` | Grava a saída diretamente em arquivo no disco. | Caminho relativo ou absoluto |
| `--dark` | Aplica estilo escuro em memoriais HTML exportados. | — |
| `--no-banner` | Omite o banner ASCII para uso em scripts/pipelines. | — |

### 2.3. Comandos Disponíveis

#### A. `design` — Dimensionamento completo pelo Método IPT/EPUSP
```powershell
concrete-mix-designer-cli.exe design --fck 30 --slump 120 --caa III --cond A
concrete-mix-designer-cli.exe design --fck 35 --slump 100 --caa II --format json --out mix_c35.json
```
Parâmetros suportados: `--fck <MPa>`, `--slump <mm>`, `--caa <I|II|III|IV>`, `--cond <A|B|C>`, `--sd <MPa>`, `--struct <rc|pc>`, `--alpha <%>`, `--mixer-vol <L>`, `--project-vol <m3>`, `--gravel-blend <0-100>`.

#### B. `calculate` (ou `calc`) — Cálculo a partir de arquivo `.cmdproj` ou parâmetros
```powershell
concrete-mix-designer-cli.exe calculate "estudo.cmdproj" --format json
```

#### C. `verify` (ou `audit`) — Auditoria normativa (NBR 12655, 6118, 7212, 8953)
```powershell
concrete-mix-designer-cli.exe verify "estudo.cmdproj" --format text
```

#### D. `report` — Geração de memorial técnico
```powershell
concrete-mix-designer-cli.exe report "estudo.cmdproj" --format html --dark --lang en --out relatorio.html
concrete-mix-designer-cli.exe report "estudo.cmdproj" --format markdown --out relatorio.md
```

#### E. `standards` — Consulta ao banco SQLite normativo
```powershell
concrete-mix-designer-cli.exe standards caa
concrete-mix-designer-cli.exe standards cements
concrete-mix-designer-cli.exe standards aggregates
concrete-mix-designer-cli.exe standards slump
concrete-mix-designer-cli.exe standards tolerances
concrete-mix-designer-cli.exe standards prep
concrete-mix-designer-cli.exe standards all --format json
```

#### F. `presets` — Cenários de referência (Cenários 0 a 5, inclusive Planilha UFPR TC031)
```powershell
concrete-mix-designer-cli.exe presets --list
concrete-mix-designer-cli.exe presets --run 5 --format json
concrete-mix-designer-cli.exe presets --export 1 c25_urbano.cmdproj
```

#### G. `test` — Autodiagnóstico analítico embutido na CLI
```powershell
concrete-mix-designer-cli.exe test
```

---

## 3. Utilitários de Manutenção (`mocks/`)

```powershell
# Auditoria de sincronização de versões/datas em todo o repositório (somente leitura)
.\build\build_Mocks_msvc\find_VersionEditor.exe --check .

# Verificação de catálogos de tradução Qt (.ts / .qm)
.\build\build_Mocks_msvc\CompileTranslationsTerminal.exe --module concrete_mix_design
```

---

## 4. Comandos de Compilação, Suíte de Testes e Empacotamento

```powershell
# 1. Compilar aplicativo + CLI (MSVC 14.52 x64 ou MinGW GCC 15.1.0)
cmd.exe /c "cmake\build_msvc.bat"
cmd.exe /c "cmake\build_mingw.bat"

# 2. Compilar Mocks
cmd.exe /c "cmake\build_mocks_msvc.bat"
cmd.exe /c "cmake\build_mocks_mingw.bat"

# 3. Compilar e executar TODA a SUITE modular de testes (9 executáveis CTest)
cmd.exe /c "cmake\build_suite_msvc.bat"
ctest --test-dir build\build_suite_msvc --output-on-failure

cmd.exe /c "cmake\build_suite_mingw.bat"
ctest --test-dir build\build_suite_mingw --output-on-failure

# 4. Gerar distribuições externas ultralimpas (SEM vc_redist.x64.exe e SEM suíte de testes)
powershell.exe -NoProfile -ExecutionPolicy Bypass -File cmake\gerar_distribuicoes_limpas.ps1

# 5. Gerar instaladores Inno Setup (MSVC e MinGW)
& "C:\Program Files\Inno Setup 7\ISCC.exe" "library\Concrete_Mix_Designer.iss"
```

</details>

---

## <a id="enus"></a>🇺🇸 English (US)

<details open>
<summary>Click to expand / collapse CLI reference in English</summary><br>

# Concrete Mix Designer — Command Line Interface (CLI), Build & Testing Reference

**Version:** `2026.11.22.0`  
**Date:** `November 22, 2026`  
**Author:** Fernando Nillsson Cidade  

---

> [!WARNING]
> **Auxiliary Scope of the CLI (`concrete-mix-designer-cli.exe`):** The command-line executable, its JSON outputs, the `verify` audit, and generated reports (`report`) share the exact same free auxiliary computational engine as the graphical desktop app, which **is not yet mature enough for definitive production or professional use**. Due to limitations, human flaws, and possible defects in the source code (encompassing both applied mathematics and underlying materials physics/technology), **all outputs must be manually verified, cross-checked against established market software, and validated through physical laboratory testing (ABNT NBR 5738 and NBR 5739)**. Should the user decide to rely exclusively on them in production, all consequences remain the user's sole and entire responsibility ([`licensing.md`](licensing.md#enus) and [Official EULA](../source/assets/EULA/EULA_en_US.txt)).

---

## 1. Project Generated Executables & Build Targets

The CMake build system (CMake 4.5.1 + Ninja 1.13.2 + C++23 + Qt 6.12.0) builds the following native `x64` targets:

1. **`ConcreteMixDesign.exe` (Primary Graphical Application — `WIN32_EXECUTABLE`):**
   Initializes the in-memory SQLite normative database (`NormativeDatabase`), loads preferences from `QSettings("AppsCpp", "ConcreteMixDesign")`, writes local telemetry-free logs to `%LOCALAPPDATA%\ConcreteMixDesigner\logs\`, and launches the 7-tab GUI with IPT diagrams.
2. **`concrete-mix-designer-cli.exe` (Standalone Console CLI Utility):**
   Independent console tool automating IPT/EPUSP concrete proportioning, $1\text{ m}^3$ volumetric closure, ABNT NBR code checks, in-memory SQLite queries, and technical report generation (`text`, `json`, `markdown`, `html`). Packaged with `concretemixdesign.exe` execution aliases in [`library/AppxManifest.xml`](../library/AppxManifest.xml).
3. **`mocks/` (`find_VersionEditor.exe`, `CompileTranslationsTerminal.exe`, `CompileTranslationsGUI.exe`):**
   Repository maintenance utilities for version/date synchronization (`--check` / `--apply`) and `.ts` $\to$ `.qm` translation compilation.
4. **Modular CTest Suite in `tests/` (`build/build_suite_{msvc,mingw}/`):**
   **9 independent CTest targets** (`verify_calculations`, `test_RuntimePluginsAndFrameworks`, `test_StandardsAndNormativeDB`, `test_MathematicalCoreAndOptimizers`, `test_ProjectDataSerializationAndReports`, `test_I18nUtilsAndPersistence`, `test_GuiWorkflowsAndCharts`, `test_CliIntegration`, `test_MocksAndAuditTools`).

---

## 2. CLI Manual (`concrete-mix-designer-cli.exe`)

### 2.1. General Syntax
```powershell
concrete-mix-designer-cli.exe [command] [options...]
```

### 2.2. Global Options
| Option | Description | Values / Default |
| :--- | :--- | :--- |
| `-h`, `--help` | Show command syntax and help usage. | — |
| `-v`, `--version` | Display binary (`2026.11.22.0`) and engine version. | — |
| `--lang <pt\|en>` | Output and report language. | `pt` (default) or `en` |
| `--format <format>` | Structured output format. | `text` (default), `json`, `markdown`, `html` |
| `--out <file>` | Write output directly to disk path. | Relative or absolute file path |
| `--dark` | Apply dark styling to exported HTML reports. | — |
| `--no-banner` | Suppress ASCII header banner for scripted parsing. | — |

### 2.3. Supported Commands

#### A. `design` — Automated IPT/EPUSP Mix Design
```powershell
concrete-mix-designer-cli.exe design --fck 30 --slump 120 --caa III --cond A
concrete-mix-designer-cli.exe design --fck 35 --slump 100 --caa II --format json --out mix_c35.json
```
Options: `--fck <MPa>`, `--slump <mm>`, `--caa <I|II|III|IV>`, `--cond <A|B|C>`, `--sd <MPa>`, `--struct <rc|pc>`, `--alpha <%>`, `--mixer-vol <L>`, `--project-vol <m3>`, `--gravel-blend <0-100>`.

#### B. `calculate` (or `calc`) — Calculate from `.cmdproj` file or parameters
```powershell
concrete-mix-designer-cli.exe calculate "study.cmdproj" --format json
```

#### C. `verify` (or `audit`) — Normative Compliance Audit (NBR 12655, 6118, 7212, 8953)
```powershell
concrete-mix-designer-cli.exe verify "study.cmdproj" --format text
```

#### D. `report` — Technical Report Export
```powershell
concrete-mix-designer-cli.exe report "study.cmdproj" --format html --dark --lang en --out report.html
concrete-mix-designer-cli.exe report "study.cmdproj" --format markdown --out report.md
```

#### E. `standards` — Query Normative SQLite Database
```powershell
concrete-mix-designer-cli.exe standards caa
concrete-mix-designer-cli.exe standards cements
concrete-mix-designer-cli.exe standards aggregates
concrete-mix-designer-cli.exe standards slump
concrete-mix-designer-cli.exe standards tolerances
concrete-mix-designer-cli.exe standards prep
concrete-mix-designer-cli.exe standards all --format json
```

#### F. `presets` — Built-in Engineering Scenarios (0 to 5, including UFPR TC031)
```powershell
concrete-mix-designer-cli.exe presets --list
concrete-mix-designer-cli.exe presets --run 5 --format json
concrete-mix-designer-cli.exe presets --export 1 c25_urban.cmdproj
```

#### G. `test` — Built-In CLI Analytical Self-Test
```powershell
concrete-mix-designer-cli.exe test
```

---

## 3. Repository Maintenance Utilities (`mocks/`)

```powershell
# Audit version/date synchronization across the repository (read-only check)
.\build\build_Mocks_msvc\find_VersionEditor.exe --check .

# Recompile Qt translation catalogs (.ts -> .qm)
.\build\build_Mocks_msvc\CompileTranslationsTerminal.exe --module concrete_mix_design
```

---

## 4. Build, Test Suite & Packaging Commands

```powershell
# 1. Build Application + CLI (MSVC 14.52 x64 or MinGW GCC 15.1.0)
cmd.exe /c "cmake\build_msvc.bat"
cmd.exe /c "cmake\build_mingw.bat"

# 2. Build Mocks Utilities
cmd.exe /c "cmake\build_mocks_msvc.bat"
cmd.exe /c "cmake\build_mocks_mingw.bat"

# 3. Build and run the ENTIRE modular Test Suite (9 CTest executables)
cmd.exe /c "cmake\build_suite_msvc.bat"
ctest --test-dir build\build_suite_msvc --output-on-failure

cmd.exe /c "cmake\build_suite_mingw.bat"
ctest --test-dir build\build_suite_mingw --output-on-failure

# 4. Generate ultra-clean external distributions (NO vc_redist.x64.exe and NO test suite)
powershell.exe -NoProfile -ExecutionPolicy Bypass -File cmake\gerar_distribuicoes_limpas.ps1

# 5. Build Inno Setup installers (MSVC & MinGW)
& "C:\Program Files\Inno Setup 7\ISCC.exe" "library\Concrete_Mix_Designer.iss"
```

</details>
