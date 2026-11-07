# Matriz de Rastreabilidade Normativa e Científica
## Concrete Mix Designer C++ (v2026.11.6.0)

Esta matriz estabelece o mapeamento estrito e bidirecional entre os requisitos das normas técnicas brasileiras (ABNT NBR), a literatura científica de referência (Método IPT/EPUSP, Helene & Terzian, Mehta & Monteiro), as funções implementadas no código-fonte em C++20 e as verificações executadas na suíte de testes automatizados (`tests/verify_calculations.cpp`).

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
   *Status: Apresentados no memorial técnico e no banco de dados normativo como parâmetros orientativos para suporte à decisão do Engenheiro Responsável.*
