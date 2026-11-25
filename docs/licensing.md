# Licenciamento e Esclarecimento Comercial / Licensing & Commercial Model — Concrete Mix Designer

<p align="center">
  <b>Selecione o idioma / Select language:</b><br>
  <a href="#ptbr">🇧🇷 Português (BR)</a> | <a href="#enus">🇺🇸 English (US)</a>
</p>

---

## <a id="ptbr"></a>🇧🇷 Português (BR)

<details open>
<summary>Clique para expandir / recolher o documento de licenciamento em português</summary><br>

# Concrete Mix Designer — Licenciamento, Termos de Código Aberto e Esclarecimento Comercial

**Versão:** `2026.11.22.0`  
**Data:** `22 de novembro de 2026`  
**Licença do Código-Fonte:** GNU Affero General Public License v3 (AGPLv3)  
**Autor e Detentor dos Direitos Autorais:** Fernando Nillsson Cidade  

---

## 1. Visão Geral do Modelo de Duplo Licenciamento

O **Concrete Mix Designer** (*Módulo de Dosagem de Concreto*) adota um modelo aberto transparente que concilia a liberdade do ecossistema de software livre copyleft forte com a distribuição oficial assinada na Microsoft Store:

| Componente | Licença | Usos Permitidos |
|---|---|---|
| **Código-Fonte (Repositório Completo)** | [GNU AGPLv3](../LICENSE) | 100% Livre: Uso, estudo, compilação, modificação, distribuição e contribuição sob termos copyleft da GNU AGPLv3. |
| **Documentação em `source/assets/`** | Autoria do Projeto (Uso Livre para Desenvolvedores) | Criada para atender exigências da Microsoft Store; desenvolvedores possuem liberdade total para alterar, excluir, adicionar e adaptar em seus próprios forks e plataformas. |
| **Binários Compilados por Terceiros (via Código)** | GNU AGPLv3 | Totalmente livres para compilar, executar, empacotar e redistribuir sob a responsabilidade do desenvolvedor, cumprindo integralmente os termos da AGPLv3. |
| **Binário Oficial Compilado na Microsoft Store** | [EULA Oficial do Autor](../source/assets/EULA/EULA_pt_BR.txt) | Distribuição gratuita oficial pelo autor. Proibida a revenda comercial paga por terceiros, apropriação indevida da marca ou engenharia reversa do pacote oficial. |
| **Bibliotecas de Terceiros (Qt 6.12.0, SQLite 3, Runtimes C++)** | GNU LGPL v3 / Domínio Público / GPLv3 Runtime Exception | Cumprem integralmente os requisitos de vinculação dinâmica e atribuição de código aberto upstream. |

---

## 2. Direitos de Código Aberto (GNU AGPLv3)

Todo o código-fonte do Concrete Mix Designer — abrangendo o núcleo analítico em C++23 (`ConcreteStandards`, `AggregateOptimizer`, `MortarContentOptimizer`, `RegressionEngine`, `MixDesignCalculator`, `ProjectData`, `NormativeDatabase`), os diagramas IPT em Qt 6.12.0 (`QuadChartWidget`, `IptUnifiedDiagramWidget`), a CLI `concrete-mix-designer-cli.exe`, os catálogos `.ts`/`.qm`, as suítes modulares de testes automatizados (`tests/`) e os presets do CMake — está licenciado sob a **GNU Affero General Public License v3 (AGPLv3)**:

```text
Concrete Mix Designer — IPT/EPUSP Experimental Concrete Mix Design Suite
Copyright (c) 2025-2026 Fernando Nillsson Cidade

Este programa é um software livre: você pode redistribuí-lo e/ou modificá-lo sob os termos da GNU Affero General Public License conforme publicada pela Free Software Foundation, seja a versão 3 da Licença, ou (a seu critério) qualquer versão posterior.

Este programa é distribuído na esperança de que seja útil, mas SEM QUALQUER GARANTIA; sem mesmo a garantia implícita de COMERCIALIZAÇÃO ou ADEQUAÇÃO A UM DETERMINADO FIM. Veja a GNU Affero General Public License para mais detalhes.

Você deve ter recebido uma cópia da GNU Affero General Public License junto com este programa. Se não, veja em: <https://www.gnu.org/licenses/>.
```

---

## 3. Documentação em `source/assets/` e Liberdade Total para Desenvolvedores

