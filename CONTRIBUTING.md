# Guia de Contribuição / Contributing Guide — Concrete Mix Designer

<p align="center">
  <b>Selecione o idioma / Select language:</b><br>
  <a href="#ptbr">🇧🇷 Português (BR)</a> | <a href="#enus">🇺🇸 English (US)</a>
</p>

---

## <a id="ptbr"></a>🇧🇷 Português (BR)

<details open>
<summary>Clique para expandir / recolher as instruções em português</summary><br>

# Contribuindo com o Concrete Mix Designer

**Versão:** `2026.11.22.0`  
**Data:** `22 de novembro de 2026`  
**Autor / Mantenedor:** Fernando Nillsson Cidade (`linceu_lighthouse@outlook.com`)  

Agradecemos profundamente seu interesse em contribuir com o **Concrete Mix Designer**! Sua colaboração auxilia no avanço de ferramentas livres, transparentes e cientificamente rigorosas para dosagem experimental e analítica de concreto pelo **Método IPT/EPUSP**.

> [!NOTE]
> **Sobre o Desenvolvimento, Suporte e Contribuições Externas:**  
> Este projeto é concebido, desenvolvido e mantido primariamente como uma iniciativa pessoal e educacional pelo autor, que está em processo contínuo de aprendizado em engenharia civil computacional e C++.  
> O envio de sugestões, abertura de *Issues* e submissão de *Pull Requests (PRs)* são sempre bem-vindos! Ressalta-se que a análise de código, interação em tópicos matemáticos/normativos e incorporação de contribuições são realizadas no próprio ritmo do autor e de forma estritamente voluntária (*best-effort*). Conforme estabelecido pela licença **GNU AGPLv3**, você possui total liberdade para criar um *Fork* independente deste repositório e evoluir suas próprias ideias e versões (inclusive liberdade total para alterar, remover ou adaptar os arquivos em `source/assets/`).  
> Justamente por ser um aplicativo gratuito ainda não maduro para produção ou uso profissional definitivo — sujeito a limitações, falhas humanas e possíveis erros no código-fonte, tanto na matemática aplicada quanto na física dos materiais —, contribuições voltadas à auditoria numérica e comparação laboratorial com ferramentas consolidadas são especialmente valiosas.

---

## 1. Código de Conduta

Ao participar deste projeto, você concorda em cumprir nosso [Código de Conduta](CODE_OF_CONDUCT.md). Trate todos os colaboradores com respeito, urbanidade e profissionalismo.

---

## 2. Pré-requisitos e Ambiente de Desenvolvimento (C++23 + Qt 6.12.0)

- **Sistema Operacional:** Windows 10 (1809+) / Windows 11 x64
- **Compiladores Dual:** **MSVC x64** (`14.52.37025`, VS 18 + Windows SDK `10.0.28000.0`) ou **MinGW-w64 x64** (GCC `15.1.0`) com **C++23** habilitado
- **Qt:** **Qt 6.12.0** (`Core`, `Gui`, `Widgets`, `Charts`, `Sql`, `Test`, `OpenGL`, `OpenGLWidgets`, `LinguistTools`)
- **Build System:** CMake `3.28+` (`4.5.1`) e Ninja `1.13.2`

### Compilando Aplicativo, Mocks e Suíte de Testes

```powershell
# Compilar aplicativo principal + CLI (MinGW ou MSVC)
cmd.exe /c "cmake\build_mingw.bat"
cmd.exe /c "cmake\build_msvc.bat"

# Compilar e executar TODA a suíte modular de testes (9 executáveis CTest)
cmd.exe /c "cmake\build_suite_mingw.bat"
ctest --test-dir build\build_suite_mingw --output-on-failure

cmd.exe /c "cmake\build_suite_msvc.bat"
ctest --test-dir build\build_suite_msvc --output-on-failure

# Auditar versões e compilar traduções (.ts -> .qm)
cmd.exe /c "cmake\build_mocks_msvc.bat"
.\build\build_Mocks_msvc\find_VersionEditor.exe --check .
```

---

## 3. Diretrizes Arquiteturais e Internacionalização

1. **Separação de Camadas:** `source/core/` (`ConcreteMixCoreLib`, apenas `Qt6::Core` e `Qt6::Sql`) nunca deve depender de `QtWidgets` ou `QtCharts`; toda interface visual e diagramas residem em `source/gui/` (`ConcreteMixGuiLib`) e a CLI headless em `source/cli/`.
2. **Privacidade 100% Local-First (Política 10.5 Microsoft Store / LGPD / GDPR):** É proibido adicionar telemetria, coleta de dados pessoais ou conexões de rede.
3. **Tradução Dinâmica Obrigatória (`pt_BR` $\leftrightarrow$ `en_US`):** Toda nova string de UI deve usar `tr("...")`, atualizar `retranslateUi()`, e manter `source/language/locale/*.ts` e `.qm` com **0 chaves `unfinished`**.
4. **Duplo Licenciamento e `source/assets/`:** Contribuições ao código público ficam sob **GNU AGPLv3**, respeitando a fronteira com a edição gratuita oficial da Microsoft Store (`EULA` / `CLC`) e mantendo liberdade total para desenvolvedores sobre `source/assets/` em seus forks ([`docs/licensing.md`](docs/licensing.md)).

