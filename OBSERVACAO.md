# Notas Técnicas, Auditoria Normativa e Conformidade Legal / Technical Notes, Normative Audit, and Legal Compliance (`OBSERVACAO.md`)

[Português (Brasil)](#português-brasil) | [English (United States)](#english-united-states)

---

## Português (Brasil)

### 1. Objetivo deste Documento

Este documento registra as observações técnicas de engenharia civil, o resultado da auditoria completa realizada sobre a planilha experimental de referência da UFPR (`TC031_Atividade09e10.Dosagem2_17.11.2021_Aperfeicoada.xlsx`), as validações frente às normas vigentes da **ABNT**, as melhorias de interface e internacionalização dinâmica implementadas na versão **2026.11.6.0** e o enquadramento jurídico para publicação no **GitHub** e distribuição comercial na **Microsoft Store** (**Microsoft Partner Center**).

---

### 2. Auditoria da Planilha Experimental UFPR (TC031) e Comparação com o Código C++20

A planilha original estruturada em 7 abas (`Composição de Britas`, `Teor de Argamassa`, `Traços ajustados`, `Traço final`, `Resultados (0)`, `Resultados (1)`, `Resultados (2)` e `Resultados (3)`) foi integralmente inspecionada quanto à árvore de precedência e dependência de células de entrada e fórmulas, resultando nas seguintes constatações e correções definitivas no **Concrete Mix Design C++**:

1. **Conflitos de Precedência / Sobrescrita na Planilha vs. Isolamento de Variáveis no C++:**
   - Na aba `Teor de Argamassa` da planilha, as células `B32:E32` (linha de $\alpha = 52\%$) continham valores literais fixos (`0,52`, `1`, `2,03`, `2,97`) sobrescritos sobre as fórmulas paramétricas que nas demais linhas dependiam de `m` (`B17`). Além disso, em `Traços ajustados!D15`, o traço rico havia sido digitado como literal `3,5` em vez de referenciar `=E15-1,5` (`3,36` quando $m=4,86$).
   - **Solução no C++:** O módulo [`MortarContentOptimizer`](source/core/MortarContentOptimizer.cpp) e o orquestrador [`ProjectData::recalculateAll()`](source/core/ProjectData.cpp) separam estritamente variáveis independentes de entrada e grandezas derivadas por fórmula, eliminando qualquer sobrescrita acidental.
2. **Inversão de Preços Unitários de Areia e Brita na Planilha:**
   - Nas abas `Resultados (0)` a `(3)`, a planilha multiplicou o consumo de areia por $\text{R\$ } 60,00/\text{t}$ e o consumo de brita por $\text{R\$ } 80,00/\text{t}$, invertendo os preços declarados no cabeçalho do próprio exercício ($\text{Areia} = \text{R\$ } 80,00/\text{t}$ e $\text{Brita} = \text{R\$ } 60,00/\text{t}$).
   - **Solução no C++:** O módulo [`MixDesignCalculator`](source/core/MixDesignCalculator.cpp) vincula cada insumo ao seu respectivo `unitCost` cadastrado em `Materials`, preservando a integridade contábil do orçamento.
3. **Arredondamentos Intermediários vs. Precisão IEEE 754 (`double`):**
   - Na aba `Traço final`, a planilha truncou $a/c = 0,473399$ para `0,47` e utilizou $C = 335\text{ kg/m}^3$ lido visualmente do gráfico para calcular a água ($157,45\text{ L/m}^3$), gerando soma volumétrica de $1,0017\text{ m}^3 \neq 1,000\text{ m}^3$.
   - **Solução no C++:** O cálculo analítico preserva a precisão de ponto flutuante de 64 bits (`double`) em toda a cadeia de regressão (Abrams $\to$ Lyse $\to$ Molinari), garantindo fechamento volumétrico exato de $1,000\text{ m}^3$ e disponibilizando a **Aba 6 (Análise de Custos)** para comparar o resultado analítico exato com qualquer leitura gráfica manual (como $a/c = 0,47$, $m = 5,80$ e $C = 335\text{ kg/m}^3$).
4. **Cenário 5 Dedicado (`loadScenario5_PlanilhaOriginal()`):**
   - Foi incluído o **Cenário 5** na barra de ferramentas principal, carregando fielmente todos os parâmetros experimentais do estudo TC031 da UFPR ($f_{ck} = 30\text{ MPa}$, $S_d = 3,5\text{ MPa}$, $f_{cj} = 35,775\text{ MPa}$, $\alpha = 52\%$, mistura $80\%$ B2 / $20\%$ B1, betoneira de $29\text{ L}$ e obra de $30\text{ m}^3$).

---

### 3. Validação frente às Normas Técnicas Brasileiras (ABNT NBR)

| Norma Técnica | Escopo e Regra Validada no Código (`source/core/`) |
| :--- | :--- |
| **ABNT NBR 12655:2022** | Tabela 6: Classes de Agressividade Ambiental CAA I a IV ($a/c_{\max}$, $f_{ck,\min}$ e $C_{\min}$ para CA e CP). Item 5.6.3.1: Condição A ($S_d = 4,0\text{ MPa}$, todas as classes), Condição B ($S_d = 5,5\text{ MPa}$, classes C10 a C20 — bloqueando automaticamente $f_{ck} > 20\text{ MPa}$) e Condição C ($S_d = 7,0\text{ MPa}$, classes C10 e C15 — bloqueando $f_{ck} > 15\text{ MPa}$). |
| **ABNT NBR 6118:2023** | Tabela 7.2: Cobrimentos nominais ($c_{\text{nom}}$ de $20\text{ mm}$ a $55\text{ mm}$ para lajes, vigas e pilares em CA e CP). Item 7.4.7.6: Verificação obrigatória $D_{\max} \le 1,2 \cdot c_{\text{nom}}$. |
| **ABNT NBR 7211:2022** | Caracterização de agregados miúdos e graúdos e otimização experimental da massa unitária compactada de misturas binárias de britas (NBR NM 45). |
| **ABNT NBR 7212:2025** | Item 5.5 (Tolerâncias operacionais para a dosagem dos materiais em central): **Cimento e Adições: $\pm 1\%$**, **Agregados: $\pm 3\%$**, **Água: $\pm 3\%$** e **Aditivos: $\pm 5\%$**. |
| **ABNT NBR 8953:2015** | Tabela 1 (Grupos I e II de resistência característica, C20 a C100) e Tabela 2 (Classes de consistência pelo abatimento de tronco de cone: **S10** $10 \le A < 50\text{ mm}$, **S50** $50 \le A < 100\text{ mm}$, **S100** $100 \le A < 160\text{ mm}$, **S160** $160 \le A < 220\text{ mm}$ e **S220** $A \ge 220\text{ mm}$). |

---

### 4. Correções de Interface Gráfica, Menu "Configurações" e Tradução Dinâmica (`v2026.11.6.0`)

1. **Fluidez de Digitação nas `QDoubleSpinBox`:**
   - Foi eliminada a propriedade `setSpecialValueText(tr(" "))` (que causava re-formatação imediata pela classe interna `QAbstractSpinBoxPrivate` ao sair do valor mínimo `0,0`), as atualizações de `setValue()` foram protegidas por verificação de foco (`!spin->hasFocus()`) e a atualização de sufixo monetário do cimento foi transferida para `editingFinished`. Assim, o usuário pode digitar livremente qualquer número sem inserção automática de zeros à direita a cada tecla pressionada.
2. **Novo Menu `Configurações` (`Ctrl+I`):**
   - Adicionado na barra de menus principal (`&Configurações` / `&Settings`), permitindo configurar **Projeto:**, **Responsável:**, **Instituição:** e **Matrícula / ID:**, com escolha direta entre reportar automaticamente as informações selecionadas em **Cenário da Planilha** ou editar manualmente o nome do projeto.
3. **Tradução Dinâmica em Tempo de Execução na Aba 7 e no Cenário 5:**
   - O **Cenário 5** na barra de ferramentas e todo o cabeçalho e conteúdo do relatório gerado na aba **7. Relatório Técnico** (incluindo o campo **Projeto:** quando vinculado ao cenário ativo) traduzem-se instantaneamente em tempo de execução ao alternar entre `pt_BR` e `en_US`, com **309 chaves de tradução 100% finalizadas (0 `unfinished`)** nos arquivos `.ts` e `.qm`.

---

### 5. Adequação à Microsoft Store, Microsoft Partner Center e Legislação

Para atender integralmente aos requisitos de publicação aberta no **GitHub** e comercialização oficial na **Microsoft Store** (**Microsoft Partner Center — Application Developer Agreement v8.9**):
- **Modelo de Licenciamento Transparente (`CLC` e `EULA`):** Harmoniza a disponibilidade do código-fonte no GitHub sob a licença GNU AGPLv3 para auditoria acadêmica/científica com o licenciamento comercial dos pacotes binários assinados distribuídos na Microsoft Store.
- **Conformidade LGPLv3 (Qt 6.11.1) e GPLv3 Runtime Exception (MinGW-w64):** Vinculação 100% dinâmica das DLLs do Qt6 e preservação integral dos textos de licença em `source/assets/LICENSES/`.
- **Privacidade e Segurança (*Local-First*):** Zero coleta de dados pessoais, zero telemetria e operação 100% offline, em conformidade com a Política 10.5 da Microsoft Store, **LGPD (Lei nº 13.709/2018)** e **GDPR (Regulamento UE 2016/679)**.
- **Legislação de Propriedade Intelectual e Engenharia:** Conformidade com as Leis Federais Brasileiras nº 9.609/1998, nº 9.610/1998, nº 8.078/1990 (CDC), nº 5.194/1966 e nº 6.496/1977 (CONFEA/CREA — exigência de ART), bem como *U.S. Copyright Act* (17 U.S.C. § 101 et seq.) e Convenção de Berna.

---

### 6. Inovações e Recursos Adicionados na Versão 2026.11.6.0

1. **5º Gráfico Unificado Abrams/Lyse/Molinari (IPT) e Otimização Visual:**
   - Implementado o painel "5° Abrams/Lyse/Molinari (IPT)" (`IptUnifiedDiagramWidget`) que integra as três curvas experimentais em coordenadas normalizadas com linhas de grade nítidas nos eixos horizontal e vertical, eliminando áreas coloridas sólidas sob a curva Molinari.
   - Ajustada a legibilidade das 4 réguas do eixo vertical no Diagrama de 4 Quadrantes (`QuadChartWidget`), com fontes balanceadas e visibilidade aprimorada dos valores numéricos.
   - A função "Exportar Imagem dos Gráficos..." foi calibrada para renderizar imagens de altíssima nitidez e proporção idêntica à visualização em tela.
2. **Símbolo de Moeda Dinâmico Bilíngue (R$ / $):**
   - Na alternância de idiomas (Português BR $\leftrightarrow$ Inglês US), o símbolo da moeda nos relatórios técnicos (Aba 7) e tabelas orçamentárias se ajusta automaticamente entre "R$" e "$", sem necessidade de reiniciar a aplicação.
3. **Nova Interface de Linha de Comando Profissional (`concrete-mix-designer-cli`):**
   - Disponibilização do utilitário autônomo de terminal com comandos `design`, `calc`, `verify`, `report`, `standards`, `presets` e `test`.
   - Permite integração em esteiras de automação (CI/CD) e emissão de laudos técnicos em Markdown, HTML, JSON e texto plano.
4. **Auditoria de Concorrência, Memória e Estabilidade Algorítmica:**
   - Adicionada proteção multithreading com `QRecursiveMutex` no banco SQLite em memória (`NormativeDatabase`).
   - Ciclo de vida seguro de janelas e diálogos na GUI (`MainWindow`), eliminando deleções concorrentes de memória.
   - Proteções algorítmicas adicionais contra divisões por zero e logaritmos fora do domínio positivo no módulo `RegressionEngine`.

```
**Root Path:** `Concrete_Mix_Designer_cpp\source`

```
├── 📁 assets
│   ├── 📁 ABOUT
│   │   ├── 📄 ABOUT_en_US.txt
│   │   ├── 📄 ABOUT_pt_BR.txt
│   │   ├── 📄 History_APP_en_US.txt
│   │   └── 📄 History_APP_pt_BR.txt
│   ├── 📁 CLC
│   │   ├── 📄 CLC_en_US.txt
│   │   └── 📄 CLC_pt_BR.txt
│   ├── 📁 COPYRIGHT
│   │   ├── 📄 AVISO DE COPYRIGHT E MARCA REGISTRADA_pt_BR.txt
│   │   └── 📄 COPYRIGHT AND TRADEMARK NOTICE_en_US.txt
│   ├── 📁 EULA
│   │   ├── 📄 EULA_en_US.txt
│   │   └── 📄 EULA_pt_BR.txt
│   ├── 📁 LICENSES
│   │   ├── 📄 Apache License Version 2.0.txt
│   │   ├── 📄 BSD License Clause-2.txt
│   │   ├── 📄 BSD License Clause-3.txt
│   │   ├── 📄 GNU AFFERO GENERAL PUBLIC LICENSE Version3.txt
│   │   ├── 📄 GNU GENERAL PUBLIC LICENSE Version 3.txt
│   │   ├── 📄 GNU LESSER GENERAL PUBLIC LICENSE Version 2.1.txt
│   │   ├── 📄 GNU LESSER GENERAL PUBLIC LICENSE Version 3.txt
│   │   ├── 📄 ISC LICENSE.txt
│   │   ├── 📄 MIT License.txt
│   │   ├── 📄 PYTHON SOFTWARE FOUNDATION LICENSE.txt
│   │   └── 📄 SQLite Is Public Domain.txt
│   ├── 📁 NOTICES
│   │   ├── 📄 NOTICE_en_US.txt
│   │   └── 📄 NOTICE_pt_BR.txt
│   ├── 📁 PRIVACY_POLICY
│   │   ├── 📄 Privacy_Policy_en_US.txt
│   │   └── 📄 Privacy_Policy_pt_BR.txt
│   ├── 📁 RELEASE
│   │   ├── 📄 RELEASE NOTES_en_US.txt
│   │   └── 📄 RELEASE NOTES_pt_BR.txt
│   └── 📁 icons
│       ├── 📄 concrete_mix_design.ico
│       ├── 🖼️ concrete_mix_design.png
│       ├── 🖼️ concrete_mix_design_1080-1080.png
│       ├── 🖼️ concrete_mix_design_1440-2160.png
│       ├── 🖼️ concrete_mix_design_150-150.png
│       ├── 🖼️ concrete_mix_design_1920-1080.png
│       ├── 🖼️ concrete_mix_design_2160-2160.png
│       ├── 🖼️ concrete_mix_design_300-300.png
│       ├── 🖼️ concrete_mix_design_3840-2160.png
│       ├── 🖼️ concrete_mix_design_584-800.png
│       └── 🖼️ concrete_mix_design_71-71.png
├── 📁 cli
│   └── ⚡ main_cli.cpp
├── 📁 core
│   ├── ⚡ AggregateOptimizer.cpp
│   ├── ⚡ AggregateOptimizer.hpp
│   ├── ⚡ ConcreteStandards.cpp
│   ├── ⚡ ConcreteStandards.hpp
│   ├── ⚡ MixDesignCalculator.cpp
│   ├── ⚡ MixDesignCalculator.hpp
│   ├── ⚡ MortarContentOptimizer.cpp
│   ├── ⚡ MortarContentOptimizer.hpp
│   ├── ⚡ NormativeDatabase.cpp
│   ├── ⚡ NormativeDatabase.hpp
│   ├── ⚡ ProjectData.cpp
│   ├── ⚡ ProjectData.hpp
│   ├── ⚡ RegressionEngine.cpp
│   └── ⚡ RegressionEngine.hpp
├── 📁 gui
│   ├── ⚡ IptUnifiedDiagramWidget.cpp
│   ├── ⚡ IptUnifiedDiagramWidget.hpp
│   ├── ⚡ MainWindow.cpp
│   ├── ⚡ MainWindow.hpp
│   ├── ⚡ ManualDialog.cpp
│   ├── ⚡ ManualDialog.hpp
│   ├── ⚡ QuadChartWidget.cpp
│   ├── ⚡ QuadChartWidget.hpp
│   ├── ⚡ SobreDialog.cpp
│   ├── ⚡ SobreDialog.hpp
│   ├── ⚡ TabCosts.cpp
│   ├── ⚡ TabCosts.hpp
│   ├── ⚡ TabExperimentalMixes.cpp
│   ├── ⚡ TabExperimentalMixes.hpp
│   ├── ⚡ TabFinalMix.cpp
│   ├── ⚡ TabFinalMix.hpp
│   ├── ⚡ TabMaterials.cpp
│   ├── ⚡ TabMaterials.hpp
│   ├── ⚡ TabMortarContent.cpp
│   ├── ⚡ TabMortarContent.hpp
│   ├── ⚡ TabProjectParams.cpp
│   ├── ⚡ TabProjectParams.hpp
│   ├── ⚡ TabTechnicalReport.cpp
│   └── ⚡ TabTechnicalReport.hpp
├── 📁 language
│   ├── 📁 locale
│   │   ├── 📄 concrete_mix_design_en_US.qm
│   │   ├── 📄 concrete_mix_design_en_US.ts
│   │   ├── 📄 concrete_mix_design_pt_BR.qm
│   │   └── 📄 concrete_mix_design_pt_BR.ts
│   ├── ⚡ GerenciadorTraducao.cpp
│   └── ⚡ GerenciadorTraducao.hpp
└── 📁 utils
    ├── ⚡ ApplicationPathUtils.cpp
    ├── ⚡ ApplicationPathUtils.hpp
    ├── ⚡ CaminhoPersistenteUtils.cpp
    ├── ⚡ CaminhoPersistenteUtils.hpp
    ├── ⚡ LogManager.cpp
    └── ⚡ LogManager.hpp
```

**Root Path:** `Concrete_Mix_Designer_cpp\tests`

```
└── ⚡ verify_calculations.cpp
```

**Root Path:** `Concrete_Mix_Designer_cpp\docs`

```
├── 📁 images
│   ├── 🖼️ banner.png
│   └── 🖼️ logo.png
├── 📝 architecture.md
├── 📝 cli-reference.md
├── 📝 compliance.md
├── 📝 licensing.md
├── 📝 matriz_rastreabilidade_normativa.md
├── 📝 release-v2026.10.26.0.md
├── 📝 release-v2026.11.6.0.md
├── 📝 security-model.md
├── 📝 suggested-issues.md
└── 📝 user-guide.md
```

**Root Path:** `Concrete_Mix_Designer_cpp\mocks`

```
├── 📁 icons
│   ├── 📄 concrete_mix_design.ico
│   ├── 🖼️ concrete_mix_design.png
│   ├── 🖼️ concrete_mix_design_1080-1080.png
│   ├── 🖼️ concrete_mix_design_1440-2160.png
│   ├── 🖼️ concrete_mix_design_150-150.png
│   ├── 🖼️ concrete_mix_design_1920-1080.png
│   ├── 🖼️ concrete_mix_design_2160-2160.png
│   ├── 🖼️ concrete_mix_design_300-300.png
│   ├── 🖼️ concrete_mix_design_3840-2160.png
│   ├── 🖼️ concrete_mix_design_584-800.png
│   └── 🖼️ concrete_mix_design_71-71.png
├── 📄 CMakeLists.txt
├── ⚡ CompileTranslationsGUI.cpp
├── ⚡ CompileTranslationsGUI.hpp
├── ⚡ CompileTranslationsTerminal.cpp
├── ⚡ CompileTranslationsTerminal.hpp
├── ⚡ find_VersionEditor.cpp
└── ⚡ find_VersionEditor.hpp
```

**Root Path:** `Concrete_Mix_Designer_cpp\repo\LISTAGEM`

```
├── 📁 en_US
│   ├── 🖼️ Captura de tela 2026-10-26 053905.png
│   ├── 🖼️ Captura de tela 2026-10-26 053915.png
│   ├── 🖼️ Captura de tela 2026-10-26 053927.png
│   ├── 🖼️ Captura de tela 2026-10-26 053938.png
│   ├── 🖼️ Captura de tela 2026-10-26 053947.png
│   ├── 🖼️ Captura de tela 2026-10-26 053957.png
│   ├── 🖼️ Captura de tela 2026-10-26 054008.png
│   ├── 🖼️ Captura de tela 2026-10-26 054025.png
│   ├── 🖼️ Captura de tela 2026-10-26 054224.png
│   ├── 🖼️ Captura de tela 2026-10-26 054233.png
│   ├── 🖼️ Captura de tela 2026-10-26 054243.png
│   └── 🖼️ Captura de tela 2026-10-26 054253.png
└── 📁 pt_BR
    ├── 🖼️ Captura de tela 2026-10-26 053540.png
    ├── 🖼️ Captura de tela 2026-10-26 053550.png
    ├── 🖼️ Captura de tela 2026-10-26 053600.png
    ├── 🖼️ Captura de tela 2026-10-26 053613.png
    ├── 🖼️ Captura de tela 2026-10-26 053623.png
    ├── 🖼️ Captura de tela 2026-10-26 053633.png
    ├── 🖼️ Captura de tela 2026-10-26 053644.png
    ├── 🖼️ Captura de tela 2026-10-26 053653.png
    ├── 🖼️ Captura de tela 2026-10-26 053705.png
    ├── 🖼️ Captura de tela 2026-10-26 053725.png
    ├── 🖼️ Captura de tela 2026-10-26 053738.png
    └── 🖼️ Captura de tela 2026-10-26 053749.png
```

**Root Path:** `Concrete_Mix_Designer_cpp\.github`

```
└── 📁 workflows
    └── ⚙️ update-readme-version.yml
```
```
---

## English (United States)

### 1. Purpose of This Document

This document records the civil engineering technical observations, the results of the comprehensive audit performed on the UFPR reference experimental spreadsheet (`TC031_Atividade09e10.Dosagem2_17.11.2021_Aperfeicoada.xlsx`), validations against current **ABNT** standards, the UI/UX and dynamic internationalization improvements implemented in version **2026.11.6.0**, and the legal framework for publication on **GitHub** and commercial distribution on the **Microsoft Store** (**Microsoft Partner Center**).

---

### 2. Audit of the UFPR Experimental Spreadsheet (TC031) and Comparison with the C++20 Code

The original 7-sheet workbook (`Composição de Britas`, `Teor de Argamassa`, `Traços ajustados`, `Traço final`, `Resultados (0)`, `Resultados (1)`, `Resultados (2)`, and `Resultados (3)`) was thoroughly inspected regarding its cell precedence and dependency tree for input variables and formulas, leading to the following findings and permanent fixes in **Concrete Mix Design C++**:

1. **Precedence / Overwrite Conflicts in the Spreadsheet vs. Variable Isolation in C++:**
   - In the `Teor de Argamassa` sheet, cells `B32:E32` ($\alpha = 52\%$ row) contained hardcoded literal values (`0.52`, `1`, `2.03`, `2.97`) overwriting the parametric formulas that in all other rows depended on `m` (`B17`). Furthermore, in `Traços ajustados!D15`, the rich mix was hardcoded as literal `3.5` instead of referencing `=E15-1.5` (`3.36` when $m=4.86$).
   - **C++ Solution:** The [`MortarContentOptimizer`](source/core/MortarContentOptimizer.cpp) module and the [`ProjectData::recalculateAll()`](source/core/ProjectData.cpp) orchestrator strictly separate independent input variables from formula-derived quantities, eliminating any accidental formula overwrite.
2. **Swapped Unit Prices for Sand and Coarse Aggregate in the Spreadsheet:**
   - Across sheets `Resultados (0)` through `(3)`, the spreadsheet multiplied sand consumption by $\text{R\$ } 60.00/\text{t}$ and coarse aggregate consumption by $\text{R\$ } 80.00/\text{t}$, inverting the prices declared in the exercise header itself ($\text{Sand} = \text{R\$ } 80.00/\text{t}$ and $\text{Coarse Aggregate} = \text{R\$ } 60.00/\text{t}$).
   - **C++ Solution:** The [`MixDesignCalculator`](source/core/MixDesignCalculator.cpp) module binds each material to its respective `unitCost` registered in `Materials`, preserving budget accounting integrity.
3. **Intermediate Rounding vs. IEEE 754 (`double`) Precision:**
   - In the `Traço final` sheet, the spreadsheet truncated $w/c = 0.473399$ to `0.47` and used a visually read chart value of $C = 335\text{ kg/m}^3$ to compute mixing water ($157.45\text{ L/m}^3$), yielding a volumetric sum of $1.0017\text{ m}^3 \neq 1.000\text{ m}^3$.
   - **C++ Solution:** The analytical calculation preserves 64-bit floating-point precision (`double`) across the entire regression chain (Abrams $\to$ Lyse $\to$ Molinari), guaranteeing an exact $1.000\text{ m}^3$ volumetric closure while providing **Tab 6 (Cost Analysis)** to compare the exact analytical output against any manual graphical reading (such as $w/c = 0.47$, $m = 5.80$, and $C = 335\text{ kg/m}^3$).
4. **Dedicated Scenario 5 (`loadScenario5_PlanilhaOriginal()`):**
   - **Scenario 5** was added to the main toolbar, faithfully loading all experimental parameters from the UFPR TC031 study ($f_{ck} = 30\text{ MPa}$, $S_d = 3.5\text{ MPa}$, $f_{cj} = 35.775\text{ MPa}$, $\alpha = 52\%$, $80\%$ B2 / $20\%$ B1 blend, $29\text{ L}$ mixer, and $30\text{ m}^3$ project volume).

---

### 3. Validation Against Brazilian Technical Standards (ABNT NBR)

| Technical Standard | Scope and Rule Validated in Code (`source/core/`) |
| :--- | :--- |
| **ABNT NBR 12655:2022** | Table 6: Environmental Aggressiveness Classes CAA I to IV ($w/c_{\max}$, $f_{ck,\min}$, and $C_{\min}$ for RC and PC). Section 5.6.3.1: Condition A ($S_d = 4.0\text{ MPa}$, all classes), Condition B ($S_d = 5.5\text{ MPa}$, classes C10 to C20 — automatically flagging $f_{ck} > 20\text{ MPa}$ as non-compliant), and Condition C ($S_d = 7.0\text{ MPa}$, classes C10 and C15 — flagging $f_{ck} > 15\text{ MPa}$). |
| **ABNT NBR 6118:2023** | Table 7.2: Nominal reinforcement covers ($c_{\text{nom}}$ from $20\text{ mm}$ to $55\text{ mm}$ for slabs, beams, and columns in RC and PC). Section 7.4.7.6: Mandatory check $D_{\max} \le 1.2 \cdot c_{\text{nom}}$. |
| **ABNT NBR 7211:2022** | Characterization of fine and coarse aggregates and experimental optimization of compacted bulk unit weight for binary coarse aggregate blends (NBR NM 45). |
| **ABNT NBR 7212:2025** | Section 5.5 (Operational batching tolerances in central plants): **Cement and Additions: $\pm 1\%$**, **Aggregates: $\pm 3\%$**, **Water: $\pm 3\%$**, and **Admixtures: $\pm 5\%$**. |
| **ABNT NBR 8953:2015** | Table 1 (Characteristic strength Groups I and II, C20 to C100) and Table 2 (Slump cone consistency classes: **S10** $10 \le A < 50\text{ mm}$, **S50** $50 \le A < 100\text{ mm}$, **S100** $100 \le A < 160\text{ mm}$, **S160** $160 \le A < 220\text{ mm}$, and **S220** $A \ge 220\text{ mm}$). |

---

### 4. Graphical Interface Fixes, "Settings" Menu, and Dynamic Translation (`v2026.11.6.0`)

1. **Smooth Typing in `QDoubleSpinBox` Controls:**
   - Removed `setSpecialValueText(tr(" "))` (which triggered immediate text reformatting inside `QAbstractSpinBoxPrivate` when leaving the `0.0` minimum value), guarded `setValue()` updates with focus checks (`!spin->hasFocus()`), and moved the cement currency suffix update to `editingFinished`. Users can now type any numeric value smoothly without intrusive trailing zeros being inserted on every keystroke.
2. **New `Settings` Menu (`Ctrl+I`):**
   - Added to the main menu bar (`&Configurações` / `&Settings`), allowing users to configure **Project:**, **Engineer:**, **Institution:**, and **Registration / ID:**, with a direct toggle between automatically reporting the selected **Spreadsheet Scenario** or manually editing the project name.
3. **Dynamic Runtime Translation in Tab 7 and Scenario 5:**
   - **Scenario 5** in the main toolbar and the entire header and body of the report generated in **7. Technical Report** (including the **Project:** field when linked to the active scenario) translate instantaneously at runtime when switching between `pt_BR` and `en_US`, backed by **309 fully translated keys (0 `unfinished`)** in the `.ts` and `.qm` files.

---

### 5. Microsoft Store, Microsoft Partner Center, and Statutory Compliance

To fully comply with open publication requirements on **GitHub** and official commercial distribution on the **Microsoft Store** (**Microsoft Partner Center — Application Developer Agreement v8.9**):
- **Transparent Licensing Model (`CLC` and `EULA`):** Harmonizes GitHub source code availability under the GNU AGPLv3 license for academic/scientific auditing with commercial licensing of signed binary packages distributed on the Microsoft Store.
- **LGPLv3 (Qt 6.11.1) and GPLv3 Runtime Exception (MinGW-w64) Compliance:** 100% dynamic linking of Qt6 DLLs and full preservation of license texts in `source/assets/LICENSES/`.
- **Privacy and Security (*Local-First*):** Zero personal data collection, zero telemetry, and 100% offline operation, complying with Microsoft Store Policy 10.5, **LGPD (Law No. 13,709/2018)**, and **GDPR (EU Regulation 2016/679)**.
- **Intellectual Property and Engineering Legislation:** Compliance with Brazilian Federal Laws No. 9,609/1998, No. 9,610/1998, No. 8,078/1990 (Consumer Defense Code), No. 5,194/1966, and No. 6,496/1977 (CONFEA/CREA — ART requirement), as well as the *U.S. Copyright Act* (17 U.S.C. § 101 et seq.) and the Berne Convention.

---

### 6. Innovations and Features Added in Version 2026.11.6.0

1. **5th Unified Abrams/Lyse/Molinari (IPT) Diagram and Visual Optimization:**
   - Implemented the "5° Abrams/Lyse/Molinari (IPT)" view (`IptUnifiedDiagramWidget`) integrating the three experimental regression curves into normalized coordinates with crisp horizontal and vertical grid lines, removing solid color fills under the Molinari curve.
   - Enhanced the legibility of the 4 vertical axis scales in the 4-Quadrant Diagram (`QuadChartWidget`), balancing font sizes for improved value readability.
   - Calibrated the "Export Chart Image..." feature to render high-definition, true-to-screen image outputs.
2. **Dynamic Bilingual Currency Symbol (R$ / $):**
   - When switching languages (Brazilian Portuguese $\leftrightarrow$ US English), currency symbols across Technical Reports (Tab 7) and cost analysis tables automatically toggle between "R$" and "$", requiring no restart.
3. **New Professional Command Line Interface (`concrete-mix-designer-cli`):**
   - Standalone terminal utility featuring `design`, `calc`, `verify`, `report`, `standards`, `presets`, and `test` commands.
   - Enables seamless CI/CD automation pipeline integration and technical report export in Markdown, HTML, JSON, and plain text.
4. **Concurrency, Memory Safety, and Algorithmic Hardening Audit:**
   - Added multithreaded synchronization via `QRecursiveMutex` for the in-memory SQLite database (`NormativeDatabase`).
   - Hardened window and dialog lifecycle management in `MainWindow`, preventing race conditions and dangling pointers.
   - Added division-by-zero guards and non-positive log-domain bounds in `RegressionEngine`.

```
**Root Path:** `Concrete_Mix_Designer_cpp\source`

```
├── 📁 assets
│   ├── 📁 ABOUT
│   │   ├── 📄 ABOUT_en_US.txt
│   │   ├── 📄 ABOUT_pt_BR.txt
│   │   ├── 📄 History_APP_en_US.txt
│   │   └── 📄 History_APP_pt_BR.txt
│   ├── 📁 CLC
│   │   ├── 📄 CLC_en_US.txt
│   │   └── 📄 CLC_pt_BR.txt
│   ├── 📁 COPYRIGHT
│   │   ├── 📄 AVISO DE COPYRIGHT E MARCA REGISTRADA_pt_BR.txt
│   │   └── 📄 COPYRIGHT AND TRADEMARK NOTICE_en_US.txt
│   ├── 📁 EULA
│   │   ├── 📄 EULA_en_US.txt
│   │   └── 📄 EULA_pt_BR.txt
│   ├── 📁 LICENSES
│   │   ├── 📄 Apache License Version 2.0.txt
│   │   ├── 📄 BSD License Clause-2.txt
│   │   ├── 📄 BSD License Clause-3.txt
│   │   ├── 📄 GNU AFFERO GENERAL PUBLIC LICENSE Version3.txt
│   │   ├── 📄 GNU GENERAL PUBLIC LICENSE Version 3.txt
│   │   ├── 📄 GNU LESSER GENERAL PUBLIC LICENSE Version 2.1.txt
│   │   ├── 📄 GNU LESSER GENERAL PUBLIC LICENSE Version 3.txt
│   │   ├── 📄 ISC LICENSE.txt
│   │   ├── 📄 MIT License.txt
│   │   ├── 📄 PYTHON SOFTWARE FOUNDATION LICENSE.txt
│   │   └── 📄 SQLite Is Public Domain.txt
│   ├── 📁 NOTICES
│   │   ├── 📄 NOTICE_en_US.txt
│   │   └── 📄 NOTICE_pt_BR.txt
│   ├── 📁 PRIVACY_POLICY
│   │   ├── 📄 Privacy_Policy_en_US.txt
│   │   └── 📄 Privacy_Policy_pt_BR.txt
│   ├── 📁 RELEASE
│   │   ├── 📄 RELEASE NOTES_en_US.txt
│   │   └── 📄 RELEASE NOTES_pt_BR.txt
│   └── 📁 icons
│       ├── 📄 concrete_mix_design.ico
│       ├── 🖼️ concrete_mix_design.png
│       ├── 🖼️ concrete_mix_design_1080-1080.png
│       ├── 🖼️ concrete_mix_design_1440-2160.png
│       ├── 🖼️ concrete_mix_design_150-150.png
│       ├── 🖼️ concrete_mix_design_1920-1080.png
│       ├── 🖼️ concrete_mix_design_2160-2160.png
│       ├── 🖼️ concrete_mix_design_300-300.png
│       ├── 🖼️ concrete_mix_design_3840-2160.png
│       ├── 🖼️ concrete_mix_design_584-800.png
│       └── 🖼️ concrete_mix_design_71-71.png
├── 📁 cli
│   └── ⚡ main_cli.cpp
├── 📁 core
│   ├── ⚡ AggregateOptimizer.cpp
│   ├── ⚡ AggregateOptimizer.hpp
│   ├── ⚡ ConcreteStandards.cpp
│   ├── ⚡ ConcreteStandards.hpp
│   ├── ⚡ MixDesignCalculator.cpp
│   ├── ⚡ MixDesignCalculator.hpp
│   ├── ⚡ MortarContentOptimizer.cpp
│   ├── ⚡ MortarContentOptimizer.hpp
│   ├── ⚡ NormativeDatabase.cpp
│   ├── ⚡ NormativeDatabase.hpp
│   ├── ⚡ ProjectData.cpp
│   ├── ⚡ ProjectData.hpp
│   ├── ⚡ RegressionEngine.cpp
│   └── ⚡ RegressionEngine.hpp
├── 📁 gui
│   ├── ⚡ IptUnifiedDiagramWidget.cpp
│   ├── ⚡ IptUnifiedDiagramWidget.hpp
│   ├── ⚡ MainWindow.cpp
│   ├── ⚡ MainWindow.hpp
│   ├── ⚡ ManualDialog.cpp
│   ├── ⚡ ManualDialog.hpp
│   ├── ⚡ QuadChartWidget.cpp
│   ├── ⚡ QuadChartWidget.hpp
│   ├── ⚡ SobreDialog.cpp
│   ├── ⚡ SobreDialog.hpp
│   ├── ⚡ TabCosts.cpp
│   ├── ⚡ TabCosts.hpp
│   ├── ⚡ TabExperimentalMixes.cpp
│   ├── ⚡ TabExperimentalMixes.hpp
│   ├── ⚡ TabFinalMix.cpp
│   ├── ⚡ TabFinalMix.hpp
│   ├── ⚡ TabMaterials.cpp
│   ├── ⚡ TabMaterials.hpp
│   ├── ⚡ TabMortarContent.cpp
│   ├── ⚡ TabMortarContent.hpp
│   ├── ⚡ TabProjectParams.cpp
│   ├── ⚡ TabProjectParams.hpp
│   ├── ⚡ TabTechnicalReport.cpp
│   └── ⚡ TabTechnicalReport.hpp
├── 📁 language
│   ├── 📁 locale
│   │   ├── 📄 concrete_mix_design_en_US.qm
│   │   ├── 📄 concrete_mix_design_en_US.ts
│   │   ├── 📄 concrete_mix_design_pt_BR.qm
│   │   └── 📄 concrete_mix_design_pt_BR.ts
│   ├── ⚡ GerenciadorTraducao.cpp
│   └── ⚡ GerenciadorTraducao.hpp
└── 📁 utils
    ├── ⚡ ApplicationPathUtils.cpp
    ├── ⚡ ApplicationPathUtils.hpp
    ├── ⚡ CaminhoPersistenteUtils.cpp
    ├── ⚡ CaminhoPersistenteUtils.hpp
    ├── ⚡ LogManager.cpp
    └── ⚡ LogManager.hpp
```

**Root Path:** `Concrete_Mix_Designer_cpp\tests`

```
└── ⚡ verify_calculations.cpp
```

**Root Path:** `Concrete_Mix_Designer_cpp\docs`

```
├── 📁 images
│   ├── 🖼️ banner.png
│   └── 🖼️ logo.png
├── 📝 architecture.md
├── 📝 cli-reference.md
├── 📝 compliance.md
├── 📝 licensing.md
├── 📝 matriz_rastreabilidade_normativa.md
├── 📝 release-v2026.10.26.0.md
├── 📝 release-v2026.11.6.0.md
├── 📝 security-model.md
├── 📝 suggested-issues.md
└── 📝 user-guide.md
```

**Root Path:** `Concrete_Mix_Designer_cpp\mocks`

```
├── 📁 icons
│   ├── 📄 concrete_mix_design.ico
│   ├── 🖼️ concrete_mix_design.png
│   ├── 🖼️ concrete_mix_design_1080-1080.png
│   ├── 🖼️ concrete_mix_design_1440-2160.png
│   ├── 🖼️ concrete_mix_design_150-150.png
│   ├── 🖼️ concrete_mix_design_1920-1080.png
│   ├── 🖼️ concrete_mix_design_2160-2160.png
│   ├── 🖼️ concrete_mix_design_300-300.png
│   ├── 🖼️ concrete_mix_design_3840-2160.png
│   ├── 🖼️ concrete_mix_design_584-800.png
│   └── 🖼️ concrete_mix_design_71-71.png
├── 📄 CMakeLists.txt
├── ⚡ CompileTranslationsGUI.cpp
├── ⚡ CompileTranslationsGUI.hpp
├── ⚡ CompileTranslationsTerminal.cpp
├── ⚡ CompileTranslationsTerminal.hpp
├── ⚡ find_VersionEditor.cpp
└── ⚡ find_VersionEditor.hpp
```

**Root Path:** `Concrete_Mix_Designer_cpp\repo\LISTAGEM`

```
├── 📁 en_US
│   ├── 🖼️ Captura de tela 2026-10-26 053905.png
│   ├── 🖼️ Captura de tela 2026-10-26 053915.png
│   ├── 🖼️ Captura de tela 2026-10-26 053927.png
│   ├── 🖼️ Captura de tela 2026-10-26 053938.png
│   ├── 🖼️ Captura de tela 2026-10-26 053947.png
│   ├── 🖼️ Captura de tela 2026-10-26 053957.png
│   ├── 🖼️ Captura de tela 2026-10-26 054008.png
│   ├── 🖼️ Captura de tela 2026-10-26 054025.png
│   ├── 🖼️ Captura de tela 2026-10-26 054224.png
│   ├── 🖼️ Captura de tela 2026-10-26 054233.png
│   ├── 🖼️ Captura de tela 2026-10-26 054243.png
│   └── 🖼️ Captura de tela 2026-10-26 054253.png
└── 📁 pt_BR
    ├── 🖼️ Captura de tela 2026-10-26 053540.png
    ├── 🖼️ Captura de tela 2026-10-26 053550.png
    ├── 🖼️ Captura de tela 2026-10-26 053600.png
    ├── 🖼️ Captura de tela 2026-10-26 053613.png
    ├── 🖼️ Captura de tela 2026-10-26 053623.png
    ├── 🖼️ Captura de tela 2026-10-26 053633.png
    ├── 🖼️ Captura de tela 2026-10-26 053644.png
    ├── 🖼️ Captura de tela 2026-10-26 053653.png
    ├── 🖼️ Captura de tela 2026-10-26 053705.png
    ├── 🖼️ Captura de tela 2026-10-26 053725.png
    ├── 🖼️ Captura de tela 2026-10-26 053738.png
    └── 🖼️ Captura de tela 2026-10-26 053749.png
```

**Root Path:** `Concrete_Mix_Designer_cpp\.github`

```
└── 📁 workflows
    └── ⚙️ update-readme-version.yml
```
```