A documentação institucional e regulatória mantida no diretório `source/assets/` (abrangendo os módulos `ABOUT`, `CLC`, `COPYRIGHT`, `EULA`, `LICENSES`, `MANUAL`, `NOTICES`, `PRIVACY_POLICY` e `RELEASE`) possui status e finalidade específicos:

1. **Finalidade Original:** Estes documentos foram elaborados por Fernando Nillsson Cidade para viabilizar a publicação, conformidade regulatória e disponibilização gratuita da aplicação oficial na **Microsoft Store** (cumprindo exigências de EULA, privacidade, suporte e conformidade LGPD/GDPR para aplicativos desktop Windows).
2. **Autorização Expressa para Desenvolvedores:** O autor **autoriza expressamente e concede total liberdade** para que desenvolvedores, contribuidores e forks realizem quaisquer alterações, exclusões, adições ou modificações gerais nessa documentação e no código associado.
3. **Condição de Responsabilidade:** Os desenvolvedores são livres para adaptar ou remover esses textos em seus próprios forks e distribuições, desde que respeitem as diretrizes, normas e a legislação vigente de seus respectivos países e das plataformas em que decidirem publicar suas próprias compilações.
4. **Consulta Direta no Aplicativo:** Os usuários podem ler diretamente dentro da interface gráfica (`Ajuda > Sobre o Módulo de Dosagem de Concreto`) as abas dedicadas de **Licença (`EULA`)**, **CLC / Licenciamento**, **Copyright**, **Avisos**, **Privacidade** e todas as **11 licenças completas** de software livre e terceiros via seletor na aba **Licenças Completas**.

---

## 4. Diferenciação dos Binários Oficiais Compilados pelo Autor

O duplo licenciamento estabelece uma fronteira legal clara entre duas formas de distribuição:

1. **Compilações Independentes a partir do Código-Fonte:** Qualquer usuário, universidade ou empresa tem o direito irrevogável de clonar o repositório, inspecionar cada equação e rotina de cálculo, compilar (via **MSVC 14.52 x64** ou **MinGW-w64 GCC 15.1.0** no Windows 10/11) e executar localmente. Caso redistribua o software modificado ou o forneça como serviço de rede, deve disponibilizar o código-fonte correspondente sob a GNU AGPLv3, preservada a proteção de marca comercial sob a Seção 7(e) (`"Concrete Mix Designer"`, `"Concrete Mix Design C++"`, `"Módulo de Dosagem de Concreto"`).
2. **Pacote Binário Oficial da Microsoft Store (`EULA` + `CLC`):** O pacote empacotado, assinado digitalmente e publicado pelo autor na loja oficial é distribuído gratuitamente sob o [EULA Oficial do Autor](../source/assets/EULA/EULA_pt_BR.txt) e o [Esclarecimento Comercial (`CLC`)](../source/assets/CLC/CLC_pt_BR.txt) para conveniência de estudantes, professores e engenheiros que buscam instalação com 1 clique, desinstalação limpa (Política 10.2.7) e atualizações gerenciadas pela Microsoft Store. O EULA opera em conformidade com os Termos de Licença Padrão para Aplicativos da Microsoft Store; terceiros não têm autorização para republicar esse pacote binário oficial assinado como produto comercial próprio pago.
3. **Garantia de Substituição LGPLv3 (Seção 4):** Em estrita observância à GNU LGPL v3.0, as bibliotecas Qt 6.12.0 (`Qt6Core.dll`, `Qt6Gui.dll`, `Qt6Widgets.dll`, `Qt6Charts.dll`, `Qt6Sql.dll`, `Qt6OpenGL.dll`, `Qt6OpenGLWidgets.dll`) são vinculadas dinamicamente e acompanham a pasta da aplicação em `D:\MISCELANEAS\VSCode\Concrete_Mix_Designer` e nos instaladores (`library/Concrete_Mix_Designer.iss`), preservando o direito do usuário final de substituir ou relinkar versões compatíveis.

---

## 5. Checklist de Conformidade e Submissão Microsoft Store (MSIX / Partner Center)

Este checklist resume o mapeamento documental exigido pelas diretrizes oficiais da Microsoft para aplicativos MSIX (**Create App Submission**, **Support Info**, **Privacy Statement** e **Microsoft Store Policies v7.20**):

