# Modelo de Segurança e Ameaças / Security & Threat Model — Concrete Mix Design C++

[Português (Brasil)](#português-brasil) | [English (United States)](#english-united-states)

---

## Português (Brasil)

### 1. Fronteiras de Confiança e Superfície de Ataque

O **Concrete Mix Design C++ (v2026.11.6.0)** é um aplicativo desktop nativo (`WIN32_EXECUTABLE`) que opera integralmente no espaço de usuário local do sistema operacional **Microsoft Windows 10/11 (x64)**. O modelo de ameaças (*STRIDE*) foi estruturado para atender à **Política 10.2 e 10.5 da Microsoft Store**, à **LGPD (Lei nº 13.709/2018)** e ao **GDPR (Regulamento UE 2016/679)**:

| Categoria STRIDE | Vetor Potencial Analisado | Mitigação Implementada no Código C++20 |
| :--- | :--- | :--- |
| **S — Spoofing (Falsificação de Identidade)** | Adulteração de autoria ou metadados em relatórios exportados | Metadados do cabeçalho (`Projeto:`, `Responsável:`, `Instituição:`, `Matrícula / ID:`) gerenciados explicitamente pelo usuário no menu **Configurações (`Ctrl+I`)** e pacote binário assinado na Microsoft Store. |
| **T — Tampering (Adulteração de Dados Normativos)** | Modificação maliciosa de tabelas de limites das normas ABNT NBR em disco | O banco de dados [`NormativeDatabase`](../source/core/NormativeDatabase.cpp) reside exclusivamente em memória RAM (`:memory:`), sendo reconstruído de forma imutável a cada inicialização do processo. |
| **R — Repudiation (Repúdio Técnico)** | Falta de rastreabilidade dos parâmetros de entrada que geraram um traço | O relatório técnico da **Aba 7** registra integralmente todos os parâmetros de projeto, materiais, equações ajustadas ($k_1 \dots k_6$, $R^2$) e verificações normativas. |
| **I — Information Disclosure (Vazamento de Dados)** | Exfiltração de segredos industriais de traços de concreto ou dados pessoais | Operação 100% offline sem sockets de rede, sem telemetria e sem conexões externas. |
| **D — Denial of Service (Negação de Serviço / Falha Numérica)** | Entradas degeneradas causando divisão por zero, `NaN` ou travamento da UI | Validação de domínio matemático em [`RegressionEngine`](../source/core/RegressionEngine.cpp) e [`MixDesignCalculator`](../source/core/MixDesignCalculator.cpp) ($y > 0$, denominadores $> 10^{-12}$, limites de `QDoubleSpinBox`). |
| **E — Elevation of Privilege / XSS Local** | Injeção de HTML/JavaScript nos campos de texto exportados para `.html`/`.pdf` | A função `escapeHtml()` em [`ProjectData::generateHtmlReport()`](../source/core/ProjectData.cpp) sanitiza todos os caracteres especiais (`&`, `<`, `>`, `"`, `'`). |
| **C — Concurrency / Race Conditions** | Acesso simultâneo a consultas normativas e tarefas de cálculo concorrentes | O singleton [`NormativeDatabase`](../source/core/NormativeDatabase.hpp) utiliza sincronização por `QRecursiveMutex` em todas as consultas SQL em memória. |

---

### 2. Persistência Local Segura (`QSettings`)

As únicas informações salvas localmente entre sessões são o código de idioma (`idioma`) e os 6 campos de preferências de cabeçalho do menu **Configurações** (`usePresetProjectName`, `customProjectName`, `engineerName`, `institution`, `studentId`), armazenados na chave de usuário padrão do Windows (`HKEY_CURRENT_USER\Software\AppsCpp\ConcreteMixDesign`), sem exigir privilégios administrativos (UAC).

---

### 3. Modelo de Segurança da Linha de Comando (CLI)

O binário `concrete-mix-designer-cli.exe` é projetado para operação segura e determinística em ambientes corporativos e de automação:
- Validação estrita de limites físicos em todos os argumentos de entrada (`--fck`, `--slump`, `--ac`, etc.);
- Sanitização de caminhos de arquivo para prevenir travessia de diretórios (*Path Traversal*);
- Validação de integridade e formato de esquemas JSON ao carregar arquivos de projetos externos;
- Códigos de saída consistentes para integração segura em pipelines de esteira (CI/CD).

---

## English (United States)

### 1. Trust Boundaries and Attack Surface

**Concrete Mix Design C++ (v2026.11.6.0)** is a native desktop application (`WIN32_EXECUTABLE`) operating entirely within the local user space of the **Microsoft Windows 10/11 (x64)** operating system. Its threat model (*STRIDE*) is engineered to comply with **Microsoft Store Policies 10.2 and 10.5**, the **Brazilian LGPD (Law No. 13,709/2018)**, and the **EU GDPR (Regulation (EU) 2016/679)**:

| STRIDE Category | Analyzed Potential Vector | Mitigation Implemented in C++20 Code |
| :--- | :--- | :--- |
| **S — Spoofing** | Tampering with authorship or metadata in exported reports | Header metadata (`Project:`, `Engineer:`, `Institution:`, `Registration / ID:`) is explicitly managed by the user in the **Settings (`Ctrl+I`)** menu, and official binaries are signed on the Microsoft Store. |
| **T — Tampering** | Malicious modification of ABNT NBR normative limit tables on disk | The [`NormativeDatabase`](../source/core/NormativeDatabase.cpp) resides exclusively in volatile RAM (`:memory:`), being immutably rebuilt at every process startup. |
| **R — Repudiation** | Lack of traceability for input parameters that produced a mix design | The **Tab 7** technical report records all project parameters, materials, fitted equations ($k_1 \dots k_6$, $R^2$), and normative compliance checks in full. |
| **I — Information Disclosure** | Exfiltration of proprietary concrete mix formulas or personal data | 100% offline operation with zero network sockets, zero telemetry, and zero external connections. |
| **D — Denial of Service (Numerical Failure)** | Degenerate inputs causing division by zero, `NaN`, or UI hangs | Mathematical domain validation in [`RegressionEngine`](../source/core/RegressionEngine.cpp) and [`MixDesignCalculator`](../source/core/MixDesignCalculator.cpp) ($y > 0$, denominators $> 10^{-12}$, `QDoubleSpinBox` bounds). |
| **E — Elevation of Privilege / Local XSS** | HTML/JavaScript injection in text fields exported to `.html`/`.pdf` | The `escapeHtml()` helper in [`ProjectData::generateHtmlReport()`](../source/core/ProjectData.cpp) sanitizes all special characters (`&`, `<`, `>`, `"`, `'`). |
| **C — Concurrency / Race Conditions** | Simultaneous standards queries and concurrent calculation tasks | The [`NormativeDatabase`](../source/core/NormativeDatabase.hpp) singleton utilizes synchronization via `QRecursiveMutex` for all in-memory SQL operations. |

---

### 2. Safe Local Persistence (`QSettings`)

The only data persisted locally across sessions are the language code (`idioma`) and the 6 header preference fields from the **Settings** menu (`usePresetProjectName`, `customProjectName`, `engineerName`, `institution`, `studentId`), stored under the standard per-user Windows Registry key (`HKEY_CURRENT_USER\Software\AppsCpp\ConcreteMixDesign`), requiring zero administrative (UAC) privileges.

---

### 3. Command Line Interface (CLI) Security Model

The `concrete-mix-designer-cli.exe` binary is designed for safe and deterministic execution in corporate and automated environments:
- Strict validation of physical and mathematical ranges on all input arguments (`--fck`, `--slump`, `--ac`, etc.);
- Safe file path canonicalization to prevent directory traversal attacks (*Path Traversal*);
- Schema and structure integrity validation when loading external JSON project files;
- Consistent process exit codes for secure integration in automated CI/CD pipelines.
