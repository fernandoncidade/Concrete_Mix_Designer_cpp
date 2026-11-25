# Conformidade Normativa e Legal / Normative & Legal Compliance — Concrete Mix Designer

<p align="center">
  <b>Selecione o idioma / Select language:</b><br>
  <a href="#ptbr">🇧🇷 Português (BR)</a> | <a href="#enus">🇺🇸 English (US)</a>
</p>

---

## <a id="ptbr"></a>🇧🇷 Português (BR)

<details open>
<summary>Clique para expandir / recolher o documento em português</summary><br>

# Concrete Mix Designer — Guia de Conformidade Normativa, Store e Legal

**Versão:** `2026.11.22.0`  
**Data:** `22 de novembro de 2026`  
**Autor:** Fernando Nillsson Cidade  

---

## 1. Conformidade com as Normas Técnicas Brasileiras (ABNT NBR)

O **Concrete Mix Designer** (`v2026.11.22.0`) incorpora em seu banco relacional em memória ([`NormativeDatabase`](../source/core/NormativeDatabase.cpp)) e no motor normativo ([`ConcreteStandards`](../source/core/ConcreteStandards.cpp)) verificações auxiliares baseadas nas normas técnicas brasileiras vigentes:

1. **ABNT NBR 12655:2022 (*Concreto de cimento Portland — Preparo, controle, recebimento e aceitação*):**
   - **Tabela 6 — Requisitos para o concreto em condições especiais de exposição:**
     - **CAA I (Fraca — Rural / Submersa):** $a/c \le 0{,}65$ (CA) / $0{,}60$ (CP); $f_{ck} \ge 20\text{ MPa}$ (CA) / $25\text{ MPa}$ (CP); $C_{\min} \ge 260\text{ kg/m}^3$ (CA) / $280\text{ kg/m}^3$ (CP).
     - **CAA II (Moderada — Urbana):** $a/c \le 0{,}60$ (CA) / $0{,}55$ (CP); $f_{ck} \ge 25\text{ MPa}$ (CA) / $30\text{ MPa}$ (CP); $C_{\min} \ge 280\text{ kg/m}^3$ (CA) / $320\text{ kg/m}^3$ (CP).
     - **CAA III (Forte — Marinha / Industrial):** $a/c \le 0{,}55$ (CA) / $0{,}50$ (CP); $f_{ck} \ge 30\text{ MPa}$ (CA) / $35\text{ MPa}$ (CP); $C_{\min} \ge 320\text{ kg/m}^3$ (CA) / $360\text{ kg/m}^3$ (CP).
     - **CAA IV (Muito Forte — Respingos de maré / Industrial severa):** $a/c \le 0{,}45$ (CA e CP); $f_{ck} \ge 40\text{ MPa}$ (CA e CP); $C_{\min} \ge 360\text{ kg/m}^3$ (CA e CP).
   - **Item 5.6.3.1 — Desvio-Padrão $S_d$ por condição de preparo:** Condição A ($S_d = 4{,}0\text{ MPa}$, todas as classes), Condição B ($S_d = 5{,}5\text{ MPa}$, bloqueando automaticamente $f_{ck} > 20\text{ MPa}$), Condição C ($S_d = 7{,}0\text{ MPa}$, bloqueando automaticamente $f_{ck} > 15\text{ MPa}$), Desvio Conhecido ($S_d \ge 2{,}0\text{ MPa}$ para $n \ge 20$) e Arbitrário de laboratório.
2. **ABNT NBR 6118:2023 (*Projeto de estruturas de concreto — Procedimento*):**
   - **Tabela 7.2:** Cobrimentos nominais ($c_{\text{nom}}$) de $20\text{ mm}$ a $55\text{ mm}$ diferenciados entre lajes, vigas/pilares e tipo estrutural (CA / CP $+5\text{ mm}$).
   - **Item 7.4.7.6:** Compatibilidade geométrica $D_{\max} \le 1{,}2 \cdot c_{\text{nom}}$.
