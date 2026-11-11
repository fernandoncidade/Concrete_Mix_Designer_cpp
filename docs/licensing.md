# Licenciamento e Propriedade Intelectual / Licensing & Intellectual Property
## Concrete Mix Designer C++ (v2026.11.11.0)

[Português (Brasil)](#português-brasil) | [English (United States)](#english-united-states)

---

## Português (Brasil)

### 1. Modelo de Licenciamento de Código Aberto (GNU AGPLv3) e Proteção de Marca Comercial

O projeto adota uma estratégia jurídica clara e transparente que harmoniza a disponibilização do código-fonte sob licença livre e recíproca com a proteção da marca e a distribuição comercial oficial na **Microsoft Store** (**Microsoft Partner Center**):

1. **Código-Fonte Aberto sob Licença GNU AGPLv3 ([`LICENSE`](../LICENSE)):**
   - Todo o código-fonte disponibilizado neste repositório do GitHub é regido pela **GNU Affero General Public License v3 (AGPLv3)**.
   - A AGPLv3 garante a qualquer pessoa a liberdade de estudar, copiar, modificar, compilar, auditar e redistribuir o software, assegurando que quaisquer melhorias, trabalhos derivados ou serviços em rede baseados no código-fonte permaneçam livres e com código aberto correspondente disponibilizado à comunidade científica e profissional.
   - Isso garante máxima transparência e rigor para auditoria de fórmulas em Engenharia Civil, pesquisa acadêmica e ensino universitário.

2. **Reserva de Marca Registrada, Nome Comercial e Identidade Visual:**
   - A licença GNU AGPLv3 aplica-se estritamente ao código-fonte e documentação técnica associada. Ela **não** concede direitos sobre o nome comercial, logotipo, ícones oficiais e identidade visual (*trade dress*) **"Concrete Mix Designer C++"** ou **"Concrete Mix Design C++"**.
   - Qualquer versão modificada ou fork redistribuído por terceiros em lojas de aplicativos (como Microsoft Store, Steam ou Google Play) não poderá utilizar a marca oficial ou apresentar-se como produto oficial de Fernando Nillsson Cidade sem autorização prévia por escrito.

3. **Edição Comercial Oficial na Microsoft Store (`CLC` + `EULA`):**
   - Os pacotes binários compilados, assinados digitalmente e distribuídos oficialmente na **Microsoft Store** (abrangendo a aplicação gráfica `ConcreteMixDesign.exe` e o utilitário de terminal `concrete-mix-designer-cli.exe`) são licenciados aos adquirentes sob os termos do **Contrato de Licenciamento Comercial** ([`source/assets/CLC/CLC_pt_BR.txt`](../source/assets/CLC/CLC_pt_BR.txt)) e do **Contrato de Licença de Usuário Final (EULA)** ([`source/assets/EULA/EULA_pt_BR.txt`](../source/assets/EULA/EULA_pt_BR.txt)), em conformidade com o **Microsoft Application Developer Agreement (ADA v8.9, Anexo B)**.
   - A aquisição na Microsoft Store inclui certificação de segurança pela Microsoft, atualizações automáticas via Windows Update e suporte do desenvolvedor. Organizações que necessitem de isenção dos termos de copyleft da AGPLv3 para integração proprietária fechada podem solicitar licenciamento comercial direto.

---

### 2. Licenças de Terceiros e Conformidade FOSS (Qt 6 LGPLv3)

Todos os textos integrais de licenças de terceiros encontram-se armazenados em [`source/assets/LICENSES/`](../source/assets/LICENSES):

| Componente | Versão | Licença Aplicável | Modalidade de Vinculação |
| :--- | :--- | :--- | :--- |
| **Qt Framework (`Core`, `Gui`, `Widgets`, `Charts`, `Sql`)** | 6.11.1 | GNU LGPL v3.0 / GPL v3.0 | Vinculação Dinâmica (`.dll` independentes) |
| **SQLite Relational Engine** | 3.x | Domínio Público (*Public Domain*) | Embarcado via plugin dinâmico `qsqlite.dll` |
| **MinGW-w64 / GCC Runtime (`libstdc++-6.dll`, `libgcc_s_seh-1.dll`, `libwinpthread-1.dll`)** | GCC 13+ | GPL v3.0 com *GCC Runtime Library Exception v3.1* + MIT/BSD | Vinculação Dinâmica (`.dll`) |

> **Garantia Estrita de Direitos LGPLv3 e Aviso de Finalidade Acadêmica:** Em estrita observância à Seção 4 da licença **GNU LGPL v3.0**, nenhuma cláusula do EULA ou termos comerciais restringe o direito do usuário final de substituir, atualizar ou relinkar as bibliotecas dinâmicas do Qt 6 presentes no diretório da aplicação por versões compatíveis de sua preferência. O software constitui uma ferramenta voltada exclusivamente para fins de estudo acadêmico, fornecida "NO ESTADO EM QUE SE ENCONTRA" ("AS-IS"), podendo conter possíveis erros e não servindo como software definitivo para tomadas de decisão em produção ou ambientes de trabalho sem conferência manual obrigatória e validação através de outras ferramentas mais consolidadas no mercado, ficando o autor integralmente isento de responsabilidades legais nos termos da legislação brasileira (Código Civil — Lei nº 10.406/2002; Leis nº 9.609/1998, nº 9.610/1998, nº 8.078/1990, nº 5.194/1966 e nº 6.496/1977) e convenções internacionais (UCC § 2-316, Convenção de Berna, OMPI/WIPO, TRIPS e Seções 15 e 16 da GNU AGPLv3).

---

## English (United States)

### 1. Open-Source Licensing Model (GNU AGPLv3) and Trademark Protection

The project adopts a clear and transparent legal strategy that harmonizes reciprocal open-source code availability with trademark protection and official commercial distribution on the **Microsoft Store** (**Microsoft Partner Center**):

1. **Open-Source Codebase under the GNU AGPLv3 ([`LICENSE`](../LICENSE)):**
   - All source code in this GitHub repository is governed by the **GNU Affero General Public License v3 (AGPLv3)**.
   - The AGPLv3 guarantees everyone the freedom to study, copy, modify, compile, audit, and redistribute the software, ensuring that any enhancements, derivative works, or network-hosted services based on this codebase remain free software with corresponding source code made available to the scientific and engineering community.
   - This ensures complete mathematical transparency and auditability for Civil Engineering formulations, academic research, and university education.

2. **Reservation of Trademark, Trade Name, and Visual Identity:**
   - The GNU AGPLv3 license applies strictly to the source code and associated technical documentation. It **does not** grant trademark rights to the trade name, logo, official icons, or visual identity (*trade dress*) of **"Concrete Mix Designer C++"** or **"Concrete Mix Design C++"**.
   - Any modified version or fork distributed by third parties across app stores (such as the Microsoft Store, Steam, or Google Play) may not use the official brand name or present itself as an official product of Fernando Nillsson Cidade without prior written consent.

3. **Official Commercial Edition on the Microsoft Store (`CLC` + `EULA`):**
   - Binary packages compiled, digitally signed, and officially distributed on the **Microsoft Store** (encompassing the graphical desktop application `ConcreteMixDesign.exe` and the companion terminal utility `concrete-mix-designer-cli.exe`) are licensed to purchasers under the **Commercial Licensing Agreement** ([`source/assets/CLC/CLC_en_US.txt`](../source/assets/CLC/CLC_en_US.txt)) and the **End-User License Agreement (EULA)** ([`source/assets/EULA/EULA_en_US.txt`](../source/assets/EULA/EULA_en_US.txt)), pursuant to the **Microsoft Application Developer Agreement (ADA v8.9, Exhibit B)**.
   - Microsoft Store purchases include Microsoft security certification, automatic updates via Windows Update, and direct developer support. Enterprises and organizations requiring an exemption from AGPLv3 copyleft terms for closed proprietary workflows may obtain direct commercial licensing.

---

### 2. Third-Party and Open-Source Licenses (Qt 6 LGPLv3 Compliance)

All full third-party license texts are stored in [`source/assets/LICENSES/`](../source/assets/LICENSES):

| Component | Version | Applicable License | Linking Mode |
| :--- | :--- | :--- | :--- |
| **Qt Framework (`Core`, `Gui`, `Widgets`, `Charts`, `Sql`)** | 6.11.1 | GNU LGPL v3.0 / GPL v3.0 | Dynamic Linking (independent `.dll` files) |
| **SQLite Relational Engine** | 3.x | Public Domain | Embedded via dynamic plugin `qsqlite.dll` |
| **MinGW-w64 / GCC Runtime (`libstdc++-6.dll`, `libgcc_s_seh-1.dll`, `libwinpthread-1.dll`)** | GCC 13+ | GPL v3.0 with *GCC Runtime Library Exception v3.1* + MIT/BSD | Dynamic Linking (`.dll`) |

> **Strict LGPLv3 Rights Reservation and Academic Purpose Notice:** Pursuant to Section 4 of the **GNU LGPL v3.0** license, no EULA or commercial clause restricts the end user's legal right to inspect, replace, update, or relink the Qt 6 dynamic libraries located in the application directory with compatible builds. The software constitutes a tool intended exclusively for academic study purposes, provided "AS-IS", may contain possible errors, and does not serve as definitive software for decision-making in production or work environments without mandatory manual verification and validation through other more consolidated market tools, with the author being fully exempt from legal liabilities under Brazilian legislation (Civil Code — Law No. 10,406/2002; Laws No. 9,609/1998, No. 9,610/1998, No. 8,078/1990, No. 5,194/1966, and No. 6,496/1977) and international conventions (UCC § 2-316, Berne Convention, WIPO, TRIPS, and Sections 15 and 16 of the GNU AGPLv3).
