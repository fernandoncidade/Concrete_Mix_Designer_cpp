# Observações Técnicas, Galeria Visual e Origem Metodológica / Technical Notes, UI Gallery & Methodological Origin

<div align="center">

[![Português (BR)](https://img.shields.io/badge/Idioma-Portugu%C3%AAs%20(BR)-009739?style=for-the-badge)](#-português-brasil)
[![English (US)](https://img.shields.io/badge/Language-English%20(US)-0052B4?style=for-the-badge)](#-english-united-states)
[![Versão](https://img.shields.io/badge/Vers%C3%A3o-2026.11.22.0-1f6feb?style=for-the-badge)](docs/release-v2026.11.22.0.md)
[![Licença](https://img.shields.io/badge/Licen%C3%A7a-Dual%20(AGPLv3%20%2B%20Store%20EULA)-8957e5?style=for-the-badge)](docs/licensing.md)

</div>

---

<a id="ptbr"></a>
<details>
<summary><h2>🇧🇷 Português (Brasil)</h2></summary>

**Versão:** `2026.11.22.0`  
**Data:** `22 de novembro de 2026`  
**Autor:** Fernando Nillsson Cidade ([`linceu_lighthouse@outlook.com`](mailto:linceu_lighthouse@outlook.com))  

---

### 1. Aviso Importante de Escopo, Maturidade e Isenção de Garantias

> **Software Auxiliar Gratuito em Evolução ("NO ESTADO EM QUE SE ENCONTRA" / *AS IS*):**  
> Este projeto consiste em um **programa gratuito auxiliar** de código aberto voltado ao apoio computacional para dosagem experimental de concreto pelo **Método IPT/EPUSP** (Helene & Terzian, 1992). **O software ainda não possui maturidade suficiente para ser utilizado de forma definitiva em obras ou projetos comerciais.** Em razão de possíveis limitações de modelagem, falhas humanas ou defeitos no código-fonte (abrangendo tanto a matemática aplicada quanto os modelos de tecnologia de materiais), todos os resultados produzidos **devem ser conferidos manualmente pelo engenheiro responsável, comparados com aplicativos consagrados de mercado e comprovados por ensaios físicos de laboratório (ABNT NBR 5738 / NBR 5739)**. O uso exclusivo ou não verificado em produção ocorre por conta e risco integral do usuário ([`docs/licensing.md`](docs/licensing.md) e [`EULA_pt_BR.txt`](source/assets/EULA/EULA_pt_BR.txt)).

---

### 2. Origem Metodológica, Auditoria da Planilha UFPR (TC031) e Melhorias em C++23

O **Concrete Mix Designer** surgiu da engenharia reversa matemática e correção algorítmica da planilha clássica **TC031 — Disciplina de Materiais de Construção Civil da UFPR**, originalmente estruturada com base no livro *"Manual de Dosagem e Controle do Concreto"* (Paulo Helene e Paulo Terzian, IPT/EPUSP, 1992).

Durante a migração e consolidação para a arquitetura nativa **C++23 + Qt 6.12.0**, foram corrigidas limitações históricas presentes em planilhas estáticas e implementados refinamentos de precisão:
1. **Substituição de Fórmulas Quebradas de Empacotamento e Padiolas:** Correção do algoritmo de mistura binária de britas (`AggregateOptimizer`, volume padrão $15{,}04\text{ L}$ conforme NBR NM 45) e dimensionamento consistente das padiolas para sacos de $50\text{ kg}$ com base $35 \times 45\text{ cm}$.
2. **Inversão Robusta de Abrams, Lyse e Molinari (`RegressionEngine`):** Substituição de aproximações manuais por regressões lineares/quadráticas via mínimos quadrados com pivoteamento parcial gaussiano e cálculo paralelo do consumo teórico por massa específica ($C = 1000 / V_s$).
3. **Banco Normativo Isolado em Memória (`NormativeDatabase`):** Banco relacional SQLite `:memory:` protegido por `QRecursiveMutex`, eliminando riscos de corrupção de disco e aplicando em tempo real os limites das normas **ABNT NBR 12655:2022**, **NBR 6118:2023**, **NBR 7211:2022**, **NBR 7212:2025**, **NBR 8953:2015** e **NBR 16697:2018**.
4. **Arquitetura Multi-Build Moderna:** Suporte integral aos compiladores **MSVC 14.52** (Windows SDK `10.0.28000.0`) e **MinGW-w64 GCC 15.1.0**, com distribuições externas enxutas (`app-local`, **sem empacotamento redundante de `vc_redist.x64.exe`**) e instaladores Inno Setup (`library/Concrete_Mix_Designer*.iss`).

---

### 3. Galeria Visual da Interface (`images/interfaces/`)

#### 3.1. Capturas em Português (`images/interfaces/pt_BR/`)

| Visão | Arquivo | Pré-visualização |
|---|---|---|
| Tela 01 | `Captura de tela 2026-10-26 053540.png` | ![PT-BR 01](images/interfaces/pt_BR/Captura%20de%20tela%202026-10-26%20053540.png) |
| Tela 02 | `Captura de tela 2026-10-26 053550.png` | ![PT-BR 02](images/interfaces/pt_BR/Captura%20de%20tela%202026-10-26%20053550.png) |
| Tela 03 | `Captura de tela 2026-10-26 053600.png` | ![PT-BR 03](images/interfaces/pt_BR/Captura%20de%20tela%202026-10-26%20053600.png) |
| Tela 04 | `Captura de tela 2026-10-26 053613.png` | ![PT-BR 04](images/interfaces/pt_BR/Captura%20de%20tela%202026-10-26%20053613.png) |
| Tela 05 | `Captura de tela 2026-10-26 053623.png` | ![PT-BR 05](images/interfaces/pt_BR/Captura%20de%20tela%202026-10-26%20053623.png) |
| Tela 06 | `Captura de tela 2026-10-26 053633.png` | ![PT-BR 06](images/interfaces/pt_BR/Captura%20de%20tela%202026-10-26%20053633.png) |
| Tela 07 | `Captura de tela 2026-10-26 053644.png` | ![PT-BR 07](images/interfaces/pt_BR/Captura%20de%20tela%202026-10-26%20053644.png) |
| Tela 08 | `Captura de tela 2026-10-26 053653.png` | ![PT-BR 08](images/interfaces/pt_BR/Captura%20de%20tela%202026-10-26%20053653.png) |
| Tela 09 | `Captura de tela 2026-10-26 053705.png` | ![PT-BR 09](images/interfaces/pt_BR/Captura%20de%20tela%202026-10-26%20053705.png) |
| Tela 10 | `Captura de tela 2026-10-26 053725.png` | ![PT-BR 10](images/interfaces/pt_BR/Captura%20de%20tela%202026-10-26%20053725.png) |
| Tela 11 | `Captura de tela 2026-10-26 053738.png` | ![PT-BR 11](images/interfaces/pt_BR/Captura%20de%20tela%202026-10-26%20053738.png) |
| Tela 12 | `Captura de tela 2026-10-26 053749.png` | ![PT-BR 12](images/interfaces/pt_BR/Captura%20de%20tela%202026-10-26%20053749.png) |

#### 3.2. Capturas em Inglês (`images/interfaces/en_US/`)

| Visão | Arquivo | Pré-visualização |
|---|---|---|
| Screen 01 | `Captura de tela 2026-10-26 053905.png` | ![EN-US 01](images/interfaces/en_US/Captura%20de%20tela%202026-10-26%20053905.png) |
| Screen 02 | `Captura de tela 2026-10-26 053915.png` | ![EN-US 02](images/interfaces/en_US/Captura%20de%20tela%202026-10-26%20053915.png) |
| Screen 03 | `Captura de tela 2026-10-26 053927.png` | ![EN-US 03](images/interfaces/en_US/Captura%20de%20tela%202026-10-26%20053927.png) |
| Screen 04 | `Captura de tela 2026-10-26 053938.png` | ![EN-US 04](images/interfaces/en_US/Captura%20de%20tela%202026-10-26%20053938.png) |
| Screen 05 | `Captura de tela 2026-10-26 053947.png` | ![EN-US 05](images/interfaces/en_US/Captura%20de%20tela%202026-10-26%20053947.png) |
| Screen 06 | `Captura de tela 2026-10-26 053957.png` | ![EN-US 06](images/interfaces/en_US/Captura%20de%20tela%202026-10-26%20053957.png) |
| Screen 07 | `Captura de tela 2026-10-26 054008.png` | ![EN-US 07](images/interfaces/en_US/Captura%20de%20tela%202026-10-26%20054008.png) |
| Screen 08 | `Captura de tela 2026-10-26 054025.png` | ![EN-US 08](images/interfaces/en_US/Captura%20de%20tela%202026-10-26%20054025.png) |
| Screen 09 | `Captura de tela 2026-10-26 054224.png` | ![EN-US 09](images/interfaces/en_US/Captura%20de%20tela%202026-10-26%20054224.png) |
| Screen 10 | `Captura de tela 2026-10-26 054233.png` | ![EN-US 10](images/interfaces/en_US/Captura%20de%20tela%202026-10-26%20054233.png) |
| Screen 11 | `Captura de tela 2026-10-26 054243.png` | ![EN-US 11](images/interfaces/en_US/Captura%20de%20tela%202026-10-26%20054243.png) |
| Screen 12 | `Captura de tela 2026-10-26 054253.png` | ![EN-US 12](images/interfaces/en_US/Captura%20de%20tela%202026-10-26%20054253.png) |

---

### 4. Independência da Microsoft Store, Privacidade Local-First e Suporte

- Este repositório e seu binário mantêm operação **100% Local-First** sem coleta de telemetria, conforme detalhado em [`docs/privacy-policy.md`](docs/privacy-policy.md).
- A **Microsoft Corporation** atua apenas como vitrine e canal de distribuição do pacote oficial assinado, **não assumindo suporte técnico, garantias ou responsabilidade pelos arquivos [`source/assets/`](source/assets) nem pelos cálculos de dosagem**. Suporte técnico voluntário deve ser encaminhado exclusivamente ao autor ([`linceu_lighthouse@outlook.com`](mailto:linceu_lighthouse@outlook.com)).

</details>

---

<a id="enus"></a>
<details>
<summary><h2>🇺🇸 English (United States)</h2></summary>

**Version:** `2026.11.22.0`  
**Date:** `November 22, 2026`  
**Author:** Fernando Nillsson Cidade ([`linceu_lighthouse@outlook.com`](mailto:linceu_lighthouse@outlook.com))  

---

### 1. Important Notice on Scope, Software Maturity & Warranty Disclaimer

> **Evolving Free Auxiliary Engineering Program ("AS IS"):**  
> This project is a **free, open-source auxiliary engineering tool** designed to support experimental Portland cement concrete mix design under the **IPT/EPUSP Method** (Helene & Terzian, 1992). **The software is NOT yet mature enough for definitive production deployment or commercial structural execution.** Due to potential modeling limitations, human mistakes, or source-code defects (spanning both applied mathematics and underlying materials technology), all calculated outputs **must be checked by hand, cross-verified against established commercial software, and confirmed via physical laboratory cylinder testing (ABNT NBR 5738 / NBR 5739)**. Any sole or unverified use in production occurs entirely at the user's own risk ([`docs/licensing.md`](docs/licensing.md#enus) and [`EULA_en_US.txt`](source/assets/EULA/EULA_en_US.txt)).

---

### 2. Methodological Origin, UFPR Spreadsheet Audit (TC031) & C++23 Refinements

**Concrete Mix Designer** evolved from an algorithmic audit and mathematical reconstruction of the classic **UFPR TC031 Civil Construction Materials Spreadsheet**, grounded in *"Manual de Dosagem e Controle do Concreto"* (Paulo Helene & Paulo Terzian, IPT/EPUSP, 1992).

During migration to **C++23 + Qt 6.12.0**, historical limitations of static spreadsheets were resolved:
1. **Granular Packing & Measuring Box Calibration:** Accurate binary coarse-aggregate optimization (`AggregateOptimizer`, $15.04\text{ L}$ vessel per NBR NM 45) and standardized jobsite measuring boxes ($35 \times 45\text{ cm}$ base for $50\text{ kg}$ cement bags).
2. **Robust Least-Squares Regressions (`RegressionEngine`):** Gaussian partial-pivoting fits for Abrams, Lyse, and Molinari laws coupled with theoretical specific-gravity cement consumption ($C = 1000 / V_s$).
3. **In-Memory SQLite Normative Isolation (`NormativeDatabase`):** Thread-safe volatile `:memory:` SQLite database protected by `QRecursiveMutex`, applying **ABNT NBR 12655:2022**, **NBR 6118:2023**, **NBR 7211:2022**, **NBR 7212:2025**, **NBR 8953:2015**, and **NBR 16697:2018**.
4. **Modern Multi-Toolchain Support:** Full support for **MSVC 14.52** (Windows SDK `10.0.28000.0`) and **MinGW-w64 GCC 15.1.0**, ultra-clean `app-local` distributions (**without redundant `vc_redist.x64.exe` bundling**), and Inno Setup scripts under `library/Concrete_Mix_Designer*.iss`.

---

### 3. Visual Interface Gallery (`images/interfaces/`)

#### 3.1. Portuguese (BR) Screenshots (`images/interfaces/pt_BR/`)

| View | File | Preview |
|---|---|---|
| Screen 01 | `Captura de tela 2026-10-26 053540.png` | ![PT-BR 01](images/interfaces/pt_BR/Captura%20de%20tela%202026-10-26%20053540.png) |
| Screen 02 | `Captura de tela 2026-10-26 053550.png` | ![PT-BR 02](images/interfaces/pt_BR/Captura%20de%20tela%202026-10-26%20053550.png) |
| Screen 03 | `Captura de tela 2026-10-26 053600.png` | ![PT-BR 03](images/interfaces/pt_BR/Captura%20de%20tela%202026-10-26%20053600.png) |
| Screen 04 | `Captura de tela 2026-10-26 053613.png` | ![PT-BR 04](images/interfaces/pt_BR/Captura%20de%20tela%202026-10-26%20053613.png) |
| Screen 05 | `Captura de tela 2026-10-26 053623.png` | ![PT-BR 05](images/interfaces/pt_BR/Captura%20de%20tela%202026-10-26%20053623.png) |
| Screen 06 | `Captura de tela 2026-10-26 053633.png` | ![PT-BR 06](images/interfaces/pt_BR/Captura%20de%20tela%202026-10-26%20053633.png) |
| Screen 07 | `Captura de tela 2026-10-26 053644.png` | ![PT-BR 07](images/interfaces/pt_BR/Captura%20de%20tela%202026-10-26%20053644.png) |
| Screen 08 | `Captura de tela 2026-10-26 053653.png` | ![PT-BR 08](images/interfaces/pt_BR/Captura%20de%20tela%202026-10-26%20053653.png) |
| Screen 09 | `Captura de tela 2026-10-26 053705.png` | ![PT-BR 09](images/interfaces/pt_BR/Captura%20de%20tela%202026-10-26%20053705.png) |
| Screen 10 | `Captura de tela 2026-10-26 053725.png` | ![PT-BR 10](images/interfaces/pt_BR/Captura%20de%20tela%202026-10-26%20053725.png) |
| Screen 11 | `Captura de tela 2026-10-26 053738.png` | ![PT-BR 11](images/interfaces/pt_BR/Captura%20de%20tela%202026-10-26%20053738.png) |
| Screen 12 | `Captura de tela 2026-10-26 053749.png` | ![PT-BR 12](images/interfaces/pt_BR/Captura%20de%20tela%202026-10-26%20053749.png) |

#### 3.2. English (US) Screenshots (`images/interfaces/en_US/`)

| View | File | Preview |
|---|---|---|
| Screen 01 | `Captura de tela 2026-10-26 053905.png` | ![EN-US 01](images/interfaces/en_US/Captura%20de%20tela%202026-10-26%20053905.png) |
| Screen 02 | `Captura de tela 2026-10-26 053915.png` | ![EN-US 02](images/interfaces/en_US/Captura%20de%20tela%202026-10-26%20053915.png) |
| Screen 03 | `Captura de tela 2026-10-26 053927.png` | ![EN-US 03](images/interfaces/en_US/Captura%20de%20tela%202026-10-26%20053927.png) |
| Screen 04 | `Captura de tela 2026-10-26 053938.png` | ![EN-US 04](images/interfaces/en_US/Captura%20de%20tela%202026-10-26%20053938.png) |
| Screen 05 | `Captura de tela 2026-10-26 053947.png` | ![EN-US 05](images/interfaces/en_US/Captura%20de%20tela%202026-10-26%20053947.png) |
| Screen 06 | `Captura de tela 2026-10-26 053957.png` | ![EN-US 06](images/interfaces/en_US/Captura%20de%20tela%202026-10-26%20053957.png) |
| Screen 07 | `Captura de tela 2026-10-26 054008.png` | ![EN-US 07](images/interfaces/en_US/Captura%20de%20tela%202026-10-26%20054008.png) |
| Screen 08 | `Captura de tela 2026-10-26 054025.png` | ![EN-US 08](images/interfaces/en_US/Captura%20de%20tela%202026-10-26%20054025.png) |
| Screen 09 | `Captura de tela 2026-10-26 054224.png` | ![EN-US 09](images/interfaces/en_US/Captura%20de%20tela%202026-10-26%20054224.png) |
| Screen 10 | `Captura de tela 2026-10-26 054233.png` | ![EN-US 10](images/interfaces/en_US/Captura%20de%20tela%202026-10-26%20054233.png) |
| Screen 11 | `Captura de tela 2026-10-26 054243.png` | ![EN-US 11](images/interfaces/en_US/Captura%20de%20tela%202026-10-26%20054243.png) |
| Screen 12 | `Captura de tela 2026-10-26 054253.png` | ![EN-US 12](images/interfaces/en_US/Captura%20de%20tela%202026-10-26%20054253.png) |

---

### 4. Microsoft Store Independence, Local-First Privacy & Support

- This application operates strictly **100% Local-First** with zero telemetry ([`docs/privacy-policy.md`](docs/privacy-policy.md#enus)).
- **Microsoft Corporation** serves strictly as the official Store distribution channel and **assumes no technical support obligations, warranties, or responsibility for [`source/assets/`](source/assets) texts or mix-design outputs**. Voluntary technical support inquiries should be directed solely to the author ([`linceu_lighthouse@outlook.com`](mailto:linceu_lighthouse@outlook.com)).

</details>

<details>
<summary><h2>📌 File Tree / Árvore de Arquivos</h2></summary>

**Root Path:** `Concrete_Mix_Designer_cpp`

```
OBSERVACAO.md
SECURITY.md
README.md
MANUAL.md
main.cpp
LICENSE
CONTRIBUTING.md
CODE_OF_CONDUCT.md
CMakeLists.txt
.gitignore
.gitattributes
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
│   ├── 📁 MANUAL
│   │   ├── 📄 MANUAL_en_US.txt
│   │   └── 📄 MANUAL_pt_BR.txt
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
├── 📁 utils
│   ├── ⚡ ApplicationPathUtils.cpp
│   ├── ⚡ ApplicationPathUtils.hpp
│   ├── ⚡ CaminhoPersistenteUtils.cpp
│   ├── ⚡ CaminhoPersistenteUtils.hpp
│   ├── ⚡ LogManager.cpp
│   └── ⚡ LogManager.hpp
└── 📄 windows_app_icon.rc.in
```

**Root Path:** `Concrete_Mix_Designer_cpp\tests`

```
├── 📄 CMakeLists.txt
├── ⚡ TestFramework.hpp
├── ⚡ test_CliIntegration.cpp
├── ⚡ test_GuiWorkflowsAndCharts.cpp
├── ⚡ test_I18nUtilsAndPersistence.cpp
├── ⚡ test_MathematicalCoreAndOptimizers.cpp
├── ⚡ test_MocksAndAuditTools.cpp
├── ⚡ test_ProjectDataSerializationAndReports.cpp
├── ⚡ test_RuntimePluginsAndFrameworks.cpp
├── ⚡ test_StandardsAndNormativeDB.cpp
└── ⚡ verify_calculations.cpp
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

**Root Path:** `Concrete_Mix_Designer_cpp\images\interfaces`

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
├── 📝 privacy-policy.md
├── 📝 release-v2026.11.22.0.md
├── 📝 security-model.md
├── 📝 suggested-issues.md
├── 📝 technical-audit-and-roadmap.md
└── 📝 user-guide.md
```

**Root Path:** `Concrete_Mix_Designer_cpp\.github`

```
└── 📁 workflows
    └── ⚙️ update-readme-version.yml
```

</details>
