# Política de Segurança / Security Policy — Concrete Mix Designer

<p align="center">
  <b>Selecione o idioma / Select language:</b><br>
  <a href="#ptbr">🇧🇷 Português (BR)</a> | <a href="#enus">🇺🇸 English (US)</a>
</p>

---

## <a id="ptbr"></a>🇧🇷 Português (BR)

<details open>
<summary>Clique para expandir / recolher a política em português</summary><br>

# Política de Segurança

**Versão:** `2026.11.22.0`  
**Data:** `22 de novembro de 2026`  
**Autor / Mantenedor:** Fernando Nillsson Cidade  

## 1. Versões com Suporte Oficial

Atualizações e correções de segurança são fornecidas para a versão estável mais recente (**C++23 / Qt 6.12.0**):

| Versão | Suportada |
|:---:|:---:|
| `v2026.11.22.0` | :white_check_mark: |
| `< v2026.11` | :x: |

---

## 2. Como Relatar uma Vulnerabilidade

Levamos a integridade, a estabilidade computacional e a segurança do **Concrete Mix Designer** com rigor técnico.

Se você identificou uma potencial vulnerabilidade de segurança (como corrupção de memória no parsing JSON `.cmdproj`, falha de sanitização HTML/PDF em relatórios ou instabilidade numérica explorável):

1. **NÃO divulgue publicamente a vulnerabilidade por meio de issues ou discussões abertas.**
2. Envie o relato de forma privada via **GitHub Security Advisories** na aba [Security](https://github.com/fernandoncidade/Concrete_Mix_Designer_cpp/security/advisories) do repositório ou contate o mantenedor diretamente via e-mail (`linceu_lighthouse@outlook.com`).
3. Inclua descrição detalhada, passos reprodutíveis e arquivo `.cmdproj` ou parâmetros de entrada, se aplicável.

---

## 3. Compromissos Arquiteturais de Segurança

- **100% Autônomo, Local e Offline:** Zero conexões de rede, zero sockets e zero telemetria.
- **Banco Normativo Isolado em RAM (`:memory:`):** [`NormativeDatabase`](source/core/NormativeDatabase.cpp) roda exclusivamente em SQLite em memória protegido por `QRecursiveMutex`.
- **Sanitização e Proteções Numéricas:** Escape HTML explícito (`escapeHtml()`) em [`ProjectData::generateHtmlReport()`](source/core/ProjectData.cpp), limites em `QDoubleSpinBox`, guardas contra divisão por zero/logaritmos não positivos em `RegressionEngine` e armazenamento mínimo local apenas em `QSettings` / `%LOCALAPPDATA%\ConcreteMixDesigner\logs\`.

> [!NOTE]
> **Escopo desta Política vs. Maturidade para Produção ("AS IS"):** Esta Política de Segurança trata exclusivamente de vulnerabilidades de segurança da aplicação e **não certifica maturidade para produção ou uso profissional definitivo**. O **Concrete Mix Designer** é um aplicativo gratuito auxiliar sujeito a limitações, falhas humanas e eventuais erros no código-fonte (matemática aplicada e física/tecnologia do concreto), devendo o usuário sempre conferir resultados manualmente ou em ferramentas consolidadas e realizar ensaios físicos em laboratório (**ABNT NBR 5738 e NBR 5739**), assumindo integral responsabilidade caso o empregue exclusivamente em produção ([`docs/licensing.md`](docs/licensing.md#ptbr) e [EULA](source/assets/EULA/EULA_pt_BR.txt)).

</details>

---

## <a id="enus"></a>🇺🇸 English (US)

<details open>
<summary>Click to expand / collapse the policy in English</summary><br>

# Security Policy

**Version:** `2026.11.22.0`  
**Date:** `November 22, 2026`  
**Author / Maintainer:** Fernando Nillsson Cidade  

## 1. Supported Versions

Security updates are provided for the latest stable release (**C++23 / Qt 6.12.0**):

| Version | Supported |
|:---:|:---:|
| `v2026.11.22.0` | :white_check_mark: |
| `< v2026.11` | :x: |

---

## 2. Reporting a Vulnerability

We take the integrity, computational stability, and security of **Concrete Mix Designer** seriously.

If you discover a potential security vulnerability (such as memory corruption in `.cmdproj` JSON parsing, HTML/PDF report escaping defects, or exploitable numerical instability):

1. **Do NOT disclose security vulnerabilities through public GitHub issues or discussions.**
2. Report privately via **GitHub Security Advisories** under the repository's [Security tab](https://github.com/fernandoncidade/Concrete_Mix_Designer_cpp/security/advisories) or contact the maintainer directly via email (`linceu_lighthouse@outlook.com`).
3. Include a detailed description, exact reproduction steps, and sample `.cmdproj` project or input parameters where applicable.

---

## 3. Core Architectural Security Commitments

- **100% Autonomous, Local & Offline:** Zero network calls, zero listening sockets, and zero telemetry.
- **In-Memory SQLite Isolation (`:memory:`):** [`NormativeDatabase`](source/core/NormativeDatabase.cpp) resides purely in volatile SQLite RAM protected by `QRecursiveMutex`.
- **Sanitization & Numerical Guards:** Explicit HTML escaping (`escapeHtml()`) inside [`ProjectData::generateHtmlReport()`](source/core/ProjectData.cpp), bounded `QDoubleSpinBox` inputs, division-by-zero / positive-domain guards inside `RegressionEngine`, and minimal local-only persistence under `QSettings` / `%LOCALAPPDATA%\ConcreteMixDesigner\logs\`.

> [!NOTE]
> **Security Scope vs. Production Maturity ("AS IS"):** This Security Policy covers application security vulnerabilities and **does not certify maturity for definitive production or professional use**. **Concrete Mix Designer** is a free auxiliary tool subject to limitations, human flaws, and possible defects in source code (applied mathematics and materials physics), requiring manual verification, established market tools, and physical lab testing (**ABNT NBR 5738 and NBR 5739**) — with all consequences resting solely on the user should they decide to rely on it exclusively in production ([`docs/licensing.md`](docs/licensing.md#enus) and [EULA](source/assets/EULA/EULA_en_US.txt)).

</details>