| Item do Partner Center | Status Documental / Referência no Repositório | Observação Operacional |
|---|---|---|
| **Categoria (Obrigatório)** | Sugerido: *Utilitários e ferramentas* / *Educação* (Subcategoria Engenharia/Científico) | Configurar na seção *Propriedades*. |
| **URL da Política de Privacidade (Obrigatório para Desktop Bridge / `runFullTrust`)** | [`docs/privacy-policy.md`](privacy-policy.md#ptbr) e [`source/assets/PRIVACY_POLICY/Privacy_Policy_pt_BR.txt`](../source/assets/PRIVACY_POLICY/Privacy_Policy_pt_BR.txt) | Apps Win32/`runFullTrust` exigem política de privacidade (Política 10.5.1). **A Microsoft não fornece política padrão e não cobre este app sob a Privacy Statement da Microsoft.** |
| **Site (Recomendado)** | `https://github.com/fernandoncidade/Concrete_Mix_Designer_cpp` | Deve apontar para o site próprio do desenvolvedor — nunca para a listagem da Store. |
| **Suporte ao Cliente (Recomendado)** | `linceu_lighthouse@outlook.com` / GitHub Issues | **A Microsoft não presta suporte ao cliente para este aplicativo**; o autor (desenvolvedor individual) oferece suporte voluntário best-effort. |
| **Telefone e Endereço** | Desenvolvedor individual (`Fernando Nillsson Cidade`) | Obrigatório apenas para contas comerciais/corporativas que ofertam produtos na França; opcional para contas individuais. |
| **Capacidades Restritas (`runFullTrust`)** | Declarada em [`library/AppxManifest.xml`](../library/AppxManifest.xml) | Identidade `fernandoncidade.ConcreteMixDesigner`, aliases `concretemixdesign.exe` e `concrete-mix-designer-cli.exe`, operando 100% offline. Justificativa pronta abaixo. |
| **Desinstalação Limpa (Política 10.2.7)** | Declarada em `README.md`, `privacy-policy.md` e `EULA` | Remoção limpa via Configurações do Windows / Menu Iniciar sem serviços em segundo plano. |
| **Listagens da Store (`pt-BR` e `en-US`)** | Diretórios [`images/interfaces/pt_BR/`](../images/interfaces/pt_BR) e [`images/interfaces/en_US/`](../images/interfaces/en_US) | Fornecer descrição precisa (Política 10.1), 12 capturas reais por idioma já organizadas e logotipos MSIX. |
| **Palavras-chave / Termos de Busca** | Máximo **7 termos** exclusivos por idioma | Sem termos de preço e sem nomes de terceiros (Política 10.1.3). |

### Textos Prontos para Preenchimento no Partner Center

- **Justificativa para Capacidade Restrita (`runFullTrust` — Opções de Envio / Notas para Certificação):**
  > *Concrete Mix Designer is a native Win32 C++23 / Qt 6.12.0 desktop Civil Engineering application using an embedded in-memory SQLite 3 database (:memory:) and standard Windows file Open/Save dialogs so users can save local `.cmdproj` studies and `.pdf`/`.html`/`.md`/`.png` technical mix-design reports and diagrams. It operates 100% offline with zero background services, zero drivers, and zero telemetry.*

- **Direitos Autorais e Marcas Registradas (`pt-BR`, máx. 200 caracteres):**
  > `© 2025-2026 Fernando Nillsson Cidade. Código aberto GNU AGPLv3; pacote oficial Microsoft Store sob EULA do autor. Qt 6 (LGPLv3) e SQLite (Domínio Público).` *(156 caracteres)*

- **Sugestão de Palavras-chave (`pt-BR`, até 7 termos — Política 10.1.3):**
  `Dosagem de Concreto`, `IPT EPUSP`, `ABNT NBR`, `Engenharia Civil`, `Diagrama IPT`, `Traço Concreto`, `Tecnologia Concreto`

---

## 6. Escopo Auxiliar Gratuito, Limitações de Maturidade e Isenção Legal ("AS IS")

1. **Uso Livre como Ferramenta Auxiliar:** Por se tratar de um aplicativo gratuito, o usuário pode utilizar o **Concrete Mix Designer** conforme lhe convir como **ferramenta computacional auxiliar** (para estudo acadêmico, ensino universitário, pesquisa teórica e comparação de resultados), e **não como ferramenta definitiva ou final**.
2. **Imaturidade para Produção e Conferência Obrigatória:** Este aplicativo **ainda não está maduro o suficiente para ser levado para produção ou uso profissional definitivo**, pois, devido às suas limitações, falhas humanas e eventuais erros no desenvolvimento do código-fonte, pode conter erros graves no código, abrangendo tanto a matemática aplicada quanto a física/tecnologia dos materiais envolvida, razão pela qual seus algoritmos podem apresentar falhas. **O usuário deve sempre utilizar outras ferramentas mais consolidadas no mercado ou conferir manualmente todos os resultados obtidos**, além de realizar ensaios físicos em laboratório (ABNT NBR 5738 e NBR 5739).
3. **Responsabilidade Exclusiva, Não Substituição e Embasamento Legal ("AS IS"):** O software é fornecido **"NO ESTADO EM QUE SE ENCONTRA" ("AS IS")**, com todas as suas falhas e sem garantias explícitas ou implícitas ([EULA Oficial](../source/assets/EULA/EULA_pt_BR.txt), Lei nº 9.609/1998 e Seções 15–16 da [GNU AGPLv3](../LICENSE), preservados os direitos irrenunciáveis da legislação consumerista aplicável). Caso o usuário decida utilizar exclusivamente este aplicativo como ferramenta definitiva de produção ou para uso profissional, em centrais dosadoras ou canteiros de obras, **todas as consequências serão de sua inteira e exclusiva responsabilidade**, não substituindo ensaios laboratoriais nem profissional legalmente habilitado com **ART/RRT** perante o **CREA/CAU** (**Leis nº 5.194/1966, nº 6.496/1977 e nº 12.378/2010**; **CDC, Lei nº 8.078/1990, art. 39, VIII**; **Código Civil, Lei nº 10.406/2002, arts. 186, 618 e 927**; **Código Penal, Decreto-Lei nº 2.848/1940, art. 256**), com integral isenção civil, criminal, administrativa e técnica do autor na extensão máxima permitida em lei.

</details>

---

## <a id="enus"></a>🇺🇸 English (US)

<details open>
<summary>Click to expand / collapse licensing in English</summary><br>

# Concrete Mix Designer — Licensing, Open Source Terms & Commercial Clarification

**Version:** `2026.11.22.0`  
**Date:** `November 22, 2026`  
**Source Code License:** GNU Affero General Public License v3 (AGPLv3)  
**Author and Copyright Holder:** Fernando Nillsson Cidade  

---

## 1. Dual-Licensing Architecture Overview

**Concrete Mix Designer** (*Concrete Mix Design Module*) adopts a transparent open model balancing the freedom of a strong-copyleft free software ecosystem with official signed distribution on the Microsoft Store:

| Component | License | Permitted Uses |
|---|---|---|
| **Source Code (Full Repository)** | [GNU AGPLv3](../LICENSE) | 100% Free: Use, study, compile, modify, distribute, and contribute under GNU AGPLv3 copyleft terms. |
| **Documentation in `source/assets/`** | Project Authorship (Free for Developers) | Created to satisfy Microsoft Store requirements; developers have complete freedom to modify, remove, add, and adapt them for their own forks and platforms. |
| **Third-Party Builds from Source** | GNU AGPLv3 | Fully free to compile, run, package, and redistribute under developer responsibility in full compliance with AGPLv3 terms. |
| **Author's Official Microsoft Store Binary** | [Author's Official EULA](../source/assets/EULA/EULA_en_US.txt) | Free official distribution by the author. Paid commercial resale, trademark hijacking, or reverse engineering of the official binary package is prohibited. |
| **Third-Party Libraries (Qt 6.12.0, SQLite 3, C++ Runtimes)** | GNU LGPL v3 / Public Domain / GPLv3 Runtime Exception | Fully compliant with dynamic linking and upstream open-source attribution requirements. |

