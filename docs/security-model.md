# Modelo de Segurança e Resiliência / Security Model — Concrete Mix Designer

<p align="center">
  <b>Selecione o idioma / Select language:</b><br>
  <a href="#ptbr">🇧🇷 Português (BR)</a> | <a href="#enus">🇺🇸 English (US)</a>
</p>

---

## <a id="ptbr"></a>🇧🇷 Português (BR)

<details open>
<summary>Clique para expandir / recolher o modelo de segurança em português</summary><br>

# Concrete Mix Designer — Modelo de Segurança, Integridade de Dados e Resiliência

**Versão:** `2026.11.22.0`  
**Data:** `22 de novembro de 2026`  
**Autor:** Fernando Nillsson Cidade  

---

## 1. Princípios de Segurança e Filosofia de Design

O **Concrete Mix Designer** é projetado para operar com isolamento local e segurança de software em estações de trabalho acadêmicas, laboratórios de ensino e ambientes de pesquisa. Sua arquitetura de segurança fundamenta-se em cinco garantias estruturais:

1. **Soberania Absoluta e Arquitetura 100% Local-First:** Opera em regime estrito offline (*air-gapped*). Não há conexões de rede, portas de escuta TCP/UDP, telemetria em segundo plano ou dependência de nuvem.
2. **Segurança de Memória e RAII em C++23:** Uso sistemático de contêineres padrão (`std::vector`, `std::string`, `std::optional`), gerenciamento de vida útil em hierarquia `QObject`, guardas `QPointer` nos diálogos Modeless (`m_sobreDialog`, `m_manualDialog`, `m_normsDialog`) e ausência de ponteiros brutos proprietários desprotegidos.
3. **Resiliência Numérica e Proteções de Domínio:** Validações explícitas em `RegressionEngine`, `AggregateOptimizer`, `MortarContentOptimizer` e `MixDesignCalculator` contra divisão por zero, determinantes singulares em regressões OLS, logaritmos não positivos ($\ln \le 0$) e entradas não finitas (`NaN`/`Inf`), mantendo funcionamento seguro mesmo no Cenário 0 (campos zerados).
4. **Isolamento do Banco Normativo SQLite (`:memory:`) & Concorrência:** O módulo `NormativeDatabase` reside integralmente em RAM volátil (`:memory:`), é populado com consultas parametrizadas e sincronizado por `QRecursiveMutex` para acesso concorrente seguro em múltiplas *threads*.
5. **Vinculação Dinâmica Limpa e Distribuição App-Local:** Implantação enxuta com bibliotecas Qt 6.12.0 verificadas, 5 DLLs locais `Microsoft.VC145.CRT` no MSVC (sem instaladores externos extras) e 3 DLLs de runtime GCC 15.1.0 no MinGW.

---

## 2. Superfície de Ataque e Modelo de Ameaças (STRIDE)

