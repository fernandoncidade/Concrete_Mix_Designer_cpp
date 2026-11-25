# Política de Privacidade / Privacy Policy — Concrete Mix Designer

<p align="center">
  <b>Selecione o idioma / Select language:</b><br>
  <a href="#ptbr">🇧🇷 Português (BR)</a> | <a href="#enus">🇺🇸 English (US)</a>
</p>

---

## <a id="ptbr"></a>🇧🇷 Português (BR)

<details open>
<summary>Clique para expandir / recolher a Política de Privacidade em português</summary><br>

# POLÍTICA DE PRIVACIDADE — CONCRETE MIX DESIGNER

**Versão:** `2026.11.22.0`  
**Data:** `22 de novembro de 2026`  
**Desenvolvedor / Controlador:** Fernando Nillsson Cidade (Desenvolvedor Individual)  
**E-mail de Contato e Privacidade:** `linceu_lighthouse@outlook.com`  
**Site Oficial do Projeto:** [https://github.com/fernandoncidade/Concrete_Mix_Designer_cpp](https://github.com/fernandoncidade/Concrete_Mix_Designer_cpp)

---

## 1. Declaração de Independência frente à Política de Privacidade da Microsoft

> [!IMPORTANT]
> **A Microsoft NÃO fornece uma política de privacidade padrão para o Concrete Mix Designer, e este aplicativo NÃO é coberto por nenhuma política de privacidade da Microsoft.**
> - Esta Política de Privacidade aplica-se exclusiva e integralmente ao software **Concrete Mix Designer** em todas as suas formas de distribuição (Microsoft Store MSIX, instalador Windows ou compilação a partir do código-fonte).
> - Ao navegar, baixar, instalar, atualizar, desinstalar, avaliar ou publicar comentários na loja **Microsoft Store** ou utilizar recursos diagnósticos opcionais do sistema operacional **Windows**, os dados tratados pela própria Microsoft são regidos pela [Política de Privacidade da Microsoft](https://www.microsoft.com/pt-br/privacy/privacystatement).
> - Além disso, a Microsoft não presta suporte técnico aos usuários do Concrete Mix Designer; dúvidas sobre privacidade ou operação do aplicativo devem ser direcionadas diretamente ao desenvolvedor por e-mail ou pelo repositório oficial no GitHub.

---

## 2. Arquitetura Local-First e Ausência de Coleta de Dados Pessoais

O **Concrete Mix Designer** foi projetado para operar **100% localmente e offline** na estação de trabalho do usuário:

1. **Zero Coleta de Dados Pessoais (PII):** O software **NÃO** coleta, solicita, lê, armazena remotamente, compartilha ou vende nome civil, CPF/CNPJ, endereço, telefone, e-mail, localização geográfica (GPS/IP), histórico de navegação ou identificadores publicitários.
2. **Zero Rede, Zero Telemetria e Zero Contas:** O código do Concrete Mix Designer não exige autenticação, não contém SDKs analíticos, rastreadores, anúncios, servidores em nuvem ou requisições HTTP/HTTPS em segundo plano. *(Observação técnica: a distribuição empacotada inclui apenas as bibliotecas padrão do runtime Qt 6.12.0 necessárias à interface gráfica, gráficos Qt Charts e driver SQLite local `:memory:`, sem uso de APIs de rede pelo aplicativo.)*
3. **Sem Coleta de Dados de Crianças ou Dados Sensíveis:** Em conformidade com os requisitos das Políticas da Microsoft Store (Seções 10.5.5 a 10.5.8), o aplicativo não coleta dados financeiros, de saúde, biométricos ou informações pessoais de crianças.

---

## 3. Dados Processados Localmente pelo Usuário

Todo processamento analítico de dosagem de concreto permanece sob controle exclusivo do usuário na memória RAM e CPU do computador local:

- **Dados de Engenharia em Memória:** Parâmetros de projeto ($f_{ck}$, $S_d$, CAA I–IV, CA/CP, abatimento), propriedades físicas de materiais e britas, custos unitários, ensaios de teor de argamassa ($\alpha$) e 3 pontos experimentais do diagrama IPT, além das tabelas normativas carregadas exclusivamente em SQLite **`:memory:`**.
- **Arquivos Salvos Explicitamente pelo Usuário:** Estudos no formato JSON aberto **`.cmdproj`**, memoriais técnicos (**`.pdf`**, **`.html`**, **`.md`**) e gráficos de dosagem (**`.png`**) são gravados **apenas** na pasta selecionada explicitamente pelo usuário nos diálogos padrão do Windows ou via argumentos explícitos na CLI.
- **Preferências Locais Mínimas e Logs Diagnósticos:** Preferências mínimas de idioma (`pt_BR` / `en_US`) e metadados opcionais de cabeçalho de relatório (*Projeto*, *Responsável*, *Instituição*, *Matrícula / ID*) são salvos localmente via `QSettings` em `HKEY_CURRENT_USER\Software\AppsCpp\ConcreteMixDesign` (ou no contêiner isolado equivalente em pacote MSIX), acompanhados por logs técnicos locais de execução sem telemetria em `%LOCALAPPDATA%\ConcreteMixDesigner\logs\concrete_mix_designer_*.log`.

---

## 4. Capacidade Declarada no Manifesto MSIX (`runFullTrust`)

Para distribuição em pacote MSIX na Microsoft Store (Política 10.6), o manifesto [`library/AppxManifest.xml`](../library/AppxManifest.xml) declara exclusivamente a capacidade `<rescap:Capability Name="runFullTrust" />`, estritamente necessária por se tratar de aplicação desktop nativa Win32 em C++23 / Qt 6.12.0 com acesso aos diálogos padrão do Windows para salvar e abrir projetos e relatórios escolhidos pelo usuário. Nenhuma capacidade de localização, câmera, microfone ou contatos é declarada ou acessada.

---

## 5. Direitos do Titular, Controle e Desinstalação Limpa (Política 10.2.7 / LGPD / GDPR)

O usuário exerce controle pleno sobre todos os dados locais:
- **Acesso, Edição, Portabilidade e Exclusão:** Você pode abrir, copiar, editar, redefinir (via `Configurações > Dados do Projeto... > Restaurar Padrões`) ou excluir livremente qualquer arquivo `.cmdproj`, `.pdf`, `.html`, `.md`, `.png` ou registro local a qualquer momento.
- **Desinstalação Limpa:** Você pode desinstalar e remover o Concrete Mix Designer de forma limpa a qualquer momento em **Configurações do Windows → Aplicativos → Aplicativos instalados** ou pelo **Menu Iniciar**. A remoção elimina integralmente os binários sem deixar serviços em segundo plano, drivers ou tarefas agendadas residuais.

---

## 6. Contato do Responsável pela Privacidade e Suporte

- **Responsável:** Fernando Nillsson Cidade
- **E-mail:** `linceu_lighthouse@outlook.com`
- **Suporte / Issues:** [https://github.com/fernandoncidade/Concrete_Mix_Designer_cpp/issues](https://github.com/fernandoncidade/Concrete_Mix_Designer_cpp/issues)

</details>

---

## <a id="enus"></a>🇺🇸 English (US)

<details open>
<summary>Click to expand / collapse Privacy Policy in English</summary><br>

# PRIVACY POLICY — CONCRETE MIX DESIGNER

**Version:** `2026.11.22.0`  
**Date:** `November 22, 2026`  
**Developer / Controller:** Fernando Nillsson Cidade (Individual Developer)  
**Privacy & Support Email:** `linceu_lighthouse@outlook.com`  
**Official Project Website:** [https://github.com/fernandoncidade/Concrete_Mix_Designer_cpp](https://github.com/fernandoncidade/Concrete_Mix_Designer_cpp)

---

## 1. Independence from Microsoft Privacy Statement

> [!IMPORTANT]
> **Microsoft does NOT provide a default privacy policy for Concrete Mix Designer, and this application is NOT covered by any Microsoft privacy policy.**
> - This Privacy Policy applies exclusively to the **Concrete Mix Designer** software across all distribution channels (Microsoft Store MSIX package, Windows installer, or independent builds from source code).
> - Data collected separately by Microsoft when you browse, download, install, update, uninstall, rate, or review apps in the **Microsoft Store** or use optional **Windows** operating system diagnostic features is governed by the [Microsoft Privacy Statement](https://www.microsoft.com/en-us/privacy/privacystatement).
> - In addition, Microsoft does not provide technical or customer support to Concrete Mix Designer users; privacy or application operation inquiries must be directed straight to the developer via email or through the official GitHub repository.

---

## 2. Local-First Architecture & Zero Personal Data Collection

**Concrete Mix Designer** is engineered to operate **100% locally and offline** on the user's workstation:

1. **Zero Personal Data Collection (PII):** The software does **NOT** collect, request, read, remotely store, share, or sell civil names, taxpayer/national IDs, addresses, phone numbers, email addresses, geographic location (GPS/IP), browsing history, or advertising identifiers.
2. **Zero Network, Zero Telemetry, and Zero Accounts:** The Concrete Mix Designer codebase requires no authentication and contains no analytics SDKs, trackers, ads, cloud servers, or background HTTP/HTTPS requests. *(Technical note: the packaged distribution includes standard Qt 6.12.0 runtime libraries required only for UI controls, Qt Charts, and the local SQLite `:memory:` driver, with zero network API usage by the application.)*
3. **No Collection of Children's or Sensitive Data:** In compliance with Microsoft Store Policies requirements (Sections 10.5.5 to 10.5.8), the application does not collect financial, health, biometric, or children's personal information.

---

## 3. Data Processed Locally by the User

All analytical concrete proportioning computations remain solely under user control in the local workstation's RAM and CPU:

- **In-Memory Engineering Data:** Project parameters ($f_{ck}$, $S_d$, CAA I–IV, RC/PC, slump), material physical properties and aggregate blends, unit prices, mortar content ($\alpha$) trials, and the 3 IPT experimental trial points, alongside normative tables loaded exclusively into in-memory SQLite **`:memory:`**.
- **Files Explicitly Saved by the User:** Project studies in open **`.cmdproj`** (JSON) format, technical reports (**`.pdf`**, **`.html`**, **`.md`**), and exported dosage charts (**`.png`**) are written **only** to folders explicitly chosen by the user via standard Windows file dialogs or explicit CLI flags.
- **Minimal Local Preferences & Diagnostic Logs:** Minimal language preferences (`pt_BR` / `en_US`) and optional report header metadata (*Project*, *Engineer*, *Institution*, *ID*) are persisted locally via `QSettings` under `HKEY_CURRENT_USER\Software\AppsCpp\ConcreteMixDesign` (or the equivalent isolated MSIX package storage container), alongside local telemetry-free diagnostic logs under `%LOCALAPPDATA%\ConcreteMixDesigner\logs\concrete_mix_designer_*.log`.

---

## 4. Capability Declared in the MSIX Manifest (`runFullTrust`)

For MSIX package distribution on the Microsoft Store (Policy 10.6), [`library/AppxManifest.xml`](../library/AppxManifest.xml) declares exclusively `<rescap:Capability Name="runFullTrust" />`, strictly required for a native Win32 C++23 / Qt 6.12.0 desktop application accessing standard Windows dialogs to save/open user-selected studies and reports. No location, camera, microphone, or contacts capabilities are declared or accessed.

---

## 5. Data Subject Rights, Control & Clean Uninstallation (Policy 10.2.7 / LGPD / GDPR)

The user exercises full control over all local data:
- **Access, Editing, Portability & Deletion:** You may freely open, copy, edit, reset (via `Settings > Project Data... > Restore Defaults`), or delete any `.cmdproj`, `.pdf`, `.html`, `.md`, `.png` file, or local log at any time.
- **Clean Uninstallation:** You may cleanly uninstall and remove Concrete Mix Designer at any time via **Windows Settings → Apps → Installed Apps** or via the **Start Menu**, removing all binaries cleanly without leaving background services, drivers, or scheduled tasks behind.

---

## 6. Privacy & Support Contact

- **Developer:** Fernando Nillsson Cidade
- **Email:** `linceu_lighthouse@outlook.com`
- **Support / Issues:** [https://github.com/fernandoncidade/Concrete_Mix_Designer_cpp/issues](https://github.com/fernandoncidade/Concrete_Mix_Designer_cpp/issues)

</details>