---

## 2. Open Source Rights (GNU AGPLv3)

All Concrete Mix Designer source code — covering the C++23 analytical core (`ConcreteStandards`, `AggregateOptimizer`, `MortarContentOptimizer`, `RegressionEngine`, `MixDesignCalculator`, `ProjectData`, `NormativeDatabase`), Qt 6.12.0 IPT diagrams (`QuadChartWidget`, `IptUnifiedDiagramWidget`), CLI utility (`concrete-mix-designer-cli.exe`), translation catalogs (`.ts`/`.qm`), modular automated test suites (`tests/`), and CMake presets — is licensed under the **GNU Affero General Public License v3 (AGPLv3)**:

```text
Concrete Mix Designer — IPT/EPUSP Experimental Concrete Mix Design Suite
Copyright (c) 2025-2026 Fernando Nillsson Cidade

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
```

---

## 3. Documentation in `source/assets/` and Full Freedom for Developers

Institutional and regulatory documentation maintained in `source/assets/` (`ABOUT`, `CLC`, `COPYRIGHT`, `EULA`, `LICENSES`, `MANUAL`, `NOTICES`, `PRIVACY_POLICY`, and `RELEASE`) carries specific purpose and permission:

1. **Original Purpose:** Prepared by Fernando Nillsson Cidade to enable official publishing, regulatory compliance, and free availability on the **Microsoft Store** (fulfilling EULA, privacy, support, and LGPD/GDPR expectations for Windows desktop apps).
2. **Explicit Authorization for Developers:** The author **expressly authorizes and grants full freedom** for developers, contributors, and forks to make any changes, deletions, additions, or modifications to these documents and associated code.
3. **Condition of Responsibility:** Developers are free to adapt or remove these texts in their own forks and distributions, provided they comply with guidelines, standards, and applicable laws of their respective countries and target distribution platforms.
4. **Direct In-App Access:** End users can consult **License (`EULA`)**, **CLC / Licensing**, **Copyright**, **Notices**, **Privacy**, and all **11 full open-source and third-party license texts** directly inside the GUI (`Help > About the Concrete Mix Design Module`) via the `Full Licenses` dropdown selector.

