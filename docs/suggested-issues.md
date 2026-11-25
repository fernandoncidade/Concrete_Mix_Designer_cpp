# Sugestões de Issues e Roadmap / Suggested Issues & Roadmap — Concrete Mix Designer

<p align="center">
  <b>Selecione o idioma / Select language:</b><br>
  <a href="#ptbr">🇧🇷 Português (BR)</a> | <a href="#enus">🇺🇸 English (US)</a>
</p>

---

## <a id="ptbr"></a>🇧🇷 Português (BR)

<details open>
<summary>Clique para expandir / recolher sugestões em português</summary><br>

# Concrete Mix Designer — Backlog Técnico e Sugestões para Contribuidores

**Versão:** `2026.11.22.0`  
**Data:** `22 de novembro de 2026`  
**Autor:** Fernando Nillsson Cidade  

Este documento reúne tópicos técnicos, extensões normativas e melhorias de engenharia mapeadas para orientar colaboradores que desejam contribuir com o **Concrete Mix Designer**. Por se tratar de um aplicativo gratuito de uso auxiliar ainda não maduro para produção ou uso profissional definitivo (sujeito a limitações, falhas humanas e eventuais erros no código-fonte, matemática aplicada e física/tecnologia do concreto), contribuições focadas em auditoria numérica e comparação laboratorial são especialmente bem-vindas.

---

### Issue 1: Curvas Granulométricas Completas por Peneiramento (ABNT NBR 17054 / NBR NM 248 / NBR 7211:2022)
- **Área:** `core` / `gui`
- **Dificuldade:** Média
- **Descrição:** Adicionar gráfico semilogarítmico interativo em Qt Charts para as zonas utilizável e ótima de agregado miúdo e faixas de agregados graúdos (Brita 0, 1, 2, 3), calculando automaticamente o Módulo de Finura ($MF$) e a Dimensão Máxima Característica ($D_{\max}$) a partir das porcentagens retidas acumuladas.
- **Benefício:** Elimina entrada manual prévia de $D_{\max}$ e oferece diagnóstico visual de descontinuidade granulométrica.

---

### Issue 2: Aditivos Plastificantes/Superplastificantes (ABNT NBR 11768) e Relação $a/\text{agl}$ para Adições Pozolânicas
- **Área:** `core` / `normas`
- **Dificuldade:** Avançada
- **Descrição:** Estender `MixDesignCalculator` e `NormativeDatabase` para dosagem de aditivos químicos ($\%$ sobre massa de cimento) com desconto automático do volume líquido de aditivo na água efetiva, além do suporte explícito a adições minerais (sílica ativa, metacaulim, cinza volante, escória) no fator água/aglomerante equivalente ($a/\text{agl}$).
- **Benefício:** Suporte abrangente a concretos de alto desempenho ($f_{ck} > 50\text{ MPa}$) e concretos duráveis de baixa pegada de carbono.

---

### Issue 3: Persistência Nativa de Estudos `.cmdproj` via Menu Arquivo (`Ctrl+O` / `Ctrl+S` / `Ctrl+Shift+S`)
- **Área:** `gui` / `io`
- **Dificuldade:** Baixa
- **Descrição:** Expor na barra de menus (`Arquivo` em `MainWindow`) ações diretas para abrir e salvar arquivos JSON `.cmdproj` aproveitando os métodos já validados [`ProjectData::toJson()`](../source/core/ProjectData.hpp) e [`ProjectData::fromJson()`](../source/core/ProjectData.hpp).
- **Benefício:** Interoperabilidade direta entre sessões da GUI e a CLI `concrete-mix-designer-cli.exe` (`calculate`, `verify`, `report`).

---

