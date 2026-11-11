# Guia Rápido do Usuário / Quick User Guide — Concrete Mix Design C++

[Português (Brasil)](#português-brasil) | [English (United States)](#english-united-states)

---

## Português (Brasil)

### 1. Fluxo Rápido de Trabalho em 7 Passos

Para realizar um estudo completo de dosagem experimental de concreto pelo **Método IPT/EPUSP** no **Concrete Mix Design C++**:

1. **Selecione o Idioma, um Cenário ou Configure a Identificação do Estudo:**
   - Acesse o menu **Configurações $\to$ Idioma** para alternar instantaneamente o aplicativo entre `Português (BR)` e `English (US)`, ou escolha em **Configurações $\to$ 📂 Cenário da Planilha:** um dos 6 cenários pré-configurados (`Cenário 0` a `Cenário 5`).
   - Acesse o menu **Configurações $\to$ Dados do Projeto e Identificação (`Ctrl+I`)** para definir o **Responsável:**, **Instituição:**, **Matrícula / ID:** e escolher se o campo **Projeto:** deve reportar automaticamente o **Cenário da Planilha** selecionado (com tradução dinâmica PT-BR / EN-US) ou usar um texto editado manualmente.
2. **Aba 1 (`1. Parâmetros de Projeto`):**
   - Informe o $f_{ck}$ desejado ($\text{MPa}$), a Condição de Preparo (Condição A, B, C, histórico ou personalizado), a Classe de Agressividade Ambiental (CAA I a IV), o Tipo de Estrutura (Concreto Armado CA ou Protendido CP) e o Abatimento (*Slump*, em $\text{mm}$).
3. **Aba 2 (`2. Materiais & Britas`):**
   - Insira a massa específica real ($\gamma$), massa unitária aparente ($\delta$), umidade de campo ($h\%$) e custo unitário do Cimento, Areia, Brita 1 e Brita 2.
   - Se desejar combinar Brita 1 e Brita 2, marque **Ativar mistura binária de agregados graúdos** e confira na tabela a proporção ótima que maximiza a massa unitária compactada.
4. **Aba 3 (`3. Teor de Argamassa (α)`):**
   - Defina o traço piloto ($m$) e a massa fixa de brita para o ensaio de laboratório, ou clique diretamente em qualquer linha da tabela ($40\%$ a $68\%$) para adotar o teor ótimo de argamassa seca ($\alpha$).
5. **Aba 4 (`4. Traços & Diagrama IPT`):**
   - Preencha os dados obtidos em laboratório para os **3 Traços Experimentais** (Rico, Piloto e Pobre): traço $m$, relação $a/c$, resistência $f_{cj}$ aos 28 dias, *Slump* e massa específica fresca ($\text{kg/dm}^3$).
   - Clique em **⚡ Ajustar Curvas e Regressões dos 4 Quadrantes** e inspecione o Diagrama de Dosagem do IPT.
   - Navegue pelas abas internas de visualização: os quadrantes individuais, o diagrama completo de 4 quadrantes e o **5° Abrams/Lyse/Molinari (IPT)** (que combina as 3 curvas experimentais em coordenadas limpas com réguas e linhas de grade nítidas).
   - Utilize o botão **Exportar Imagem dos Gráficos...** para salvar imagens em alta resolução (`.png`) ou exportar para `.pdf`.
6. **Aba 5 (`5. Traço Final & Amassadas`) e Aba 6 (`6. Análise de Custos`):**
   - Na **Aba 5**, consulte o traço seco e úmido final, o consumo para $1\text{ m}^3$, a pesagem para o volume da sua betoneira de laboratório e o dimensionamento das padiolas de obra ($35 \times 45\text{ cm}$) para 1 saco de $50\text{ kg}$ de cimento.
   - Na **Aba 6**, avalie o custo por $\text{m}^3$, o custo total para o volume da obra e a comparação com leituras gráficas manuais.
7. **Aba 7 (`7. Relatório Técnico`):**
   - Revise o Memorial de Cálculo Técnico completo (que acompanha dinamicamente o idioma selecionado no submenu **Configurações $\to$ Idioma** e o símbolo monetário R$ ou $) e exporte-o em **Texto**, **Markdown (`.md`)**, **HTML (`.html`)** ou **PDF Oficial (`.pdf`)**.

### 2. Automação e Uso via Linha de Comando (CLI)

O executável complementar `concrete-mix-designer-cli.exe` permite operar o motor de cálculo diretamente pelo terminal:

```powershell
# Dosagem automatizada por fck e abatimento
concrete-mix-designer-cli.exe design --fck 30 --slump 120 --caa II

# Fechamento volumétrico rápido de 1 m³
concrete-mix-designer-cli.exe calc --m 4.5 --ac 0.48 --alpha 52.0

# Auditoria de conformidade normativa NBR 6118 / NBR 12655
concrete-mix-designer-cli.exe verify --fck 30 --caa III --ac 0.48 --cement-content 360

# Geração de relatório técnico completo em HTML
concrete-mix-designer-cli.exe report --project "Edifício Central" --fck 30 --caa II --format html --out relatorio.html

# Bateria de testes analíticos
concrete-mix-designer-cli.exe test
```

> **Aviso de Finalidade Acadêmica e Isenção Legal:** Este aplicativo destina-se exclusivamente a fins de estudo acadêmico, podendo conter possíveis erros e não servindo como software definitivo para tomadas de decisão em produção ou ambientes de trabalho, sendo obrigatório conferir manualmente os resultados ou validá-los através de outras ferramentas mais consolidadas no mercado, ficando o autor isento de responsabilidades legais nos termos da legislação brasileira e convenções internacionais. Para detalhes matemáticos e normativos completos, consulte o [**MANUAL.md**](../MANUAL.md) ou pressione **`F1`** dentro do aplicativo.

---

## English (United States)

### 1. Quick 7-Step Workflow

To complete a full experimental concrete mix design study using the **IPT/EPUSP Method** in **Concrete Mix Design C++**:

1. **Select the Language, a Scenario, or Configure Study Identification:**
   - Open the **Settings $\to$ Language** menu to instantaneously switch the application between `Português (BR)` and `English (US)`, or select one of the 6 pre-configured scenarios (`Scenario 0` to `Scenario 5`) under **Settings $\to$ 📂 Spreadsheet Scenario:**.
   - Open the **Settings $\to$ Project Data and Identification (`Ctrl+I`)** menu to set **Engineer:**, **Institution:**, **Registration / ID:**, and choose whether the **Project:** field should automatically report the selected **Spreadsheet Scenario** (with dynamic PT-BR / EN-US translation) or use manually edited text.
2. **Tab 1 (`1. Project Parameters`):**
   - Enter the target $f_{ck}$ ($\text{MPa}$), Batching Condition (Condition A, B, C, historical, or custom), Environmental Aggressiveness Class (CAA I to IV), Structural Type (Reinforced Concrete RC or Prestressed Concrete PC), and target Slump ($\text{mm}$).
3. **Tab 2 (`2. Materials & Coarse Aggregates`):**
   - Enter the specific gravity ($\gamma$), loose bulk unit weight ($\delta$), field moisture ($h\%$), and unit cost for Cement, Sand, Coarse Aggregate 1, and Coarse Aggregate 2.
   - To blend Coarse Aggregates 1 and 2, check **Enable binary coarse aggregate blend** and inspect the table for the optimal proportion that maximizes compacted bulk density.
4. **Tab 3 (`3. Mortar Content (α)`):**
   - Set the pilot mix ($m$) and fixed coarse aggregate mass for the laboratory trial, or click any row in the table ($40\%$ to $68\%$) to adopt the optimal dry mortar content ($\alpha$).
5. **Tab 4 (`4. Mixes & IPT Diagram`):**
   - Enter the laboratory test results for the **3 Experimental Mixes** (Rich, Pilot, and Lean): mix ratio $m$, $w/c$ ratio, 28-day compressive strength $f_{cj}$, Slump, and fresh concrete density ($\text{kg/dm}^3$).
   - Click **⚡ Fit Curves and 4-Quadrant Regressions** and inspect the IPT Mix Design Diagram.
   - Navigate across the chart view tabs: individual quadrants, the complete 4-quadrant diagram, and the **5° Abrams/Lyse/Molinari (IPT)** view (combining all 3 curves in normalized coordinates with clear grid lines and axes).
   - Use the **Export Chart Image...** button to save high-resolution graphics (`.png`) or export to `.pdf`.
6. **Tab 5 (`5. Final Mix & Batches`) and Tab 6 (`6. Cost Analysis`):**
   - In **Tab 5**, review the final dry and wet unit mixes, material consumption per $1\text{ m}^3$, batch masses for your laboratory mixer volume, and jobsite volume-box (*padiola*, $35 \times 45\text{ cm}$) dimensions per $50\text{ kg}$ cement bag.
   - In **Tab 6**, evaluate the cost per $\text{m}^3$, total project cost, and comparison against manual graphical chart readings.
7. **Tab 7 (`7. Technical Report`):**
   - Review the complete Technical Calculation Report (which dynamically follows the language and currency selected in the **Settings $\to$ Language** submenu: $ in English and R$ in Portuguese) and export it to **Clipboard**, **Markdown (`.md`)**, **HTML (`.html`)**, or **Official PDF (`.pdf`)**.

### 2. Automation and Command Line Usage (CLI)

The companion `concrete-mix-designer-cli.exe` executable allows running calculations directly from the command prompt:

```powershell
# Automated mix design by fck and slump
concrete-mix-designer-cli.exe design --fck 30 --slump 120 --caa II

# Fast 1 m³ volumetric closure
concrete-mix-designer-cli.exe calc --m 4.5 --ac 0.48 --alpha 52.0

# Normative compliance audit (NBR 6118 / NBR 12655)
concrete-mix-designer-cli.exe verify --fck 30 --caa III --ac 0.48 --cement-content 360

# Full HTML technical report generation
concrete-mix-designer-cli.exe report --project "Central Tower" --fck 30 --caa II --format html --out report.html

# Analytical test battery
concrete-mix-designer-cli.exe test
```

> **Academic Purpose and Legal Disclaimer Notice:** This application is intended exclusively for academic study purposes, may contain possible errors, and does not serve as definitive software for decision-making in production or work environments, making it mandatory to manually verify all results or validate them through other more consolidated market tools, with the author being exempt from legal liabilities under Brazilian legislation and international conventions. For complete mathematical and normative details, see [**MANUAL.md**](../MANUAL.md) or press **`F1`** inside the application.