3. **ABNT NBR 7211:2022 & NBR NM 45 (*Agregados para concreto*):**
   - Caracterização física e ensaio de mistura binária (Brita 1 + Brita 2, $0\%$ a $100\%$) para máxima massa unitária compactada e mínimo índice de vazios.
4. **ABNT NBR 7212:2025 (*Concreto dosado em central*):**
   - **Item 5.5:** Tolerâncias de pesagem em central dosadora: Cimento e Adições ($\pm 1\%$), Agregados ($\pm 3\%$), Água ($\pm 3\%$) e Aditivos ($\pm 5\%$).
5. **ABNT NBR 8953:2015 (*Classificação*) & NBR 16697:2018 (*Cimento Portland*):**
   - Grupos de resistência I (C20–C50) e II (C55–C100), classes de abatimento (*Slump* S10 a S220) e catálogo completo de cimentos CP I a CP V-ARI.

*(Consulte também a tabela cláusula-a-cláusula em [`docs/matriz_rastreabilidade_normativa.md`](matriz_rastreabilidade_normativa.md) e a auditoria técnica V&V em [`docs/technical-audit-and-roadmap.md`](technical-audit-and-roadmap.md).)*

---

## 2. Conformidade Microsoft Store (`Políticas v7.20`), FOSS & Independência

- **Modelo de Duplo Licenciamento Transparente ([`docs/licensing.md`](licensing.md)):** Código-fonte integral sob **GNU AGPLv3** no GitHub + liberdade total para desenvolvedores nos textos de `source/assets/`; pacote oficial gratuito na Microsoft Store regido pelo **EULA Oficial** do autor e pelo **Esclarecimento Comercial (`CLC`)** sem revenda comercial por terceiros.
- **Independência da Microsoft (Política 10.5 & Suporte):** **A Microsoft NÃO fornece política de privacidade padrão nem presta suporte técnico para o Concrete Mix Designer.** Aplica-se exclusivamente [`docs/privacy-policy.md`](privacy-policy.md) (`100% Local-First`, zero telemetria, zero rede) e o suporte voluntário *best-effort* pelo autor em `linceu_lighthouse@outlook.com`.
- **Desinstalação Limpa (Política 10.2.7) & `runFullTrust` (Política 10.6):** Manifesto [`library/AppxManifest.xml`](../library/AppxManifest.xml) declara estritamente `<rescap:Capability Name="runFullTrust" />` para diálogos de arquivo locais Windows e execução C++23 / Qt 6.12.0 offline, permitindo remoção limpa pelas Configurações do Windows sem serviços residuais.
- **Conformidade FOSS (GNU LGPLv3 Seção 4 & Domínio Público):** Vinculação 100% dinâmica das bibliotecas **Qt 6.12.0** (preservando substituição de DLLs pelo usuário) e **SQLite 3** em domínio público, com textos integrais em [`source/assets/LICENSES/`](../source/assets/LICENSES) e atribuições em [`source/assets/NOTICES/NOTICE_pt_BR.txt`](../source/assets/NOTICES/NOTICE_pt_BR.txt).

---

## 3. Escopo Auxiliar Gratuito, Limitações de Maturidade e Isenção Legal ("AS IS")

Por ser um aplicativo gratuito, o usuário pode utilizar o **Concrete Mix Designer** conforme lhe convir como **ferramenta auxiliar** (educacional, acadêmica, ensino universitário e comparação de resultados), e **não** como ferramenta definitiva ou final. Este software **ainda não está maduro o suficiente para ser levado para produção ou uso profissional definitivo**, pois, devido a limitações, falhas humanas e eventuais erros no código-fonte — abrangendo tanto a matemática aplicada quanto a física/tecnologia dos materiais envolvidas, razão pela qual seus algoritmos podem apresentar falhas —, exige sempre conferência manual de todos os resultados, uso de ferramentas mais consolidadas no mercado e ensaios físicos laboratoriais (**ABNT NBR 5738** e **NBR 5739**).