| Vetor Potencial | Nível de Risco | Medida de Mitigação Implementada |
|---|:---:|---|
| **Spoofing / Falsificação de Relatórios** | Baixo | Cabeçalho (`Projeto:`, `Responsável:`, `Instituição:`, `ID`) controlado explicitamente em **Configurações (`Ctrl+I`)** e pacotes oficiais da Microsoft Store assinados digitalmente. |
| **Tampering / Adulteração de Tabelas NBR** | Baixo | [`NormativeDatabase`](../source/core/NormativeDatabase.cpp) roda exclusivamente em SQLite **`:memory:`**, reconstruído de forma imutável a cada inicialização do processo. |
| **Injeção HTML / XSS em Relatórios** | Baixo | Sanitização via `escapeHtml()` em [`ProjectData::generateHtmlReport()`](../source/core/ProjectData.cpp) para todos os textos editáveis pelo usuário (`&`, `<`, `>`, `"`, `'`). |
| **Entradas JSON (`.cmdproj`) ou CLI Malformadas** | Médio | Desserialização estrita via `QJsonDocument` com validação de tipos/faixas numéricas, limites em `QDoubleSpinBox` e verificação de argumentos finitos na CLI `concrete-mix-designer-cli.exe`. |
| **Condições de Corrida (Concurrency)** | Baixo | [`NormativeDatabase`](../source/core/NormativeDatabase.hpp) e [`LogManager`](../source/utils/LogManager.hpp) utilizam `QRecursiveMutex` em todas as leituras/escritas concorrentes. |
| **Persistência Local e Logs (`QSettings` / `%LOCALAPPDATA%`)** | Baixo | Apenas preferências de idioma e metadados opcionais em `HKEY_CURRENT_USER\Software\AppsCpp\ConcreteMixDesign` e logs locais sem dados pessoais em `%LOCALAPPDATA%\ConcreteMixDesigner\logs\` sem exigir elevação UAC. |

---

## 3. Validação Numérica e Sanitização de Entrada

- **Limites Físicos na UI & CLI:** Valores de $f_{ck}$, $S_d$, $a/c$, abatimento, umidades ($h\%$) e massas específicas ($\gamma$) são validados frente a limites físicos positivos.
- **Falha Graciosamente Rastreável:** Quando pontos experimentais não formam tendência física monotônica para Abrams ou Lyse, `SolutionType` registra explicitamente `ExponentialFallback`, `ExperimentalAverage` ou `NoSolution` em `MixDesignResult::solutionDiagnostic`.

---

## 4. Distinção Entre Segurança de Software e Maturidade de Dosagem ("AS IS")

As garantias descritas neste documento referem-se exclusivamente à segurança de execução local da aplicação (memória, banco `:memory:`, parsing JSON/HTML e operação offline) e **não certificam maturidade definitiva para produção nem ausência de erros de cálculo em dosagem de concreto**. O **Concrete Mix Designer** é um aplicativo gratuito de uso auxiliar ainda não maduro para produção ou uso profissional definitivo, sujeito a limitações, falhas humanas e eventuais erros no código-fonte (matemática aplicada e física/tecnologia dos materiais). Todos os resultados exigem conferência manual, ferramentas mais consolidadas no mercado e ensaios físicos em laboratório (**ABNT NBR 5738 e NBR 5739**); caso o usuário decida utilizá-lo exclusivamente em produção, todas as consequências serão de sua inteira e exclusiva responsabilidade ([`licensing.md`](licensing.md#ptbr) e [EULA Oficial](../source/assets/EULA/EULA_pt_BR.txt)).

</details>

---

## <a id="enus"></a>🇺🇸 English (US)

<details open>
<summary>Click to expand / collapse security model in English</summary><br>

# Concrete Mix Designer — Security Model, Data Integrity & Resilience

**Version:** `2026.11.22.0`  
**Date:** `November 22, 2026`  
**Author:** Fernando Nillsson Cidade  

---

## 1. Security Principles & Architectural Guarantees

**Concrete Mix Designer** is engineered to run cleanly with local isolation across academic workstations, university teaching labs, and research environments. Its security design rests on five guarantees:

1. **Absolute Sovereignty & 100% Local-First Architecture:** Operates strictly offline (*air-gapped*). Zero network connections, zero TCP/UDP sockets, zero background telemetry, and zero cloud dependencies.
2. **Memory Safety & RAII in C++23:** Systematic use of standard containers (`std::vector`, `std::string`, `std::optional`), Qt `QObject` lifetime trees, `QPointer` guards for modeless dialogs (`m_sobreDialog`, `m_manualDialog`, `m_normsDialog`), and zero unmanaged owning raw pointers.
3. **Numerical Resilience & Domain Guards:** Explicit protections across `RegressionEngine`, `AggregateOptimizer`, `MortarContentOptimizer`, and `MixDesignCalculator` against zero division, singular OLS matrices, non-positive logarithms ($\ln \le 0$), and non-finite inputs (`NaN`/`Inf`), preserving safe operation even under Scenario 0 (zeroed fields).
4. **In-Memory SQLite Isolation (`:memory:`) & Thread Safety:** `NormativeDatabase` resides strictly in volatile RAM (`:memory:`), relies on parameterized queries, and synchronizes concurrent access via `QRecursiveMutex`.
5. **Clean App-Local Dynamic Deployment:** Ships verified Qt 6.12.0 runtime libraries alongside 5 app-local `Microsoft.VC145.CRT` DLLs on MSVC (no external redistributable executable) and 3 GCC 15.1.0 runtime DLLs on MinGW.

---

## 2. Attack Surface & Threat Model (STRIDE)

| Threat Vector | Risk Level | Implemented Mitigation |
|---|:---:|---|
| **Report Authorship Spoofing** | Low | Header fields (`Project:`, `Engineer:`, `Institution:`, `ID`) controlled explicitly under **Settings (`Ctrl+I`)**, alongside signed official Store packages. |
| **Normative Table Tampering** | Low | [`NormativeDatabase`](../source/core/NormativeDatabase.cpp) operates exclusively in SQLite **`:memory:`**, cleanly rebuilt on every process startup. |
| **HTML / XSS Injection in Reports** | Low | Strict escaping via `escapeHtml()` inside [`ProjectData::generateHtmlReport()`](../source/core/ProjectData.cpp) across all user-editable strings (`&`, `<`, `>`, `"`, `'`). |
| **Malformed `.cmdproj` JSON or CLI Inputs** | Medium | Schema/type checks in `QJsonDocument` deserialization, strict `QDoubleSpinBox` bounds, and physical parameter validation in `concrete-mix-designer-cli.exe`. |
| **Concurrency / Race Conditions** | Low | [`NormativeDatabase`](../source/core/NormativeDatabase.hpp) and [`LogManager`](../source/utils/LogManager.hpp) serialize concurrent access via `QRecursiveMutex`. |
| **Local Persistence & Logs (`QSettings` / `%LOCALAPPDATA%`)** | Low | Stores only language and optional header settings under `HKEY_CURRENT_USER\Software\AppsCpp\ConcreteMixDesign` plus telemetry-free logs under `%LOCALAPPDATA%\ConcreteMixDesigner\logs\` requiring zero UAC elevation. |

---

## 3. Strict Input Validation & Graceful Diagnostics

- **Physical Ranges Across UI & CLI:** Strengths ($f_{ck}$, $S_d$), $w/c$ ratios, slump, moistures ($h\%$), and specific gravities ($\gamma$) undergo strict domain validation.
- **Traceable Solution Degradation:** Whenever laboratory mix points violate physical monotonicity for Abrams or Lyse fitting, `SolutionType` explicitly flags `ExponentialFallback`, `ExperimentalAverage`, or `NoSolution` in `MixDesignResult::solutionDiagnostic`.

---

## 4. Software Security vs. Production Mix-Design Maturity ("AS IS")

Guarantees in this security model cover local application execution safety (memory, `:memory:` SQLite, JSON/HTML sanitization, and offline execution) and **do not certify definitive production maturity or freedom from proportioning calculation defects**. **Concrete Mix Designer** is a free auxiliary tool not yet mature enough for definitive production or professional use, subject to limitations, human flaws, and possible defects in source code (applied mathematics and underlying materials physics). All outputs require manual verification, more established market tools, and physical laboratory tests (**ABNT NBR 5738 and NBR 5739**); should the user decide to rely exclusively on it in production, all resulting consequences rest solely on the user ([`licensing.md`](licensing.md#enus) and [Official EULA](../source/assets/EULA/EULA_en_US.txt)).

</details>
