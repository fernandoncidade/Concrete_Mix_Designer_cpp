# Guia de Contribuição / Contribution Guide — Concrete Mix Design C++

[Português (Brasil)](#português-brasil) | [English (United States)](#english-united-states)

---

## Português (Brasil)

Obrigado pelo seu interesse em contribuir para o **Concrete Mix Design C++**! Este documento estabelece as diretrizes técnicas, científicas, arquiteturais e jurídicas para envio de contribuições ao repositório oficial no GitHub, garantindo total compatibilidade com a distribuição comercial homologada na **Microsoft Store** (**Microsoft Partner Center**).

### 1. Diretrizes Gerais e Princípios de Engenharia

O **Concrete Mix Design C++** é um software técnico-científico de Engenharia Civil fundamentado no **Método IPT/EPUSP (Helene & Terzian, 1992)** e nas normas brasileiras vigentes da ABNT (**NBR 12655:2022**, **NBR 6118:2023**, **NBR 7211:2022**, **NBR 7212:2025** e **NBR 8953:2015**). Toda contribuição deve respeitar os seguintes pilares inegociáveis:

1. **Rigor Matemático e Normativo:** Nenhuma alteração em `source/core/` pode comprometer a precisão em dupla precisão (`double` IEEE 754) das equações de regressão por mínimos quadrados ordinários (Leis de Abrams, Lyse e Molinari), do empacotamento granular de britas, do ensaio piloto de teor de argamassa ($\alpha$), do balanço volumétrico de $1\text{ m}^3$ ou dos limites normativos de durabilidade.
2. **Arquitetura 100% Local-First e Offline (Política 10.5 da Microsoft Store / LGPD / GDPR):** É estritamente proibido introduzir bibliotecas de telemetria, coleta de dados pessoais, rastreadores, anúncios, chamadas de rede não solicitadas ou dependências de serviços de nuvem.
3. **Padrão C++20 Moderno e Desacoplamento Camada Core / GUI / CLI:**
   - A camada `source/core/` não deve depender de widgets visuais (`QtWidgets` / `QtCharts`), mantendo-se testável via linha de comando por meio dos utilitários de console `verify_calculations` e `concrete-mix-designer-cli`.
   - Utilizar gerenciamento seguro de memória (RAII, `std::vector`, `std::optional`, ponteiros inteligentes e hierarquia pai-filho de `QObject`), sem uso de `new`/`delete` manuais desprotegidos.
4. **Internacionalização Bilíngue Obrigatória (`pt_BR` e `en_US` via `Configurações > Idioma`):**
   - Toda nova string de interface gráfica (`source/gui/`) deve estar envolvida em `tr("...")` e atualizada no método `retranslateUi()`, mantendo o seletor de idioma alocado no submenu `Configurações > Idioma` (`Settings > Language`).
   - Os arquivos de tradução `source/language/locale/concrete_mix_design_pt_BR.ts` e `concrete_mix_design_en_US.ts` devem conter 100% das chaves traduzidas (0 `unfinished`) e ser compilados para `.qm` via `lrelease`.
   - Toda alteração na documentação (`README.md`, `MANUAL.md`, `OBSERVACAO.md`, `docs/*.md` e `source/assets/`) deve manter a mesma quantidade exata de linhas entre ambas as traduções, sem quebras de linha no meio dos parágrafos, preservando os avisos de finalidade estritamente acadêmica, conferência manual obrigatória, validação em ferramentas consolidadas no mercado e isenção de responsabilidade legal.

### 2. Fluxo de Trabalho para Contribuições (*Pull Requests*)

1. **Discuta Antes Mudanças Estruturais:** Para alterações na formulação matemática, no banco de dados SQLite normativo (`NormativeDatabase`) ou na disposição das 7 abas da interface, abra primeiramente uma *Issue* descrevendo a fundamentação teórica e a cláusula normativa da ABNT correspondente.
2. **Fork e Branch:** Crie uma *branch* descritiva a partir da `main` (ex.: `fix/spinbox-editing`, `feat/cli-automation`, `docs/ms-store-compliance`).
3. **Compilação e Verificação Automatizada:**
   Compile o projeto com MSVC ou MinGW e execute as suítes de verificação analítica e da CLI antes de submeter o *Pull Request*:
   ```powershell
   # Compilação automatizada via script MinGW ou MSVC
   cmd.exe /c "cmake\build_mingw.bat"
   # ou: cmd.exe /c "cmake\build_msvc.bat"

   # Execução dos testes analíticos
   .\build\build_concrete_mix_design_mingw\verify_calculations.exe
   .\build\build_concrete_mix_design_mingw\concrete-mix-designer-cli.exe test
   ```
4. **Atualização de Traduções Qt:**
   ```powershell
   C:\Qt\6.11.1\mingw_64\bin\lupdate.exe source -ts source/language/locale/concrete_mix_design_en_US.ts source/language/locale/concrete_mix_design_pt_BR.ts
   C:\Qt\6.11.1\mingw_64\bin\lrelease.exe source/language/locale/concrete_mix_design_en_US.ts source/language/locale/concrete_mix_design_pt_BR.ts
   ```

### 3. Acordo de Licença de Contribuidor (CLA) e Direitos Comerciais (Microsoft Store)

Ao submeter um *Pull Request*, patch, tradução ou documentação para este repositório, você declara e concorda expressamente que:

1. Você é o autor original da contribuição e detém todos os direitos necessários para licenciá-la, não violando direitos autorais, segredos industriais ou patentes de terceiros (Lei nº 9.609/1998, Lei nº 9.610/1998 e 17 U.S.C. § 101 et seq.);
2. Você concede ao autor e publicador titular do projeto (**Fernando Nillsson Cidade**) uma licença perpétua, mundial, irrevogável, gratuita, não exclusiva e sublicenciável para usar, reproduzir, modificar, compilar, empacotar e distribuir sua contribuição tanto no repositório aberto do **GitHub** (sob os termos da licença GNU AGPLv3) quanto nas edições binárias comerciais distribuídas na **Microsoft Store** (**Microsoft Partner Center**) sob os termos do Contrato de Licenciamento Comercial (`CLC`) e do Contrato de Licença de Usuário Final (`EULA`).

---

## English (United States)

Thank you for your interest in contributing to **Concrete Mix Design C++**! This document establishes the technical, scientific, architectural, and legal guidelines for submitting contributions to the official GitHub repository, ensuring full compatibility with the certified commercial distribution on the **Microsoft Store** (**Microsoft Partner Center**).

### 1. General Guidelines and Engineering Principles

**Concrete Mix Design C++** is a technical-scientific Civil Engineering software application grounded in the **IPT/EPUSP Method (Helene & Terzian, 1992)** and current Brazilian ABNT standards (**NBR 12655:2022**, **NBR 6118:2023**, **NBR 7211:2022**, **NBR 7212:2025**, and **NBR 8953:2015**). Every contribution must uphold the following non-negotiable pillars:

1. **Mathematical and Normative Rigor:** No modification in `source/core/` may compromise the double-precision (`double` IEEE 754) accuracy of the ordinary least squares regression equations (Abrams', Lyse's, and Molinari's Laws), coarse aggregate granular packing, pilot mortar content ($\alpha$) trial sequence, $1\text{ m}^3$ volumetric balance, or normative durability limits.
2. **100% Local-First and Offline Architecture (Microsoft Store Policy 10.5 / LGPD / GDPR):** Introducing telemetry libraries, personal data collection, trackers, advertisements, unsolicited network calls, or cloud service dependencies is strictly prohibited.
3. **Modern C++20 Standard and Core / GUI / CLI Layer Decoupling:**
   - The `source/core/` layer must not depend on visual widgets (`QtWidgets` / `QtCharts`), remaining fully testable via command line through the `verify_calculations` test runner and `concrete-mix-designer-cli`.
   - Use safe memory management (RAII, `std::vector`, `std::optional`, smart pointers, and `QObject` parent-child ownership), avoiding unprotected manual `new`/`delete` calls.
4. **Mandatory Bilingual Internationalization (`pt_BR` and `en_US` via `Settings > Language`):**
   - Every new graphical user interface string (`source/gui/`) must be wrapped in `tr("...")` and updated inside the `retranslateUi()` method, keeping the language selector allocated under the `Settings > Language` (`Configurações > Idioma`) submenu.
   - The translation files `source/language/locale/concrete_mix_design_pt_BR.ts` and `concrete_mix_design_en_US.ts` must contain 100% translated keys (0 `unfinished`) and be compiled into `.qm` binaries via `lrelease`.
   - Every documentation change (`README.md`, `MANUAL.md`, `OBSERVACAO.md`, `docs/*.md`, and `source/assets/`) must maintain the exact same line count between both translations, with no mid-paragraph line breaks, preserving the notices of strictly academic purpose, mandatory manual verification, validation in consolidated market tools, and legal liability exemption.

### 2. Contribution Workflow (*Pull Requests*)

1. **Discuss Structural Changes First:** For changes affecting mathematical formulations, the normative in-memory SQLite database (`NormativeDatabase`), or the layout of the 7 UI tabs, please open an *Issue* first describing the theoretical basis and the corresponding ABNT standard clause.
2. **Fork and Branch:** Create a descriptive branch from `main` (e.g., `fix/spinbox-editing`, `feat/cli-automation`, `docs/ms-store-compliance`).
3. **Compilation and Automated Verification:**
   Build the project with MSVC or MinGW and run both the analytical and CLI test verification suites before submitting your *Pull Request*:
   ```powershell
   # Automated build via MinGW or MSVC scripts
   cmd.exe /c "cmake\build_mingw.bat"
   # or: cmd.exe /c "cmake\build_msvc.bat"

   # Run analytical and CLI test suites
   .\build\build_concrete_mix_design_mingw\verify_calculations.exe
   .\build\build_concrete_mix_design_mingw\concrete-mix-designer-cli.exe test
   ```
4. **Updating Qt Translations:**
   ```powershell
   C:\Qt\6.11.1\mingw_64\bin\lupdate.exe source -ts source/language/locale/concrete_mix_design_en_US.ts source/language/locale/concrete_mix_design_pt_BR.ts
   C:\Qt\6.11.1\mingw_64\bin\lrelease.exe source/language/locale/concrete_mix_design_en_US.ts source/language/locale/concrete_mix_design_pt_BR.ts
   ```

### 3. Contributor License Agreement (CLA) and Commercial Rights (Microsoft Store)

By submitting a *Pull Request*, patch, translation, or documentation to this repository, you expressly represent and agree that:

1. You are the original author of the contribution and hold all necessary rights to license it, without infringing any third-party copyrights, trade secrets, or patents (Brazilian Law No. 9,609/1998, Law No. 9,610/1998, and 17 U.S.C. § 101 et seq.);
2. You grant the project's author and official publisher (**Fernando Nillsson Cidade**) a perpetual, worldwide, irrevocable, royalty-free, non-exclusive, and sublicensable license to use, reproduce, modify, compile, package, and distribute your contribution both in the open **GitHub** repository (under the GNU AGPLv3 license) and in commercial binary editions distributed on the **Microsoft Store** (**Microsoft Partner Center**) under the terms of the Commercial Licensing Agreement (`CLC`) and End-User License Agreement (`EULA`).
