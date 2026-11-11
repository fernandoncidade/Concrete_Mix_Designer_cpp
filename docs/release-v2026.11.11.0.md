# Notas de Lançamento / Release Notes — v2026.11.11.0

[Português (Brasil)](#português-brasil) | [English (United States)](#english-united-states)

---

## Português (Brasil)

### 1. Identificação da Release
- **Produto:** Concrete Mix Design C++ — Dosagem Experimental de Concreto (Método IPT/EPUSP)
- **Versão Oficial:** `2026.11.11.0`
- **Tag de Versão:** `v2026.11.11.0`
- **Data de Publicação:** 11 de novembro de 2026
- **Autor e Publicador:** Fernando Nillsson Cidade
- **Plataforma:** Microsoft Windows 10 (1809+) e Windows 11 (x64)
- **Canais de Distribuição:** Microsoft Store (Microsoft Partner Center) e [GitHub Concrete_Mix_Designer_cpp](../)

---

### 2. Artefatos de Instalação e Hashes Criptográficos

Valide a integridade do pacote baixado antes da execução utilizando o cmdlet nativo do PowerShell:

```powershell
Get-FileHash -Path ".\build\build_msvc\bin\ConcreteMixDesign.exe" -Algorithm SHA256
Get-FileHash -Path ".\build\build_msvc\bin\concrete-mix-designer-cli.exe" -Algorithm SHA256
```

| Nome do Artefato | Formato | Arquitetura Alvo | Descrição |
|---|---|:---:|---|
| `ConcreteMixDesign.exe` | Executável Nativo Windows (GUI) | x64 (MSVC/MinGW) | Aplicação desktop em C++20 acelerada com framework Qt 6.11.1 para dosagem experimental pelo Método IPT/EPUSP. |
| `concrete-mix-designer-cli.exe` | Utilitário Autônomo de Console | x64 (MSVC/MinGW) | Utilitário autônomo de terminal para auditoria normativa, verificação de traços e geração automatizada de relatórios. |
| `Source code (zip / tar.gz)` | Arquivos de Código-Fonte | Qualquer | Código-fonte completo em C++20 sob a licença copyleft GNU AGPLv3. |

---

### 3. Novidades, Destaques Técnicos e Correções Definitivas na Versão `2026.11.11.0`

Esta versão consolida aprimoramentos fundamentais no motor analítico de engenharia civil, ergonomia de menus, governança jurídica, suporte técnico oficial, transparência de distribuição e harmonização internacional:

1. **Motor Analítico IPT/EPUSP e Banco Normativo SQLite (`:memory:`):**
   - Regressão por mínimos quadrados ordinários (OLS) em ponto flutuante de 64 bits (`double`) para as Leis de **Abrams**, **Lyse** e **Molinari** ($R^2$).
   - Validação automática das normas **ABNT NBR 12655:2022** (Tabela 6 e item 5.6.3.1 — bloqueio de $f_{ck} > 20\text{ MPa}$ na Condição B e $f_{ck} > 15\text{ MPa}$ na Condição C), **NBR 6118:2023** (cobrimento nominal e $D_{\max} \le 1,2 \cdot c_{\text{nom}}$), **NBR 7211:2022** (mistura binária de britas), **NBR 7212:2025** (item 5.5 — tolerâncias de pesagem: Cimento e Adições $\pm 1\%$, Agregados $\pm 3\%$, Água $\pm 3\%$, Aditivos $\pm 5\%$) e **NBR 8953:2015** (classes de abatimento *Slump* S10 a S220).
2. **Menu `Configurações` (`Ctrl+I`) e Submenus Padronizados (`📂 Cenário da Planilha:` e `🌐 Idioma`):**
   - Padronização do submenu **Configurações > 📂 Cenário da Planilha:** (contendo os Cenários 0 a 5 com marcação visual do cenário ativo) e transferência da seleção de idioma para o submenu **Configurações > 🌐 Idioma**.
   - Permite configurar **Projeto:**, **Responsável:**, **Instituição:** e **Matrícula / ID:** diretamente na barra de menus, escolhendo entre **reportar automaticamente as informações selecionadas em "Cenário da Planilha"** (com tradução dinâmica em tempo de execução) ou **editar as informações de "Projeto:" manualmente**.
3. **Correção Definitiva de Digitação nas `QDoubleSpinBox`:**
   - Removido o preenchimento automático intrusivo de zeros à direita durante a digitação nos campos numéricos de todas as abas (`TabProjectParams`, `TabMaterials`, `TabMortarContent`, `TabFinalMix`, `TabCosts`).
4. **Tradução Dinâmica Integral (`pt_BR` $\leftrightarrow$ `en_US`) e Símbolo de Moeda Dinâmico (`R$` / `$`):**
   - O **Cenário 5** (*Planilha UFPR Original Corrigida*), os submenus de **Configurações**, todas as informações de cabeçalho (`Projeto:`, `Responsável:`, `Instituição:`, `Matrícula / ID:`), o corpo do relatório na aba **7. Relatório Técnico** e os símbolos monetários (`R$` em Português e `$` em Inglês) traduzem-se dinamicamente em tempo de execução, com **334 chaves de tradução completas (0 `unfinished`)** nos arquivos `.ts` e `.qm`.
5. **5º Gráfico Unificado Abrams/Lyse/Molinari (IPT) e Renderização Vetorial Nítida:**
   - Implementado o painel `IptUnifiedDiagramWidget` na Aba 4 reunindo as três curvas de dosagem com réguas de precisão nos eixos e remoção de áreas coloridas sob a curva Molinari.
   - Otimizada a legibilidade dos rótulos e valores no Diagrama de 4 Quadrantes e a ferramenta de exportação em alta resolução de imagens (`.png`) e `.pdf`.
6. **Ferramenta CLI (`concrete-mix-designer-cli.exe`) e Concorrência Segura:**
   - Utilitário autônomo de terminal com comandos `design`, `calc`, `verify`, `report`, `standards`, `presets` e `test`.
   - Banco SQLite em memória (`NormativeDatabase`) sincronizado com `QRecursiveMutex` e proteções rigorosas contra divisão por zero em `RegressionEngine`.
7. **Aviso Expresso de Finalidade Acadêmica e Isenção de Responsabilidade Legal:**
   - Consolidação em todos os documentos e termos de uso de que esta aplicação se destina exclusivamente a estudos acadêmicos, podendo conter possíveis erros e não servindo como ferramenta definitiva para tomada de decisões em produção ou ambientes de trabalho, exigindo conferência manual e validação em ferramentas mais consolidadas no mercado, sob total isenção de responsabilidade legal do autor conforme a legislação brasileira (Lei nº 10.406/2002, Lei nº 9.609/1998, Lei nº 9.610/1998, Lei nº 8.078/1990 e Leis nº 5.194/1966 e nº 6.496/1977) e convenções internacionais (*UCC § 2-316*, Convenção de Berna, OMPI, TRIPS e *GNU AGPLv3* Seções 15 e 16).
8. **Formalização do Modelo de Duplo Licenciamento (Dual-Licensing) e Suporte Oficial:**
   - Definição clara da coexistência entre a edição comunitária em código aberto (sob licença GNU AGPLv3) e a edição comercial oficial distribuída através da Microsoft Store, com isenção explícita de obrigações copyleft para uso corporativo interno do binário comercial adquirido na loja.
   - Atualização unificada em todas as telas, janelas informativas ("Sobre"), contratos e manuais do endereço de suporte oficial em regime de melhor esforço (*best-effort*): `linceu_lighthouse@outlook.com`.
9. **Privacidade Soberana Local-First, Conformidade LGPD/GDPR e Higienização do Repositório:**
   - Operação 100% offline, local e isolada no computador do usuário, sem nuvem e sem telemetria invasiva.
   - Paridade simétrica linha por linha em todos os arquivos de termos e documentação oficial em Português e Inglês, links internos relativos portáteis e normalização via `.gitattributes`.

---

### 4. Requisitos de Sistema

- **Sistema Operacional:** Microsoft Windows 10 (versão 1809 ou superior) e Windows 11 (64-bit x64).
- **Processador:** Arquitetura x86-64 (Intel Core i3/i5/i7/i9 ou AMD Ryzen).
- **Memória RAM:** Mínimo de 512 MB (recomendado 1 GB).
- **Espaço em Disco:** ~100 MB livres para instalação e armazenamento de arquivos exportados.

---

## English (United States)

### 1. Release Identification
- **Product:** Concrete Mix Design C++ — Experimental Concrete Mix Design (IPT/EPUSP Method)
- **Official Version:** `2026.11.11.0`
- **Version Tag:** `v2026.11.11.0`
- **Publication Date:** November 11, 2026
- **Author and Publisher:** Fernando Nillsson Cidade
- **Platform:** Microsoft Windows 10 (1809+) and Windows 11 (x64)
- **Distribution Channels:** Microsoft Store (Microsoft Partner Center) and [GitHub Concrete_Mix_Designer_cpp](../)

---

### 2. Installation Packages & Cryptographic Checksums

Validate package integrity prior to launch using native Windows PowerShell:

```powershell
Get-FileHash -Path ".\build\build_msvc\bin\ConcreteMixDesign.exe" -Algorithm SHA256
Get-FileHash -Path ".\build\build_msvc\bin\concrete-mix-designer-cli.exe" -Algorithm SHA256
```

| Package Name | Format | Target Arch | Description |
|---|---|:---:|---|
| `ConcreteMixDesign.exe` | Native Windows GUI Executable | x64 (MSVC/MinGW) | High-performance C++20 / Qt 6.11.1 desktop application for experimental concrete mix design using the IPT/EPUSP method. |
| `concrete-mix-designer-cli.exe` | Headless Console Executable | x64 (MSVC/MinGW) | Autonomous CLI utility for batch audits, standards verification, and automated technical reporting. |
| `Source code (zip / tar.gz)` | Source Code Bundle | Any | Complete C++20 source code licensed under the copyleft GNU AGPLv3. |

---

### 3. New Features, Technical Highlights, and Permanent Fixes in Version `2026.11.11.0`

This version consolidates essential improvements in the civil engineering analytical engine, menu ergonomics, legal governance, official support channels, distribution clarity, and international harmonization:

1. **IPT/EPUSP Analytical Engine and In-Memory SQLite Normative Database (`:memory:`):**
   - 64-bit floating-point (`double`) ordinary least squares (OLS) regression for **Abrams'**, **Lyse's**, and **Molinari's** Laws ($R^2$).
   - Automatic validation against **ABNT NBR 12655:2022** (Table 6 and Section 5.6.3.1 — blocking $f_{ck} > 20\text{ MPa}$ under Condition B and $f_{ck} > 15\text{ MPa}$ under Condition C), **NBR 6118:2023** (nominal cover and $D_{\max} \le 1.2 \cdot c_{\text{nom}}$), **NBR 7211:2022** (binary coarse aggregate blends), **NBR 7212:2025** (Section 5.5 — batching tolerances: Cement and Additions $\pm 1\%$, Aggregates $\pm 3\%$, Water $\pm 3\%$, Admixtures $\pm 5\%$), and **NBR 8953:2015** (Slump consistency classes S10 to S220).
2. **`Settings` Menu (`Ctrl+I`) and Standardized Submenus (`📂 Spreadsheet Scenario:` and `🌐 Language`):**
   - Standardization of the **Settings > 📂 Spreadsheet Scenario:** submenu (containing Scenarios 0 through 5 with a visual checkmark on the active scenario) and relocation of language selection into the **Settings > 🌐 Language** submenu.
   - Enables configuring **Project:**, **Engineer:**, **Institution:**, and **Registration / ID:** directly from the menu bar, choosing between **automatically reporting the information selected in "Spreadsheet Scenario"** (with dynamic runtime translation) or **editing the "Project:" information manually**.
3. **Permanent Fix for `QDoubleSpinBox` Typing Behavior:**
   - Eliminated intrusive automatic trailing-zero padding while typing in numeric fields across all tabs (`TabProjectParams`, `TabMaterials`, `TabMortarContent`, `TabFinalMix`, `TabCosts`).
4. **Full Dynamic Runtime Translation (`pt_BR` $\leftrightarrow$ `en_US`) and Dynamic Currency Symbol (`R$` / `$`):**
   - **Scenario 5** (*Corrected Original UFPR Spreadsheet*), the **Settings** submenus, all header metadata (`Project:`, `Engineer:`, `Institution:`, `Registration / ID:`), the report body in **7. Technical Report**, and currency symbols (`R$` in Portuguese and `$` in English) translate dynamically at runtime, backed by **334 completed translation keys (0 `unfinished`)** in the `.ts` and `.qm` files.
5. **5th Unified Abrams/Lyse/Molinari (IPT) Diagram and Crisp Vector Rendering:**
   - Implemented the `IptUnifiedDiagramWidget` panel in Tab 4 bringing together the three mix design curves with precision axis grid lines and removing solid color fills under the Molinari curve.
   - Enhanced label readability on the 4-Quadrant Diagram and calibrated the high-resolution image export (`.png`) and `.pdf` features.
6. **CLI Executable (`concrete-mix-designer-cli.exe`) and Thread-Safety:**
   - Standalone terminal utility supporting `design`, `calc`, `verify`, `report`, `standards`, `presets`, and `test` commands.
   - In-memory SQLite database (`NormativeDatabase`) synchronized with `QRecursiveMutex` and strict division-by-zero guards in `RegressionEngine`.
7. **Explicit Academic Study Purpose Notice and Legal Liability Exemption:**
   - Consolidation across all documents and terms of use that this application is intended exclusively for academic study, may contain possible errors, and does not serve as a definitive tool for decision-making in production or work environments, requiring manual verification and validation in more consolidated market tools, under full legal liability exemption of the author pursuant to Brazilian legislation (Law No. 10,406/2002, Law No. 9,609/1998, Law No. 9,610/1998, Law No. 8,078/1990, and Laws No. 5,194/1966 and No. 6,496/1977) and international conventions (*UCC § 2-316*, Berne Convention, WIPO, TRIPS, and *GNU AGPLv3* Sections 15 and 16).
8. **Dual-Licensing Framework Formalization and Official Support Channel:**
   - Clear establishment of the coexistence between the community open-source edition (licensed under GNU AGPLv3) and the official commercial edition distributed through the Microsoft Store, with explicit copyleft exemption for internal corporate use of official commercial binaries.
   - Unified update across all application windows, "About" dialogs, contracts, and manuals of the official best-effort technical support email address: `linceu_lighthouse@outlook.com`.
9. **Local-First Sovereign Privacy, LGPD/GDPR Compliance, and Repository Hygiene:**
   - 100% offline, local data processing on the user's workstation, with zero cloud dependency and zero intrusive telemetry.
   - Symmetrical line-by-line parity across all legal terms and official documentation files in Portuguese and English, portable relative internal links, and `.gitattributes` normalization.

---

### 4. System Requirements

- **Operating System:** Microsoft Windows 10 (version 1809 or higher) and Windows 11 (64-bit x64).
- **Processor:** x86-64 architecture (Intel Core i3/i5/i7/i9 or AMD Ryzen).
- **RAM:** Minimum 512 MB (1 GB recommended).
- **Disk Space:** ~100 MB free disk space for installation and exported reports.
