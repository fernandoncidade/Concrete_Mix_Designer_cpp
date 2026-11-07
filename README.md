# Concrete Mix Design C++ — Dosagem Experimental de Concreto (Método IPT/EPUSP)

[Português (Brasil)](#português-brasil) | [English (United States)](#english-united-states)

---

## Português (Brasil)

### Visão Geral do Projeto

O **Concrete Mix Design C++ (v2026.11.6.0)** é uma plataforma desktop nativa de Engenharia Civil e Tecnologia do Concreto desenvolvida em **C++20 moderno** e **Qt 6.11.1 (Widgets, Charts, Sql)** para **Microsoft Windows 10 e Windows 11 (x64)**. O software automatiza com precisão analítica em ponto flutuante de 64 bits (`double` IEEE 754) o estudo completo de dosagem experimental de concreto de cimento Portland pelo consagrado **Método IPT/EPUSP (Helene & Terzian, 1992)**, integrando um banco de dados relacional **SQLite 3 embarcado em memória (`:memory:`)** com todas as verificações obrigatórias das normas técnicas brasileiras vigentes da **ABNT**:

- **ABNT NBR 12655:2022** — *Concreto de cimento Portland — Preparo, controle, recebimento e aceitação* (Classes de Agressividade Ambiental CAA I a IV, desvios-padrão $S_d$ para Condições A, B e C, relação $a/c$ máxima, $f_{ck}$ mínimo e consumo mínimo de cimento $C_{\min}$);
- **ABNT NBR 6118:2023** — *Projeto de estruturas de concreto — Procedimento* (Cobrimentos nominais de armadura $c_{\text{nom}}$ para Concreto Armado CA e Protendido CP, e verificação geométrica $D_{\max} \le 1,2 \cdot c_{\text{nom}}$);
- **ABNT NBR 7211:2022** — *Agregados para concreto — Requisitos* (Caracterização física de agregados miúdos e graúdos e empacotamento granular de misturas binárias de britas — NBR NM 45);
- **ABNT NBR 7212:2025** — *Concreto dosado em central — Preparo, fornecimento e controle* (Tolerâncias operacionais de pesagem em central dosadora conforme item 5.5: Cimento e Adições $\pm 1\%$, Agregados $\pm 3\%$, Água $\pm 3\%$ e Aditivos $\pm 5\%$);
- **ABNT NBR 8953:2015** — *Concreto para fins estruturais — Classificação pela massa específica, por grupos de resistência e consistência* (Grupos I e II de resistência e classes de abatimento *Slump* S10 a S220);
- **Fundamentos Científicos:** *Helene & Terzian (1992)* e *Mehta & Monteiro (2014)*.

---

### Principais Funcionalidades e Arquitetura de 7 Abas + Menu "Configurações"

1. **Menu `Configurações` (`Ctrl+I`):**
   - Permite configurar e persistir localmente os metadados de identificação do estudo (**Projeto:**, **Responsável:**, **Instituição:** e **Matrícula / ID:**).
   - Para o campo **Projeto:**, oferece a escolha entre **reportar automaticamente as informações selecionadas em "Cenário da Planilha"** (com tradução dinâmica instantânea PT-BR / EN-US) ou **editar as informações manualmente**.
2. **6 Cenários de Engenharia Pré-Configurados (`Cenário 0` a `Cenário 5`):**
   - Inclui cenários urbanos, marinhos, industriais, fundações e o **Cenário 5: Planilha UFPR Original Corrigida (TC031 - C30, Sd 3,5 MPa, α 52%, Obra 30 m³)**, todos com tradução dinâmica em tempo de execução.
3. **As 7 Abas de Trabalho Especializadas:**
   - **Aba 1 (`1. Parâmetros de Projeto`):** Cálculo de $f_{cj} = f_{ck} + 1,65 \cdot S_d$, seleção de CAA I a IV, tipo estrutural (CA / CP), *Slump* (S10 a S220) e validação normativa imediata.
   - **Aba 2 (`2. Materiais & Britas`):** Caracterização de Cimento, Areia, Brita 1 e Brita 2 e ensaio experimental de empacotamento granular de misturas binárias (0% a 100% de Brita 2).
   - **Aba 3 (`3. Teor de Argamassa (α)`):** Sequência laboratorial completa de adições na betoneira para teores de argamassa de 40% a 68% (passo de 2%).
   - **Aba 4 (`4. Traços & Diagrama IPT`):** Ajuste numérico por mínimos quadrados ordinários (OLS) das Leis de **Abrams**, **Lyse** e **Molinari** ($R^2$) e renderização vetorial interativa do **Diagrama de Dosagem de 4 Quadrantes do IPT** e do novo gráfico unificado **5° Abrams/Lyse/Molinari (IPT)** com linhas de grade de precisão e exportação de imagem em alta definição (PNG e PDF).
   - **Aba 5 (`5. Traço Final & Amassadas`):** Traço unitário seco e corrigido pela umidade superficial, consumo para $1\text{ m}^3$, amassada exata para a betoneira de laboratório e conversão para canteiro de obra por saco de $50\text{ kg}$ de cimento com dimensionamento de padiolas ($35 \times 45\text{ cm}$).
   - **Aba 6 (`6. Análise de Custos`):** Comparativo econômico detalhado entre a solução analítica exata das equações e a leitura gráfica manual em ábacos.
   - **Aba 7 (`7. Relatório Técnico`):** Geração dinâmica bilíngue em tempo de execução (`pt_BR` / `en_US`) do Memorial de Cálculo Técnico com símbolo monetário adaptativo (R$ em Português e $ em Inglês), e exportação nativa para Área de Transferência, Markdown (`.md`), HTML5 (`.html`) e PDF oficial A4 (`.pdf`).
4. **Interface de Linha de Comando (CLI - `concrete-mix-designer-cli.exe`):**
   - Ferramenta de terminal para automação completa de dosagens (`design`), fechamento volumétrico (`calc`), auditoria normativa (`verify`), geração de relatórios (`report`), consulta de tabelas normativas (`standards`), execução de traços padronizados (`presets`) e bateria de testes analíticos (`test`).

---

### Compilação e Execução (Windows x64 — MinGW-w64 / MSVC 2022)

#### Pré-requisitos
- **Sistema Operacional:** Microsoft Windows 10 (1809+) ou Windows 11 (x64)
- **Compilador C++20:** MinGW-w64 GCC 13+ (recomendado via Qt Maintenance Tool) ou MSVC 2022
- **CMake:** 3.20 ou superior e **Ninja**
- **Qt Framework:** Qt 6.11.1 (`Core`, `Gui`, `Widgets`, `Charts`, `Sql`)

#### Comandos de Build, CLI e Verificação Analítica
```powershell
# 1. Compilação rápida automatizada (MinGW ou MSVC)
cmd.exe /c "cmake\build_mingw.bat"
# ou: cmd.exe /c "cmake\build_msvc.bat"

# 2. Executar a suíte de verificação analítica
.\build\build_concrete_mix_design_mingw\verify_calculations.exe

# 3. Executar a ferramenta de linha de comando (CLI)
.\build\build_concrete_mix_design_mingw\concrete-mix-designer-cli.exe --help
.\build\build_concrete_mix_design_mingw\concrete-mix-designer-cli.exe test

# 4. Executar o aplicativo gráfico principal
.\build\build_concrete_mix_design_mingw\ConcreteMixDesign.exe
```

---

### Licenciamento, Microsoft Store e Conformidade Legal

- **Autor e Publicador:** Fernando Nillsson Cidade ([`@fernandoncidade`](https://github.com/fernandoncidade)).
- **Modelo de Duplo Licenciamento (GitHub & Microsoft Store):**
  - O código-fonte neste repositório do **GitHub** é licenciado sob a **[GNU Affero General Public License v3 (AGPLv3)](LICENSE)**, assegurando a liberdade de estudo, modificação, auditoria científica e reciprocidade de código aberto (copyleft), reservando-se os direitos de marca e identidade visual (veja detalhes em **[docs/licensing.md](docs/licensing.md)**).
  - Os pacotes binários oficiais pré-compilados e distribuídos comercialmente na **Microsoft Store** (**Microsoft Partner Center**) são regidos pelo **Contrato de Licenciamento Comercial** ([`source/assets/CLC`](source/assets/CLC/CLC_pt_BR.txt)) e pelo **Contrato de Licença de Usuário Final — EULA** ([`source/assets/EULA`](source/assets/EULA/EULA_pt_BR.txt)), contando com certificação de segurança da Microsoft e atualizações automáticas via Windows Update.
- **Matriz de Rastreabilidade Normativa:**
  - O mapeamento item a item das normas ABNT NBR 12655:2022, NBR 6118:2023, NBR 7211:2022, NBR 7212:2025 e NBR 8953:2015 encontra-se documentado em **[docs/matriz_rastreabilidade_normativa.md](docs/matriz_rastreabilidade_normativa.md)**.
- **Conformidade com Software Livre (Qt 6.11.1 LGPLv3 & SQLite):** Vinculação 100% dinâmica das bibliotecas Qt6 sob a licença **GNU LGPL v3.0** e **SQLite** em domínio público (textos integrais em [`source/assets/LICENSES/`](source/assets/LICENSES)).
- **Privacidade Local-First (LGPD / GDPR / Política 10.5 da Microsoft Store):** Operação 100% local e offline, sem coleta de dados pessoais e sem telemetria ([`source/assets/PRIVACY_POLICY`](source/assets/PRIVACY_POLICY/Privacy_Policy_pt_BR.txt)).
- **Responsabilidade Profissional (CONFEA/CREA):** Em cumprimento à Lei Federal nº 5.194/1966 e à Lei nº 6.496/1977, o software atua como ferramenta de apoio computacional, cabendo exclusivamente ao Engenheiro Civil habilitado a realização dos ensaios laboratoriais e a emissão da respectiva **ART**.

---

## English (United States)

### Project Overview

**Concrete Mix Design C++ (v2026.11.6.0)** is a native desktop Civil Engineering and Concrete Technology platform built with **modern C++20** and **Qt 6.11.1 (Widgets, Charts, Sql)** for **Microsoft Windows 10 and Windows 11 (x64)**. The software automates with 64-bit floating-point (`double` IEEE 754) analytical precision the complete experimental mix design study of Portland cement concrete via the renowned **IPT/EPUSP Method (Helene & Terzian, 1992)**, integrating an **embedded in-memory SQLite 3 relational database (`:memory:`)** with all mandatory checks from current Brazilian **ABNT** technical standards:

- **ABNT NBR 12655:2022** — *Portland cement concrete — Preparation, control, receipt, and acceptance* (Environmental Aggressiveness Classes CAA I to IV, standard deviations $S_d$ for Conditions A, B, and C, maximum $w/c$ ratio, minimum $f_{ck}$, and minimum cement content $C_{\min}$);
- **ABNT NBR 6118:2023** — *Design of concrete structures — Procedure* (Nominal reinforcement cover $c_{\text{nom}}$ for Reinforced Concrete RC and Prestressed Concrete PC, and geometric check $D_{\max} \le 1.2 \cdot c_{\text{nom}}$);
- **ABNT NBR 7211:2022** — *Aggregates for concrete — Requirements* (Physical characterization of fine and coarse aggregates and granular packing optimization of binary coarse aggregate blends — NBR NM 45);
- **ABNT NBR 7212:2025** — *Ready-mixed concrete — Preparation, delivery, and control* (Central batching plant weighing tolerances under Section 5.5: Cement and Additions $\pm 1\%$, Aggregates $\pm 3\%$, Water $\pm 3\%$, and Admixtures $\pm 5\%$);
- **ABNT NBR 8953:2015** — *Concrete for structural purposes — Classification by density, strength groups, and consistency* (Strength Groups I and II and Slump consistency classes S10 to S220);
- **Scientific Foundations:** *Helene & Terzian (1992)* and *Mehta & Monteiro (2014)*.

---

### Key Features and 7-Tab Architecture + "Settings" Menu

1. **`Settings` Menu (`Ctrl+I`):**
   - Allows configuring and locally persisting study identification metadata (**Project:**, **Engineer:**, **Institution:**, and **Registration / ID:**).
   - For the **Project:** field, offers the choice between **automatically reporting the information selected in "Spreadsheet Scenario"** (with instant dynamic runtime translation between PT-BR and EN-US) or **editing the information manually**.
2. **6 Pre-Configured Engineering Scenarios (`Scenario 0` to `Scenario 5`):**
   - Includes urban, marine, industrial, foundation scenarios, and **Scenario 5: Corrected Original UFPR Spreadsheet (TC031 - C30, Sd 3.5 MPa, α 52%, Project 30 m³)**, all featuring dynamic runtime translation.
3. **The 7 Specialized Workspace Tabs:**
   - **Tab 1 (`1. Project Parameters`):** Calculation of $f_{cj} = f_{ck} + 1.65 \cdot S_d$, selection of CAA I to IV, structural type (RC / PC), Slump (S10 to S220), and immediate normative validation.
   - **Tab 2 (`2. Materials & Coarse Aggregates`):** Characterization of Cement, Sand, Coarse Aggregate 1, and Coarse Aggregate 2, plus experimental granular packing optimization for binary blends (0% to 100% Coarse Aggregate 2).
   - **Tab 3 (`3. Mortar Content (α)`):** Full laboratory mixer addition sequence for dry mortar contents from 40% to 68% (2% step).
   - **Tab 4 (`4. Mixes & IPT Diagram`):** Ordinary least squares (OLS) curve fitting of **Abrams'**, **Lyse's**, and **Molinari's** Laws ($R^2$), interactive vector rendering of the **IPT 4-Quadrant Mix Design Diagram**, and the new **5° Abrams/Lyse/Molinari (IPT)** unified chart with high-precision grid lines and high-DPI image export (PNG and PDF).
   - **Tab 5 (`5. Final Mix & Batches`):** Dry and surface-moisture-corrected unit mixes, material consumption per $1\text{ m}^3$, exact batch masses for the laboratory mixer, and jobsite conversion per $50\text{ kg}$ cement bag with volume-box (*padiola*) sizing ($35 \times 45\text{ cm}$).
   - **Tab 6 (`6. Cost Analysis`):** Detailed economic comparison between the exact analytical equation solution and manual graphical reading on nomograms.
   - **Tab 7 (`7. Technical Report`):** Dynamic runtime bilingual generation (`pt_BR` / `en_US`) of the Technical Calculation Report with adaptive currency symbols ($ in English and R$ in Portuguese), and native export to Clipboard, Markdown (`.md`), HTML5 (`.html`), and official A4 PDF (`.pdf`).
4. **Command Line Interface (CLI - `concrete-mix-designer-cli.exe`):**
   - Native console tool for automated mix design (`design`), volumetric closure (`calc`), normative compliance auditing (`verify`), report generation (`report`), standards database queries (`standards`), standardized industrial presets (`presets`), and automated analytical regression tests (`test`).

---

### Building and Running (Windows x64 — MinGW-w64 / MSVC 2022)

#### Prerequisites
- **Operating System:** Microsoft Windows 10 (1809+) or Windows 11 (x64)
- **C++20 Compiler:** MinGW-w64 GCC 13+ (recommended via Qt Maintenance Tool) or MSVC 2022
- **CMake:** 3.20 or higher and **Ninja**
- **Qt Framework:** Qt 6.11.1 (`Core`, `Gui`, `Widgets`, `Charts`, `Sql`)

#### Build, CLI, and Analytical Verification Commands
```powershell
# 1. Fast automated build (MinGW or MSVC)
cmd.exe /c "cmake\build_mingw.bat"
# or: cmd.exe /c "cmake\build_msvc.bat"

# 2. Run the analytical verification suite
.\build\build_concrete_mix_design_mingw\verify_calculations.exe

# 3. Run the Command Line Interface (CLI)
.\build\build_concrete_mix_design_mingw\concrete-mix-designer-cli.exe --help
.\build\build_concrete_mix_design_mingw\concrete-mix-designer-cli.exe test

# 4. Launch the primary graphical application
.\build\build_concrete_mix_design_mingw\ConcreteMixDesign.exe
```

---

### Licensing, Normative Traceability, and Legal Compliance

- **Author and Publisher:** Fernando Nillsson Cidade ([`@fernandoncidade`](https://github.com/fernandoncidade)).
- **Open-Source & Store Distribution Model:**
  - The source code in this repository is licensed under the **[GNU Affero General Public License v3 (AGPLv3)](LICENSE)**, guaranteeing freedoms to study, modify, verify scientific accuracy, and share improvements under copyleft reciprocity, while trademark and visual identity rights remain reserved (see **[docs/licensing.md](docs/licensing.md)**).
  - The official trademark, trade name, and visual identity ("Concrete Mix Designer C++" / "Concrete Mix Design C++") remain reserved; unauthorized commercial redistribution under the official brand on app stores is prohibited.
  - Official precompiled binaries on the **Microsoft Store** (**Microsoft Partner Center**) are distributed under the **Commercial Licensing Agreement** ([`source/assets/CLC`](source/assets/CLC/CLC_en_US.txt)) and **EULA** ([`source/assets/EULA`](source/assets/EULA/EULA_en_US.txt)) with automated store updates and Microsoft security certification.
- **Normative Traceability Matrix:**
  - Comprehensive clause-by-clause mapping of ABNT NBR 12655:2022, NBR 6118:2023, NBR 7211:2022, NBR 7212:2025, NBR 8953:2015, and IPT/EPUSP references is published in **[docs/matriz_rastreabilidade_normativa.md](docs/matriz_rastreabilidade_normativa.md)**.
- **Open-Source Compliance (Qt 6.11.1 LGPLv3 & SQLite):** 100% dynamic linking of Qt6 libraries under the **GNU LGPL v3.0** license (with end-user right to replace DLLs per Section 4) and public-domain **SQLite** (full license texts in [`source/assets/LICENSES/`](source/assets/LICENSES)).
- **Unprivileged Execution & Local-First Privacy (LGPD / GDPR / Microsoft Store Policy 10.5):** 100% local and offline operation, zero personal data collection, and database persistence in `%LOCALAPPDATA%/ConcreteMixDesigner` without requiring administrative privileges ([`source/assets/PRIVACY_POLICY`](source/assets/PRIVACY_POLICY/Privacy_Policy_en_US.txt)).
- **Professional Engineering Liability (CONFEA/CREA):** Pursuant to Brazilian Federal Law No. 5,194/1966 and Law No. 6,496/1977 (and international equivalents), the software serves as a computational support tool, while physical laboratory testing and issuance of a **Technical Responsibility Record (ART / PE Seal)** remain the sole responsibility of the licensed Civil Engineer.
