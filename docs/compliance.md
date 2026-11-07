# Conformidade Normativa, Microsoft Store e Legal / Normative, Microsoft Store & Legal Compliance

[Português (Brasil)](#português-brasil) | [English (United States)](#english-united-states)

---

## Português (Brasil)

### 1. Conformidade com as Normas Técnicas Brasileiras (ABNT NBR)

O **Concrete Mix Design C++ (v2026.11.6.0)** incorpora em seu banco de dados relacional em memória ([`NormativeDatabase`](../source/core/NormativeDatabase.cpp)) e no motor normativo ([`ConcreteStandards`](../source/core/ConcreteStandards.cpp)) os requisitos obrigatórios das normas técnicas brasileiras vigentes:

1. **ABNT NBR 12655:2022 (*Concreto de cimento Portland — Preparo, controle, recebimento e aceitação*):**
   - **Tabela 6 — Requisitos para o concreto em condições especiais de exposição:**
     - **CAA I (Fraca — Rural / Submersa):** $a/c \le 0,65$ (CA) / $0,60$ (CP); $f_{ck} \ge 20\text{ MPa}$ (CA) / $25\text{ MPa}$ (CP); $C_{\min} \ge 260\text{ kg/m}^3$ (CA) / $280\text{ kg/m}^3$ (CP).
     - **CAA II (Moderada — Urbana):** $a/c \le 0,60$ (CA) / $0,55$ (CP); $f_{ck} \ge 25\text{ MPa}$ (CA) / $30\text{ MPa}$ (CP); $C_{\min} \ge 280\text{ kg/m}^3$ (CA) / $320\text{ kg/m}^3$ (CP).
     - **CAA III (Forte — Marinha / Industrial):** $a/c \le 0,55$ (CA) / $0,50$ (CP); $f_{ck} \ge 30\text{ MPa}$ (CA) / $35\text{ MPa}$ (CP); $C_{\min} \ge 320\text{ kg/m}^3$ (CA) / $360\text{ kg/m}^3$ (CP).
     - **CAA IV (Muito Forte — Respingos de maré / Industrial severa):** $a/c \le 0,45$ (CA e CP); $f_{ck} \ge 40\text{ MPa}$ (CA e CP); $C_{\min} \ge 360\text{ kg/m}^3$ (CA e CP).
   - **Item 5.6.3.1 — Desvio-Padrão $S_d$ segundo a condição de preparo:**
     - **Condição A ($S_d = 4,0\text{ MPa}$):** Todas as classes de resistência;
     - **Condição B ($S_d = 5,5\text{ MPa}$):** Restrita às classes C10 a C20 (o software emite alerta de não conformidade caso $f_{ck} > 20\text{ MPa}$);
     - **Condição C ($S_d = 7,0\text{ MPa}$):** Restrita às classes C10 e C15 (o software emite alerta de não conformidade caso $f_{ck} > 15\text{ MPa}$).
2. **ABNT NBR 6118:2023 (*Projeto de estruturas de concreto — Procedimento*):**
   - **Tabela 7.2:** Cobrimento nominal ($c_{\text{nom}}$) de $20\text{ mm}$ a $55\text{ mm}$ conforme CAA I a IV e tipo estrutural (CA / CP).
   - **Item 7.4.7.6:** Verificação de compatibilidade geométrica entre a Dimensão Máxima Característica do agregado graúdo e o cobrimento nominal ($D_{\max} \le 1,2 \cdot c_{\text{nom}}$).
3. **ABNT NBR 7211:2022 (*Agregados para concreto — Requisitos*):**
   - Caracterização física e otimização da massa unitária compactada de misturas binárias de agregados graúdos.
4. **ABNT NBR 7212:2025 (*Concreto dosado em central — Preparo, fornecimento e controle*):**
   - **Item 5.5:** Tolerâncias operacionais máximas de pesagem em central dosadora: **Cimento e Adições ($\pm 1\%$)**, **Agregados ($\pm 3\%$)**, **Água ($\pm 3\%$)** e **Aditivos ($\pm 5\%$)**.
5. **ABNT NBR 8953:2015 (*Concreto para fins estruturais — Classificação*):**
   - Grupos de resistência I (C20 a C50) e II (C55 a C100) e classes de consistência pelo abatimento de tronco de cone (**S10**, **S50**, **S100**, **S160** e **S220**).
6. **Automação de Auditoria Normativa via Terminal (`concrete-mix-designer-cli verify`):**
   - Permite verificar programaticamente a conformidade de qualquer traço de concreto frente às normas ABNT NBR 12655, NBR 6118, NBR 7212 e NBR 8953, retornando códigos de saída padronizados (`0` para conforme, `2` para não conforme) para integração com esteiras de automação e controle de qualidade.

---

### 2. Conformidade com a Microsoft Store e Microsoft Partner Center

O aplicativo cumpre integralmente os requisitos técnicos, comerciais e de conteúdo do **Microsoft Application Developer Agreement (ADA v8.9)** e das **Políticas da Microsoft Store**:

- **Política 10.1 (Funções e Valor Distintos):** Aplicativo profissional completo de engenharia civil, sem bloqueios artificiais e com documentação integrada (`F1`).
- **Política 10.2 (Segurança Técnica):** Binário C++20 x64 nativo, sem dependências ocultas, sem execução de código remoto e com banco SQLite isolado em memória (`:memory:`).
- **Política 10.5 (Informações Pessoais e Privacidade):** Arquitetura *Local-First* 100% offline, sem coleta de dados pessoais (PII) e sem telemetria ([`Privacy_Policy_pt_BR.txt`](../source/assets/PRIVACY_POLICY/Privacy_Policy_pt_BR.txt)).
- **Seção 3(c)(vi) do ADA (Conformidade FOSS / LGPLv3):** Vinculação 100% dinâmica das bibliotecas Qt 6.11.1 sob a licença **GNU LGPL v3.0**, preservando todos os direitos do usuário final previstos na LGPLv3 ([`NOTICE_pt_BR.txt`](../source/assets/NOTICES/NOTICE_pt_BR.txt)).

---

### 3. Conformidade Legislativa e Regulamentação Profissional

- **Legislação Brasileira:** Lei do Software (Lei nº 9.609/1998), Lei de Direitos Autorais (Lei nº 9.610/1998), LGPD (Lei nº 13.709/2018), Código de Defesa do Consumidor (Lei nº 8.078/1990) e regulamentação profissional de Engenharia Civil (Lei Federal nº 5.194/1966 e Lei nº 6.496/1977 — CONFEA/CREA e exigência de ART).
- **Legislação Internacional:** *United States Copyright Act* (17 U.S.C. § 101 et seq.), Convenção de Berna, Tratado OMPI/WCT, Regulamento Europeu **GDPR (UE 2016/679)**, **CCPA/CPRA** e regulamentos de controle de exportação (**EAR**).

---

## English (United States)

### 1. Compliance with Brazilian Technical Standards (ABNT NBR)

**Concrete Mix Design C++ (v2026.11.6.0)** embeds within its in-memory relational database ([`NormativeDatabase`](../source/core/NormativeDatabase.cpp)) and normative engine ([`ConcreteStandards`](../source/core/ConcreteStandards.cpp)) the mandatory requirements of current Brazilian technical standards:

1. **ABNT NBR 12655:2022 (*Portland cement concrete — Preparation, control, receipt, and acceptance*):**
   - **Table 6 — Requirements for concrete under special exposure conditions:**
     - **CAA I (Weak — Rural / Submerged):** $w/c \le 0.65$ (RC) / $0.60$ (PC); $f_{ck} \ge 20\text{ MPa}$ (RC) / $25\text{ MPa}$ (PC); $C_{\min} \ge 260\text{ kg/m}^3$ (RC) / $280\text{ kg/m}^3$ (PC).
     - **CAA II (Moderate — Urban):** $w/c \le 0.60$ (RC) / $0.55$ (PC); $f_{ck} \ge 25\text{ MPa}$ (RC) / $30\text{ MPa}$ (PC); $C_{\min} \ge 280\text{ kg/m}^3$ (RC) / $320\text{ kg/m}^3$ (PC).
     - **CAA III (Strong — Marine / Industrial):** $w/c \le 0.55$ (RC) / $0.50$ (PC); $f_{ck} \ge 30\text{ MPa}$ (RC) / $35\text{ MPa}$ (PC); $C_{\min} \ge 320\text{ kg/m}^3$ (RC) / $360\text{ kg/m}^3$ (PC).
     - **CAA IV (Very Strong — Tidal splash / Severe industrial):** $w/c \le 0.45$ (RC and PC); $f_{ck} \ge 40\text{ MPa}$ (RC and PC); $C_{\min} \ge 360\text{ kg/m}^3$ (RC and PC).
   - **Section 5.6.3.1 — Standard Deviation $S_d$ according to batching condition:**
     - **Condition A ($S_d = 4.0\text{ MPa}$):** All strength classes;
     - **Condition B ($S_d = 5.5\text{ MPa}$):** Restricted to classes C10 through C20 (the software flags non-compliance whenever $f_{ck} > 20\text{ MPa}$);
     - **Condition C ($S_d = 7.0\text{ MPa}$):** Restricted to classes C10 and C15 (the software flags non-compliance whenever $f_{ck} > 15\text{ MPa}$).
2. **ABNT NBR 6118:2023 (*Design of concrete structures — Procedure*):**
   - **Table 7.2:** Nominal cover ($c_{\text{nom}}$) from $20\text{ mm}$ to $55\text{ mm}$ according to CAA I to IV and structural type (RC / PC).
   - **Section 7.4.7.6:** Geometric compatibility check between Maximum Characteristic Coarse Aggregate Size and nominal cover ($D_{\max} \le 1.2 \cdot c_{\text{nom}}$).
3. **ABNT NBR 7211:2022 (*Aggregates for concrete — Requirements*):**
   - Physical characterization and optimization of compacted bulk unit weight for binary coarse aggregate blends.
4. **ABNT NBR 7212:2025 (*Ready-mixed concrete — Preparation, delivery, and control*):**
   - **Section 5.5:** Maximum operational weighing tolerances in central batching plants: **Cement and Additions ($\pm 1\%$)**, **Aggregates ($\pm 3\%$)**, **Water ($\pm 3\%$)**, and **Admixtures ($\pm 5\%$)**.
5. **ABNT NBR 8953:2015 (*Concrete for structural purposes — Classification*):**
   - Strength Groups I (C20 to C50) and II (C55 to C100) and slump cone consistency classes (**S10**, **S50**, **S100**, **S160**, and **S220**).
6. **Automated Normative Auditing via Terminal (`concrete-mix-designer-cli verify`):**
   - Enables programmatic compliance verification of any concrete mix against ABNT NBR 12655, NBR 6118, NBR 7212, and NBR 8953 standards, returning standardized exit codes (`0` for compliant, `2` for non-compliant) for quality control and automated pipelines.

---

### 2. Microsoft Store and Microsoft Partner Center Compliance

The application fully complies with the technical, commercial, and content requirements of the **Microsoft Application Developer Agreement (ADA v8.9)** and **Microsoft Store Policies**:

- **Policy 10.1 (Distinct Function and Value):** Complete professional civil engineering application with zero artificial locks and built-in interactive documentation (`F1`).
- **Policy 10.2 (Technical Security):** Native C++20 x64 binary with zero hidden dependencies, zero remote code execution, and an isolated in-memory SQLite database (`:memory:`).
- **Policy 10.5 (Personal Information and Privacy):** 100% offline *Local-First* architecture with zero Personally Identifiable Information (PII) collection and zero telemetry ([`Privacy_Policy_en_US.txt`](../source/assets/PRIVACY_POLICY/Privacy_Policy_en_US.txt)).
- **ADA Section 3(c)(vi) (FOSS / LGPLv3 Compliance):** 100% dynamic linking of Qt 6.11.1 libraries under the **GNU LGPL v3.0** license, preserving all end-user rights granted by the LGPLv3 ([`NOTICE_en_US.txt`](../source/assets/NOTICES/NOTICE_en_US.txt)).

---

### 3. Statutory Compliance and Professional Engineering Regulation

- **Brazilian Legislation:** Software Act (Law No. 9,609/1998), Copyright Act (Law No. 9,610/1998), LGPD (Law No. 13,709/2018), Consumer Defense Code (Law No. 8,078/1990), and Civil Engineering professional regulation (Federal Law No. 5,194/1966 and Law No. 6,496/1977 — CONFEA/CREA and ART requirement).
- **International Legislation:** *United States Copyright Act* (17 U.S.C. § 101 et seq.), Berne Convention, WIPO Copyright Treaty (WCT), European **GDPR (Regulation (EU) 2016/679)**, **CCPA/CPRA**, and Export Administration Regulations (**EAR**).