</details>

---

## <a id="enus"></a>🇺🇸 English (US)

<details open>
<summary>Click to expand / collapse instructions in English</summary><br>

# Contributing to Concrete Mix Designer

**Version:** `2026.11.22.0`  
**Date:** `November 22, 2026`  
**Author / Maintainer:** Fernando Nillsson Cidade (`linceu_lighthouse@outlook.com`)  

Thank you for your interest in contributing to **Concrete Mix Designer**! Your support helps advance open, transparent, and mathematically rigorous tools for IPT/EPUSP experimental and analytical concrete mix design.

> [!NOTE]
> **Development Context, Support & External Contributions:**  
> This project is designed, developed, and maintained primarily as a personal and educational initiative by the author, who is continuously learning computational civil engineering and modern C++.  
> Suggestions, *Issues*, and *Pull Requests (PRs)* are always welcome! Code review, discussions, and merges take place at the author's own pace on a strictly voluntary *best-effort* basis. Under **GNU AGPLv3**, you hold complete freedom to fork this repository and advance your own versions (including full freedom to modify, remove, or adapt files under `source/assets/`).  
> Precisely because this free application is not yet mature enough for definitive production or professional use — remaining subject to limitations, human flaws, and possible defects in source code covering applied mathematics and concrete physics — contributions focused on numerical auditing and laboratory validation against established tools are especially valuable.

---

## 1. Code of Conduct

By participating in this project, you agree to abide by our [Code of Conduct](CODE_OF_CONDUCT.md). Treat all contributors with respect and professionalism.

---

## 2. Prerequisites & Development Environment (C++23 + Qt 6.12.0)

- **Operating System:** Windows 10 (1809+) / Windows 11 x64
- **Dual Compilers:** **MSVC x64** (`14.52.37025`, VS 18 + Windows SDK `10.0.28000.0`) or **MinGW-w64 x64** (GCC `15.1.0`) with **C++23** enabled
- **Qt:** **Qt 6.12.0** (`Core`, `Gui`, `Widgets`, `Charts`, `Sql`, `Test`, `OpenGL`, `OpenGLWidgets`, `LinguistTools`)
- **Build System:** CMake `3.28+` (`4.5.1`) and Ninja `1.13.2`

### Building App, Mocks, and Test Suite

```powershell
# Build main app + CLI (MinGW or MSVC)
cmd.exe /c "cmake\build_mingw.bat"
cmd.exe /c "cmake\build_msvc.bat"

# Build and run the ENTIRE modular test suite (9 CTest executables)
cmd.exe /c "cmake\build_suite_mingw.bat"
ctest --test-dir build\build_suite_mingw --output-on-failure

cmd.exe /c "cmake\build_suite_msvc.bat"
ctest --test-dir build\build_suite_msvc --output-on-failure

# Audit version/date metadata and compile translations (.ts -> .qm)
cmd.exe /c "cmake\build_mocks_msvc.bat"
.\build\build_Mocks_msvc\find_VersionEditor.exe --check .
```

---

## 3. Architectural & Internationalization Guidelines

1. **Layer Separation:** `source/core/` (`ConcreteMixCoreLib`, linked only to `Qt6::Core` and `Qt6::Sql`) must never depend on `QtWidgets` or `QtCharts`; all UI and chart plots live in `source/gui/` (`ConcreteMixGuiLib`) and the headless CLI lives in `source/cli/`.
2. **100% Local-First Privacy (Microsoft Store Policy 10.5 / LGPD / GDPR):** Introducing telemetry, personal data collection, or network calls is strictly prohibited.
3. **Mandatory Dynamic I18N (`pt_BR` $\leftrightarrow$ `en_US`):** Wrap every UI string in `tr("...")`, update `retranslateUi()`, and keep `source/language/locale/*.ts` / `.qm` with **0 `unfinished` keys**.
4. **Dual Licensing & `source/assets/`:** Public code contributions remain under **GNU AGPLv3**, preserving the boundary with the free official Microsoft Store package (`EULA` / `CLC`) while granting developers full freedom over `source/assets/` in their own forks ([`docs/licensing.md`](docs/licensing.md)).

</details>