---

## 4. Distinct Boundary for Official Store Binaries

Dual licensing draws a clear legal boundary between two distribution paths:

1. **Independent Builds from Source:** Any individual, university, or organization holds the irrevocable right to clone the repository, inspect every equation and calculation routine, compile (via **MSVC 14.52 x64** or **MinGW-w64 GCC 15.1.0** on Windows 10/11), and run locally. Redistributing modified versions or offering the software as a network service requires supplying the corresponding source code under GNU AGPLv3 while preserving trademark protection under Section 7(e) (`"Concrete Mix Designer"`, `"Concrete Mix Design C++"`, `"Módulo de Dosagem de Concreto"`).
2. **Official Microsoft Store Binary Package (`EULA` + `CLC`):** The package compiled, digitally signed, and published by the author on the official store is distributed free of charge under the author's [Official EULA](../source/assets/EULA/EULA_en_US.txt) and [Commercial Licensing Clarification (`CLC`)](../source/assets/CLC/CLC_en_US.txt) for students, educators, and engineers seeking 1-click installation, clean uninstallation (Policy 10.2.7), and Microsoft Store-managed updates. The EULA operates in compliance with the Microsoft Store Standard Application License Terms; third parties are not authorized to republish that official signed binary as their own paid commercial product.
3. **LGPLv3 Library Replacement Guarantee (Section 4):** Pursuant to **GNU LGPL v3.0**, Qt 6.12.0 runtime libraries (`Qt6Core.dll`, `Qt6Gui.dll`, `Qt6Widgets.dll`, `Qt6Charts.dll`, `Qt6Sql.dll`, `Qt6OpenGL.dll`, `Qt6OpenGLWidgets.dll`) are linked dynamically alongside the binaries in `D:\MISCELANEAS\VSCode\Concrete_Mix_Designer` and Inno Setup installers (`library/Concrete_Mix_Designer.iss`), safeguarding the end user's right to inspect, replace, or relink compatible builds.

---

## 5. Microsoft Store MSIX Submission, Privacy & Support Checklist

This checklist summarizes the documentation mapping required by official Microsoft guidelines for MSIX apps (**Create App Submission**, **Support Info**, **Privacy Statement**, and **Microsoft Store Policies v7.20**):

