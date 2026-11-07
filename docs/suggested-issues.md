# Roteiro de Evolução e Issues Sugeridas / Roadmap & Suggested Issues — Concrete Mix Design C++

[Português (Brasil)](#português-brasil) | [English (United States)](#english-united-states)

---

## Português (Brasil)

### 1.Itens Concluídos na Versão `2026.11.6.0`

- [x] **Auditoria Completa da Planilha Experimental UFPR (TC031):** Correção de precedência de células, eliminação de arredondamentos prematuros, correção da inversão de preços de areia/brita e criação do **Cenário 5** dedicado.
- [x] **Validação Normativa ABNT NBR Completa:** Conformidade com **NBR 12655:2022** (incluindo restrições de $f_{ck}$ para Condições B e C), **NBR 6118:2023** ($D_{\max} \le 1,2 \cdot c_{\text{nom}}$), **NBR 7211:2022**, **NBR 7212:2025** (item 5.5: $\pm 1\%$, $\pm 3\%$, $\pm 3\%$, $\pm 5\%$) e **NBR 8953:2015**.
- [x] **Correção de Edição das `QDoubleSpinBox`:** Eliminação definitiva do preenchimento automático de zeros à direita durante a digitação.
- [x] **Menu `Configurações` (`Ctrl+I`):** Configuração de `Projeto:`, `Responsável:`, `Instituição:` e `Matrícula / ID:`, com opção de reportar automaticamente o `Cenário da Planilha` ou editar manualmente.
- [x] **Tradução Dinâmica em Tempo de Execução (`pt_BR` $\leftrightarrow$ `en_US`):** Tradução instantânea na **Aba 7 (Relatório Técnico)**, no **Cenário 5** e em 100% das 309 chaves dos arquivos `.ts` e `.qm`.
- [x] **Conformidade Microsoft Store, Microsoft Partner Center e GitHub:** Revisão jurídica e técnica de todos os arquivos em `source/assets/`, `docs/` e raiz do projeto.
- [x] **5º Gráfico Unificado Abrams/Lyse/Molinari (IPT):** Visualização unificada das 3 curvas experimentais em coordenadas limpas com linhas de grade nítidas, sem sobreposição de cores e com exportação em alta resolução.
- [x] **Adequação Dinâmica de Moeda (R$ / $):** Símbolo monetário adaptado automaticamente em tempo de execução na Aba 7 e tabelas orçamentárias.
- [x] **Interface de Linha de Comando (CLI - `concrete-mix-designer-cli`):** Suíte completa de terminal com comandos `design`, `calc`, `verify`, `report`, `standards`, `presets` e `test`.
- [x] **Auditoria de Concorrência e Robustez Numérica:** Proteção por `QRecursiveMutex` no `NormativeDatabase`, gestão segura do ciclo de vida de janelas na GUI e proteções contra divisão por zero em `RegressionEngine`.

---

### 2. Melhorias Futuras Sugeridas (Backlog de Engenharia)

1. **`[Enhancement]` Curvas Granulométricas Completas por Peneiramento (ABNT NBR NM 248 / NBR 7211):**
   - Adicionar gráfico semilogarítmico interativo das zonas utilizável e ótima de agregado miúdo e faixas de britas (Brita 0, 1, 2, 3), calculando automaticamente o Módulo de Finura ($MF$) e a Dimensão Máxima Característica ($D_{\max}$) a partir das porcentagens retidas em cada peneira da série normal e intermediária.
2. **`[Enhancement]` Dosagem de Aditivos Plastificantes e Superplastificantes (ABNT NBR 11768:2019):**
   - Incluir campo específico de teor de aditivo químico ($\%$ sobre a massa de cimento) e desconto automático do volume líquido de aditivo na água de amassamento para concretos de alto desempenho ($f_{ck} > 50\text{ MPa}$).
3. **`[Enhancement]` Adições Minerais Pozolânicas e Sílica Ativa (ABNT NBR 12653 / NBR 13956):**
   - Permitir o cálculo explícito do fator água/aglomerante equivalente ($a/\text{agl}$) quando houver substituição parcial ou adição de sílica ativa, metacaulim ou cinza volante em central dosadora.
4. **`[Feature]` Persistência de Arquivos de Estudo (`.cmdjson`) via Menu Arquivo:**
   - Adicionar ações `Abrir Estudo... (Ctrl+O)` e `Salvar Estudo Como... (Ctrl+S)` utilizando os métodos já implementados `ProjectData::toJson()` e `ProjectData::fromJson()`.

---

## English (United States)

### 1. Completed Items in Version `2026.11.6.0`

- [x] **Comprehensive Audit of the UFPR Experimental Spreadsheet (TC031):** Fixed cell precedence conflicts, eliminated premature intermediate rounding, corrected swapped sand/coarse aggregate unit prices, and created the dedicated **Scenario 5**.
- [x] **Full ABNT NBR Normative Validation:** Compliance with **NBR 12655:2022** (including $f_{ck}$ restrictions for Conditions B and C), **NBR 6118:2023** ($D_{\max} \le 1.2 \cdot c_{\text{nom}}$), **NBR 7211:2022**, **NBR 7212:2025** (Section 5.5: $\pm 1\%$, $\pm 3\%$, $\pm 3\%$, $\pm 5\%$), and **NBR 8953:2015**.
- [x] **`QDoubleSpinBox` Editing Fix:** Permanently eliminated automatic trailing-zero padding while typing.
- [x] **`Settings` Menu (`Ctrl+I`):** Configuration of `Project:`, `Engineer:`, `Institution:`, and `Registration / ID:`, with the option to automatically report the `Spreadsheet Scenario` or edit manually.
- [x] **Dynamic Runtime Translation (`pt_BR` $\leftrightarrow$ `en_US`):** Instant runtime translation across **Tab 7 (Technical Report)**, **Scenario 5**, and 100% of the 309 keys in the `.ts` and `.qm` files.
- [x] **Microsoft Store, Microsoft Partner Center, and GitHub Compliance:** Legal and technical alignment across all files in `source/assets/`, `docs/`, and the project root.
- [x] **5th Unified Abrams/Lyse/Molinari (IPT) Diagram:** Combined view of the 3 experimental curves in normalized coordinates with high-contrast grid lines, zero color fills, and high-DPI export.
- [x] **Dynamic Currency Symbol Adaptability ($ / R$):** Automatic runtime switching of currency symbols across Tab 7 reports and cost analysis tables.
- [x] **Command Line Interface (CLI - `concrete-mix-designer-cli`):** Standalone terminal suite with `design`, `calc`, `verify`, `report`, `standards`, `presets`, and `test` commands.
- [x] **Concurrency and Numerical Hardening Audit:** Thread-safe `NormativeDatabase` via `QRecursiveMutex`, leak-free GUI dialog management, and division-by-zero protections in `RegressionEngine`.

---

### 2. Suggested Future Enhancements (Engineering Backlog)

1. **`[Enhancement]` Full Sieve Analysis Grading Curves (ABNT NBR NM 248 / NBR 7211):**
   - Add an interactive semi-logarithmic chart of usable and optimal fine aggregate zones and coarse aggregate grading envelopes (Grades 0, 1, 2, 3), automatically computing the Fineness Modulus ($FM$) and Maximum Characteristic Size ($D_{\max}$) from retained percentages across standard and intermediate sieves.
2. **`[Enhancement]` Plasticizer and Superplasticizer Admixture Proportioning (ABNT NBR 11768:2019):**
   - Include a dedicated chemical admixture dosage field ($\%$ by mass of cement) and automatic deduction of liquid admixture volume from mixing water for high-performance concretes ($f_{ck} > 50\text{ MPa}$).
3. **`[Enhancement]` Pozzolanic Mineral Additions and Silica Fume (ABNT NBR 12653 / NBR 13956):**
   - Enable explicit calculation of the equivalent water/binder ratio ($w/b$) when incorporating partial replacement or addition of silica fume, metakaolin, or fly ash in batching plants.
4. **`[Feature]` Study File Persistence (`.cmdjson`) via the File Menu:**
   - Add `Open Study... (Ctrl+O)` and `Save Study As... (Ctrl+S)` actions leveraging the already implemented `ProjectData::toJson()` and `ProjectData::fromJson()` methods.
