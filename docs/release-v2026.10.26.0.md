# Notas da Versão / Release Notes — v2026.10.26.0

[Português (Brasil)](#português-brasil) | [English (United States)](#english-united-states)

---

## Português (Brasil)

### Identificação da Release
- **Produto:** Concrete Mix Design C++ — Dosagem Experimental de Concreto (Método IPT/EPUSP)
- **Versão:** `2026.10.26.0`
- **Data de Lançamento:** 26 de outubro de 2026
- **Autor e Publicador:** Fernando Nillsson Cidade
- **Plataforma:** Microsoft Windows 10 (1809+) e Windows 11 (x64)
- **Canais de Distribuição:** Microsoft Store (Microsoft Partner Center) e GitHub Releases

---

### Novidades e Correções Definitivas na Versão `2026.10.26.0`

1. **Motor Analítico IPT/EPUSP e Banco Normativo SQLite (`:memory:`):**
   - Regressão por mínimos quadrados ordinários (OLS) em ponto flutuante de 64 bits (`double`) para as Leis de **Abrams**, **Lyse** e **Molinari** ($R^2$).
   - Validação automática das normas **ABNT NBR 12655:2022** (Tabela 6 e item 5.6.3.1 — bloqueio de $f_{ck} > 20\text{ MPa}$ na Condição B e $f_{ck} > 15\text{ MPa}$ na Condição C), **NBR 6118:2023** (cobrimento nominal e $D_{\max} \le 1,2 \cdot c_{\text{nom}}$), **NBR 7211:2022** (mistura binária de britas), **NBR 7212:2025** (item 5.5 — tolerâncias de pesagem: Cimento e Adições $\pm 1\%$, Agregados $\pm 3\%$, Água $\pm 3\%$, Aditivos $\pm 5\%$) e **NBR 8953:2015** (classes de abatimento *Slump* S10 a S220).
2. **Novo Menu `Configurações` (`Ctrl+I`):**
   - Permite configurar **Projeto:**, **Responsável:**, **Instituição:** e **Matrícula / ID:** diretamente na barra de menus.
   - Disponibiliza a escolha entre **reportar automaticamente as informações selecionadas em "Cenário da Planilha"** (com tradução dinâmica em tempo de execução) ou **editar as informações de "Projeto:" manualmente**.
3. **Correção Definitiva de Digitação nas `QDoubleSpinBox`:**
   - Removido o preenchimento automático intrusivo de zeros à direita durante a digitação nos campos numéricos de todas as abas (`TabProjectParams`, `TabMaterials`, `TabMortarContent`, `TabFinalMix`, `TabCosts`).
4. **Tradução Dinâmica Integral (`pt_BR` $\leftrightarrow$ `en_US`):**
   - O **Cenário 5** (*Planilha UFPR Original Corrigida*) e todas as informações de cabeçalho (`Projeto:`, `Responsável:`, `Instituição:`, `Matrícula / ID:`) e corpo do relatório na aba **7. Relatório Técnico** traduzem-se dinamicamente em tempo de execução, com **309 chaves de tradução completas (0 `unfinished`)** nos arquivos `.ts` e `.qm`.
5. **Conformidade Microsoft Store e Documentação Bilíngue Simétrica:**
   - Alinhamento integral dos arquivos em `source/assets/`, `docs/` e raiz do projeto com as diretrizes da Microsoft, Microsoft Store, Microsoft Partner Center e legislação aplicável.
6. **5º Gráfico Unificado Abrams/Lyse/Molinari (IPT) e Renderização Vetorial Nítida:**
   - Implementado o painel `IptUnifiedDiagramWidget` na Aba 4 reunindo as três curvas de dosagem com réguas de precisão nos eixos e remoção de áreas coloridas sob a curva Molinari.
   - Otimizada a legibilidade dos rótulos e valores no Diagrama de 4 Quadrantes e a ferramenta de exportação em alta resolução de imagens (`.png`) e `.pdf`.
7. **Símbolo de Moeda Dinâmico Bilíngue (R$ / $):**
   - O Memorial de Cálculo Técnico (Aba 7) e as tabelas de materiais e custos adaptam o símbolo da moeda em tempo de execução ao alternar o idioma (R$ para Português e $ para Inglês).
8. **Nova Ferramenta CLI (`concrete-mix-designer-cli.exe`) e Concorrência Segura:**
   - Utilitário autônomo de terminal com comandos `design`, `calc`, `verify`, `report`, `standards`, `presets` e `test`.
   - Banco SQLite em memória (`NormativeDatabase`) sincronizado com `QRecursiveMutex` e proteções rigorosas contra divisão por zero em `RegressionEngine`.

---

## English (United States)

### Release Identification
- **Product:** Concrete Mix Design C++ — Experimental Concrete Mix Design (IPT/EPUSP Method)
- **Version:** `2026.10.26.0`
- **Release Date:** October 26, 2026
- **Author and Publisher:** Fernando Nillsson Cidade
- **Platform:** Microsoft Windows 10 (1809+) and Windows 11 (x64)
- **Distribution Channels:** Microsoft Store (Microsoft Partner Center) and GitHub Releases

---

### New Features and Permanent Fixes in Version `2026.10.26.0`

1. **IPT/EPUSP Analytical Engine and In-Memory SQLite Normative Database (`:memory:`):**
   - 64-bit floating-point (`double`) ordinary least squares (OLS) regression for **Abrams'**, **Lyse's**, and **Molinari's** Laws ($R^2$).
   - Automatic validation against **ABNT NBR 12655:2022** (Table 6 and Section 5.6.3.1 — blocking $f_{ck} > 20\text{ MPa}$ under Condition B and $f_{ck} > 15\text{ MPa}$ under Condition C), **NBR 6118:2023** (nominal cover and $D_{\max} \le 1.2 \cdot c_{\text{nom}}$), **NBR 7211:2022** (binary coarse aggregate blends), **NBR 7212:2025** (Section 5.5 — batching tolerances: Cement and Additions $\pm 1\%$, Aggregates $\pm 3\%$, Water $\pm 3\%$, Admixtures $\pm 5\%$), and **NBR 8953:2015** (Slump consistency classes S10 to S220).
2. **New `Settings` Menu (`Ctrl+I`):**
   - Enables configuring **Project:**, **Engineer:**, **Institution:**, and **Registration / ID:** directly from the menu bar.
   - Provides the choice between **automatically reporting the information selected in "Spreadsheet Scenario"** (with dynamic runtime translation) or **editing the "Project:" information manually**.
3. **Permanent Fix for `QDoubleSpinBox` Typing Behavior:**
   - Eliminated intrusive automatic trailing-zero padding while typing in numeric fields across all tabs (`TabProjectParams`, `TabMaterials`, `TabMortarContent`, `TabFinalMix`, `TabCosts`).
4. **Full Dynamic Runtime Translation (`pt_BR` $\leftrightarrow$ `en_US`):**
   - **Scenario 5** (*Corrected Original UFPR Spreadsheet*) and all header metadata (`Project:`, `Engineer:`, `Institution:`, `Registration / ID:`) and report body sections in **7. Technical Report** translate dynamically at runtime, backed by **309 completed translation keys (0 `unfinished`)** in the `.ts` and `.qm` files.
5. **Microsoft Store Compliance and Symmetric Bilingual Documentation:**
   - Comprehensive alignment of all files in `source/assets/`, `docs/`, and the project root with Microsoft, Microsoft Store, Microsoft Partner Center guidelines, and applicable legislation.
6. **5th Unified Abrams/Lyse/Molinari (IPT) Diagram and Crisp Vector Rendering:**
   - Implemented the `IptUnifiedDiagramWidget` panel in Tab 4 bringing together the three mix design curves with precision axis grid lines and removing solid color fills under the Molinari curve.
   - Enhanced label readability on the 4-Quadrant Diagram and calibrated the high-resolution image export (`.png`) and `.pdf` features.
7. **Dynamic Bilingual Currency Symbol (R$ / $):**
   - The Technical Calculation Report (Tab 7) and cost/materials tables dynamically adjust their currency symbol at runtime upon language changes ($ in English and R$ in Portuguese).
8. **New CLI Executable (`concrete-mix-designer-cli.exe`) and Thread-Safety:**
   - Standalone terminal utility supporting `design`, `calc`, `verify`, `report`, `standards`, `presets`, and `test` commands.
   - In-memory SQLite database (`NormativeDatabase`) synchronized with `QRecursiveMutex` and strict division-by-zero guards in `RegressionEngine`.
