# Política de Segurança / Security Policy — Concrete Mix Design C++

[Português (Brasil)](#português-brasil) | [English (United States)](#english-united-states)

---

## Português (Brasil)

### 1. Compromisso com a Segurança Técnica, Finalidade Acadêmica e Integridade Estrutural

A segurança da informação, a integridade de memória do aplicativo e a exatidão matemática dos cálculos didáticos de engenharia civil são prioridades absolutas do **Concrete Mix Design C++**. O aplicativo destina-se exclusivamente a fins de estudo acadêmico, podendo conter possíveis erros e não servindo como software definitivo para tomadas de decisão em produção ou ambientes de trabalho, sendo obrigatório conferir manualmente todos os resultados ou validá-los através de outras ferramentas mais consolidadas no mercado, ficando o autor isento de responsabilidades legais nos termos da legislação brasileira (Código Civil — Lei nº 10.406/2002; Leis nº 9.609/1998, nº 9.610/1998, nº 8.078/1990, nº 5.194/1966 e nº 6.496/1977) e convenções internacionais (UCC § 2-316 "AS-IS", Convenção de Berna, OMPI/WIPO, TRIPS e Seções 15 e 16 da GNU AGPLv3). Este documento estabelece a política oficial de segurança do produto em conformidade com as **Políticas da Microsoft Store (Seção 10.2 — Segurança)**, o **Microsoft Application Developer Agreement (ADA v8.9)**, as diretrizes de divulgação coordenada de vulnerabilidades do GitHub (**CVD — Coordinated Vulnerability Disclosure**) e a legislação aplicável (**LGPD — Lei nº 13.709/2018**, **Marco Civil da Internet — Lei nº 12.965/2014** e **GDPR — Regulamento UE 2016/679**).

### 2. Versões Suportadas

Apenas a versão estável mais recente distribuída na **Microsoft Store** e no repositório oficial do **GitHub** recebe atualizações de segurança e correções normativas:

| Versão | Status de Suporte | Plataforma / Arquitetura | Padrão C++ / Framework |
| :--- | :--- | :--- | :--- |
| **2026.11.11.0** | ✅ Suportada (Ativa) | Windows 10 (1809+) / Windows 11 (x64) | C++20 / Qt 6.11.1 |
| `< 2026.11.11.0` | ❌ Descontinuada | — | — |

### 3. Arquitetura de Segurança por Design (*Security & Privacy by Design*)

O **Concrete Mix Design C++** adota controles rigorosos de engenharia de software para eliminar superfícies de ataque:

1. **Superfície de Rede Zero (100% Offline):** O aplicativo não abre portas de escuta (*sockets*), não realiza requisições HTTP/HTTPS externas, não baixa scripts remotos e não transmite telemetria.
2. **Isolamento do Banco de Dados Normativo (`:memory:`) e Concorrência Segura:** O módulo `NormativeDatabase` instancia o SQLite 3 exclusivamente em memória volátil (`:memory:`) com consultas parametrizadas, tabelas imutáveis inicializadas em tempo de execução e sincronização multithreading via mutex recursivo (`QRecursiveMutex`), prevenindo injeção de SQL (*SQL Injection*), corrupção de memória e condições de corrida (*race conditions*).
3. **Validação Estrita de Entradas Numéricas e Robustez Algorítmica:** Todos os campos de entrada da interface (`QDoubleSpinBox` e tabelas experimentais) e argumentos da linha de comando impõem limites físicos e matemáticos estritos. Os módulos de regressão (`RegressionEngine`) contam com proteções contra divisão por zero, logaritmos de números não positivos e geração de `NaN` ou `Infinity`.
4. **Sanitização na Exportação de Relatórios e Ferramenta CLI:** Na geração de relatórios HTML e PDF (`ProjectData::generateHtmlReport`), os campos textuais personalizáveis pelo usuário passam por escape de entidades HTML (`escapeHtml`), prevenindo injeção de marcação ou *Cross-Site Scripting* (XSS) local. A ferramenta de linha de comando (`concrete-mix-designer-cli`) opera com validação de esquema em arquivos JSON e sanitização de caminhos de saída.
5. **Vinculação Dinâmica Segura:** Todas as bibliotecas de tempo de execução do Qt 6.11.1 e MinGW-w64 são implantadas localmente a partir de binários oficiais verificados.

### 4. Como Reportar uma Vulnerabilidade

Se você identificar uma vulnerabilidade de segurança de software (como corrupção de memória, falha de sanitização em leitura de JSON ou exportação HTML/PDF) ou uma falha crítica de segurança de cálculo normativo que possa induzir a erro estrutural:

1. **NÃO divulgue a falha publicamente** em *Issues* abertas antes que uma correção seja disponibilizada, quando se tratar de vulnerabilidade explorável de segurança da informação.
2. Utilize o recurso de **Report a Vulnerability (Private Security Advisory)** na aba **Security** do repositório oficial no GitHub (`https://github.com/fernandoncidade/Concrete_Mix_Designer_cpp/security/advisories`) ou entre em contato diretamente com o mantenedor e publicador (**Fernando Nillsson Cidade**) pelos canais oficiais listados na Microsoft Store / GitHub.
3. Inclua no relato:
   - Descrição detalhada da vulnerabilidade ou inconsistência normativa;
   - Passos exatos para reprodução (cenário selecionado, valores de entrada ou arquivo de projeto);
   - Versão do sistema operacional Windows e versão do aplicativo (`2026.11.11.0`);
   - Impacto técnico estimado.

### 5. Prazos de Resposta e Divulgação Coordenada (Melhor Esforço)

O mantenedor envidará os melhores esforços técnicos para tratar todos os relatos de forma ágil, ética e transparente, de acordo com o seguinte cronograma orientativo:

- **Confirmação de Recebimento:** Envidaremos os melhores esforços para acusar o recebimento do relato em tempo hábil (tipicamente em até **48 a 72 horas úteis**);
- **Triagem e Diagnóstico Técnico:** Avaliação preliminar de reprodutibilidade e classificação de severidade em até **5 a 10 dias úteis**;
- **Desenvolvimento e Publicação de Patch (GitHub & Microsoft Store):** Para vulnerabilidades críticas confirmadas, o desenvolvimento do patch terá prioridade máxima, com submissão de pacote corretivo ao **Microsoft Partner Center** e publicação de *Release* no GitHub assim que a correção for validada em testes analíticos.

---

## English (United States)

### 1. Commitment to Technical Security, Academic Purpose, and Structural Integrity

Information security, application memory integrity, and the mathematical accuracy of educational civil engineering calculations are absolute priorities of **Concrete Mix Design C++**. The application is intended exclusively for academic study purposes, may contain possible errors, and does not serve as definitive software for decision-making in production or work environments, making it mandatory to manually verify all results or validate them through other more consolidated market tools, with the author being exempt from legal liabilities under Brazilian legislation (Civil Code — Law No. 10,406/2002; Laws No. 9,609/1998, No. 9,610/1998, No. 8,078/1990, No. 5,194/1966, and No. 6,496/1977) and international conventions (UCC § 2-316 "AS-IS", Berne Convention, WIPO, TRIPS, and Sections 15 and 16 of the GNU AGPLv3). This document establishes the official product security policy in compliance with **Microsoft Store Policies (Section 10.2 — Security)**, the **Microsoft Application Developer Agreement (ADA v8.9)**, GitHub's **Coordinated Vulnerability Disclosure (CVD)** guidelines, and applicable legislation (**LGPD — Brazilian Law No. 13,709/2018**, **Brazilian Civil Rights Framework for the Internet — Law No. 12,965/2014**, and **GDPR — EU Regulation 2016/679**).

### 2. Supported Versions

Only the latest stable version distributed on the **Microsoft Store** and in the official **GitHub** repository receives security updates and normative fixes:

| Version | Support Status | Platform / Architecture | C++ Standard / Framework |
| :--- | :--- | :--- | :--- |
| **2026.11.11.0** | ✅ Supported (Active) | Windows 10 (1809+) / Windows 11 (x64) | C++20 / Qt 6.11.1 |
| `< 2026.11.11.0` | ❌ Discontinued | — | — |

### 3. Security & Privacy by Design Architecture

**Concrete Mix Design C++** enforces strict software engineering controls to eliminate attack surfaces:

1. **Zero Network Surface (100% Offline):** The application opens no listening sockets, makes no external HTTP/HTTPS requests, downloads no remote scripts, and transmits zero telemetry.
2. **In-Memory Normative Database Isolation (`:memory:`) and Thread-Safety:** The `NormativeDatabase` module instantiates SQLite 3 exclusively in volatile memory (`:memory:`) using parameterized queries, immutable runtime-initialized tables, and multithreaded synchronization via a recursive mutex (`QRecursiveMutex`), preventing SQL Injection, memory corruption, and race conditions.
3. **Strict Numeric Input Validation and Algorithmic Robustness:** All UI input controls (`QDoubleSpinBox` and experimental tables) and CLI arguments enforce strict physical and mathematical bounds. The regression modules (`RegressionEngine`) include division-by-zero guards, log-domain validations, and prevent `NaN` or `Infinity`.
4. **Report Export and CLI Tool Sanitization:** When generating HTML and PDF reports (`ProjectData::generateHtmlReport`), customizable text fields undergo HTML entity escaping (`escapeHtml`), preventing markup injection or local Cross-Site Scripting (XSS). The command-line tool (`concrete-mix-designer-cli`) implements schema validation for JSON files and path canonicalization.
5. **Safe Dynamic Linking:** All Qt 6.11.1 and MinGW-w64 runtime libraries are deployed locally from verified official binaries.

### 4. How to Report a Vulnerability

If you identify a software security vulnerability (such as memory corruption, sanitization failure in JSON parsing or HTML/PDF export) or a critical normative calculation flaw that could lead to structural engineering error:

1. **DO NOT disclose the flaw publicly** in open *Issues* before a fix is released when dealing with an exploitable information security vulnerability.
2. Use the **Report a Vulnerability (Private Security Advisory)** feature under the **Security** tab of the official GitHub repository (`https://github.com/fernandoncidade/Concrete_Mix_Designer_cpp/security/advisories`) or contact the maintainer and publisher (**Fernando Nillsson Cidade**) directly through the official channels listed on the Microsoft Store / GitHub.
3. Please include in your report:
   - Detailed description of the vulnerability or normative inconsistency;
   - Exact steps to reproduce (selected scenario, input values, or project file);
   - Windows operating system version and application version (`2026.11.11.0`);
   - Estimated technical impact.

### 5. Response Timelines and Coordinated Disclosure (Best-Effort Basis)

The maintainer will make best efforts to address all vulnerability reports promptly, ethically, and transparently, according to the following target schedule:

- **Acknowledgment of Receipt:** Best efforts will be made to acknowledge receipt of the report promptly (typically within **48 to 72 business hours**);
- **Triage and Technical Diagnosis:** Preliminary reproduction and severity classification within **5 to 10 business days**;
- **Patch Development and Publication (GitHub & Microsoft Store):** For confirmed critical vulnerabilities, patch development receives top priority, with submission of an updated package to the **Microsoft Partner Center** and a GitHub *Release* published as soon as analytical verification tests pass.
