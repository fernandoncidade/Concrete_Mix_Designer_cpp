# Guia do Usuário / User Guide — Concrete Mix Designer

<p align="center">
  <b>Selecione o idioma / Select language:</b><br>
  <a href="#ptbr">🇧🇷 Português (BR)</a> | <a href="#enus">🇺🇸 English (US)</a>
</p>

---

## <a id="ptbr"></a>🇧🇷 Português (BR)

<details open>
<summary>Clique para expandir / recolher o guia do usuário em português</summary><br>

# Concrete Mix Designer — Guia do Usuário e Operação Rápida

**Versão:** `2026.11.22.0`  
**Data:** `22 de novembro de 2026`  
**Autor:** Fernando Nillsson Cidade  

---

> [!WARNING]
> **Escopo Auxiliar Gratuito, Limitações de Maturidade e Responsabilidade Exclusiva ("AS IS"):** Por ser um aplicativo gratuito, você pode utilizar o **Concrete Mix Designer** conforme lhe convir como **ferramenta auxiliar** (educacional, acadêmica e comparativa), e **não** como ferramenta definitiva ou final. Este software **ainda não está maduro o suficiente** para ser levado para produção ou uso profissional definitivo (podendo conter limitações, falhas humanas e erros no código-fonte, matemática aplicada e física dos materiais), exigindo **conferência manual obrigatória, comparação em ferramentas consolidadas no mercado e ensaios físicos laboratoriais (ABNT NBR 5738 e NBR 5739)** sob responsabilidade de Engenheiro Civil habilitado (**ART/CREA**). Caso decida utilizá-lo exclusivamente em produção, todas as consequências serão de sua inteira e exclusiva responsabilidade ([`licensing.md`](licensing.md#ptbr) e [EULA Oficial](../source/assets/EULA/EULA_pt_BR.txt)).

---

## 1. Fluxo Rápido de Trabalho em 7 Passos

1. **Menu `Configurações` (`Ctrl+I`), Cenários e Submenu `Idioma`:**
   - Acesse **Configurações $\to$ 🌐 Idioma** para alternar instantaneamente interface, gráficos, relatórios e moedas (`R$` / `$`) entre `Português (BR)` e `English (US)`.
   - Escolha em **Configurações $\to$ 📂 Cenário da Planilha:** um dos 6 cenários pré-configurados (`Cenário 0` a `Cenário 5`, sendo o Cenário 5 a planilha auditada UFPR TC031).
   - Acesse **Configurações $\to$ 📝 Dados do Projeto e Identificação (`Ctrl+I`)** para definir **Responsável:**, **Instituição:**, **Matrícula / ID:** e escolher se **Projeto:** segue dinamicamente o cenário ativo ou usa texto manual.
2. **Aba 1 (`1. Parâmetros de Projeto`):** Informe $f_{ck}$, Condição de Preparo ($S_d$), CAA I–IV, Tipo Estrutural (CA/CP) e *Slump* (mm).
3. **Aba 2 (`2. Materiais & Britas`):** Cadastre massa específica ($\gamma$), massa unitária ($\delta$), umidade de campo ($h\%$) e custo unitário, além de otimizar opcionalmente a mistura binária Brita 1 + Brita 2.
4. **Aba 3 (`3. Teor de Argamassa (α)`):** Avalie a progressão laboratorial de $\alpha$ ($40\%$ a $68\%$) ou selecione o teor ótimo clicando diretamente na linha desejada da tabela.
5. **Aba 4 (`4. Traços & Diagrama IPT`):** Insira os 3 traços experimentais (Rico, Piloto, Pobre), clique em **Ajustar Curvas**, alterne entre os 4 quadrantes, zoom individual e o **5º Abrams/Lyse/Molinari (IPT)** e exporte em PNG/PDF de alta resolução.
6. **Aba 5 (`5. Traço Final & Amassadas`) & Aba 6 (`6. Análise de Custos`):** Consulte o traço seco/úmido corrigido, consumos por $1\text{ m}^3$, pesagem para betoneira, padiolas ($35 \times 45\text{ cm}$) e comparativo financeiro Analítico vs. Gráfico.
7. **Aba 7 (`7. Relatório Técnico`):** Gere o memorial bilíngue completo e exporte para Área de Transferência, Markdown (`.md`), HTML5 (`.html`) ou PDF A4 oficial (`.pdf`).
8. **Janela `Sobre` (`Ajuda` $\to$ `Sobre o Módulo de Dosagem de Concreto`):** Consulte as **9 abas informativas e legais** (`Detalhes`, `Histórico`, `Licença`, `CLC / Licenciamento`, `Copyright`, `Avisos`, `Privacidade`, `Notas de Lançamento` e `Licenças Completas`), utilizando a lista suspensa em `Licenças Completas` para ler qualquer uma das 11 licenças completas com pesquisa integrada e tradução instantânea.

---

## 2. Automação via Linha de Comando (`concrete-mix-designer-cli.exe`)

```powershell
# Dosagem automatizada por fck, abatimento e CAA
concrete-mix-designer-cli.exe design --fck 30 --slump 120 --caa II

# Exportação direta de memorial HTML bilíngue
concrete-mix-designer-cli.exe presets --run 5 --format json
concrete-mix-designer-cli.exe verify --fck 30 --caa III --ac 0.48 --cement-content 360
```

Para referência completa de todas as equações e parâmetros, consulte o [`MANUAL.md`](../MANUAL.md#parte-1-manual-de-instruções-português---br) ou pressione **`F1`** dentro do programa.

</details>

---

## <a id="enus"></a>🇺🇸 English (US)

<details open>
<summary>Click to expand / collapse user guide in English</summary><br>

# Concrete Mix Designer — User Guide & Quick Operations

**Version:** `2026.11.22.0`  
**Date:** `November 22, 2026`  
**Author:** Fernando Nillsson Cidade  

---

> [!WARNING]
> **Free Auxiliary Scope, Maturity Limitations, and Sole Responsibility ("AS IS"):** Because **Concrete Mix Designer** is a free application, you may use it as you see fit as an **auxiliary tool** (educational, academic, and comparative), and **not** as a definitive or final tool. This software **is not yet mature enough** to be taken to production or definitive professional use (remaining subject to limitations, human flaws, and possible defects in source code, applied mathematics, and concrete materials physics), strictly requiring **manual verification, cross-checking with more established market software, and physical laboratory trials (ABNT NBR 5738 and NBR 5739)** under a legally licensed Civil Engineer (**ART/PE**). Should you decide to rely on it exclusively in production, all consequences remain solely yours ([`licensing.md`](licensing.md#enus) and [Official EULA](../source/assets/EULA/EULA_en_US.txt)).

---

## 1. Quick 7-Step Workflow

1. **`Settings` Menu (`Ctrl+I`), Scenarios & `Language` Submenu:**
   - Open **Settings $\to$ 🌐 Language** to switch interface texts, charts, reports, and currency symbols (`$` / `R$`) live between `Português (BR)` and `English (US)`.
   - Pick any of the 6 built-in presets under **Settings $\to$ 📂 Spreadsheet Scenario:** (`Scenario 0` through `Scenario 5`, where Scenario 5 reproduces the audited UFPR TC031 study).
   - Open **Settings $\to$ 📝 Project Data and Identification (`Ctrl+I`)** to enter **Engineer:**, **Institution:**, **ID / Code:**, and toggle automatic scenario titles vs. manual project naming.
2. **Tab 1 (`1. Project Parameters`):** Enter $f_{ck}$, Preparation Condition ($S_d$), CAA I–IV, Structural Type (RC/PC), and target Slump (mm).
3. **Tab 2 (`2. Materials & Coarse Aggregates`):** Enter specific gravity ($\gamma$), loose bulk density ($\delta$), field moisture ($h\%$), and unit costs, plus optional binary Coarse Aggregate 1 + 2 packing optimization.
4. **Tab 3 (`3. Mortar Content (α)`):** Review the $\alpha$ laboratory progression ($40\%$ to $68\%$) or click any table row directly to apply the chosen dry mortar content.
5. **Tab 4 (`4. Mixes & IPT Diagram`):** Enter the 3 experimental trial mixes (Rich, Pilot, Lean), click **Fit Curves**, browse individual quadrants alongside the **5th Abrams/Lyse/Molinari (IPT)** chart, and export high-DPI PNG/PDF plots.
6. **Tab 5 (`5. Final Mix & Batches`) & Tab 6 (`6. Cost Analysis`):** Inspect dry/wet unit mixes, $1\text{ m}^3$ consumptions, lab mixer batching, volume-boxes (*padiolas*, $35 \times 45\text{ cm}$), and Analytical vs. Graphical cost differences.
7. **Tab 7 (`7. Technical Report`):** Generate the complete bilingual calculation report and export to Clipboard, Markdown (`.md`), HTML5 (`.html`), or A4 PDF (`.pdf`).
8. **`About` Window (`Help` $\to$ `About the Concrete Mix Design Module`):** Explore all **9 informational and legal tabs** (`Details`, `Changelog`, `License`, `CLC / Licensing`, `Copyright`, `Notices`, `Privacy`, `Release Notes`, and `Full Licenses`), using the dropdown selector inside `Full Licenses` to inspect any of the 11 complete licenses with shared search and live language switching.

---

## 2. Command Line Usage (`concrete-mix-designer-cli.exe`)

```powershell
# Automated design by fck, slump, and CAA
concrete-mix-designer-cli.exe design --fck 30 --slump 120 --caa II

# Run Scenario 5 as JSON or verify compliance
concrete-mix-designer-cli.exe presets --run 5 --format json
concrete-mix-designer-cli.exe verify --fck 30 --caa III --ac 0.48 --cement-content 360
```

For full mathematical equations and menu references, see [`MANUAL.md`](../MANUAL.md#part-2-operations-manual--civil-engineering-guide-english---us) or press **`F1`** inside the application.

</details>