| Partner Center Field | Repository Reference / Status | Operational Notes |
|---|---|---|
| **Category (Required)** | Suggested: *Utilities & tools* / *Education* (Engineering/Scientific) | Configure under *Properties*. |
| **Privacy Policy URL (Required for Desktop Bridge / `runFullTrust`)** | [`docs/privacy-policy.md`](privacy-policy.md#enus) and [`source/assets/PRIVACY_POLICY/Privacy_Policy_en_US.txt`](../source/assets/PRIVACY_POLICY/Privacy_Policy_en_US.txt) | Win32/`runFullTrust` apps require a privacy policy (Policy 10.5.1). **Microsoft does not provide a default policy and does not cover this app under Microsoft's Privacy Statement.** |
| **Website (Recommended)** | `https://github.com/fernandoncidade/Concrete_Mix_Designer_cpp` | Must point to the developer's own website — never to the Store listing URL. |
| **Customer Support (Recommended)** | `linceu_lighthouse@outlook.com` / GitHub Issues | **Microsoft does not provide customer support for this application**; the author (individual developer) offers voluntary best-effort support. |
| **Phone & Street Address** | Individual Developer (`Fernando Nillsson Cidade`) | Required only for business/company accounts offering products in France; optional for individual accounts. |
| **Restricted Capabilities (`runFullTrust`)** | Declared in [`library/AppxManifest.xml`](../library/AppxManifest.xml) | Identity `fernandoncidade.ConcreteMixDesigner`, aliases `concretemixdesign.exe` and `concrete-mix-designer-cli.exe`, operating 100% offline. Copy-paste justification below. |
| **Clean Uninstall (Policy 10.2.7)** | Declared in `README.md`, `privacy-policy.md`, and `EULA` | Clean removal via Windows Settings / Start Menu without residual background services. |
| **Store Listings (`pt-BR` and `en-US`)** | Directories [`images/interfaces/pt_BR/`](../images/interfaces/pt_BR) and [`images/interfaces/en_US/`](../images/interfaces/en_US) | Provide accurate description (Policy 10.1), 12 real screenshots per locale ready to upload, and MSIX logos. |
| **Keywords / Search Terms** | Maximum **7 unique terms** per language | Zero pricing claims and zero third-party brand names (Policy 10.1.3). |

### Ready-to-Paste Texts for Partner Center

- **Restricted Capability Justification (`runFullTrust` — Submission Options / Notes for Certification):**
  > *Concrete Mix Designer is a native Win32 C++23 / Qt 6.12.0 desktop Civil Engineering application using an embedded in-memory SQLite 3 database (:memory:) and standard Windows file Open/Save dialogs so users can save local `.cmdproj` studies and `.pdf`/`.html`/`.md`/`.png` technical mix-design reports and diagrams. It operates 100% offline with zero background services, zero drivers, and zero telemetry.*

- **Copyright & Trademark Info (`en-US`, max. 200 chars):**
  > `© 2025-2026 Fernando Nillsson Cidade. Open source GNU AGPLv3; official Microsoft Store binary under author EULA. Qt 6 (LGPLv3), SQLite (Public Domain).` *(151 chars)*

- **Suggested Keywords (`en-US`, up to 7 terms — Policy 10.1.3):**
  `Concrete Mix Design`, `IPT EPUSP`, `Civil Engineering`, `ABNT NBR`, `Dosage Diagram`, `Concrete Technology`, `Proportioning`

---

## 6. Free Auxiliary Scope, Maturity Limitations & Legal Disclaimer ("AS IS")

1. **Free Use as an Auxiliary Tool:** Because **Concrete Mix Designer** is a free application, the user may use it as they see fit as an **auxiliary computational tool** (for educational assistance, academic study, research, and result comparison), and **not as a definitive or final tool**.
2. **Production Immaturity and Mandatory Verification:** This application **is not yet mature enough to be taken to production or definitive professional use** because, due to its limitations, human flaws, and possible defects during source-code development, it may contain serious errors in its source code — encompassing both applied mathematics and underlying materials physics — as a result of which its algorithms may produce errors. **The user must always use other, more established tools in the market or manually verify all obtained results**, alongside physical laboratory testing (ABNT NBR 5738 and NBR 5739).
3. **Sole Responsibility, Non-Replacement, and Legal Grounding ("AS IS"):** The software is supplied **"AS IS"**, with all faults and without express or implied warranties ([Official EULA](../source/assets/EULA/EULA_en_US.txt), Brazilian Law No. 9.609/1998, and [GNU AGPLv3](../LICENSE) Sections 15–16, preserving non-waivable statutory consumer rights where applicable). Should the user decide to rely exclusively on this application as a definitive tool for production or professional use at batching plants or construction sites, **all resulting consequences shall be the user's sole and entire responsibility**, and outputs do not replace laboratory testing or oversight by a legally licensed engineer issuing **ART/RRT** before **CREA/CAU** (**Brazilian Laws No. 5.194/1966, No. 6.496/1977, and No. 12.378/2010**; **Consumer Defense Code, Law No. 8.078/1990, Art. 39, VIII**; **Civil Code, Law No. 10.406/2002, Arts. 186, 618, and 927**; **Penal Code, Decree-Law No. 2.848/1940, Art. 256**; or equivalent PE/CEng licensure in other jurisdictions), with complete civil, criminal, administrative, and technical exemption of the author to the maximum extent permitted by applicable law.

</details>