Caso o usuário decida utilizá-lo exclusivamente em produção, em centrais dosadoras ou canteiros de obras, todas as consequências serão de sua inteira e exclusiva responsabilidade, nos termos do [EULA Oficial](../source/assets/EULA/EULA_pt_BR.txt), da [GNU AGPLv3](../LICENSE) (Seções 15 e 16), da legislação brasileira (**Código Civil — Lei nº 10.406/2002, arts. 186, 618 e 927**; **Lei nº 9.609/1998**; **Lei nº 9.610/1998**; **CDC art. 39, VIII — Lei nº 8.078/1990**; **Código Penal art. 256**; **Leis nº 5.194/1966, nº 6.496/1977 e nº 12.378/2010 — ART/RRT perante CREA/CAU**) e convenções internacionais (*UCC § 2-316*, Convenção de Berna, OMPI/WIPO, TRIPS).

</details>

---

## <a id="enus"></a>🇺🇸 English (US)

<details open>
<summary>Click to expand / collapse compliance guide in English</summary><br>

# Concrete Mix Designer — Normative, Microsoft Store & Legal Compliance Guide

**Version:** `2026.11.22.0`  
**Date:** `November 22, 2026`  
**Author:** Fernando Nillsson Cidade  

---

## 1. Compliance with Brazilian Technical Standards (ABNT NBR)

**Concrete Mix Designer** (`v2026.11.22.0`) embeds within its in-memory relational database ([`NormativeDatabase`](../source/core/NormativeDatabase.cpp)) and normative engine ([`ConcreteStandards`](../source/core/ConcreteStandards.cpp)) auxiliary checks grounded in current Brazilian technical standards:

1. **ABNT NBR 12655:2022 (*Portland cement concrete — Preparation, control, receipt, and acceptance*):**
   - **Table 6 — Concrete exposure requirements:**
     - **CAA I (Weak — Rural / Submerged):** $w/c \le 0.65$ (RC) / $0.60$ (PC); $f_{ck} \ge 20\text{ MPa}$ (RC) / $25\text{ MPa}$ (PC); $C_{\min} \ge 260\text{ kg/m}^3$ (RC) / $280\text{ kg/m}^3$ (PC).
     - **CAA II (Moderate — Urban):** $w/c \le 0.60$ (RC) / $0.55$ (PC); $f_{ck} \ge 25\text{ MPa}$ (RC) / $30\text{ MPa}$ (PC); $C_{\min} \ge 280\text{ kg/m}^3$ (RC) / $320\text{ kg/m}^3$ (PC).
     - **CAA III (Strong — Marine / Industrial):** $w/c \le 0.55$ (RC) / $0.50$ (PC); $f_{ck} \ge 30\text{ MPa}$ (RC) / $35\text{ MPa}$ (PC); $C_{\min} \ge 320\text{ kg/m}^3$ (RC) / $360\text{ kg/m}^3$ (PC).
     - **CAA IV (Very Strong — Tidal splash / Severe industrial):** $w/c \le 0.45$ (RC & PC); $f_{ck} \ge 40\text{ MPa}$ (RC & PC); $C_{\min} \ge 360\text{ kg/m}^3$ (RC & PC).
   - **Section 5.6.3.1 — Standard Deviation $S_d$:** Condition A ($4.0\text{ MPa}$, all classes), Condition B ($5.5\text{ MPa}$, automatically blocking $f_{ck} > 20\text{ MPa}$), Condition C ($7.0\text{ MPa}$, automatically blocking $f_{ck} > 15\text{ MPa}$), Known Historical ($S_d \ge 2.0\text{ MPa}$ for $n \ge 20$), and Arbitrary lab mode.
2. **ABNT NBR 6118:2023 (*Design of concrete structures*):**
   - **Table 7.2:** Nominal reinforcement covers ($c_{\text{nom}}$) from $20\text{ mm}$ to $55\text{ mm}$ separated by slabs, beams/columns, and structural type (RC / PC $+5\text{ mm}$).
   - **Section 7.4.7.6:** Geometric check $D_{\max} \le 1.2 \cdot c_{\text{nom}}$.
