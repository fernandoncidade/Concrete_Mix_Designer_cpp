# Matriz de Rastreabilidade Normativa e Científica / Normative & Scientific Traceability Matrix — Concrete Mix Design C++

[Português (Brasil)](#português-brasil) | [English (United States)](#english-united-states)

---

## Português (Brasil)

Esta matriz estabelece o mapeamento estrito e bidirecional entre os requisitos das normas técnicas brasileiras (ABNT NBR), a literatura científica de referência (Método IPT/EPUSP, Helene & Terzian, Mehta & Monteiro), as funções implementadas no código-fonte em C++20 e as verificações executadas na suíte de testes automatizados (`tests/verify_calculations.cpp`). **AVISO DE FINALIDADE ACADÊMICA E ISENÇÃO DE RESPONSABILIDADE:** Esta aplicação destina-se exclusivamente a fins de estudo acadêmico e pesquisa científica, podendo conter possíveis erros, e não serve como software definitivo para tomadas de decisão em produção ou ambientes de trabalho, sendo obrigatório conferir manualmente todos os resultados e validá-los por meio de ferramentas mais consolidadas no mercado e ensaios laboratoriais oficiais, sob integral isenção de responsabilidade civil, técnica ou penal do autor nos termos da legislação brasileira (Lei nº 10.406/2002, Lei nº 9.609/1998, Lei nº 9.610/1998, Lei nº 8.078/1990 e Leis nº 5.194/1966 e nº 6.496/1977) e das convenções internacionais (*UCC § 2-316*, Convenção de Berna, OMPI, TRIPS e *GNU AGPLv3* Seções 15 e 16).

---

### Resumo dos Instrumentos Normativos e Bibliográficos Auditados

| Identificador | Norma / Referência Bibliográfica | Edição / Ano | Título / Escopo |
| :--- | :--- | :--- | :--- |
| **NBR 12655** | ABNT NBR 12655 | 2022 | Concreto de cimento Portland — Preparo, controle, recebimento e aceitação — Procedimento |
| **NBR 6118** | ABNT NBR 6118 | 2023 | Projeto de estruturas de concreto — Procedimento |
| **NBR 7211** | ABNT NBR 7211 | 2022 | Agregados para concreto — Requisitos |
| **NBR 7212** | ABNT NBR 7212 | 2025 | Concreto dosado em central — Preparo, fornecimento e controle |
| **NBR 8953** | ABNT NBR 8953 | 2015 | Concreto para fins estruturais — Classificação pela massa específica, por grupos de resistência e consistência |
| **IPT/EPUSP** | Helene & Terzian | 1993 | Manual de Dosagem e Controle do Concreto (PINI / IPT) |
| **Mehta & Monteiro** | Mehta & Monteiro | 2014 | Concreto: Microestrutura, Propriedades e Materiais (4ª Edição) |

---

### Matriz de Rastreabilidade Detalhada

| Item | Norma / Fonte | Cláusula / Tabela | Requisito / Regra Implementada | Implementação no Código | Teste Automatizado | Evidência Independente / Referência | Situação |
| :---: | :--- | :--- | :--- | :--- | :--- | :--- | :---: |
| **1** | ABNT NBR 12655:2022 | Item 5.6.3.2 | Resistência média de dosagem: $f_{cj} = f_{ck} + 1,65 \cdot S_d$ | [`ConcreteStandards::calculateDosageStrength()`](../source/core/ConcreteStandards.cpp#L120-L125) | `verify_calculations.cpp` (Cenários 1, 2, 3, 4, 5) | Planilha UFPR TC031; Helene & Terzian (1993) p. 45 | **Validado** |
| **2** | ABNT NBR 12655:2022 | Tabela 1 / Item 5.6.3.1 | Desvio-padrão $S_d$: Condição A ($4,0\text{ MPa}$), B ($5,5\text{ MPa}$), C ($7,0\text{ MPa}$) | [`ConcreteStandards::getStandardDeviation()`](../source/core/ConcreteStandards.cpp#L90-L99) | `verify_calculations.cpp` (Cenário 1, Cenário 4) | ABNT NBR 12655:2022 Tabela 1 | **Validado** |
| **3** | ABNT NBR 12655:2022 | Itens 5.4.2 e 5.6.3.1 | Condição B restrita a $f_{ck} \le 20\text{ MPa}$; Condição C restrita a $f_{ck} \le 15\text{ MPa}$ | [`ConcreteStandards::validateMixDesign()`](../source/core/ConcreteStandards.cpp#L245-L260) | `verify_calculations.cpp` (Cenário 4, Frente 2) | NBR 12655:2022 itens 5.4.2 e 5.6.3.1 | **Validado** |
| **4** | ABNT NBR 12655:2022 / NBR 6118:2023 | NBR 12655 Tab. 2 / NBR 6118 Tab. 7.1 | Relação água/cimento máxima por CAA (CA: 0,65 / 0,60 / 0,55 / 0,45; CP: 0,60 / 0,55 / 0,50 / 0,45) | [`ConcreteStandards::getDurabilityRequirement()`](../source/core/ConcreteStandards.cpp#L42-L88) e [`validateMixDesign()`](../source/core/ConcreteStandards.cpp#L214-L229) | `verify_calculations.cpp` (Frente 2: limites 0.599, 0.600, 0.601) | NBR 12655:2022 Tabela 2; NBR 6118:2023 Tabela 7.1 | **Validado** |
| **5** | ABNT NBR 12655:2022 | Tabela 2 | Consumo mínimo de cimento por CAA (260, 280, 320, 360 kg/m³) | [`ConcreteStandards::getDurabilityRequirement()`](../source/core/ConcreteStandards.cpp#L58-L82) e [`validateMixDesign()`](../source/core/ConcreteStandards.cpp#L230-L244) | `verify_calculations.cpp` (Frente 2: limites 279 e 280 kg/m³) | NBR 12655:2022 Tabela 2 | **Validado** |
| **6** | ABNT NBR 12655:2022 / NBR 6118:2023 | NBR 12655 Tab. 2 / NBR 6118 Tab. 7.1 | Classe mínima de concreto por CAA (CA: C20, C25, C30, C40; CP: C25, C30, C35, C40) | [`ConcreteStandards::getDurabilityRequirement()`](../source/core/ConcreteStandards.cpp#L56-L80) e [`validateMixDesign()`](../source/core/ConcreteStandards.cpp#L200-L213) | `verify_calculations.cpp` (Cenários 1 a 4) | NBR 6118:2023 Tabela 7.1 | **Validado** |
| **7** | ABNT NBR 6118:2023 | Tabela 7.2 | Cobrimento nominal ($c_{nom}$) para lajes (20, 25, 35, 45 mm) e vigas/pilares (25, 30, 40, 50 mm) | [`ConcreteStandards::getDurabilityRequirement()`](../source/core/ConcreteStandards.cpp#L59-L84) | `verify_calculations.cpp` (Cenário 1, Frente 2) | NBR 6118:2023 Tabela 7.2 | **Validado** |
| **8** | ABNT NBR 6118:2023 | Item 7.4.7.6 | Compatibilidade geométrica do agregado graúdo ($DMC \le 1,2 \cdot c_{nom}$) diferenciada entre vigas/pilares e lajes | [`ProjectData::checkDmcCoverCompliance()`](../source/core/ProjectData.cpp#L13-L36) e [`ConcreteStandards::validateMixDesign()`](../source/core/ConcreteStandards.cpp#L273-L315) | `verify_calculations.cpp` (Frente 2: Brita 2 com DMC 25mm em CAA I) | NBR 6118:2023 Item 7.4.7.6 | **Validado** |
| **9** | ABNT NBR 8953:2015 | Tabela 1 | Classificação da consistência por abatimento (S10, S50, S100, S160, S220) | [`ConcreteStandards::classifySlump()`](../source/core/ConcreteStandards.cpp#L131-L140) | `verify_calculations.cpp` (Cenários 1, 2, 3 e Banco Normativo) | NBR 8953:2015 Tabela 1 | **Validado** |
| **10** | ABNT NBR 7212:2025 | Item 5.8 / Tabela 1 | Tolerâncias de recebimento para o abatimento ($\pm 10\text{ mm}$, $\pm 20\text{ mm}$, $\pm 30\text{ mm}$) | [`ConcreteStandards::validateMixDesign()`](../source/core/ConcreteStandards.cpp#L261-L272) | `verify_calculations.cpp` (Frente 2: Slump 100mm e Slump 0mm) | NBR 7212:2025 Item 5.8 e Tabela 1 | **Validado** |
| **11** | ABNT NBR 7212:2025 | Item 5.5 | Tolerâncias de pesagem em central dosadora (Cimento $\pm 1\%$, Agregados $\pm 3\%$, Água $\pm 3\%$, Aditivos $\pm 5\%$) | [`NormativeDatabase::getBatchingTolerances()`](../source/core/NormativeDatabase.cpp) | `verify_calculations.cpp` (Banco de Dados Normativo) | NBR 7212:2025 Item 5.5 | **Validado** |
| **12** | ABNT NBR 7211:2022 | Cláusulas 4 e 5 | Caracterização granulométrica e massa unitária compactada em misturas binárias de britas | [`AggregateOptimizer::calculateBinaryBlend()`](../source/core/AggregateOptimizer.cpp) | `verify_calculations.cpp` (Otimização de Mistura de Britas) | NBR 7211:2022 e Helene & Terzian (1993) | **Validado** |
| **13** | IPT / EPUSP | Helene & Terzian (1993) Cap. 3 | Lei de Abrams ($f_{cj} \times a/c$), Lei de Lyse ($m \times a/c$) e Lei de Molinari ($C \times a/c$) | [`RegressionEngine::fitPolynomial2()`](../source/core/RegressionEngine.cpp#L25-L95), [`fitLinear()`](../source/core/RegressionEngine.cpp#L97-L138) | `verify_calculations.cpp` (Cenários 1, 2, 3, 5; Frente 1) | Helene & Terzian (1993); Planilha UFPR TC031 | **Validado** |
| **14** | IPT / EPUSP | Helene & Terzian (1993) Cap. 4 | Equação de Volumes Absolutos: $1000 = C \cdot (1/\rho_c + a/\rho_a + p/\rho_p + (a/c)/\rho_w)$ | [`RegressionEngine::calculateTheoreticalConsumption()`](../source/core/RegressionEngine.cpp#L173-L191) | `verify_calculations.cpp` (Cenários 1 a 5; Frente 3) | Helene & Terzian (1993) p. 78 | **Validado** |
| **15** | IPT / EPUSP | Helene & Terzian (1993) Cap. 5 | Correção de umidade dos agregados e balanço estrito de água na amassada | [`MixDesignCalculator::calculate()`](../source/core/MixDesignCalculator.cpp#L320-L375) | `verify_calculations.cpp` (Frente 3: Balanço de Massas 1 m³) | Balanço estrito de massa: $M_{seca} + A_{teor} \equiv M_{umida} + A_{efetiva}$ | **Validado** |
| **16** | Prática Construtiva Brasileira | ABCP / Helene & Terzian | Dimensionamento de padiolas de canteiro ($35 \times 45\text{ cm}$) por saco de $50\text{ kg}$ de cimento | [`MixDesignCalculator::calculate()`](../source/core/MixDesignCalculator.cpp#L417-L425) | `verify_calculations.cpp` (Frente 3: Geometria de padiolas em cm) | Altura real em centímetros: $h = (V_{litros} / (15,75 \cdot n)) \cdot 10$ | **Validado** |
| **17** | Robustez Numérica | Métodos Numéricos | Centralização e escalonamento de dados em regressão polinomial ($u_i = (x_i - \bar{x})/s_x$) | [`RegressionEngine::fitPolynomial2()`](../source/core/RegressionEngine.cpp#L45-L75) | `verify_calculations.cpp` (Frente 1: Centramento e Singularidades) | Golub & Van Loan (Matrix Computations) | **Validado** |
| **18** | Rastreabilidade Matemática | Auditoria de Software | Rastreamento explícito de `SolutionType` e bloqueio de conformidade em caso de falha de regressão | [`MixDesignCalculator::calculate()`](../source/core/MixDesignCalculator.cpp#L205-L270) | `verify_calculations.cpp` (Frente 1: $f_{ck} = 150\text{ MPa}$) | Requisito de transparência técnica do software | **Validado** |
| **19** | Transparência Econômica | Engenharia de Custos | Unidades explícitas de preço (`PriceUnit`: R$/kg, R$/sc 50kg, R$/t, R$/m³) sem inferência heurística | [`MixDesignCalculator::convertCementPriceToKg()`](../source/core/MixDesignCalculator.cpp#L18-L45) | `verify_calculations.cpp` (Frente 4: Conversões de Cimento e Agregados) | Requisito de clareza de entrada de custos | **Validado** |
| **20** | Auditoria e Perícia | CONFEA / CREA / NBR 12655 | Geração de memorial técnico com quadro diagnóstico de resolução matemática e rastreabilidade | [`ProjectData::generateMarkdownReport()`](../source/core/ProjectData.cpp#L535-L600) e [`generateHtmlReport()`](../source/core/ProjectData.cpp#L820-L890) | `verify_calculations.cpp` (Frente 5: Tags e Badges de Diagnóstico) | Memorial descritivo para ART de dosagem de concreto | **Validado** |
| **21** | Internacionalização de Moeda | Engenharia de Custos / Relatórios | Adequação dinâmica do símbolo monetário (R$ para pt_BR e $ para en_US) no Memorial Técnico e tabelas de custos | [`ProjectData::currencySymbol()`](../source/core/ProjectData.cpp) e [`ProjectData::generateHtmlReport()`](../source/core/ProjectData.cpp) | `verify_calculations.cpp` / CLI tests | Suporte a mercado e laudos internacionais | **Validado** |
| **22** | Auditoria Automatizada | CI/CD / Controle de Qualidade | Auditoria normativa via linha de comando (`concrete-mix-designer-cli verify`) validando limites de resistência, $a/c$ e consumo de cimento | [`main_cli.cpp`](../source/cli/main_cli.cpp) | `concrete-mix-designer-cli.exe test` | Códigos de saída 0 (OK) e 2 (Não Conforme) | **Validado** |

---

### Classificação dos Requisitos Quanto à Natureza da Validação

1. **Cálculos Determinísticos Obrigatórios:**
   - Resistência de dosagem ($f_{cj}$);
   - Relação água/cimento máxima e consumo mínimo de cimento (NBR 12655);
   - Correção de umidade dos agregados e balanço de massas;
   - Dimensionamento volumétrico de padiolas para canteiro de obras.
   *Status: 100% verificados e cobertos por testes unitários e de integração.*

2. **Verificações de Compatibilidade Geométrica:**
   - Comparação da DMC dos agregados graúdos contra o cobrimento nominal ($DMC \le 1,2 \cdot c_{nom}$) para vigas/pilares e para lajes.
   *Status: Tratamento diferenciado por tipo de elemento estrutural implementado e validado.*

3. **Requisitos de Execução e Julgamento Profissional (Informativos / Laboratoriais):**
   - Tolerâncias de pesagem em central dosadora (NBR 7212:2025);
   - Ensaios de caracterização de agregados (massa unitária, absorção, abrasão Los Angeles, teor de material pulverulento);
   - Confecção e cura de corpos de prova (NBR 5738) e ensaio de compressão axial (NBR 5739).
   *Status: Apresentados no memorial técnico e no banco de dados normativo como parâmetros orientativos para estudo acadêmico, sendo obrigatória a conferência manual de todos os resultados e a validação em softwares mais consolidados no mercado antes de qualquer aplicação real, sob total isenção de responsabilidade legal do autor.*

---

## English (United States)

This matrix establishes a strict bidirectional mapping between Brazilian technical standards requirements (ABNT NBR), reference scientific literature (IPT/EPUSP Method, Helene & Terzian, Mehta & Monteiro), functions implemented in the C++20 source code, and verifications executed in the automated test suite (`tests/verify_calculations.cpp`). **ACADEMIC STUDY PURPOSE AND LIABILITY DISCLAIMER NOTICE:** This application is intended exclusively for academic study and scientific research purposes, may contain possible errors, and does not serve as definitive software for decision-making in production or work environments, making it mandatory to manually check all results and validate them through more consolidated market tools and official laboratory testing, under complete civil, technical, and criminal liability exemption of the author pursuant to Brazilian legislation (Law No. 10,406/2002, Law No. 9,609/1998, Law No. 9,610/1998, Law No. 8,078/1990, and Laws No. 5,194/1966 and No. 6,496/1977) and international conventions (*UCC § 2-316*, Berne Convention, WIPO, TRIPS, and *GNU AGPLv3* Sections 15 and 16).

---

### Summary of Audited Normative and Bibliographic Instruments

| Identifier | Standard / Bibliographic Reference | Edition / Year | Title / Scope |
| :--- | :--- | :--- | :--- |
| **NBR 12655** | ABNT NBR 12655 | 2022 | Portland cement concrete — Preparation, control, receipt and acceptance — Procedure |
| **NBR 6118** | ABNT NBR 6118 | 2023 | Design of concrete structures — Procedure |
| **NBR 7211** | ABNT NBR 7211 | 2022 | Aggregates for concrete — Requirements |
| **NBR 7212** | ABNT NBR 7212 | 2025 | Ready-mixed concrete — Preparation, supply and control |
| **NBR 8953** | ABNT NBR 8953 | 2015 | Concrete for structural purposes — Classification by density, strength groups, and consistency |
| **IPT/EPUSP** | Helene & Terzian | 1993 | Concrete Mix Design and Control Manual (PINI / IPT) |
| **Mehta & Monteiro** | Mehta & Monteiro | 2014 | Concrete: Microstructure, Properties, and Materials (4th Edition) |

---

### Detailed Traceability Matrix

| Item | Standard / Source | Clause / Table | Requirement / Implemented Rule | Code Implementation | Automated Test | Independent Evidence / Reference | Status |
| :---: | :--- | :--- | :--- | :--- | :--- | :--- | :---: |
| **1** | ABNT NBR 12655:2022 | Section 5.6.3.2 | Target mean dosage strength: $f_{cj} = f_{ck} + 1.65 \cdot S_d$ | [`ConcreteStandards::calculateDosageStrength()`](../source/core/ConcreteStandards.cpp#L120-L125) | `verify_calculations.cpp` (Scenarios 1, 2, 3, 4, 5) | UFPR TC031 Spreadsheet; Helene & Terzian (1993) p. 45 | **Validated** |
| **2** | ABNT NBR 12655:2022 | Table 1 / Section 5.6.3.1 | Standard deviation $S_d$: Condition A ($4.0\text{ MPa}$), B ($5.5\text{ MPa}$), C ($7.0\text{ MPa}$) | [`ConcreteStandards::getStandardDeviation()`](../source/core/ConcreteStandards.cpp#L90-L99) | `verify_calculations.cpp` (Scenario 1, Scenario 4) | ABNT NBR 12655:2022 Table 1 | **Validated** |
| **3** | ABNT NBR 12655:2022 | Sections 5.4.2 and 5.6.3.1 | Condition B restricted to $f_{ck} \le 20\text{ MPa}$; Condition C restricted to $f_{ck} \le 15\text{ MPa}$ | [`ConcreteStandards::validateMixDesign()`](../source/core/ConcreteStandards.cpp#L245-L260) | `verify_calculations.cpp` (Scenario 4, Front 2) | NBR 12655:2022 Sections 5.4.2 and 5.6.3.1 | **Validated** |
| **4** | ABNT NBR 12655:2022 / NBR 6118:2023 | NBR 12655 Tab. 2 / NBR 6118 Tab. 7.1 | Maximum water/cement ratio per EAC (RC: 0.65 / 0.60 / 0.55 / 0.45; PC: 0.60 / 0.55 / 0.50 / 0.45) | [`ConcreteStandards::getDurabilityRequirement()`](../source/core/ConcreteStandards.cpp#L42-L88) and [`validateMixDesign()`](../source/core/ConcreteStandards.cpp#L214-L229) | `verify_calculations.cpp` (Front 2: limits 0.599, 0.600, 0.601) | NBR 12655:2022 Table 2; NBR 6118:2023 Table 7.1 | **Validated** |
| **5** | ABNT NBR 12655:2022 | Table 2 | Minimum cement consumption per EAC (260, 280, 320, 360 kg/m³) | [`ConcreteStandards::getDurabilityRequirement()`](../source/core/ConcreteStandards.cpp#L58-L82) and [`validateMixDesign()`](../source/core/ConcreteStandards.cpp#L230-L244) | `verify_calculations.cpp` (Front 2: limits 279 and 280 kg/m³) | NBR 12655:2022 Table 2 | **Validated** |
| **6** | ABNT NBR 12655:2022 / NBR 6118:2023 | NBR 12655 Tab. 2 / NBR 6118 Tab. 7.1 | Minimum concrete strength class per EAC (RC: C20, C25, C30, C40; PC: C25, C30, C35, C40) | [`ConcreteStandards::getDurabilityRequirement()`](../source/core/ConcreteStandards.cpp#L56-L80) and [`validateMixDesign()`](../source/core/ConcreteStandards.cpp#L200-L213) | `verify_calculations.cpp` (Scenarios 1 to 4) | NBR 6118:2023 Table 7.1 | **Validated** |
| **7** | ABNT NBR 6118:2023 | Table 7.2 | Nominal cover ($c_{nom}$) for slabs (20, 25, 35, 45 mm) and beams/columns (25, 30, 40, 50 mm) | [`ConcreteStandards::getDurabilityRequirement()`](../source/core/ConcreteStandards.cpp#L59-L84) | `verify_calculations.cpp` (Scenario 1, Front 2) | NBR 6118:2023 Table 7.2 | **Validated** |
| **8** | ABNT NBR 6118:2023 | Section 7.4.7.6 | Coarse aggregate geometric compatibility ($D_{\max} \le 1.2 \cdot c_{nom}$) differentiated between beams/columns and slabs | [`ProjectData::checkDmcCoverCompliance()`](../source/core/ProjectData.cpp#L13-L36) and [`ConcreteStandards::validateMixDesign()`](../source/core/ConcreteStandards.cpp#L273-L315) | `verify_calculations.cpp` (Front 2: Grade 2 with DMC 25mm in EAC I) | NBR 6118:2023 Section 7.4.7.6 | **Validated** |
| **9** | ABNT NBR 8953:2015 | Table 1 | Slump consistency classification (S10, S50, S100, S160, S220) | [`ConcreteStandards::classifySlump()`](../source/core/ConcreteStandards.cpp#L131-L140) | `verify_calculations.cpp` (Scenarios 1, 2, 3 and Normative DB) | NBR 8953:2015 Table 1 | **Validated** |
| **10** | ABNT NBR 7212:2025 | Section 5.8 / Table 1 | Slump acceptance tolerances ($\pm 10\text{ mm}$, $\pm 20\text{ mm}$, $\pm 30\text{ mm}$) | [`ConcreteStandards::validateMixDesign()`](../source/core/ConcreteStandards.cpp#L261-L272) | `verify_calculations.cpp` (Front 2: Slump 100mm and Slump 0mm) | NBR 7212:2025 Section 5.8 and Table 1 | **Validated** |
| **11** | ABNT NBR 7212:2025 | Section 5.5 | Batching plant weighing tolerances (Cement $\pm 1\%$, Aggregates $\pm 3\%$, Water $\pm 3\%$, Admixtures $\pm 5\%$) | [`NormativeDatabase::getBatchingTolerances()`](../source/core/NormativeDatabase.cpp) | `verify_calculations.cpp` (Normative Database) | NBR 7212:2025 Section 5.5 | **Validated** |
| **12** | ABNT NBR 7211:2022 | Clauses 4 and 5 | Grading characterization and compacted bulk density in binary coarse aggregate blends | [`AggregateOptimizer::calculateBinaryBlend()`](../source/core/AggregateOptimizer.cpp) | `verify_calculations.cpp` (Coarse Aggregate Blend Optimization) | NBR 7211:2022 and Helene & Terzian (1993) | **Validated** |
| **13** | IPT / EPUSP | Helene & Terzian (1993) Ch. 3 | Abrams' Law ($f_{cj} \times w/c$), Lyse's Law ($m \times w/c$), and Molinari's Law ($C \times w/c$) | [`RegressionEngine::fitPolynomial2()`](../source/core/RegressionEngine.cpp#L25-L95), [`fitLinear()`](../source/core/RegressionEngine.cpp#L97-L138) | `verify_calculations.cpp` (Scenarios 1, 2, 3, 5; Front 1) | Helene & Terzian (1993); UFPR TC031 Spreadsheet | **Validated** |
| **14** | IPT / EPUSP | Helene & Terzian (1993) Ch. 4 | Absolute Volumes Equation: $1000 = C \cdot (1/\rho_c + a/\rho_a + p/\rho_p + (w/c)/\rho_w)$ | [`RegressionEngine::calculateTheoreticalConsumption()`](../source/core/RegressionEngine.cpp#L173-L191) | `verify_calculations.cpp` (Scenarios 1 to 5; Front 3) | Helene & Terzian (1993) p. 78 | **Validated** |
| **15** | IPT / EPUSP | Helene & Terzian (1993) Ch. 5 | Aggregate moisture correction and strict batch water balance | [`MixDesignCalculator::calculate()`](../source/core/MixDesignCalculator.cpp#L320-L375) | `verify_calculations.cpp` (Front 3: 1 m³ Mass Balance) | Strict mass balance: $M_{dry} + W_{theor} \equiv M_{wet} + W_{effective}$ | **Validated** |
| **16** | Brazilian Construction Practice | ABCP / Helene & Terzian | Jobsite volume gauge box sizing ($35 \times 45\text{ cm}$) per $50\text{ kg}$ bag of cement | [`MixDesignCalculator::calculate()`](../source/core/MixDesignCalculator.cpp#L417-L425) | `verify_calculations.cpp` (Front 3: Gauge box geometry in cm) | Actual height in centimeters: $h = (V_{liters} / (15.75 \cdot n)) \cdot 10$ | **Validated** |
| **17** | Numerical Robustness | Numerical Methods | Data centering and scaling in polynomial regression ($u_i = (x_i - \bar{x})/s_x$) | [`RegressionEngine::fitPolynomial2()`](../source/core/RegressionEngine.cpp#L45-L75) | `verify_calculations.cpp` (Front 1: Centering and Singularities) | Golub & Van Loan (Matrix Computations) | **Validated** |
| **18** | Mathematical Traceability | Software Audit | Explicit `SolutionType` tracking and compliance blocking upon regression failure | [`MixDesignCalculator::calculate()`](../source/core/MixDesignCalculator.cpp#L205-L270) | `verify_calculations.cpp` (Front 1: $f_{ck} = 150\text{ MPa}$) | Software technical transparency requirement | **Validated** |
| **19** | Economic Transparency | Cost Engineering | Explicit price units (`PriceUnit`: $/kg, $/50kg bag, $/t, $/m³) without heuristic inference | [`MixDesignCalculator::convertCementPriceToKg()`](../source/core/MixDesignCalculator.cpp#L18-L45) | `verify_calculations.cpp` (Front 4: Cement and Aggregate Conversions) | Cost input clarity requirement | **Validated** |
| **20** | Audit and Forensics | CONFEA / CREA / NBR 12655 | Technical report generation with mathematical resolution diagnostic table and traceability | [`ProjectData::generateMarkdownReport()`](../source/core/ProjectData.cpp#L535-L600) and [`generateHtmlReport()`](../source/core/ProjectData.cpp#L820-L890) | `verify_calculations.cpp` (Front 5: Diagnostic Tags and Badges) | Descriptive report for concrete mix design documentation | **Validated** |
| **21** | Currency Internationalization | Cost Engineering / Reports | Dynamic currency symbol adaptation (R$ for pt_BR and $ for en_US) in Technical Reports and cost tables | [`ProjectData::currencySymbol()`](../source/core/ProjectData.cpp) and [`ProjectData::generateHtmlReport()`](../source/core/ProjectData.cpp) | `verify_calculations.cpp` / CLI tests | Support for international markets and reports | **Validated** |
| **22** | Automated Auditing | CI/CD / Quality Control | Command-line normative audit (`concrete-mix-designer-cli verify`) validating strength, $w/c$, and cement consumption limits | [`main_cli.cpp`](../source/cli/main_cli.cpp) | `concrete-mix-designer-cli.exe test` | Exit codes 0 (OK) and 2 (Non-Compliant) | **Validated** |

---

### Classification of Requirements by Validation Nature

1. **Mandatory Deterministic Calculations:**
   - Target dosage strength ($f_{cj}$);
   - Maximum water/cement ratio and minimum cement consumption (NBR 12655);
   - Aggregate moisture correction and mass balance;
   - Volumetric sizing of jobsite gauge boxes.
   *Status: 100% verified and covered by unit and integration tests.*

2. **Geometric Compatibility Checks:**
   - Comparison of coarse aggregate DMC against nominal cover ($D_{\max} \le 1.2 \cdot c_{nom}$) for beams/columns and slabs.
   *Status: Differentiated treatment by structural element type implemented and validated.*

3. **Execution and Professional Judgment Requirements (Informative / Laboratory):**
   - Batching plant weighing tolerances (NBR 7212:2025);
   - Aggregate characterization tests (bulk density, absorption, Los Angeles abrasion, powdery material content);
   - Molding and curing of test specimens (NBR 5738) and axial compression testing (NBR 5739).
   *Status: Presented in the technical report and normative database as reference parameters for academic study, making it mandatory to manually check all results and validate them in more consolidated market software prior to any real-world application, under complete legal liability exemption of the author.*
