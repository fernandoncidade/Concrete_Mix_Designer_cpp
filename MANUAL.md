# MANUAL DO USUÁRIO E GUIA DE ENGENHARIA CIVIL — CONCRETE MIX DESIGNER
# USER MANUAL AND CIVIL ENGINEERING GUIDE — CONCRETE MIX DESIGNER

**Versão / Version:** `2026.11.22.0`  
**Data / Date:** `22 de novembro de 2026` / `November 22, 2026`  
**Ambiente Tecnológico / Tech Stack:** C++23 | Qt 6.12.0 | SQLite 3 (`:memory:`) | Qt Charts (`QChart` / `IptUnifiedDiagramWidget`)  
**Plataforma / Platform:** Windows 11 / Windows 10 x64  
**Autor / Author:** Fernando Nillsson Cidade (`linceu_lighthouse@outlook.com`)  
**Repositório Oficial / Official Repository:** https://github.com/fernandoncidade/Concrete_Mix_Designer_cpp  

---

## Sumário / Table of Contents

- [PARTE 1: MANUAL DE INSTRUÇÕES (PORTUGUÊS - BR)](#parte-1-manual-de-instruções-português---br)
  - [1. Aviso de Escopo Auxiliar Gratuito e Licenciamento Duplo](#1-aviso-de-escopo-auxiliar-gratuito-e-licenciamento-duplo)
  - [2. Fundamentos Matemáticos — Método IPT / EPUSP (Helene & Terzian, 1992)](#2-fundamentos-matemáticos--método-ipt--epusp-helene--terzian-1992)
  - [3. Banco Normativo SQLite em Memória e Regras ABNT NBR](#3-banco-normativo-sqlite-em-memória-e-regras-abnt-nbr)
  - [4. Fluxo Operacional nas 7 Abas da Interface Gráfica](#4-fluxo-operacional-nas-7-abas-da-interface-gráfica)
  - [5. Cenários Pré-Configurados (0 a 5), Padiolas e Amassadas de Obra](#5-cenários-pré-configurados-0-a-5-padiolas-e-amassadas-de-obra)
  - [6. Relatórios Técnicos (Markdown / HTML5 / PDF) e Persistência (`.cmdproj`)](#6-relatórios-técnicos-markdown--html5--pdf-e-persistência-cmdproj)
  - [7. Interface de Linha de Comando (CLI) e Ferramentas Auxiliares (`mocks/`)](#7-interface-de-linha-de-comando-cli-e-ferramentas-auxiliares-mocks)
  - [8. Atalhos, Preferências Locais, Desinstalação Limpa e Suporte](#8-atalhos-preferências-locais-desinstalação-limpa-e-suporte)
- [PART 2: OPERATIONS MANUAL & CIVIL ENGINEERING GUIDE (ENGLISH - US)](#part-2-operations-manual--civil-engineering-guide-english---us)
  - [1. Free Auxiliary Engineering Scope & Dual-Licensing Notice](#1-free-auxiliary-engineering-scope--dual-licensing-notice)
  - [2. Mathematical Formulation — IPT / EPUSP Method (Helene & Terzian, 1992)](#2-mathematical-formulation--ipt--epusp-method-helene--terzian-1992)
  - [3. In-Memory SQLite Normative Database & ABNT NBR Rules](#3-in-memory-sqlite-normative-database--abnt-nbr-rules)
  - [4. Operational Workflow Across the 7 Graphical Tabs](#4-operational-workflow-across-the-7-graphical-tabs)
  - [5. Built-In Study Presets (0 to 5), Jobsite Measuring Boxes & Batches](#5-built-in-study-presets-0-to-5-jobsite-measuring-boxes--batches)
  - [6. Technical Reports (Markdown / HTML5 / PDF) & Persistence (`.cmdproj`)](#6-technical-reports-markdown--html5--pdf--persistence-cmdproj)
  - [7. Command-Line Interface (CLI) & Maintenance Utilities (`mocks/`)](#7-command-line-interface-cli--maintenance-utilities-mocks)
  - [8. Shortcuts, Local Preferences, Clean Uninstallation & Support](#8-shortcuts-local-preferences-clean-uninstallation--support)

---

# PARTE 1: MANUAL DE INSTRUÇÕES (PORTUGUÊS - BR)

## 1. Aviso de Escopo Auxiliar Gratuito e Licenciamento Duplo

> **ADVERTÊNCIA OBRIGATÓRIA DE ENGENHARIA E MATURIDADE DO SOFTWARE:**  
> O **Concrete Mix Designer** é uma ferramenta computacional **gratuita de caráter estritamente auxiliar** voltada ao estudo, ensino acadêmico e apoio à dosagem experimental de concreto de cimento Portland pelo **Método IPT/EPUSP**. **O programa ainda não apresenta maturidade suficiente para utilização definitiva em obras ou projetos executivos.** Em função de possíveis limitações algorítmicas, falhas humanas ou eventuais defeitos no código-fonte (abrangendo tanto a matemática aplicada quanto a tecnologia de materiais empregada), o programa é fornecido **"NO ESTADO EM QUE SE ENCONTRA" (*AS IS*)**, sem garantias de qualquer natureza. **Todos os resultados, traços unitários, padiolas e verificações normativas devem ser conferidos manualmente, confrontados com ferramentas consolidadas de mercado e validados obrigatoriamente por ensaios físicos de laboratório (ABNT NBR 5738 e NBR 5739)** sob responsabilidade exclusiva de profissional legalmente habilitado.

- **Duplo Licenciamento Transparente:**
  - **Código-fonte público ([`docs/licensing.md`](docs/licensing.md)):** Licenciado sob **GNU AGPLv3**, assegurando a desenvolvedores e pesquisadores plena liberdade de estudo, modificação, recompilação e adaptação (inclusive dos arquivos de documentação e avisos em `source/assets/` em suas próprias distribuições).
  - **Binário oficial assinado na Microsoft Store ([`EULA_pt_BR.txt`](source/assets/EULA/EULA_pt_BR.txt) + [`CLC_pt_BR.txt`](source/assets/CLC/CLC_pt_BR.txt)):** Regido pelo EULA Oficial do Autor e Esclarecimento Comercial, proibindo republicação paga do binário assinado da loja e respeitando integralmente a autonomia jurídica e técnica da Microsoft Corporation.

---

## 2. Fundamentos Matemáticos — Método IPT / EPUSP (Helene & Terzian, 1992)

### 2.1. Resistência de Dosagem ($f_{cj}$) e Condições de Preparo (ABNT NBR 12655:2022)
A resistência média à compressão esperada aos $j$ dias é determinada a partir da resistência característica especificada ($f_{ck}$, em MPa) e do desvio-padrão de dosagem ($S_d$, em MPa):
$$f_{cj} = f_{ck} + 1{,}65 \cdot S_d$$

| Condição de Preparo | Critério Técnico (NBR 12655) | $S_d$ Padrão |
|---|---|---|
| **Condição A** | Materiais dosados em massa; controle rigoroso de umidade | $4{,}0\text{ MPa}$ (ou valor experimental histórico) |
| **Condição B** | Cimento em massa; agregados em volume corrigido; água por hidrômetro | $5{,}5\text{ MPa}$ |
| **Condição C** | Cimento em saco inteiro; agregados em massa úmida sem ensaio frequente de umidade | $7{,}0\text{ MPa}$ |

### 2.2. Mistura Binária de Britas (`AggregateOptimizer`)
Quando ativada a composição entre Brita 1 e Brita 2, [`AggregateOptimizer`](source/core/AggregateOptimizer.cpp) calcula a massa unitária compactada combinada em recipiente normatizado (volume padrão $15{,}04\text{ L}$, conforme NBR NM 45) e o índice de vazios ao longo da rampa de mistura (0% a 100%), determinando o ponto ótimo de máximo empacotamento granular.

### 2.3. Teor Ideal de Argamassa ($\alpha$) e Amassada Piloto (`MortarContentOptimizer`)
Com base no volume útil de laboratório da betoneira ($V_{\text{betoneira}}$ em litros) e no traço piloto inicial $1 : m_0$ e relação água/cimento inicial $(a/c)_0$, calcula-se o rendimento da mistura pelos volumes absolutos:
$$V_s = \frac{1}{\gamma_c} + \frac{a}{\gamma_a} + \frac{p}{\gamma_b} + \frac{a/c}{\gamma_w}$$
onde $a = \alpha(1 + m_0) - 1$ e $p = m_0 - a$. A massa de cimento da amassada piloto é $M_{c,\text{piloto}} = \frac{V_{\text{betoneira}}}{V_s}$.

### 2.4. Regressões Experimentais (`RegressionEngine`) e Curvas do Diagrama IPT
A partir dos corpos de prova moldados em pelo menos 3 níveis de traço (ex.: Traço Rico $1:3{,}5$, Traço Normal/Auxiliar $1:5{,}0$ e Traço Pobre $1:6{,}5$), o motor ajusta por mínimos quadrados ordinários com pivoteamento parcial gaussiano:
1. **Curva de Abrams ($f_{cj}$ vs. $a/c$):** Polinômio de 2º grau e modelo exponencial de Abrams ($f_c = A / B^{a/c}$), determinando a relação $(a/c)_{\text{alvo}}$ por inversão quadrática ou bissecção no domínio físico.
2. **Lei de Lyse ($m$ vs. $a/c$):** Regressão linear $m = k_3 + k_4 (a/c)$, fornecendo o traço seco total $m_{\text{alvo}}$.
3. **Lei de Molinari ($m$ vs. $C$):** Polinômio de 2º grau relacionando $m$ e o consumo de cimento $C$ ($\text{kg/m}^3$), confrontado em paralelo com o consumo teórico por massa específica:
   $$C_{\text{teórico}} = \frac{1000}{\frac{1}{\gamma_c} + \frac{a}{\gamma_a} + \frac{p}{\gamma_b} + \frac{a/c}{\gamma_w}}$$

---

## 3. Banco Normativo SQLite em Memória e Regras ABNT NBR

Na inicialização, [`NormativeDatabase`](source/core/NormativeDatabase.cpp) popula um banco **SQLite 3 exclusivo em memória (`:memory:`)** protegido por `QRecursiveMutex`, eliminando arquivos de banco gravados no disco do usuário:
- **ABNT NBR 16697:2018:** Catálogo de cimentos (`CP I`, `CP II-E`, `CP II-Z`, `CP II-F`, `CP III`, `CP IV`, `CP V-ARI` e variantes `RS`/`BC`) com massas específicas e unitárias padrão.
- **ABNT NBR 7211:2022:** Agregados miúdos e graúdos (`Brita 0` a `Brita 3`) com dimensão máxima característica ($D_{\max}$).
- **ABNT NBR 12655:2022:** Classes de Agressividade Ambiental (**CAA I a IV**), relação $a/c$ máxima admissível, consumo mínimo de cimento ($C_{\min}$) e classe mínima de $f_{ck}$ para Concreto Armado (**CA**) e Concreto Protendido (**CP**).
- **ABNT NBR 6118:2023 (Tabela 7.2 e Item 18.3.2.2):** Cobrimento nominal mínimo de armadura ($c_{\text{nom}}$, com acréscimo de $+5\text{ mm}$ para CP quando aplicável) e verificação automática $D_{\max} \le 1{,}2 \cdot c_{\text{nom}}$.
- **ABNT NBR 8953:2015:** Classes de resistência estrutural (`C20` a `C100`) e abatimentos (`S10` a `S220`).
- **ABNT NBR 7212:2025 (Item 5.5):** Tolerâncias de pesagem em central dosadora (cimento $\pm 1\%$, agregados $\pm 2\%$, água e aditivos líquidos $\pm 1{,}5\%$ ou conforme norma).

---

## 4. Fluxo Operacional nas 7 Abas da Interface Gráfica

1. **Aba 1 — Materiais e Normas:** Seleção de cimento, areia (massa específica, massa unitária, umidade superficial e inchamento), brita(s), classe de resistência ($f_{ck}$), tipo estrutural (CA/CP), classe CAA, condição de preparo (A/B/C), abatimento e preços unitários ($\text{R\$}$ ou US$\$$ conforme idioma).
2. **Aba 2 — Dosagem Inicial e Mistura de Britas:** Tabela interativa de empacotamento granular (mistura binária de Brita 1 + Brita 2 no recipiente de $15{,}04\text{ L}$) e cálculo de massa da amassada piloto na betoneira.
3. **Aba 3 — Ensaios Experimentais (IPT / EPUSP):** Registro do ensaio de determinação do teor ideal de argamassa ($\alpha$) e das 3 famílias de traços experimentais ($a/c$, consumo $C$ e resistências $f_{c,7}/f_{c,28}$ com desvio amostral).
4. **Aba 4 — Traços & Diagrama IPT:** Visualização simultânea dos 4 quadrantes clássicos do método IPT/EPUSP (`QuadChartWidget`) e do diagrama unificado interativo (`IptUnifiedDiagramWidget`) com exportação de imagens em alta resolução (`.png`) em Tema Claro ou Escuro.
5. **Aba 5 — Traço Final & Amassadas:** Apresentação do traço unitário seco ($1:a:p$), correção automática pela umidade da areia e das britas, consumos por $1\text{ m}^3$, cálculo das padiolas de canteiro (base $35 \times 45\text{ cm}$ para sacos de $50\text{ kg}$ de cimento) e ajuste para o volume total da betoneira ou obra.
6. **Aba 6 — Análise de Custos:** Comparativo econômico entre o traço analítico (Molinari/consumo teórico) e o experimental ajustado, detalhando custos por componente, por metro cúbico e para o volume total da obra.
7. **Aba 7 — Relatório Técnico:** Pré-visualização formatada do memorial de cálculo completo em tempo real e exportação direta para **HTML5**, **Markdown (`.md`)** e **PDF** (via `QPdfWriter`).

### 4.1. Janela **Sobre** (`Ajuda > Sobre o Módulo de Dosagem de Concreto` — 9 Abas)
A caixa de diálogo modeless **Sobre** organiza documentação institucional, conformidade e textos integrais de licenciamento em **9 abas navegáveis** com pesquisa global (`Pesquisar:`) e recarga automática ao trocar o idioma em **Configurações > Idioma**:
- **Detalhes, Histórico, Licença (`EULA`), Avisos, Privacidade e Notas de Lançamento:** Informações do aplicativo, evolução por versão, termos da Microsoft Store, atribuições de terceiros e Política de Privacidade Local-First.
- **CLC / Licenciamento:** Exibe o Esclarecimento de Licenciamento Comercial completo (`CLC_pt_BR.txt` / `CLC_en_US.txt`), política de suporte exclusivo do autor e isenção de vínculo da Microsoft Corporation.
- **Copyright:** Exibe os avisos de direitos autorais, marcas registradas nominais e condições de uso da documentação (`AVISO DE COPYRIGHT E MARCA REGISTRADA_pt_BR.txt` / `COPYRIGHT AND TRADEMARK NOTICE_en_US.txt`).
- **Licenças Completas (Lista Suspensa):** Apresenta a lista suspensa **`Selecione a licença:`** com todas as **11 licenças completas** de software livre e componentes de terceiros embarcadas offline; basta selecionar qualquer licença na lista para lê-la na íntegra ou pesquisar termos (preservando a seleção ao alternar o idioma).

---

## 5. Cenários Pré-Configurados (0 a 5), Padiolas e Amassadas de Obra

Acessíveis em **Configurações > Cenário da Planilha**:
- **Cenário 0:** Personalizável pelo usuário (mantém parâmetros livres para entrada manual).
- **Cenário 1:** Concreto C25 Urbano (CAA II) — CP II-F-32, Slump 100 mm, Obra $30\text{ m}^3$.
- **Cenário 2:** Concreto C30 Marinho (CAA III) — CP III-40 RS, Britas 1+2, Obra $50\text{ m}^3$.
- **Cenário 3:** Concreto C40 Alto Desempenho (CAA IV) — CP V-ARI, Slump 160 mm, Obra $100\text{ m}^3$.
- **Cenário 4:** Concreto C20 Fundações (CAA I — Condição B) — CP II-Z-32, Brita 2, Obra $40\text{ m}^3$.
- **Cenário 5:** Planilha UFPR Original Corrigida (TC031 — C30, $S_d = 3{,}5\text{ MPa}$, $\alpha = 52\%$, Obra $30\text{ m}^3$).

---

## 6. Relatórios Técnicos (Markdown / HTML5 / PDF) e Persistência (`.cmdproj`)

- **Arquivos `.cmdproj`:** Formato JSON legível contendo materiais, agregados, pontos experimentais, metadados do estudo (*Projeto*, *Responsável*, *Instituição*, *ID*) e parâmetros normativos, totalmente compatível entre a interface gráfica e a CLI.
- **Cabeçalho Inteligente:** Em **Configurações > Dados do Projeto e Identificação (`Ctrl+I`)**, alterne entre reportar automaticamente o nome do cenário selecionado ou editar manualmente os dados do responsável e da obra.
- **Sanitização HTML:** Toda saída HTML5 utiliza escape explícito (`escapeHtml()`) sobre campos textuais para prevenir injeção de código em visualizadores e navegadores.

---

## 7. Interface de Linha de Comando (CLI) e Ferramentas Auxiliares (`mocks/`)

O executável **`concrete-mix-designer-cli.exe`** permite automação sem interface gráfica:
```powershell
# Dimensionamento rápido com preset e parâmetros customizados (saída JSON)
.\concrete-mix-designer-cli.exe design --preset 5 --fck 35 --slump 120 --format json

# Recálculo a partir de projeto salvo .cmdproj
.\concrete-mix-designer-cli.exe calculate estudo.cmdproj --format text

# Auditoria de conformidade ABNT NBR
.\concrete-mix-designer-cli.exe verify estudo.cmdproj --format json

# Geração automatizada de memorial técnico HTML5 ou Markdown
.\concrete-mix-designer-cli.exe report estudo.cmdproj --format html --out memorial.html --lang pt_BR

# Consulta ao banco SQLite em memória (cimentos, agregados, abatimentos, tolerâncias)
.\concrete-mix-designer-cli.exe standards all --format json
```
Consulte o guia completo em [`docs/cli-reference.md`](docs/cli-reference.md).

Utilitários de desenvolvimento e manutenção em `mocks/`:
- **`find_VersionEditor.exe`:** Auditoria (`--check .`) e sincronização (`--apply .`) de versão e data em `main.cpp`, `source/cli/main_cli.cpp`, `source/gui/SobreDialog.cpp`, `source/assets/`, `docs/`, `library/` (`AppxManifest.xml`, `.iss`, `Concrete_Mix_Designer_cpp.txt`) e arquivos Markdown da raiz.
- **`CompileTranslationsTerminal.exe` / `CompileTranslationsGUI.exe`:** Validação e compilação de catálogos Qt Linguist (`.ts` $\to$ `.qm`).

---

## 8. Atalhos, Preferências Locais, Desinstalação Limpa e Suporte

- **Atalhos Confirmados na Janela Principal:**
  - `F5` (*Refresh*): Recalcular módulo e atualizar gráficos IPT;
  - `Ctrl+I`: Abrir janela de Dados do Projeto e Identificação;
  - `F1` (*HelpContents*): Abrir Manual do Usuário;
  - Atalho padrão de saída (*Quit*): Encerrar aplicação e janelas auxiliares *modeless*.
- **Idioma Dinâmico:** Menu **Configurações > Idioma** alterna em tempo real entre `Português (BR)` (`pt_BR`, moeda `R$`) e `English (US)` (`en_US`, moeda `$`) via `.qm` sem reiniciar o programa.
- **Armazenamento Local & Desinstalação Limpa (Política 10.2.7 Microsoft Store):**
  - Preferências gravadas via `QSettings("AppsCpp", "ConcreteMixDesign")` (ou hive isolado no MSIX) e logs técnicos locais em `%LOCALAPPDATA%\ConcreteMixDesigner\logs\`. Nenhum serviço em segundo plano é registrado e nenhum dado sai do computador.
  - Em instalações tradicionais via Inno Setup, utilize *Configurações > Aplicativos* do Windows; eventuais pastas residuais de logs em `%LOCALAPPDATA%\ConcreteMixDesigner` podem ser apagadas manualmente sem deixar chaves órfãs no sistema.
- **Contato com o Autor / Issues:** [`linceu_lighthouse@outlook.com`](mailto:linceu_lighthouse@outlook.com) ou [GitHub Issues](https://github.com/fernandoncidade/Concrete_Mix_Designer_cpp/issues). A Microsoft Corporation não fornece suporte técnico nem manutenção para este aplicativo ([`CLC_pt_BR.txt`](source/assets/CLC/CLC_pt_BR.txt)).

---

# PART 2: OPERATIONS MANUAL & CIVIL ENGINEERING GUIDE (ENGLISH - US)

## 1. Free Auxiliary Engineering Scope & Dual-Licensing Notice

> **MANDATORY ENGINEERING & SOFTWARE MATURITY DISCLAIMER:**  
> **Concrete Mix Designer** is a **free, strictly auxiliary engineering computation tool** designed for academic study, education, and laboratory mix-design support under the **IPT/EPUSP Method**. **The program is NOT yet mature enough for definitive production deployment or unsupervised structural construction use.** Owing to potential algorithmic simplifications, human error, or defects in the source code (spanning both applied mathematics and underlying materials technology), the software is provided **"AS IS"**, without warranty of any kind. **All mix ratios, water/cement factors, jobsite measuring boxes, and ABNT NBR checks must be independently verified by hand, cross-checked against established commercial software, and validated through physical laboratory cylinder testing (ABNT NBR 5738 and NBR 5739)** under the sole responsibility of a licensed civil engineer.

- **Transparent Dual Licensing:**
  - **Public Source Code ([`docs/licensing.md`](docs/licensing.md#enus)):** Licensed under **GNU AGPLv3**, granting developers full freedom to inspect, fork, recompile, and adapt the codebase (including modifying or removing files inside `source/assets/` in their own downstream forks/builds).
  - **Official Signed Microsoft Store Binary ([`EULA_en_US.txt`](source/assets/EULA/EULA_en_US.txt) + [`CLC_en_US.txt`](source/assets/CLC/CLC_en_US.txt)):** Distributed free of charge under the Author's Official EULA and Commercial Licensing Clarification, prohibiting paid commercial repackaging of the signed Store package while preserving complete legal and technical independence from Microsoft Corporation.

---

## 2. Mathematical Formulation — IPT / EPUSP Method (Helene & Terzian, 1992)

### 2.1. Target Mean Strength ($f_{cj}$) & Preparation Conditions (ABNT NBR 12655:2022)
The required average 28-day compressive strength is derived from specified characteristic strength ($f_{ck}$, MPa) and batching standard deviation ($S_d$, MPa):
$$f_{cj} = f_{ck} + 1.65 \cdot S_d$$

| Preparation Condition | Technical Criterion (NBR 12655) | Default $S_d$ |
|---|---|---|
| **Condition A** | All materials batched by mass; continuous moisture control | $4.0\text{ MPa}$ (or historical plant $S_d$) |
| **Condition B** | Cement by mass; aggregates by moisture-corrected volume; water by meter | $5.5\text{ MPa}$ |
| **Condition C** | Whole-bag cement; aggregates by damp mass without frequent moisture tests | $7.0\text{ MPa}$ |

### 2.2. Binary Coarse Aggregate Optimization (`AggregateOptimizer`)
When coarse aggregate blending is enabled, [`AggregateOptimizer`](source/core/AggregateOptimizer.cpp) evaluates compacted unit mass inside a standardized $15.04\text{ L}$ container (NBR NM 45) across mixing ratios from 0% to 100%, identifying minimum void ratio and peak packing density.

### 2.3. Optimal Mortar Content ($\alpha$) & Pilot Batch (`MortarContentOptimizer`)
From laboratory drum mixer capacity ($V_{\text{mixer}}$ in liters), initial mix $1 : m_0$, and initial $(w/c)_0$, absolute-volume specific yield is computed as:
$$V_s = \frac{1}{\gamma_c} + \frac{a}{\gamma_a} + \frac{p}{\gamma_b} + \frac{w/c}{\gamma_w}$$
with $a = \alpha(1 + m_0) - 1$ and $p = m_0 - a$, yielding pilot cement mass $M_{c,\text{pilot}} = \frac{V_{\text{mixer}}}{V_s}$.

### 2.4. Least-Squares Regressions (`RegressionEngine`) & IPT Curves
Using laboratory trial batches spanning at least three mix richness levels (e.g., Rich $1:3.5$, Normal/Auxiliary $1:5.0$, Lean $1:6.5$), Gaussian partial-pivoting least squares fits:
1. **Abrams' Law ($f_{cj}$ vs. $w/c$):** 2nd-degree polynomial and exponential Abrams curve ($f_c = A / B^{w/c}$), inverted analytically/numerically to solve $(w/c)_{\text{target}}$.
2. **Lyse's Law ($m$ vs. $w/c$):** Linear regression $m = k_3 + k_4 (w/c)$ solving total dry aggregate ratio $m_{\text{target}}$.
3. **Molinari's Law ($m$ vs. $C$):** Quadratic fit calibrated against specific-gravity theoretical cement consumption:
   $$C_{\text{theoretical}} = \frac{1000}{\frac{1}{\gamma_c} + \frac{a}{\gamma_a} + \frac{p}{\gamma_b} + \frac{w/c}{\gamma_w}}$$

---

## 3. In-Memory SQLite Normative Database & ABNT NBR Rules

At startup, [`NormativeDatabase`](source/core/NormativeDatabase.cpp) initializes a volatile **in-memory SQLite 3 database (`:memory:`)** guarded by `QRecursiveMutex`:
- **ABNT NBR 16697:2018:** Portland cement types (`CP I`, `CP II-E`, `CP II-Z`, `CP II-F`, `CP III`, `CP IV`, `CP V-ARI`, `RS`/`BC`) with default densities and bulk unit masses.
- **ABNT NBR 7211:2022:** Fine and coarse aggregate grading tables (`Brita 0`–`Brita 3`) with Maximum Characteristic Dimension ($D_{\max}$).
- **ABNT NBR 12655:2022:** Environmental Aggressiveness Classes (**CAA I–IV**), maximum permissible $w/c$, minimum cement content $C_{\min}$, and minimum $f_{ck}$ for Reinforced (**CA**) and Prestressed (**CP**) Concrete.
- **ABNT NBR 6118:2023 (Table 7.2 & Item 18.3.2.2):** Nominal cover verification ($c_{\text{nom}}$ with $+5\text{ mm}$ prestressing increment where applicable) and rebar clearance constraint $D_{\max} \le 1.2 \cdot c_{\text{nom}}$.
- **ABNT NBR 8953:2015:** Concrete strength grades (`C20`–`C100`) and slump classes (`S10`–`S220`).
- **ABNT NBR 7212:2025 (Item 5.5):** Batching plant weighing tolerances.

---

## 4. Operational Workflow Across the 7 Graphical Tabs

1. **Tab 1 — Materials & Standards:** Configure cement type, fine aggregate (specific gravity, unit mass, moisture, bulking), coarse aggregate(s), $f_{ck}$, structural type (CA/CP), CAA class, preparation condition (A/B/C), target slump, and unit material pricing (`$` or `R$`).
2. **Tab 2 — Initial Mix & Coarse Blend:** Interactive binary coarse-aggregate packing table ($15.04\text{ L}$ vessel) and pilot mixer batch calculation.
3. **Tab 3 — Experimental Trials (IPT / EPUSP):** Entry of mortar content ($\alpha$) determination trial and experimental mix families ($w/c$, cement content $C$, 7/28-day specimen strengths).
4. **Tab 4 — Mix Ratios & IPT Diagram:** Dual synchronized charting via `QuadChartWidget` (4-quadrant analytical curves) and `IptUnifiedDiagramWidget` (zoomable unified IPT chart with Light/Dark PNG export).
5. **Tab 5 — Final Mix & Jobsite Batches:** Unit dry proportions ($1:a:p$), moisture correction for sand/gravel, $1\text{ m}^3$ batch weights, jobsite measuring boxes ($35 \times 45\text{ cm}$ base for $50\text{ kg}$ cement bags), and custom batch scaling.
6. **Tab 6 — Cost Analysis:** Side-by-side analytical vs. experimental cost breakdown per component, per $\text{m}^3$, and for total project volume.
7. **Tab 7 — Technical Report:** Live formatted report preview with one-click export to **HTML5**, **Markdown (`.md`)**, and **PDF** (via `QPdfWriter`).

### 4.1. **About** Window (`Help > About the Concrete Mix Design Module` — 9 Tabs)
The modeless **About** dialog organizes institutional disclosures, legal compliance, and complete open-source license texts across **9 tabs** with unified text search (`Search:`) and live reload when switching languages under **Settings > Language**:
- **Details, Changelog, License (`EULA`), Notices, Privacy & Release Notes:** Scope and free auxiliary notice, version history, official Microsoft Store EULA, third-party attributions, and 100% local-first Privacy Policy.
- **CLC / Licensing:** Displays the complete Commercial Licensing Clarification (`CLC_en_US.txt` / `CLC_pt_BR.txt`), exclusive author support policy, and Microsoft Corporation non-coverage notice.
- **Copyright:** Displays authorship notices, copyright ownership, referential ABNT/IPT citations, and trademark conditions (`COPYRIGHT AND TRADEMARK NOTICE_en_US.txt` / `AVISO DE COPYRIGHT E MARCA REGISTRADA_pt_BR.txt`).
- **Full Licenses (Dropdown Selector):** Provides the **`Select license:`** dropdown containing all **11 full open-source and third-party license texts** bundled offline; select any entry to inspect the full text or search across terms (selection persists across live language switches).

---

## 5. Built-In Study Presets (0 to 5), Jobsite Measuring Boxes & Batches

Available under **Settings > Spreadsheet Scenario**:
- **Preset 0:** User-Customizable Baseline.
- **Preset 1:** Urban C25 Concrete (CAA II) — CP II-F-32, Slump 100 mm, $30\text{ m}^3$.
- **Preset 2:** Marine C30 Concrete (CAA III) — CP III-40 RS, Coarse Blend 1+2, $50\text{ m}^3$.
- **Preset 3:** High-Performance C40 Concrete (CAA IV) — CP V-ARI, Slump 160 mm, $100\text{ m}^3$.
- **Preset 4:** Foundations C20 Concrete (CAA I — Cond. B) — CP II-Z-32, Gravel 2, $40\text{ m}^3$.
- **Preset 5:** Corrected Original UFPR Spreadsheet (TC031 — C30, $S_d = 3.5\text{ MPa}$, $\alpha = 52\%$, $30\text{ m}^3$).

---

## 6. Technical Reports (Markdown / HTML5 / PDF) & Persistence (`.cmdproj`)

- **`.cmdproj` Project Files:** Portable UTF-8 JSON schema interoperable between the GUI and CLI.
- **Metadata Header Modes (`Ctrl+I`):** Switch dynamically between scenario titles and custom Project / Engineer / Institution / ID metadata.
- **XSS-Safe HTML5 Export:** All user-editable strings are sanitized through `escapeHtml()` before rendering.

---

## 7. Command-Line Interface (CLI) & Maintenance Utilities (`mocks/`)

`concrete-mix-designer-cli.exe` provides headless batch engineering automation:
```powershell
.\concrete-mix-designer-cli.exe design --preset 5 --fck 35 --slump 120 --format json
.\concrete-mix-designer-cli.exe calculate study.cmdproj --format text
.\concrete-mix-designer-cli.exe verify study.cmdproj --format json
.\concrete-mix-designer-cli.exe report study.cmdproj --format html --out report.html --lang en_US
.\concrete-mix-designer-cli.exe standards all --format json
```
See [`docs/cli-reference.md`](docs/cli-reference.md#enus) for complete flag specifications.

Development helpers in `mocks/`:
- **`find_VersionEditor.exe`:** Read-only (`--check .`) and synchronized (`--apply .`) version/date auditing across C++ sources, assets, docs, `library/`, and root `.md` files.
- **`CompileTranslationsTerminal.exe` / `CompileTranslationsGUI.exe`:** Qt Linguist `.ts` $\to$ `.qm` validation and compilation.

---

## 8. Shortcuts, Local Preferences, Clean Uninstallation & Support

- **Confirmed Main Window Shortcuts:**
  - `F5` (*Refresh*): Recalculate all modules and redraw IPT diagrams;
  - `Ctrl+I`: Open Project Metadata & Identification dialog;
  - `F1` (*HelpContents*): Open User Manual;
  - Standard system *Quit* shortcut: Close main window and child modeless dialogs.
- **Dynamic Runtime Language Switching:** Toggle between `Português (BR)` (`pt_BR`, `R$`) and `English (US)` (`en_US`, `$`) under **Settings > Language** without restarting.
- **Local Storage & Clean Uninstall (Microsoft Store Policy 10.2.7):**
  - Preferences persist solely in `QSettings("AppsCpp", "ConcreteMixDesign")` (or MSIX container hive) and diagnostic logs in `%LOCALAPPDATA%\ConcreteMixDesigner\logs\`. Zero network telemetry, zero background daemons.
  - Uninstall via *Windows Settings > Apps*; optional log files under `%LOCALAPPDATA%\ConcreteMixDesigner` can be removed cleanly without registry leftovers.
- **Author Support & Issue Tracker:** [`linceu_lighthouse@outlook.com`](mailto:linceu_lighthouse@outlook.com) or [GitHub Issues](https://github.com/fernandoncidade/Concrete_Mix_Designer_cpp/issues). Microsoft Corporation provides no technical support or maintenance for this application ([`CLC_en_US.txt`](source/assets/CLC/CLC_en_US.txt)).
