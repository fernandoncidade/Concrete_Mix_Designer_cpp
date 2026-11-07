# Notas de Lançamento / Release Notes — v2026.11.6.0

<p align="center">
  <b>Selecione o idioma / Select language:</b><br>
  <a href="#ptbr">🇧🇷 Português (BR)</a> | <a href="#enus">🇺🇸 English (US)</a>
</p>

---

## <a id="ptbr"></a>🇧🇷 Português (BR)

<details>
<summary>Clique para expandir / recolher as notas de versão em português</summary><br>

# Concrete Mix Designer — Release v2026.11.6.0

**Versão Oficial:** `2026.11.6.0`  
**Tag de Versão:** `v2026.11.6.0`  
**Data de Publicação:** 6 de novembro de 2026  
**Autor:** Fernando Nillsson Cidade  
**Repositório Oficial:** [GitHub Concrete_Mix_Designer_cpp](../)  

---

## 1. Artefatos de Instalação e Hashes Criptográficos

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

## 2. Destaques da Versão 2026.11.6.0

Esta versão consolida aprimoramentos fundamentais na governança jurídica, suporte técnico oficial, transparência de distribuição e harmonização internacional:

- **Formalização do Modelo de Duplo Licenciamento (Dual-Licensing):**
  - Definição clara da coexistência entre a edição comunitária em código aberto (sob licença GNU AGPLv3) e a edição comercial oficial distribuída através da Microsoft Store.
  - Isenção explícita de obrigações copyleft para uso corporativo interno do binário comercial adquirido na loja.
- **Canal Oficial Centralizado de Atendimento e Suporte Técnico:**
  - Atualização unificada em todas as telas, janelas informativas ("Sobre"), contratos e manuais do endereço de suporte oficial: `linceu_lighthouse@outlook.com`.
  - Atendimento direto e ágil via e-mail em regime de melhor esforço (best-effort), sem necessidade de contato telefônico ou acesso remoto aos dispositivos.
- **Harmonização e Paridade Estrutural nos Idiomas Suportados (Português BR e Inglês US):**
  - Atualização completa de todos os textos informativos, contratos EULA, termos comerciais (CLC), políticas de privacidade e manuais de uso em Português e Inglês.
  - Paridade simétrica linha por linha em todos os arquivos de termos e documentação oficial.
- **Privacidade Soberana Local-First e Conformidade com a LGPD:**
  - Reafirmação da operação 100% offline, local e isolada no computador do usuário. Todos os cálculos de dosagem, regressões de Abrams/Lyse/Molinari e memoriais operam sem nuvem e sem telemetria invasiva.
- **Correção e Confiabilidade de Links na Documentação:**
  - Revisão e conversão de todos os links de navegação interna nos guias de arquitetura, manuais do usuário e referências de conformidade para links relativos portáteis.
  - Neutralização e generalização de dados cadastrais de exemplo nos memoriais.
- **Normalização de Arquivos e Higienização do Repositório (`.gitattributes`):**
  - Implementação de regras de normalização de finais de linha (`LF` para código-fonte e documentação, `CRLF` para scripts Windows) e atributos de diff C++ para inspeção limpa no Git.
- **Rastreabilidade Normativa Contínua ABNT:**
  - Preservação da conformidade analítica com as normas ABNT NBR 12655:2022, NBR 6118:2023, NBR 7211:2022, NBR 7212:2025 e NBR 8953:2015.

---

## 3. Requisitos de Sistema

- **Sistema Operacional:** Microsoft Windows 10 (versão 1809 ou superior) e Windows 11 (64-bit x64).
- **Processador:** Arquitetura x86-64 (Intel Core i3/i5/i7/i9 ou AMD Ryzen).
- **Memória RAM:** Mínimo de 512 MB (recomendado 1 GB).
- **Espaço em Disco:** ~100 MB livres para instalação e armazenamento de arquivos exportados.

</details>

---

## <a id="enus"></a>🇺🇸 English (US)

<details>
<summary>Click to expand / collapse the release notes in English</summary><br>

# Concrete Mix Designer — Release v2026.11.6.0

**Official Version:** `2026.11.6.0`  
**Version Tag:** `v2026.11.6.0`  
**Publication Date:** November 6, 2026  
**Author:** Fernando Nillsson Cidade  
**Official Repository:** [GitHub Concrete_Mix_Designer_cpp](../)  

---

## 1. Installation Packages & Cryptographic Checksums

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

## 2. Release Highlights (v2026.11.6.0)

This version consolidates essential improvements in legal governance, official support channels, distribution clarity, and international harmonization:

- **Dual-Licensing Framework Formalization:**
  - Clear establishment of the coexistence between the community open-source edition (licensed under GNU AGPLv3) and the official commercial edition distributed through the Microsoft Store.
  - Explicit copyleft exemption for internal corporate use of official commercial binaries.
- **Centralized Official Support Channel:**
  - Unified update of the official technical support contact across all application windows, "About" dialogs, and legal terms: `linceu_lighthouse@outlook.com`.
  - Direct, reliable best-effort email support without requiring telephone calls or intrusive remote desktop sessions.
- **Full Harmonization & Exact Structural Parity Across Supported Languages (Brazilian Portuguese and US English):**
  - Complete updates for informative dialogs, EULA agreements, commercial terms (CLC), privacy policies, and user documentation across Portuguese and English.
  - Symmetrical parity across all legal terms and documentation files.
- **Local-First Sovereign Privacy & LGPD/GDPR Compliance:**
  - Reaffirmation of 100% offline, local data processing on the user's workstation. All mix design calculations, Abrams/Lyse/Molinari regressions, and technical reports operate with zero cloud dependency and zero telemetry.
- **Documentation Link Integrity & Verification:**
  - Comprehensive inspection and correction of internal navigation links across user manuals, architecture documentation, and compliance guides using portable relative links.
  - Neutralization and generalization of sample template identification data in reports.
- **Git Attributes Normalization & Repository Hygiene (`.gitattributes`):**
  - Implementation of comprehensive line ending rules (`LF` for source code and documentation, `CRLF` for Windows scripts) and C++ diff drivers for clean Git history.
- **Continuous ABNT Normative Traceability:**
  - Preservation of full analytical compliance with ABNT NBR 12655:2022, NBR 6118:2023, NBR 7211:2022, NBR 7212:2025, and NBR 8953:2015.

---

## 3. System Requirements

- **Operating System:** Microsoft Windows 10 (version 1809 or higher) and Windows 11 (64-bit x64).
- **Processor:** x86-64 architecture (Intel Core i3/i5/i7/i9 or AMD Ryzen).
- **RAM:** Minimum 512 MB (1 GB recommended).
- **Disk Space:** ~100 MB free disk space for installation and exported reports.

</details>