3. **ABNT NBR 7211:2022 & NBR NM 45 (*Concrete aggregates*):**
   - Physical characterization and binary coarse aggregate blend trials (Gravel 1 + Gravel 2, $0\%$ to $100\%$) for maximum compacted bulk density and minimum voids.
4. **ABNT NBR 7212:2025 (*Ready-mixed concrete*):**
   - **Section 5.5:** Plant weighing tolerances: Cement and Additions ($\pm 1\%$), Aggregates ($\pm 3\%$), Water ($\pm 3\%$), and Admixtures ($\pm 5\%$).
5. **ABNT NBR 8953:2015 (*Classification*) & NBR 16697:2018 (*Portland cement*):**
   - Strength Groups I (C20–C50) and II (C55–C100), slump consistency classes (S10 to S220), and full cement catalog (CP I to CP V-ARI).

*(Also refer to [`docs/matriz_rastreabilidade_normativa.md`](matriz_rastreabilidade_normativa.md) and [`docs/technical-audit-and-roadmap.md`](technical-audit-and-roadmap.md).)*

---

## 2. Microsoft Store Compliance (`Policies v7.20`), FOSS & Independence

- **Transparent Dual-Licensing Model ([`docs/licensing.md`](licensing.md)):** Full source code under **GNU AGPLv3** on GitHub + full developer freedom over `source/assets/` texts; free official Microsoft Store package governed by the author's **Official EULA** and **Commercial Licensing Clarification (`CLC`)** prohibiting paid commercial resale by third parties.
- **Microsoft Independence (Policy 10.5 & Support):** **Microsoft does NOT provide a default privacy policy and does NOT provide customer/technical support for Concrete Mix Designer.** Only [`docs/privacy-policy.md`](privacy-policy.md) (`100% Local-First`, zero telemetry, zero network) and voluntary best-effort developer support at `linceu_lighthouse@outlook.com` apply.
- **Clean Uninstall (Policy 10.2.7) & `runFullTrust` (Policy 10.6):** [`library/AppxManifest.xml`](../library/AppxManifest.xml) declares strictly `<rescap:Capability Name="runFullTrust" />` for local Windows file dialogs and offline C++23 / Qt 6.12.0 execution, allowing clean removal via Windows Settings with zero residual background services.
- **FOSS Compliance (GNU LGPLv3 Section 4 & Public Domain):** 100% dynamic linking of **Qt 6.12.0** DLLs (preserving end-user library replacement rights) and public-domain **SQLite 3**, with full license texts under [`source/assets/LICENSES/`](../source/assets/LICENSES) and attributions in [`source/assets/NOTICES/NOTICE_en_US.txt`](../source/assets/NOTICES/NOTICE_en_US.txt).

---

## 3. Free Auxiliary Scope, Maturity Limitations & Legal Disclaimer ("AS IS")

Because **Concrete Mix Designer** is a free application, the user may use it as they see fit as an **auxiliary tool** (educational, academic, university teaching, and result comparison), and **not** as a definitive or final tool. This software **is not yet mature enough to be taken to production or definitive professional use** because, due to limitations, human flaws, and possible source-code errors — encompassing both applied mathematics and underlying materials physics/technology — its algorithms may produce errors. It always requires manual verification of all results, cross-checking with more established market tools, and physical laboratory trials (**ABNT NBR 5738** and **NBR 5739**).

Should the user decide to rely exclusively on it in production, at batching plants, or on construction sites, all consequences remain the user's sole and entire responsibility under the [Official EULA](../source/assets/EULA/EULA_en_US.txt), [GNU AGPLv3](../LICENSE) (Sections 15–16), Brazilian legislation (**Civil Code — Law No. 10,406/2002, Arts. 186, 618, and 927**; **Law No. 9,609/1998**; **Law No. 9,610/1998**; **Consumer Defense Code Art. 39, VIII — Law No. 8,078/1990**; **Penal Code Art. 256**; **Laws No. 5,194/1966, No. 6,496/1977, and No. 12,378/2010 — ART/RRT before CREA/CAU**), and international conventions (*UCC § 2-316*, Berne Convention, WIPO, TRIPS).

</details>
