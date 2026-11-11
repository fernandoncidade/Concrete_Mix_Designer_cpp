# Manual do Usuário / User Manual — Concrete Mix Design C++

[Português (Brasil)](#português-brasil) | [English (United States)](#english-united-states)

---

## Português (Brasil)

### 1. Visão Geral e Fundamentação Científica

O **Concrete Mix Design C++ (v2026.11.11.0)** é um sistema computacional nativo de alto desempenho desenvolvido em **C++20** e **Qt 6.11.1**, voltado exclusivamente para fins de **estudo acadêmico, ensino universitário e pesquisa teórica** de **Dosagem Experimental e Analítica de Concreto de Cimento Portland** segundo o **Método IPT/EPUSP** (*Instituto de Pesquisas Tecnológicas / Escola Politécnica da USP* — Paulo Roberto do Lago Helene e Paulo Terzian, 1992), integrado às exigências obrigatórias das normas brasileiras vigentes da **ABNT** e aos princípios de ciência dos materiais de **Mehta & Monteiro (2014)**.

O aplicativo substitui o traçado manual sujeito a erros visuais de leitura em ábacos de papel milimetrado por regressões numéricas exatas em ponto flutuante de dupla precisão (`double` IEEE 754), realizando a verificação automática de durabilidade normativa a partir de um banco de dados relacional **SQLite 3 embarcado em memória (`:memory:`)**.

---

### 2. Barra de Menus, Barra de Ferramentas e Menu "Configurações"

#### 2.1. Barra de Menus Principal
- **Arquivo (`&Arquivo`):**
  - `🔄 Recalcular Tudo (F5)`: Executa imediatamente o motor analítico completo e atualiza todas as 7 abas, o Diagrama de 4 Quadrantes, o rodapé de status e o Memorial Técnico.
  - `Sair (Ctrl+Q)`: Encerra o aplicativo com segurança.
- **Exibir (`&Exibir`):** Atalhos diretos de navegação para cada uma das 7 abas de trabalho.
- **Configurações (`&Configurações`):**
  - **Submenu `Configurações > 📂 Cenário da Planilha:`:** Submenu integrado ao topo do menu Configurações para seleção imediata entre os Cenários 0 a 5 (com marcação visual do cenário ativo e tradução dinâmica em tempo de execução).
  - `📝 Dados do Projeto e Identificação (Projeto, Responsável, Instituição, ID)... (Ctrl+I)`: Abre a janela modal de configuração dos metadados institucionais reportados no cabeçalho do relatório técnico (**Aba 7**).
  - `📂 Projeto: Reportar 'Cenário da Planilha' Automaticamente`: Quando selecionado (padrão), o campo **Projeto:** do relatório técnico exibe automaticamente o título do cenário selecionado no submenu **📂 Cenário da Planilha:**, traduzindo-o dinamicamente em tempo de execução ao alternar entre Português (`pt_BR`) e Inglês (`en_US`).
  - `✏️ Projeto: Editar Informações Manualmente...`: Permite definir um nome personalizado para **Projeto:**, além de editar livremente **Responsável:**, **Instituição:** e **Matrícula / ID:**, com persistência automática via `QSettings` e opção **Restaurar Padrão**.
  - **Submenu `Configurações > 🌐 Idioma (🌐 &Idioma)`:** Alternância instantânea em tempo de execução entre `🇧🇷 Português (BR) - pt_BR` e `🇺🇸 English (US) - en_US`, atualizando automaticamente toda a interface gráfica, o memorial de cálculo, os símbolos monetários (`R$` / `$`) e os documentos legais sem reiniciar o programa.
- **Ajuda (`A&juda`):**
  - `📖 Manual do Usuário (F1)`: Abre o manual interativo bilíngue integrado ao aplicativo.
  - `📋 Normas Técnicas (NBR)`: Exibe o quadro resumo das normas ABNT NBR aplicadas.
  - `ℹ️ Sobre o Concrete Mix Design`: Exibe informações institucionais, histórico do aplicativo, licença de uso (EULA), avisos de terceiros (LGPLv3), política de privacidade e notas de lançamento.

#### 2.2. Seletor de Cenários de Engenharia ("Cenário da Planilha")
O seletor **Cenário da Planilha:** no menu **Configurações** oferece **6 cenários pré-configurados** com tradução dinâmica em tempo de execução:
- **Cenário 0:** *Personalizável pelo usuário* (inicia com campos prontos para inserção livre de um novo estudo de laboratório).
- **Cenário 1:** *Concreto C25 Urbano (CAA II) - CP II-F-32, Slump 100mm, Obra 30 m³*.
- **Cenário 2:** *Concreto C30 Marinho (CAA III) - CP III-40 RS, Britas 1+2, Obra 50 m³*.
- **Cenário 3:** *Concreto C40 Alto Desempenho (CAA IV) - CP V-ARI, Slump 160mm, Obra 100 m³*.
- **Cenário 4:** *Concreto C20 Fundações (CAA I - Condição B) - CP II-Z-32, Brita 2, Obra 40 m³*.
- **Cenário 5:** *Planilha UFPR Original Corrigida (TC031 - C30, Sd 3,5 MPa, α 52%, Obra 30 m³)* — reproduz com exatidão matemática o estudo experimental da disciplina TC031 da UFPR após auditoria normativa e correção de fórmulas.

> **Nota sobre Edição nos Campos Numéricos (`QDoubleSpinBox`):** Todos os campos numéricos permitem digitação contínua e fluida sem preenchimento automático intrusivo de zeros à direita a cada dígito inserido; a formatação decimal padrão ocorre apenas ao concluir a edição do campo.

---

### 3. Guia Passo a Passo das 7 Abas de Trabalho

#### Aba 1: `1. Parâmetros de Projeto`
Define os requisitos estruturais e de durabilidade segundo a **ABNT NBR 12655:2022**, **ABNT NBR 6118:2023** e **ABNT NBR 8953:2015**:
1. **Resistência Característica ($f_{ck}$):** De $10,0\text{ MPa}$ a $100,0\text{ MPa}$ (Grupos I e II da NBR 8953).
2. **Condição de Preparo e Desvio-Padrão ($S_d$):**
   - **Condição A ($S_d = 4,0\text{ MPa}$):** Aplicável a todas as classes; cimento e agregados medidos em massa, água medida em massa ou volume com correção da umidade dos agregados.
   - **Condição B ($S_d = 5,5\text{ MPa}$):** Aplicável às classes C10 a C20; cimento medido em massa, agregados medidos em massa combinada com volume.
   - **Condição C ($S_d = 7,0\text{ MPa}$):** Aplicável apenas às classes C10 e C15; cimento medido em massa, agregados medidos em volume.
   - **Desvio Conhecido Histórico ($S_d \ge 2,0\text{ MPa}$):** Para concreto produzido com os mesmos materiais e equipamentos com registro estatístico de $n \ge 20$ exemplares consecutivos.
   - **Laboratório / Arbitrário (Personalizado):** Permite informar qualquer valor de $S_d$ experimental (como $S_d = 3,5\text{ MPa}$ no Cenário 5 da UFPR).
3. **Resistência de Dosagem aos 28 Dias ($f_{cj}$):**
   $$f_{cj} = f_{ck} + 1,65 \cdot S_d$$
4. **Classe de Agressividade Ambiental (CAA I a IV) e Tipo de Estrutura (CA / CP):**
   Consulta automaticamente a Tabela 6 da NBR 12655:2022 e a Tabela 7.2 da NBR 6118:2023, exibindo a relação $a/c$ máxima, a classe mínima de resistência, o consumo mínimo de cimento ($C_{\min}$), o cobrimento nominal ($c_{\text{nom}}$ para lajes e vigas/pilares) e a verificação geométrica da dimensão máxima característica do agregado graúdo ($D_{\max} \le 1,2 \cdot c_{\text{nom}}$ conforme NBR 6118 item 7.4.7.6).
5. **Tolerâncias Operacionais de Pesagem em Central (ABNT NBR 7212:2025, item 5.5):**
   - Cimento e Adições: $\pm 1\%$ | Agregados: $\pm 3\%$ | Água: $\pm 3\%$ | Aditivos: $\pm 5\%$.

#### Aba 2: `2. Materiais & Britas`
1. **Caracterização Físico-Mecânica (ABNT NBR 7211:2022):**
   Informe para o **Cimento Portland**, **Areia Natural**, **Brita 1** e **Brita 2**:
   - Identificação / Tipo (ou DMC para as britas: $9,5\text{ mm}$, $19,0\text{ mm}$, $25,0\text{ mm}$ ou $37,5\text{ mm}$);
   - Massa específica real ($\gamma$, em $\text{kg/dm}^3$);
   - Massa unitária aparente solta ($\delta$, em $\text{kg/dm}^3$);
   - Umidade superficial de campo ($h$, em $\%$);
   - Custo unitário (com seletor de unidade para o cimento: $\text{R\$/sc}$ de $50\text{ kg}$, $\text{R\$/t}$ ou $\text{R\$/kg}$; e $\text{R\$/t}$ para areia e britas).
2. **Ensaio de Otimização da Mistura de Britas (Empacotamento Granular — NBR NM 45):**
   Avalia 11 proporções de mistura binária (de $0\%$ a $100\%$ de Brita 2 com passos de $10\%$) para uma massa base fixa de Brita 2 e um volume de recipiente calibrado, identificando automaticamente a composição ótima que maximiza a massa unitária compactada ($\text{kg/dm}^3$) e minimiza o índice de vazios intergranulares.

#### Aba 3: `3. Teor de Argamassa (α)`
Simula o ensaio experimental de laboratório para determinação do teor ideal de argamassa seca ($\alpha$, de $40\%$ a $68\%$ com passo de $2\%$) mantendo fixa a massa de brita ($M_b$) na betoneira para o traço piloto ($m$):
$$\alpha = \frac{1 + a}{1 + m} \implies a = \alpha \cdot (1 + m) - 1, \qquad p = m - a$$
$$M_c = \frac{M_b}{p}, \qquad M_a = a \cdot M_c$$
A tabela calcula para cada linha os valores de $a$, $p$, areia total, acréscimo de areia, cimento total e acréscimo de cimento. Ao clicar em uma linha da tabela ou ajustar o campo **Teor Ideal Adotado α (%)**, o valor de $\alpha$ é imediatamente aplicado a todo o estudo de dosagem.

#### Aba 4: `4. Traços & Diagrama IPT`
1. **Dados dos 3 Traços Experimentais Confeccionados em Laboratório:**
   Insira para o **Traço 1 (Rico)**, **Traço 2 (Piloto)** e **Traço 3 (Pobre)** o traço seco total ($m$), a relação água/cimento ($a/c$), a resistência medida aos 28 dias ($f_{cj}$, em $\text{MPa}$), o abatimento (*Slump*, em $\text{mm}$) e a massa específica do concreto fresco ($\gamma_{\text{conc}}$, em $\text{kg/dm}^3$). O consumo experimental de cimento é atualizado automaticamente por:
   $$C_{\text{exp}} = \frac{1000 \cdot \gamma_{\text{conc}}}{1 + m + a/c}$$
2. **Ajuste Analítico por Mínimos Quadrados Ordinários (OLS):**
   - **1º Quadrante — Lei de Abrams:** $f_{cj} = \dfrac{k_1}{k_2^{(a/c)}} \implies (a/c)_{\text{calc}} = \dfrac{\ln(k_1 / f_{cj,\text{alvo}})}{\ln(k_2)}$, aplicando a restrição normativa de durabilidade $(a/c)_{\text{adotado}} = \min\big((a/c)_{\text{calc}},\, (a/c)_{\max,\text{NBR}}\big)$.
   - **2º Quadrante — Lei de Lyse:** $m = k_3 + k_4 \cdot (a/c) \implies H = 1000 \cdot \dfrac{a/c}{k_5 + k_6 \cdot m}$.
   - **3º Quadrante — Lei de Molinari:** $C = \dfrac{1000}{k_5 + k_6 \cdot m}$, verificando $C \ge C_{\min,\text{NBR}}$.
   - **4º Quadrante — Consumo de Cimento × Custo por $\text{m}^3$:** Correlaciona o consumo de cimento com o custo unitário total dos insumos por metro cúbico de concreto.
3. **Diagrama Vetorial Interativo:** Permite alternar entre 6 modos de visualização avançada através da caixa de seleção:
   - *Diagrama Completo de 4 Quadrantes (IPT)*: Visão panorâmica dos quatro quadrantes em arranjo matricial;
   - *1º Quad: Abrams (fcj x a/c)*: Curva de dosagem da resistência em função da relação água/cimento;
   - *2º Quad: Lyse (m x a/c)*: Proporção agregado/cimento a abatimento constante;
   - *3º Quad: Molinari (m x C)*: Hipérbole de consumo de cimento por metro cúbico;
   - *4º Quad: Consumo e Custo*: Correlação entre custo financeiro por m³ e resistência à compressão;
   - *5° Abrams/Lyse/Molinari (IPT)*: Nova composição cartesiana contínua dos quatro quadrantes integrada em um único plano com fundo sólido limpo sem gradientes reflexivos, malha de grade de alta precisão métrica perfeitamente calibrada, escalas visíveis de fácil leitura e suporte a exportação gráfica em alta resolução (`.png` / `.pdf`).

#### Aba 5: `5. Traço Final & Amassadas`
Apresenta quatro quadros operacionais completos:
1. **Síntese do Traço Unitário Final:** Traço unitário em massa seca ($1 : a : p : a/c$) e traço corrigido para a umidade de campo ($1 : a_{\text{úm}} : p_{\text{úm}} : (a/c)_{\text{efetivo}}$).
2. **Consumo de Materiais por Metro Cúbico ($1\text{ m}^3$):** Massa seca ($\text{kg}$), umidade superficial ($\%$), massa úmida corrigida ($\text{kg}$) e volume aparente úmido ($\text{m}^3$ ou $\text{L}$) para Cimento, Areia, Brita 1, Brita 2 e Água de Amassamento.
3. **Amassada de Laboratório para o Volume do Misturador (Betoneira):** Dimensiona as massas secas e massas efetivas a pesar (descontando a água livre contida nos agregados úmidos) para qualquer volume de betoneira em litros.
4. **Dosagem em Canteiro de Obra (1 Saco de $50\text{ kg}$ de Cimento — Padiolas):**
   Converte o traço para múltiplos de 1 saco fechado de cimento ($50\text{ kg}$) e dimensiona a quantidade e a altura interna ($h$, em $\text{cm}$) de padiolas de madeira com base padrão de $35\text{ cm} \times 45\text{ cm}$ ($\text{área} = 15,75\text{ dm}^2$), limitando a altura máxima ergonômica a $35\text{ cm}$ por caixa.

#### Aba 6: `6. Análise de Custos`
Compara lado a lado a **Solução Analítica Exata (Equações de Regressão)** com a **Leitura Gráfica Manual (Ábaco)** para o volume total da obra ($\text{m}^3$), quantificando:
- Consumo de cimento ($\text{kg/m}^3$), custo de cimento, areia e brita ($\text{R\$/m}^3$ ou $\text{\$/m}^3$) e custo total por $\text{m}^3$;
- Custo total da obra, consumo total de cimento ($\text{kg}$) e total de sacos de $50\text{ kg}$;
- Diferença financeira absoluta ($|\Delta|$) e economia/precisão proporcionada pela solução analítica.

#### Aba 7: `7. Relatório Técnico`
Gera em tempo real o **Memorial de Cálculo Técnico de Dosagem e Verificação Normativa** completo:
- Traduz dinamicamente em tempo de execução (`pt_BR` $\leftrightarrow$ `en_US`) todo o cabeçalho (`Projeto:`, `Responsável:`, `Instituição:`, `Matrícula / ID:`) e todas as seções técnicas e alertas de conformidade;
- **Símbolo Monetário Dinâmico:** Alterna de forma automática e consistente entre Real Brasileiro (`R$`) e Dólar Americano (`$`) em todas as tabelas econômicas, custos unitários e totais do projeto conforme o idioma ativo;
- Disponibiliza quatro botões de exportação imediata: **📋 Copiar Texto**, **💾 Salvar Markdown (`.md`)**, **🌐 Salvar HTML (`.html`)** e **📄 Exportar PDF Oficial (`.pdf`)**.

---

### 4. Interface de Linha de Comando Profissional (CLI — `concrete-mix-designer-cli.exe`)

O aplicativo disponibiliza um binário de console nativo independente, voltado para automações, rotinas em lote, esteiras de integração contínua (CI/CD) e auditorias normativas programadas:

```powershell
concrete-mix-designer-cli.exe [comando] [opcoes...]
```

#### 4.1. Comandos Principais
- `design`: Executa o dimensionamento completo de traço a partir de requisitos de entrada:
  ```powershell
  concrete-mix-designer-cli.exe design --fck 30 --slump 120 --caa III --cond A --format json
  ```
- `calculate` (ou `calc`): Realiza o fechamento volumétrico de $1\text{ m}^3$ a partir de $m$, $a/c$ e $\alpha$:
  ```powershell
  concrete-mix-designer-cli.exe calc --m 4.5 --ac 0.48 --alpha 52.0
  ```
- `verify` (ou `audit`): Audita a conformidade de um traço frente às normas NBR 6118:2023, NBR 8953:2015 e NBR 12655:2022:
  ```powershell
  concrete-mix-designer-cli.exe verify --fck 30 --caa III --ac 0.48 --cement-content 360
  ```
- `report`: Emite relatórios completos em formato texto puro, Markdown, JSON ou HTML com suporte a tema escuro:
  ```powershell
  concrete-mix-designer-cli.exe report --project "Ponte Sul" --engineer "Dr. Silva" --fck 35 --slump 100 --caa II --format html --dark --out relatorio.html --lang pt
  ```
- `standards`: Consulta tabelas do banco de dados normativo SQLite (`cements`, `aggregates`, `slump`, `tolerances`).
- `presets`: Lista e executa diretamente os cenários pré-configurados (Cenários 0 a 5, incluindo a planilha UFPR).
- `test`: Executa bateria interna de autodiagnóstico analítico, fechamento de massas e verificação monetária.

#### 4.2. Opções Globais
- `--lang <pt|en>`: Idioma de saída (português `pt` ou inglês `en`);
- `--format <text|json|markdown|html>`: Formato estruturado da resposta;
- `--out <arquivo>`: Redireciona a saída para arquivo em disco;
- `--dark`: Aplica folha de estilo escura para relatórios HTML;
- `--no-banner`: Omite o banner ASCII para consumo por scripts.

---

### 5. Finalidade Estritamente Acadêmica, Conferência Manual Obrigatória e Isenção de Responsabilidade Legal

Este aplicativo constitui uma ferramenta desenvolvida **exclusivamente para fins de estudo acadêmico, instrução didática e pesquisa teórica**, sendo fornecida **"NO ESTADO EM QUE SE ENCONTRA" ("AS-IS" e "WITH ALL FAULTS")**, podendo conter possíveis erros algorítmicos, numéricos, de precisão ou de modelagem, e **não servindo como um software definitivo para tomadas de decisão em produção real, canteiros de obras, centrais dosadoras ou ambientes de trabalho profissionais**. É estritamente obrigatório conferir manualmente todos os cálculos e resultados gerados ou validá-los através de outras ferramentas mais consolidadas no mercado, além de realizar ensaios laboratoriais físicos com os lotes reais de insumos e emitir **Anotação de Responsabilidade Técnica (ART)** por Engenheiro Civil legalmente habilitado (**Sistema CONFEA/CREA — Leis Federais nº 5.194/1966 e nº 6.496/1977**). Em estrita conformidade com a legislação brasileira (**Código Civil — Lei nº 10.406/2002, arts. 186, 393, 421 e 927; Lei do Software — Lei nº 9.609/1998; Lei de Direitos Autorais — Lei nº 9.610/1998; Código de Defesa do Consumidor — Lei nº 8.078/1990**) e com as convenções internacionais aplicáveis (**UCC § 2-316, Convenção de Berna, Tratados da OMPI/WIPO, Acordo TRIPS e Seções 15 e 16 da licença GNU AGPLv3**), o autor fica integralmente isento de quaisquer responsabilidades civis, administrativas, trabalhistas, patrimoniais ou penais quanto ao uso real ou definitivo deste software em produção e ambientes de trabalho.

---

## English (United States)

### 1. Overview and Scientific Foundation

**Concrete Mix Design C++ (v2026.11.11.0)** is a high-performance native computational system built with **C++20** and **Qt 6.11.1**, intended exclusively for **academic study, university teaching, and theoretical research** of **Experimental and Analytical Portland Cement Concrete Mix Design** according to the **IPT/EPUSP Method** (*Technological Research Institute / Polytechnic School of USP* — Paulo Roberto do Lago Helene and Paulo Terzian, 1992), integrated with mandatory requirements from current Brazilian **ABNT** standards and the materials science principles of **Mehta & Monteiro (2014)**.

The application replaces manual graphical chart plotting — which is prone to visual reading errors on millimeter-paper nomograms — with exact double-precision (`double` IEEE 754) numerical regressions, performing automatic normative durability verification backed by an **embedded in-memory SQLite 3 relational database (`:memory:`)**.

---

### 2. Menu Bar, Toolbar, and "Settings" Menu

#### 2.1. Main Menu Bar
- **File (`&File`):**
  - `🔄 Recalculate All (F5)`: Immediately runs the full analytical engine and updates all 7 tabs, the 4-Quadrant Diagram, the status bar summary, and the Technical Report.
  - `Exit (Ctrl+Q)`: Safely closes the application.
- **View (`&View`):** Direct navigation shortcuts to each of the 7 workspace tabs.
- **Settings (`&Settings`):**
  - **Submenu `Settings > 📂 Spreadsheet Scenario:`:** Submenu integrated at the top of the Settings menu for immediate selection among Scenarios 0 through 5 (with active scenario checkmark and dynamic runtime translation).
  - `📝 Project Data and Identification (Project, Engineer, Institution, ID)... (Ctrl+I)`: Opens the modal configuration dialog for institutional metadata reported in the technical report header (**Tab 7**).
  - `📂 Project: Automatically Report 'Spreadsheet Scenario'`: When checked (default), the **Project:** field in the technical report automatically displays the title of the scenario selected in the **📂 Spreadsheet Scenario:** submenu, dynamically translating it at runtime when switching between Portuguese (`pt_BR`) and English (`en_US`).
  - `✏️ Project: Edit Information Manually...`: Allows defining a custom name for **Project:**, as well as freely editing **Engineer:**, **Institution:**, and **Registration / ID:**, with automatic persistence via `QSettings` and a **Restore Defaults** button.
  - **Submenu `Settings > 🌐 Language (🌐 &Language)`:** Instant runtime language switching between `🇧🇷 Português (BR) - pt_BR` and `🇺🇸 English (US) - en_US`, automatically updating the entire graphical interface, calculation report, currency symbols (`R$` / `$`), and legal documents without restarting the program.
- **Help (`&Help`):**
  - `📖 User Manual (F1)`: Opens the built-in interactive bilingual user manual.
  - `📋 Technical Standards (NBR)`: Displays the summary dialog of applied ABNT NBR standards.
  - `ℹ️ About Concrete Mix Design`: Displays institutional details, application history, End-User License Agreement (EULA), third-party notices (LGPLv3), privacy policy, and release notes.

#### 2.2. Engineering Scenario Selector ("Spreadsheet Scenario")
The **Spreadsheet Scenario:** selector in the **Settings** menu provides **6 pre-configured engineering scenarios** with dynamic runtime translation:
- **Scenario 0:** *User customizable* (initializes fields ready for free entry of a new laboratory study).
- **Scenario 1:** *C25 Urban Concrete (CAA II) - CP II-F-32, Slump 100mm, Project 30 m³*.
- **Scenario 2:** *C30 Marine Concrete (CAA III) - CP III-40 RS, Coarse Agg 1+2, Project 50 m³*.
- **Scenario 3:** *C40 High-Performance Concrete (CAA IV) - CP V-ARI, Slump 160mm, Project 100 m³*.
- **Scenario 4:** *C20 Foundations Concrete (CAA I - Condition B) - CP II-Z-32, Coarse Agg 2, Project 40 m³*.
- **Scenario 5:** *Corrected Original UFPR Spreadsheet (TC031 - C30, Sd 3.5 MPa, α 52%, Project 30 m³)* — mathematically reproduces the UFPR TC031 experimental study following normative auditing and formula correction.

> **Note on Numeric Field Editing (`QDoubleSpinBox`):** All numeric spin boxes allow smooth, continuous typing without intrusive automatic trailing-zero padding after every typed digit; standard decimal formatting is applied only when editing of the field is completed.

---

### 3. Step-by-Step Guide to the 7 Workspace Tabs

#### Tab 1: `1. Project Parameters`
Defines structural and durability requirements in accordance with **ABNT NBR 12655:2022**, **ABNT NBR 6118:2023**, and **ABNT NBR 8953:2015**:
1. **Characteristic Compressive Strength ($f_{ck}$):** From $10.0\text{ MPa}$ to $100.0\text{ MPa}$ (Strength Groups I and II of NBR 8953).
2. **Batching Condition and Standard Deviation ($S_d$):**
   - **Condition A ($S_d = 4.0\text{ MPa}$):** Applicable to all strength classes; cement and aggregates measured by mass, mixing water measured by mass or volume with aggregate moisture correction.
   - **Condition B ($S_d = 5.5\text{ MPa}$):** Applicable to classes C10 through C20; cement measured by mass, aggregates measured by combined mass and volume.
   - **Condition C ($S_d = 7.0\text{ MPa}$):** Applicable only to classes C10 and C15; cement measured by mass, aggregates measured by volume.
   - **Historical Known Deviation ($S_d \ge 2.0\text{ MPa}$):** For concrete produced with the same materials and equipment backed by a statistical record of $n \ge 20$ consecutive specimens.
   - **Laboratory / Arbitrary (Custom):** Allows entering any experimental $S_d$ value (such as $S_d = 3.5\text{ MPa}$ in UFPR Scenario 5).
3. **28-Day Target Mix Design Strength ($f_{cj}$):**
   $$f_{cj} = f_{ck} + 1.65 \cdot S_d$$
4. **Environmental Aggressiveness Class (CAA I to IV) and Structural Type (RC / PC):**
   Automatically queries Table 6 of NBR 12655:2022 and Table 7.2 of NBR 6118:2023, displaying the maximum $w/c$ ratio, minimum strength class, minimum cement content ($C_{\min}$), nominal cover ($c_{\text{nom}}$ for slabs and beams/columns), and geometric verification of the maximum characteristic coarse aggregate size ($D_{\max} \le 1.2 \cdot c_{\text{nom}}$ per NBR 6118 Section 7.4.7.6).
5. **Central Batching Plant Weighing Tolerances (ABNT NBR 7212:2025, Section 5.5):**
   - Cement and Additions: $\pm 1\%$ | Aggregates: $\pm 3\%$ | Water: $\pm 3\%$ | Admixtures: $\pm 5\%$.

#### Tab 2: `2. Materials & Coarse Aggregates`
1. **Physical-Mechanical Characterization (ABNT NBR 7211:2022):**
   Enter for **Portland Cement**, **Natural Sand**, **Coarse Aggregate 1**, and **Coarse Aggregate 2**:
   - Identification / Type (or $D_{\max}$ for coarse aggregates: $9.5\text{ mm}$, $19.0\text{ mm}$, $25.0\text{ mm}$, or $37.5\text{ mm}$);
   - Specific gravity ($\gamma$, in $\text{kg/dm}^3$);
   - Loose bulk unit weight ($\delta$, in $\text{kg/dm}^3$);
   - Field surface moisture ($h$, in $\%$);
   - Unit cost (with unit selector for cement: $\text{R\$/bag}$ of $50\text{ kg}$, $\text{R\$/t}$, or $\text{R\$/kg}$; and $\text{R\$/t}$ for sand and coarse aggregates).
2. **Coarse Aggregate Blend Optimization Test (Granular Packing — NBR NM 45):**
   Evaluates 11 binary blend proportions (from $0\%$ to $100\%$ Coarse Aggregate 2 in $10\%$ increments) for a fixed base mass of Coarse Aggregate 2 and a calibrated container volume, automatically identifying the optimal blend that maximizes compacted bulk density ($\text{kg/dm}^3$) and minimizes intergranular voids.

#### Tab 3: `3. Mortar Content (α)`
Simulates the laboratory experimental sequence for determining the optimal dry mortar content ($\alpha$, from $40\%$ to $68\%$ in $2\%$ steps) while keeping the coarse aggregate mass ($M_b$) fixed in the mixer for the pilot mix ($m$):
$$\alpha = \frac{1 + a}{1 + m} \implies a = \alpha \cdot (1 + m) - 1, \qquad p = m - a$$
$$M_c = \frac{M_b}{p}, \qquad M_a = a \cdot M_c$$
The table computes $a$, $p$, total sand, sand increment, total cement, and cement increment for each step. Clicking any row in the table or adjusting the **Selected Optimal Content α (%)** field immediately applies $\alpha$ across the entire mix design study.

#### Tab 4: `4. Mixes & IPT Diagram`
1. **Data from the 3 Laboratory Experimental Mixes:**
   Enter for **Mix 1 (Rich)**, **Mix 2 (Pilot)**, and **Mix 3 (Lean)** the total dry aggregate-to-cement ratio ($m$), water/cement ratio ($w/c$), measured 28-day compressive strength ($f_{cj}$, in $\text{MPa}$), slump (in $\text{mm}$), and fresh concrete specific gravity ($\gamma_{\text{conc}}$, in $\text{kg/dm}^3$). Experimental cement consumption is automatically calculated as:
   $$C_{\text{exp}} = \frac{1000 \cdot \gamma_{\text{conc}}}{1 + m + w/c}$$
2. **Ordinary Least Squares (OLS) Analytical Curve Fitting:**
   - **1st Quadrant — Abrams' Law:** $f_{cj} = \dfrac{k_1}{k_2^{(w/c)}} \implies (w/c)_{\text{calc}} = \dfrac{\ln(k_1 / f_{cj,\text{target}})}{\ln(k_2)}$, enforcing the normative durability cap $(w/c)_{\text{adopted}} = \min\big((w/c)_{\text{calc}},\, (w/c)_{\max,\text{NBR}}\big)$.
   - **2nd Quadrant — Lyse's Law:** $m = k_3 + k_4 \cdot (w/c) \implies H = 1000 \cdot \dfrac{w/c}{k_5 + k_6 \cdot m}$.
   - **3rd Quadrant — Molinari's Law:** $C = \dfrac{1000}{k_5 + k_6 \cdot m}$, verifying $C \ge C_{\min,\text{NBR}}$.
   - **4th Quadrant — Cement Consumption × Cost per $\text{m}^3$:** Correlates cement consumption with total unit material cost per cubic meter of concrete.
3. **Interactive Vector Diagram:** Switch between 6 advanced visualization modes via the combo box:
   - *Complete 4-Quadrant Diagram (IPT)*: Multi-quadrant matrix overview;
   - *1st Quad: Abrams (fcj x w/c)*: Compressive strength dosage curve vs. water/cement ratio;
   - *2nd Quad: Lyse (m x w/c)*: Aggregate/cement ratio at constant slump;
   - *3rd Quad: Molinari (m x C)*: Cement consumption hyperbola per cubic meter;
   - *4th Quad: Consumption & Cost*: Correlation between unit financial cost and compressive strength;
   - *5th Abrams/Lyse/Molinari (IPT)*: New continuous Cartesian composition strictly matching the official ABNT NBR standard layout with clean non-gradient background, high-density axis grid lines, legible high-precision scales, and high-resolution export (`.png` / `.pdf`).

#### Tab 5: `5. Final Mix & Batches`
Provides four comprehensive operational tables:
1. **Final Unit Mix Summary:** Dry mass unit mix ($1 : a : p : w/c$) and field-moisture-corrected unit mix ($1 : a_{\text{wet}} : p_{\text{wet}} : (w/c)_{\text{effective}}$).
2. **Material Consumption per Cubic Meter ($1\text{ m}^3$):** Dry mass ($\text{kg}$), surface moisture ($\%$), corrected wet mass ($\text{kg}$), and loose wet volume ($\text{m}^3$ or $\text{L}$) for Cement, Sand, Coarse Aggregate 1, Coarse Aggregate 2, and Mixing Water.
3. **Laboratory Batch for Mixer Volume:** Sizes exact dry masses and effective scale masses (subtracting free surface water carried by wet aggregates) for any laboratory mixer volume in liters.
4. **Jobsite Batching (1 × $50\text{ kg}$ Cement Bag — Volume Boxes / Padiolas):**
   Converts the mix to multiples of 1 unopened cement bag ($50\text{ kg}$) and sizes the count and internal height ($h$, in $\text{cm}$) of standard wooden volume boxes (*padiolas*) with a $35\text{ cm} \times 45\text{ cm}$ internal base ($\text{area} = 15.75\text{ dm}^2$), capping the maximum ergonomic height at $35\text{ cm}$ per box.

#### Tab 6: `6. Cost Analysis`
Compares side-by-side the **Exact Analytical Solution (Regression Equations)** against **Manual Graphical Reading (Nomogram)** for the total project volume ($\text{m}^3$), quantifying:
- Cement consumption ($\text{kg/m}^3$), cement, sand, and coarse aggregate costs ($\text{R\$/m}^3$ or $\text{\$/m}^3$), and total cost per $\text{m}^3$;
- Total project cost, total cement consumption ($\text{kg}$), and total $50\text{ kg}$ cement bags;
- Absolute financial difference ($|\Delta|$) and cost/material precision achieved by the analytical solution.

#### Tab 7: `7. Technical Report`
Generates in real time the complete **Technical Mix Design and Normative Compliance Calculation Report**:
- Dynamically translates at runtime (`pt_BR` $\leftrightarrow$ `en_US`) the entire report header (`Project:`, `Engineer:`, `Institution:`, `Registration / ID:`) and all technical sections and compliance notices;
- **Dynamic Currency Symbol:** Automatically and consistently switches between Brazilian Real (`R$`) and US Dollar (`$`) across all economic tables, unit costs, and project totals according to the active language;
- Provides four instant export actions: **📋 Copy Text**, **💾 Save Markdown (`.md`)**, **🌐 Save HTML (`.html`)**, and **📄 Export Official PDF (`.pdf`)**.

---

### 4. Professional Command Line Interface (CLI — `concrete-mix-designer-cli.exe`)

The application includes an independent, native standalone console executable tailored for automation, batch processing, continuous integration (CI/CD) pipelines, and scheduled normative audits:

```powershell
concrete-mix-designer-cli.exe [command] [options...]
```

#### 4.1. Core Commands
- `design`: Complete automated mix design calculation based on engineering constraints:
  ```powershell
  concrete-mix-designer-cli.exe design --fck 30 --slump 120 --caa III --cond A --format json
  ```
- `calculate` (or `calc`): Volumetric closure for $1\text{ m}^3$ from user-supplied $m$, $w/c$, and $\alpha$:
  ```powershell
  concrete-mix-designer-cli.exe calc --m 4.5 --ac 0.48 --alpha 52.0
  ```
- `verify` (or `audit`): Compliance audit against ABNT NBR 6118:2023, NBR 8953:2015, and NBR 12655:2022:
  ```powershell
  concrete-mix-designer-cli.exe verify --fck 30 --caa III --ac 0.48 --cement-content 360
  ```
- `report`: Full engineering calculation reports in plain text, Markdown, JSON, or HTML with dark mode styling:
  ```powershell
  concrete-mix-designer-cli.exe report --project "South Bridge" --engineer "Dr. Silva" --fck 35 --slump 100 --caa II --format html --dark --out report.html --lang en
  ```
- `standards`: Query in-memory SQLite tables (`cements`, `aggregates`, `slump`, `tolerances`).
- `presets`: List and run standard pre-configured scenarios (Scenarios 0 to 5, including UFPR).
- `test`: Internal self-test suite covering analytical regressions, mass balance, and currency localization.

#### 4.2. Global Options
- `--lang <pt|en>`: Output language (`pt` for Portuguese, `en` for English);
- `--format <text|json|markdown|html>`: Structured response format;
- `--out <file>`: Direct output to disk;
- `--dark`: Enable dark theme styling in exported HTML reports;
- `--no-banner`: Suppress ASCII header banner for headless scripting.

---

### 5. Strictly Academic Purpose, Mandatory Manual Verification, and Legal Liability Disclaimer

This application constitutes a tool developed **exclusively for academic study, didactic instruction, and theoretical research purposes**, being provided **"AS-IS" and "WITH ALL FAULTS"**, and may contain possible algorithmic, numerical, precision, or modeling errors, **not serving as definitive software for decision-making in real production, construction sites, batching plants, or professional work environments**. It is strictly mandatory to manually verify all generated calculations and results or validate them through other more consolidated market tools, in addition to performing physical laboratory testing with the actual material batches and issuing a **Technical Responsibility Record (ART / Professional Engineer Seal)** by a legally licensed Civil Engineer (**CONFEA/CREA System — Brazilian Federal Laws No. 5,194/1966 and No. 6,496/1977**). In strict accordance with Brazilian legislation (**Brazilian Civil Code — Law No. 10,406/2002, Articles 186, 393, 421, and 927; Software Law — Law No. 9,609/1998; Copyright Law — Law No. 9,610/1998; Consumer Defense Code — Law No. 8,078/1990**) and applicable international conventions (**UCC § 2-316, Berne Convention, WIPO Treaties, TRIPS Agreement, and Sections 15 and 16 of the GNU AGPLv3 license**), the author is fully exempt from any civil, administrative, labor, property, or criminal liability regarding the real or definitive use of this software in production and work environments.
