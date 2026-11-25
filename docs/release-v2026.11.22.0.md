# Release Notes — Concrete Mix Designer v2026.11.22.0

<p align="center">
  <b>Selecione o idioma / Select language:</b><br>
  <a href="#ptbr">🇧🇷 Português (BR)</a> | <a href="#enus">🇺🇸 English (US)</a>
</p>

---

## <a id="ptbr"></a>🇧🇷 Português (BR)

<details open>
<summary>Clique para expandir / recolher notas de lançamento em português</summary><br>

# Concrete Mix Designer — Versão 2026.11.22.0 (C++23, Qt 6.12.0, Duplo Licenciamento, Distribuições Limpas & Suíte Completa de Testes)

**Data de Lançamento:** 22 de novembro de 2026  
**Versão:** `2026.11.22.0`  
**Autor:** Fernando Nillsson Cidade (`linceu_lighthouse@outlook.com`)  
**Repositório:** `fernandoncidade/Concrete_Mix_Designer_cpp`  
**Licenciamento:** GNU AGPLv3 (Código Aberto) / EULA Oficial do Autor (Microsoft Store)  

---

## 1. Artefatos e Verificação de Integridade

Valide a integridade dos binários compilados localmente antes da execução utilizando `Get-FileHash`:

```powershell
Get-FileHash -Path ".\build\build_concrete_mix_design_msvc\ConcreteMixDesign.exe" -Algorithm SHA256
Get-FileHash -Path ".\build\build_concrete_mix_design_msvc\concrete-mix-designer-cli.exe" -Algorithm SHA256
```

| Artefato | Formato | Arquitetura / Toolchain | Descrição |
|---|---|---|---|
| `ConcreteMixDesign.exe` | Executável Nativo Windows (GUI) | x64 (`MSVC 14.52` / `MinGW GCC 15.1.0`) | Plataforma desktop **C++23 + Qt 6.12.0** para dosagem experimental pelo Método IPT/EPUSP (7 abas + 5º diagrama unificado IPT). |
| `concrete-mix-designer-cli.exe` | Utilitário Console Autônomo (CLI) | x64 (`MSVC 14.52` / `MinGW GCC 15.1.0`) | CLI headless para automação de traços, fechamento volumétrico, auditoria ABNT NBR e exportação de relatórios. |
| `Concrete_Mix_Designer_{mingw,msvc}_v2026.11.22.0.exe` | Instalador Inno Setup 7 | x64 (`app-local`) | Instaladores enxutos sem empacotamento redundante de `vc_redist.x64.exe` (`13,89 MB` MinGW / `11,91 MB` MSVC), embarcando `46`/`48` arquivos com `assets/{CLC,COPYRIGHT,LICENSES}`. |
| `Source code (.zip / .tar.gz)` | Código-Fonte Completo | C++23 / Qt 6.12.0 | Repositório integral sob licença copyleft **GNU AGPLv3**. |

---

## 2. Destaques e Entregas da Versão `2026.11.22.0`

- **Migração Integral para C++23, Qt 6.12.0, MSVC 14.52 & MinGW-w64 GCC 15.1.0:**
  Atualização de `CMakeLists.txt`, `CMakePresets.json`, `mocks/CMakeLists.txt`, scripts `.bat`/`.ps1` em `cmake/`, manifesto [`library/AppxManifest.xml`](../library/AppxManifest.xml) (`Windows SDK 10.0.28000.0`) e instaladores [`library/Concrete_Mix_Designer*.iss`](../library/Concrete_Mix_Designer.iss), com implantação *app-local* enxuta para as 5 DLLs `Microsoft.VC145.CRT` no MSVC e as 3 DLLs do runtime GCC 15.1.0 no MinGW, mantendo os instaladores livres de `vc_redist.x64.exe`.
- **Janela `Sobre` Expandida para 9 Abas (`CLC`, `Copyright` e `Licenças Completas`), Manual e I18N Completo:**
  Ampliação de [`SobreDialog`](../source/gui/SobreDialog.cpp) com abas para `CLC / Licenciamento`, `Copyright` e `Licenças Completas` (seletor `QComboBox` com as 11 licenças completas de terceiros em `source/assets/LICENSES`), expansão do [`ManualDialog`](../source/gui/ManualDialog.cpp) para 14 seções com `tr(...)` completo, compilação de `342` chaves bilíngues `.qm` (`0 unfinished`) e empacotamento integral nas distribuições limpas.
- **Segurança, Isolamento em Memória & Reorganização Estrutural (`tests/`, `library/`):**
  Adição de `escapeHtml()` nos metadados de relatório HTML5 em [`ProjectData.cpp`](../source/core/ProjectData.cpp), operação exclusiva `:memory:` em [`NormativeDatabase.cpp`](../source/core/NormativeDatabase.cpp), correção do caminho de ícone em [`ApplicationPathUtils.cpp`](../source/utils/ApplicationPathUtils.cpp), movimentação para `library/`, `images/interfaces/{pt_BR,en_US}/` e [`source/windows_app_icon.rc.in`](../source/windows_app_icon.rc.in), 6 tarefas padronizadas no VS Code e **suíte modular CTest de 9 alvos em `tests/`**.