### Issue 4: Controle Estatístico de Resistência à Compressão (Cartas de Controle $f_{ck,\text{est}}$ — ABNT NBR 12655:2022)
- **Área:** `core` / `charts`
- **Dificuldade:** Avançada
- **Descrição:** Adicionar aba ou módulo de acompanhamento de amostragem parcial/total ($n < 20$ e $n \ge 20$ exemplares) calculando $f_{ck,\text{est}}$ conforme o item 6.2.3 da ABNT NBR 12655:2022 e realimentando o desvio-padrão conhecido ($S_d$) no projeto.
- **Benefício:** Integração didática entre o estudo prévio de dosagem em laboratório e o controle estatístico de recebimento em obra.

</details>

---

## <a id="enus"></a>🇺🇸 English (US)

<details open>
<summary>Click to expand / collapse suggested issues in English</summary><br>

# Concrete Mix Designer — Technical Backlog & Contributor Suggestions

**Version:** `2026.11.22.0`  
**Date:** `November 22, 2026`  
**Author:** Fernando Nillsson Cidade  

This document outlines technical enhancements, normative extensions, and engineering improvements mapped to guide contributors wishing to collaborate on **Concrete Mix Designer**. Because it is a free auxiliary tool that is not yet mature enough for definitive production or professional use (remaining subject to limitations, human flaws, and possible defects in source code, applied mathematics, and concrete materials physics), contributions focused on numerical auditing and laboratory cross-checking are especially welcome.

---

### Issue 1: Full Sieve Analysis Grading Curves (ABNT NBR 17054 / NBR NM 248 / NBR 7211:2022)
- **Area:** `core` / `gui`
- **Difficulty:** Medium
- **Description:** Add an interactive semi-logarithmic Qt Charts view showing usable and optimal fine-aggregate zones alongside coarse-aggregate grading envelopes (Grades 0, 1, 2, 3), automatically computing Fineness Modulus ($FM$) and Maximum Characteristic Size ($D_{\max}$) from cumulative retained mass percentages.
- **Benefit:** Eliminates prior manual entry of $D_{\max}$ and provides visual diagnostics of gap-graded aggregate blends.

---

### Issue 2: Chemical Admixtures (ABNT NBR 11768) & Equivalent Water/Binder Ratio ($w/b$) for Pozzolanic Additions
- **Area:** `core` / `standards`
- **Difficulty:** Advanced
- **Description:** Extend `MixDesignCalculator` and `NormativeDatabase` to support plasticizer/superplasticizer dosage ($\%$ by cement mass) with automatic liquid-volume water deduction, alongside supplementary cementitious materials (silica fume, metakaolin, fly ash, slag) in $w/b$ calculations.
- **Benefit:** Comprehensive support for high-performance concretes ($f_{ck} > 50\text{ MPa}$) and low-clinker durable mixes.

---

### Issue 3: Direct `.cmdproj` Study File Open/Save Actions via the File Menu (`Ctrl+O` / `Ctrl+S` / `Ctrl+Shift+S`)
- **Area:** `gui` / `io`
- **Difficulty:** Low
- **Description:** Expose `Open...`, `Save...`, and `Save As...` actions inside the `File` menu in `MainWindow` backed by the validated [`ProjectData::toJson()`](../source/core/ProjectData.hpp) and [`ProjectData::fromJson()`](../source/core/ProjectData.hpp) methods.
- **Benefit:** Seamless workflow sharing between GUI studies and `concrete-mix-designer-cli.exe` (`calculate`, `verify`, `report`).

---

### Issue 4: Statistical Compressive Strength Control Charts ($f_{ck,\text{est}}$ — ABNT NBR 12655:2022)
- **Area:** `core` / `charts`
- **Difficulty:** Advanced
- **Description:** Implement partial/total batching lot sampling ($n < 20$ and $n \ge 20$ specimens) computing $f_{ck,\text{est}}$ per Section 6.2.3 of ABNT NBR 12655:2022 and feeding historical $S_d$ back into dosage updates.
- **Benefit:** Educational bridge connecting prior laboratory mix design with field statistical conformity control.

</details>