- **Documentação Completa, Duplo Licenciamento & Motor IPT/EPUSP:**
  Alinhamento integral dos documentos da raiz (`README.md`, `MANUAL.md`, `OBSERVACAO.md`, `CONTRIBUTING.md`, `CODE_OF_CONDUCT.md`, `SECURITY.md`), `docs/` e `source/assets/`, preservando Cenário 5 UFPR TC031, verificações ABNT NBR, diagramas IPT, moeda dinâmica (`R$`/`$`), CLI e auditoria `find_VersionEditor.exe`.

</details>

---

## <a id="enus"></a>🇺🇸 English (US)

<details open>
<summary>Click to expand / collapse release notes in English</summary><br>

# Concrete Mix Designer — Version 2026.11.22.0 (C++23, Qt 6.12.0, Dual Licensing, Clean Distributions & Full Test Suite)

**Release Date:** November 22, 2026  
**Version:** `2026.11.22.0`  
**Author:** Fernando Nillsson Cidade (`linceu_lighthouse@outlook.com`)  
**Repository:** `fernandoncidade/Concrete_Mix_Designer_cpp`  
**Licensing:** GNU AGPLv3 (Open Source) / Author's Official EULA (Microsoft Store)  

---

## 1. Release Artifacts & Cryptographic Integrity Verification

Verify compiled artifact integrity locally before execution using `Get-FileHash`:

```powershell
Get-FileHash -Path ".\build\build_concrete_mix_design_msvc\ConcreteMixDesign.exe" -Algorithm SHA256
Get-FileHash -Path ".\build\build_concrete_mix_design_msvc\concrete-mix-designer-cli.exe" -Algorithm SHA256
```

| Artifact | Format | Architecture / Toolchain | Description |
|---|---|---|---|
| `ConcreteMixDesign.exe` | Native Windows GUI Executable | x64 (`MSVC 14.52` / `MinGW GCC 15.1.0`) | **C++23 + Qt 6.12.0** desktop platform for IPT/EPUSP experimental mix design (7 workspace tabs + 5th unified IPT chart). |
| `concrete-mix-designer-cli.exe` | Standalone Console Executable (CLI) | x64 (`MSVC 14.52` / `MinGW GCC 15.1.0`) | Headless CLI for automated mix proportioning, volumetric closure, ABNT NBR checks, and report export. |
| `Concrete_Mix_Designer_{mingw,msvc}_v2026.11.22.0.exe` | Inno Setup 7 Installer | x64 (`app-local`) | Ultra-clean installers without redundant `vc_redist.x64.exe` bundling (`13.89 MB` MinGW / `11.91 MB` MSVC), packaging `46`/`48` files including `assets/{CLC,COPYRIGHT,LICENSES}`. |
| `Source code (.zip / .tar.gz)` | Full Source Archive | C++23 / Qt 6.12.0 | Complete repository source under copyleft **GNU AGPLv3**. |

---

## 2. Highlights & Deliverables in Version `2026.11.22.0`

- **Full Migration to C++23, Qt 6.12.0, MSVC 14.52 & MinGW-w64 GCC 15.1.0:**
  Updated `CMakeLists.txt`, `CMakePresets.json`, `mocks/CMakeLists.txt`, `.bat`/`.ps1` scripts under `cmake/`, MSIX manifest [`library/AppxManifest.xml`](../library/AppxManifest.xml) (`Windows SDK 10.0.28000.0`), and [`library/Concrete_Mix_Designer*.iss`](../library/Concrete_Mix_Designer.iss), delivering ultra-clean *app-local* runtime deployments (5 `Microsoft.VC145.CRT` DLLs for MSVC and 3 GCC 15.1.0 runtime DLLs for MinGW, without bundling `vc_redist.x64.exe`).
- **Expanded 9-Tab `About` Dialog (`CLC`, `Copyright`, `Full Licenses`), Manual & Complete Runtime I18N:**
  Extended [`SobreDialog`](../source/gui/SobreDialog.cpp) with `CLC / Licensing`, `Copyright`, and `Full Licenses` (`QComboBox` selector for all 11 complete licenses under `source/assets/LICENSES`), expanded [`ManualDialog`](../source/gui/ManualDialog.cpp) to 14 sections with full `tr(...)` catalogs (`342` translated entries, `0 unfinished` in `.qm`), and included all new documents in the clean external distributions.
- **Security Hardening, In-Memory Isolation & Structural Reorganization (`tests/`, `library/`):**
  Added HTML metadata escaping (`escapeHtml()`) inside [`ProjectData.cpp`](../source/core/ProjectData.cpp), memory-only default mode (`:memory:`) in [`NormativeDatabase.cpp`](../source/core/NormativeDatabase.cpp), fixed application icon resolution in [`ApplicationPathUtils.cpp`](../source/utils/ApplicationPathUtils.cpp), organized `library/`, `images/interfaces/{pt_BR,en_US}/`, and [`source/windows_app_icon.rc.in`](../source/windows_app_icon.rc.in), streamlined VS Code tasks to 6 visible workflows, and implemented the **9-target CTest suite under `tests/`**.
- **Complete Documentation, Dual Licensing & IPT/EPUSP Engine:**
  Aligned root documentation (`README.md`, `MANUAL.md`, `OBSERVACAO.md`, `CONTRIBUTING.md`, `CODE_OF_CONDUCT.md`, `SECURITY.md`), `docs/`, and `source/assets/`, preserving audited UFPR TC031 accuracy (**Scenario 5**), ABNT NBR checks, IPT diagrams, live currency switching (`R$`/`$`), CLI automation, and `find_VersionEditor.exe` synchronization.

</details>
