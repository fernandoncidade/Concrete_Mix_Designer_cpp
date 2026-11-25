# Concrete Mix Designer — Engenharia Computacional de Dosagem de Concreto (IPT/EPUSP)

<div align="center">

[![Português (BR)](https://img.shields.io/badge/Idioma-Portugu%C3%AAs%20(BR)-009739?style=for-the-badge)](#-português-brasil)
[![English (US)](https://img.shields.io/badge/Language-English%20(US)-0052B4?style=for-the-badge)](#-english-united-states)
[![Versão](https://img.shields.io/badge/Vers%C3%A3o-2026.11.22.0-1f6feb?style=for-the-badge)](docs/release-v2026.11.22.0.md)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-00599C?style=for-the-badge&logo=cplusplus)](CMakeLists.txt)
[![Qt 6.12.0](https://img.shields.io/badge/Qt-6.12.0-41CD52?style=for-the-badge&logo=qt)](CMakeLists.txt)
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

### 1. Visão Geral e Aviso de Maturidade ("NO ESTADO EM QUE SE ENCONTRA")

O **Concrete Mix Designer** é uma aplicação nativa em **C++23 + Qt 6.12.0** dedicada à dosagem racional e experimental de concreto de cimento Portland pelo **Método IPT/EPUSP** (Helene & Terzian, 1992), equipada com motor de otimização granulométrica binária, regressões por mínimos quadrados (Abrams, Lyse e Molinari), banco normativo **SQLite 3 em memória (`:memory:`)**, diagramas interativos em **Qt Charts** (`QuadChartWidget`, `IptUnifiedDiagramWidget`), **CLI completa** e geração automatizada de relatórios técnicos em **HTML5**, **Markdown** e **PDF**.

> **⚠️ Aviso de Software Auxiliar Gratuito e Isenção de Garantias:**  
> O **Concrete Mix Designer** é um aplicativo **gratuito e estritamente auxiliar** para apoio acadêmico e de engenharia. **O programa ainda não apresenta maturidade suficiente para utilização definitiva em produção ou obras comerciais.** Devido a eventuais limitações, erros humanos ou falhas no código-fonte (em matemática aplicada e tecnologia dos materiais), o programa é distribuído **"NO ESTADO EM QUE SE ENCONTRA" (*AS IS*)**, sem garantias de qualquer espécie. **Todos os resultados devem ser conferidos manualmente, comparados com softwares consolidados e comprovados em laboratório (ABNT NBR 5738 e NBR 5739)** sob exclusiva responsabilidade do usuário.

---

### 2. Principais Capacidades e Conformidade Normativa (ABNT NBR)

- **Banco Relacional em Memória (`:memory:`):** [`NormativeDatabase`](source/core/NormativeDatabase.cpp) consulta tabelas em RAM (com `QRecursiveMutex`) para **ABNT NBR 12655:2022** (CAA I–IV, Condições A/B/C), **NBR 6118:2023** (Tabela 7.2 — cobrimentos nominais CA/CP e relação $D_{\max} \le 1{,}2 \cdot c_{\text{nom}}$), **NBR 7211:2022**, **NBR 7212:2025** (tolerâncias de pesagem Item 5.5), **NBR 8953:2015** e **NBR 16697:2018**.
- **Otimização & Regressão Analítica (`core/`):** [`AggregateOptimizer`](source/core/AggregateOptimizer.cpp) (recipiente $15{,}04\text{ L}$), [`MortarContentOptimizer`](source/core/MortarContentOptimizer.cpp) ($\alpha$ ideal e amassada piloto), [`RegressionEngine`](source/core/RegressionEngine.cpp) (Abrams polinomial/exponencial, Lyse linear, Molinari quadrática e consumo teórico), [`MixDesignCalculator`](source/core/MixDesignCalculator.cpp) e [`ProjectData`](source/core/ProjectData.cpp) (6 cenários `0..5` incluindo **UFPR TC031**, serialização `.cmdproj` e relatórios).
- **Interface Bilíngue 100% Dinâmica (`source/language/`):** Troca imediata entre **Português (BR)** (`R$`) e **English (US)** (`$`) sem reiniciar a aplicação, adaptando menus, tabelas, diagramas, relatórios e todas as **9 abas da janela Sobre** (`CLC / Licenciamento`, `Copyright`, `Licenças Completas` com seletor de 11 licenças em lista suspensa, etc.) ao tema **Claro/Escuro** do Windows 11.
- **Privacidade 100% Local-First:** Zero telemetria, zero rede, apenas `QSettings("AppsCpp", "ConcreteMixDesign")` e logs locais em `%LOCALAPPDATA%\ConcreteMixDesigner\logs\` ([`docs/privacy-policy.md`](docs/privacy-policy.md)).

---

### 3. Galeria Visual da Interface — Português (Brasil) (`images/interfaces/pt_BR/`)

Capturas reais de todas as abas e diagramas do **Concrete Mix Designer** em **Português (Brasil)** armazenadas em [`images/interfaces/pt_BR/`](images/interfaces/pt_BR) *(para a galeria equivalente em inglês, consulte a [Seção 3 de English (United States)](#galeria-enus))*:

<a id="galeria-ptbr"></a>

<p align="center">
  <img src="images/interfaces/pt_BR/Captura%20de%20tela%202026-10-26%20053540.png" alt="Aba 1 — Parâmetros de Projeto e Limitações Normativas de Durabilidade (pt-BR)" width="680"><br>
  <strong>Figura 1 (pt-BR) — Aba 1: Parâmetros de Projeto e Requisitos Normativos de Durabilidade</strong><br>
  <em>Configuração da resistência característica (<code>fck = 25,0 MPa</code>), condição de preparo <code>Condição A (Sd = 4,0 MPa)</code> e cálculo automático da resistência média de dosagem (<code>fcj = fck + 1,65&middot;Sd = 31.600 MPa</code>), além da Classe de Agressividade Ambiental <code>Classe II - Moderada (Urbana)</code>, estrutura em Concreto Armado (CA), abatimento alvo de <code>100 mm</code> (classe <code>S100</code> — NBR 8953) e painel automático de limites da NBR 12655:2022, NBR 6118:2023 (<code>a/c &le; 0.60</code>, <code>C25</code>, consumo mínimo <code>&ge; 280 kg/m<sup>3</sup></code>, cobrimento <code>25 mm / 30 mm</code>, verificação DMC vs. cobrimento) e tolerâncias de pesagem da NBR 7212:2025. Na barra de status inferior consta o selo verde <code>NBR 12655: CONFORME</code>.</em>
</p>

<p align="center">
  <img src="images/interfaces/pt_BR/Captura%20de%20tela%202026-10-26%20053550.png" alt="Aba 2 — Caracterização dos Materiais e Otimização Binária de Britas (pt-BR)" width="680"><br>
  <strong>Figura 2 (pt-BR) — Aba 2: Materiais e Ensaio de Otimização da Mistura de Britas</strong><br>
  <em>Caracterização físico-mecânica segundo a NBR 7211:2022 para Cimento Portland (<code>CP II-F-32</code>), Areia Média Natural (umidade <code>3,50 %</code>), Brita 1 Granítica e Brita 2 Granítica (massas específicas reais, massas unitárias aparentes, umidade de campo e preços em <code>R$/kg</code>). No bloco inferior, o ensaio de empacotamento granular (NBR 7211 / NM 45, recipiente <code>15,04 L</code>, massa base <code>B2 = 12,00 kg</code>) compara combinações de <code>100% / 0%</code> a <code>50% / 50%</code> e destaca em verde a proporção ótima <code>70% Brita 2 / 30% Brita 1</code> com massa unitária compactada de <code>1.689 kg/dm<sup>3</sup></code>.</em>
</p>

<p align="center">
  <img src="images/interfaces/pt_BR/Captura%20de%20tela%202026-10-26%20053600.png" alt="Aba 3 — Ensaio Piloto de Teor de Argamassa Ideal (pt-BR)" width="680"><br>
  <strong>Figura 3 (pt-BR) — Aba 3: Teor de Argamassa Ideal (&alpha;) e Adições Experimentais na Betoneira</strong><br>
  <em>Parâmetros do ensaio piloto IPT/EPUSP (<code>Traço Piloto m = 5,00</code>, massa fixa de brita <code>38,68 kg</code> e massa de água <code>5,37 kg</code>) integrados à tabela sequencial de adições na betoneira entre <code>&alpha; = 40%</code> e <code>68%</code> (passo de <code>2%</code>). A linha selecionada <code>&alpha; = 52%</code> aparece destacada em verde com o traço piloto correspondente <code>1 : 2.12 : 2.88</code>, totais acumulados e acréscimos incrementais de areia (<code>+2.69 kg</code>) e cimento (<code>+0.54 kg</code>) para atingir trabalhabilidade adequada sem exsudação.</em>
</p>

<p align="center">
  <img src="images/interfaces/pt_BR/Captura%20de%20tela%202026-10-26%20053613.png" alt="Aba 4 — Traços Experimentais e Diagrama Completo de 4 Quadrantes IPT (pt-BR)" width="680"><br>
  <strong>Figura 4 (pt-BR) — Aba 4: Traços Experimentais e Diagrama Completo de 4 Quadrantes (IPT)</strong><br>
  <em>Entrada dos três traços experimentais de laboratório — Rico (<code>1:4.0</code>, <code>a/c = 0.4400</code>, <code>fcj = 40.5 MPa</code>), Médio (<code>1:5.0</code>, <code>a/c = 0.5200</code>, <code>32.8 MPa</code>) e Pobre (<code>1:6.0</code>, <code>a/c = 0.6200</code>, <code>25.2 MPa</code>) — acompanhada das equações ajustadas de Abrams, Lyse e Molinari (com <code>R<sup>2</sup></code> e valores ótimos <code>a/c = 0.534</code>, <code>m = 5.08</code> e <code>C = 360.0 kg/m<sup>3</sup></code>) e da grade 2&times;2 interativa dos quatro quadrantes em Qt Charts com botão para exportação de imagem.</em>
</p>

<p align="center">
  <img src="images/interfaces/pt_BR/Captura%20de%20tela%202026-10-26%20053623.png" alt="Aba 4 — 1º Quadrante: Lei de Abrams ampliada (pt-BR)" width="680"><br>
  <strong>Figura 5 (pt-BR) — Aba 4 (Subaba 1º Quadrante): Lei de Abrams (fcj &times; a/c)</strong><br>
  <em>Visualização ampliada da curva de regressão de Abrams (<code>fcj = 112.5000&middot;a/c<sup>2</sup> &minus; 204.2500&middot;a/c + 108.5900</code>, <code>R<sup>2</sup> = 1.000</code>), plotando os três pontos experimentais de laboratório e as linhas tracejadas verdes que associam a resistência alvo de dosagem (<code>fcj = 31.60 MPa</code>) à relação água/cimento ótima <code>a/c = 0.534</code>.</em>
</p>

<p align="center">
  <img src="images/interfaces/pt_BR/Captura%20de%20tela%202026-10-26%20053633.png" alt="Aba 4 — 2º Quadrante: Lei de Lyse ampliada (pt-BR)" width="680"><br>
  <strong>Figura 6 (pt-BR) — Aba 4 (Subaba 2º Quadrante): Lei de Lyse (m &times; a/c)</strong><br>
  <em>Visualização ampliada da reta de regressão linear de Lyse (<code>m = 11.0656&middot;a/c &minus; 0.8279</code>, <code>R<sup>2</sup> = 0.996</code>), relacionando a relação água/cimento ao traço seco total <code>m (1:m)</code>. As guias tracejadas verdes marcam a determinação direta de <code>m ótimo = 5.08</code> a partir de <code>a/c = 0.534</code>.</em>
</p>

<p align="center">
  <img src="images/interfaces/pt_BR/Captura%20de%20tela%202026-10-26%20053644.png" alt="Aba 4 — 3º Quadrante: Lei de Molinari ampliada (pt-BR)" width="680"><br>
  <strong>Figura 7 (pt-BR) — Aba 4 (Subaba 3º Quadrante): Lei de Molinari (m &times; C)</strong><br>
  <em>Visualização ampliada da curva quadrática de Molinari (<code>m = 0.000036&middot;C<sup>2</sup> &minus; 0.0421&middot;C + 15.6273</code>, <code>R<sup>2</sup> = 1.000</code>), correlacionando o traço seco total <code>m</code> ao consumo de cimento <code>C (kg/m<sup>3</sup>)</code>. As projeções tracejadas verdes indicam o consumo de projeto <code>C = 360.0 kg/m<sup>3</sup></code> correspondente a <code>m = 5.08</code>.</em>
</p>

<p align="center">
  <img src="images/interfaces/pt_BR/Captura%20de%20tela%202026-10-26%20053653.png" alt="Aba 4 — 4º Quadrante: Consumo de Cimento versus Custo por m³ (pt-BR)" width="680"><br>
  <strong>Figura 8 (pt-BR) — Aba 4 (Subaba 4º Quadrante): Consumo de Cimento &times; Custo por m<sup>3</sup></strong><br>
  <em>Visualização ampliada da curva de sensibilidade econômica relacionando o consumo de cimento <code>C</code> (<code>250</code> a <code>499 kg/m<sup>3</sup></code>) ao custo estimado por metro cúbico em <code>R$/m<sup>3</sup></code>, com destaque tracejado verde sobre o ponto de operação adotado no projeto (<code>C = 360.0 kg/m<sup>3</sup></code>).</em>
</p>

<p align="center">
  <img src="images/interfaces/pt_BR/Captura%20de%20tela%202026-10-26%20053705.png" alt="Aba 4 — 5ª Subaba: Diagrama Unificado Clássico Abrams/Lyse/Molinari IPT (pt-BR)" width="680"><br>
  <strong>Figura 9 (pt-BR) — Aba 4 (Subaba 5º Diagrama): Ábaco Unificado Clássico IPT (Abrams / Lyse / Molinari)</strong><br>
  <em>Diagrama unificado de dosagem IPT/EPUSP em eixos ortogonais contínuos integrando simultaneamente a curva exponencial de Abrams (<code>y = 129.16e<sup>&minus;2.636x</sup></code>, <code>R<sup>2</sup> = 1.0000</code>), a reta de Lyse (<code>y = 11.066x &minus; 0.828</code>, <code>R<sup>2</sup> = 0.9959</code>) e a curva de Molinari (<code>m = 0.000036&middot;C<sup>2</sup> &minus; 0.0421&middot;C + 15.63</code>, <code>R<sup>2</sup> = 1.0000</code>), conectadas pelo caminho tracejado vermelho de leitura gráfica (<code>fcj 31.6 MPa &rarr; a/c 0.534 &rarr; m 5.081 &rarr; Ci 360 kg/m<sup>3</sup></code>).</em>
</p>

<p align="center">
  <img src="images/interfaces/pt_BR/Captura%20de%20tela%202026-10-26%20053725.png" alt="Aba 5 — Síntese do Traço Final, Consumo por m³, Betoneira e Padiolas (pt-BR)" width="680"><br>
  <strong>Figura 10 (pt-BR) — Aba 5: Traço Final, Consumo por m<sup>3</sup>, Amassada de Laboratório e Padiolas de Obra</strong><br>
  <em>Consolidação do traço unitário seco (<code>1 : 2.16 : 2.92</code>) e úmido (<code>1 : 2.24 : 2.95</code>), consumo de cimento (<code>360.0 kg/m<sup>3</sup></code>), relação <code>a/c = 0.534</code> (água efetiva <code>154.5 L/m<sup>3</sup></code>) e selo diagnóstico <code>Analítica Exata (Abrams)</code>. Inclui tabela completa por <code>1 m<sup>3</sup></code> com correção de umidade da areia (<code>3.5%</code>) e brita (<code>1.0%</code>), conversão para betoneira de laboratório (<code>29,00 Litros</code>) e tabela de canteiro por saco de <code>50 kg</code> com dimensionamento de padiolas <code>35&times;45 cm</code> (<code>1 padiola h = 48.0 cm</code> para areia úmida, <code>2 padiolas h = 32.3 cm</code> para brita úmida e <code>21.5 L</code> de água).</em>
</p>

<p align="center">
  <img src="images/interfaces/pt_BR/Captura%20de%20tela%202026-10-26%20053738.png" alt="Aba 6 — Análise de Custos e Comparativo Analítico vs. Leitura Manual de Ábaco (pt-BR)" width="680"><br>
  <strong>Figura 11 (pt-BR) — Aba 6: Análise de Custos e Comparativo Analítico &times; Leitura Manual de Ábaco</strong><br>
  <em>Configuração do volume total da obra (<code>30,00 m<sup>3</sup></code>) e entradas para comparação contra leitura manual de ábaco (<code>a/c = 0,5340</code>, <code>m = 5,08</code>, <code>C = 360,0 kg/m<sup>3</sup></code>). A tabela detalhada contrasta <code>Solução Analítica (Equações)</code>, <code>Leitura Manual (Ábaco)</code> e <code>Diferença (|&Delta;|)</code> para custos unitários por m<sup>3</sup>, custo total da obra, consumo total de cimento (<code>10801 kg</code> vs. <code>10800 kg</code>) e total de sacos de <code>50 kg</code> (<code>217</code> vs. <code>216 sacos</code>), com resumo financeiro no rodapé em <code>R$</code>.</em>
</p>

<p align="center">
  <img src="images/interfaces/pt_BR/Captura%20de%20tela%202026-10-26%20053749.png" alt="Aba 7 — Memorial de Cálculo e Relatório Técnico Exportável (pt-BR)" width="680"><br>
  <strong>Figura 12 (pt-BR) — Aba 7: Relatório Técnico e Memorial de Cálculo Experimental</strong><br>
  <em>Pré-visualização em HTML do <code>MEMORIAL DE CÁLCULO E ESTUDO DE DOSAGEM EXPERIMENTAL</code> (Cenário 1 — Concreto C25 Urbano CAA II, CP II-F-32, Slump 100 mm, obra <code>30 m<sup>3</sup></code>), reunindo destaques do traço com selo <code>CONFORME NBR 12655</code>, tabela de parâmetros de projeto e referências normativas, equações ajustadas dos quadrantes IPT e rastreabilidade do diagnóstico matemático, junto à barra de exportação rápida (<code>Copiar Texto</code>, <code>Salvar Markdown (.md)...</code>, <code>Salvar HTML...</code> e <code>Exportar PDF Oficial...</code>).</em>
</p>

---

### 4. Modelo de Duplo Licenciamento (Dual Licensing)

| Canal / Modalidade | Licença Aplicável | Liberdades e Regras Principais |
|---|---|---|
| **Código-Fonte Público (GitHub / Forks)** | **GNU AGPLv3** ([`docs/licensing.md`](docs/licensing.md)) | Plena liberdade para estudar, modificar, compilar, distribuir e adaptar livremente inclusive os documentos de [`source/assets/`](source/assets) nas suas versões derivadas, respeitando o copyleft forte da AGPLv3 e atribuição ao autor original. |
| **Binário Oficial na Microsoft Store** | **EULA Oficial + CLC** ([`EULA_pt_BR.txt`](source/assets/EULA/EULA_pt_BR.txt), [`CLC_pt_BR.txt`](source/assets/CLC/CLC_pt_BR.txt)) | Distribuição gratuita em pacote assinado pelo autor com desinstalação limpa (Política 10.2.7). Proibida republicação comercial paga do binário assinado da loja. **A Microsoft Corporation não presta suporte nem responde pelos textos em `source/assets/`.** |

---

### 5. Estrutura do Repositório

```text
Concrete_Mix_Designer_cpp/
├── CMakeLists.txt                   # Orquestração C++23 / Qt 6.12.0 (GUI, CLI, Mocks e Suíte)
├── CMakePresets.json                # Presets qt6-mingw, qt6-msvc, suite-mingw e suite-msvc
├── main.cpp                         # Ponto de entrada da aplicação gráfica Qt Widgets
├── source/
│   ├── windows_app_icon.rc.in       # Template de recurso e ícones Win32
│   ├── core/                        # Motor numérico, normas ABNT, regressão e SQLite :memory:
│   ├── cli/                         # Executável headless concrete-mix-designer-cli.exe
│   ├── gui/                         # MainWindow (7 abas), QuadChartWidget, IptUnifiedDiagramWidget e diálogos
│   ├── language/                    # GerenciadorTraducao + catálogos .ts/.qm (pt_BR e en_US)
│   ├── utils/                       # ApplicationPathUtils, LogManager, CaminhoPersistenteUtils
│   └── assets/                      # Documentos embutidos (ABOUT, CLC, COPYRIGHT, EULA, LICENSES, MANUAL, NOTICES, PRIVACY_POLICY, RELEASE) e ícones
├── tests/                           # Suíte modular abrangente com 9 alvos CTest
├── mocks/                           # find_VersionEditor.exe e compiladores de tradução .ts -> .qm
├── cmake/                           # Scripts .bat/.ps1 de compilação, distribuição limpa e limpeza de processos
├── library/                         # AppxManifest.xml, scripts Inno Setup (.iss) e prompt/especificação
├── images/interfaces/               # Galeria de capturas de tela em pt_BR/ e en_US/
└── docs/                            # Arquitetura, CLI, Conformidade NBR, Licenciamento, Privacidade, Release e Auditoria
```

---

### 6. Compilação, Testes e Empacotamento Enxuto

- **Requisitos:** Qt **6.12.0** (`Core`, `Gui`, `Widgets`, `Charts`, `Sql`, `OpenGL`, `OpenGLWidgets`, `Test`), CMake 3.24+, Ninja, **MSVC 14.52 + Windows SDK `10.0.28000.0`** e/ou **MinGW-w64 GCC 15.1.0**.
- **Tarefas no VS Code (`.vscode/tasks.json`):**
  1. `Qt6: Compilar + Executar Concrete (MinGW)`
  2. `Qt6: Compilar + Executar Concrete (MSVC)`
  3. `Qt6: Compilar + Executar Mocks (MinGW)`
  4. `Qt6: Compilar + Executar Mocks (MSVC)`
  5. `Qt6: Compilar e Executar SUITE (MinGW)`
  6. `Qt6: Compilar e Executar SUITE (MSVC)`
- **Distribuição Externa Limpa & Instaladores Inno Setup:** [`cmake/gerar_distribuicoes_limpas.ps1`](cmake/gerar_distribuicoes_limpas.ps1) gera apenas os binários e plugins estritamente necessários em `D:\MISCELANEAS\VSCode\Concrete_Mix_Designer` usando runtime **`app-local` sem `vc_redist.x64.exe`**, permitindo compilação dos instaladores enxutos via [`library/Concrete_Mix_Designer.iss`](library/Concrete_Mix_Designer.iss). A suíte de testes (`build/build_suite_{mingw,msvc}`) é executada exclusivamente via `CTest` e nunca é incluída nas distribuições externas.

---

### 7. Índice de Documentação

| Documento | Finalidade |
|---|---|
| [`MANUAL.md`](MANUAL.md) | Manual do Usuário completo e formulação de engenharia |
| [`OBSERVACAO.md`](OBSERVACAO.md) | Notas técnicas, auditoria UFPR TC031 e galeria de telas (`images/interfaces/`) |
| [`docs/licensing.md`](docs/licensing.md) | Detalhes do Duplo Licenciamento (AGPLv3 + EULA/CLC Microsoft Store) |
| [`docs/privacy-policy.md`](docs/privacy-policy.md) | Declaração Local-First e desinstalação limpa (Política 10.2.7) |
| [`docs/architecture.md`](docs/architecture.md) | Arquitetura de software em camadas (`core`, `gui`, `cli`, `tests`) |
| [`docs/cli-reference.md`](docs/cli-reference.md) | Referência completa da linha de comando e `find_VersionEditor` |
| [`docs/compliance.md`](docs/compliance.md) / [`docs/matriz_rastreabilidade_normativa.md`](docs/matriz_rastreabilidade_normativa.md) | Conformidade ABNT NBR e matriz de rastreabilidade |
| [`docs/security-model.md`](docs/security-model.md) / [`SECURITY.md`](SECURITY.md) | Modelo de segurança, escopo sanitizado e relato de vulnerabilidades |
| [`docs/technical-audit-and-roadmap.md`](docs/technical-audit-and-roadmap.md) | Auditoria técnica contínua e roadmap evolutivo |
| [`docs/release-v2026.11.22.0.md`](docs/release-v2026.11.22.0.md) | Notas de lançamento da versão atual |
| [`CONTRIBUTING.md`](CONTRIBUTING.md) / [`CODE_OF_CONDUCT.md`](CODE_OF_CONDUCT.md) | Diretrizes de contribuição e conduta da comunidade |

</details>

---

<a id="enus"></a>
<details>
<summary><h2>🇺🇸 English (United States)</h2></summary>

**Version:** `2026.11.22.0`  
**Date:** `November 22, 2026`  
**Author:** Fernando Nillsson Cidade ([`linceu_lighthouse@outlook.com`](mailto:linceu_lighthouse@outlook.com))  

---

### 1. Overview & "AS IS" Software Maturity Notice

**Concrete Mix Designer** is a native **C++23 + Qt 6.12.0** desktop and CLI engineering suite for experimental Portland cement concrete mix design using the **IPT/EPUSP Method** (Helene & Terzian, 1992). It combines binary coarse aggregate packing optimization, ordinary least-squares regression (Abrams, Lyse, Molinari), an **in-memory SQLite 3 normative database (`:memory:`)**, interactive **Qt Charts** (`QuadChartWidget`, `IptUnifiedDiagramWidget`), a full **CLI**, and technical reporting in **HTML5**, **Markdown**, and **PDF**.

> **⚠️ Free Auxiliary Software & Warranty Disclaimer:**  
> **Concrete Mix Designer** is a **free, strictly auxiliary computational aid** for education and laboratory concrete mix-design research. **The program is NOT yet mature enough for definitive production deployment or commercial construction work.** Due to potential modeling limitations, human errors, or defects in the source code (across both applied mathematics and concrete materials technology), the software is supplied **"AS IS"**, without warranty of any kind. **All outputs must be checked manually, cross-verified against established commercial software, and confirmed via physical laboratory cylinder testing (ABNT NBR 5738 and NBR 5739)** under the user's sole responsibility.

---

### 2. Core Capabilities & Normative Compliance (ABNT NBR)

- **In-Memory Relational Database (`:memory:`):** [`NormativeDatabase`](source/core/NormativeDatabase.cpp) holds volatile RAM tables protected by `QRecursiveMutex` for **ABNT NBR 12655:2022** (CAA I–IV, Preparation Conditions A/B/C), **NBR 6118:2023** (Table 7.2 — CA/CP nominal cover & $D_{\max} \le 1.2 \cdot c_{\text{nom}}$), **NBR 7211:2022**, **NBR 7212:2025**, **NBR 8953:2015**, and **NBR 16697:2018**.
- **Analytical Core (`core/`):** [`AggregateOptimizer`](source/core/AggregateOptimizer.cpp) ($15.04\text{ L}$ vessel), [`MortarContentOptimizer`](source/core/MortarContentOptimizer.cpp) (optimal $\alpha$ & pilot mixer batch), [`RegressionEngine`](source/core/RegressionEngine.cpp), [`MixDesignCalculator`](source/core/MixDesignCalculator.cpp), and [`ProjectData`](source/core/ProjectData.cpp) (6 presets `0..5` including **UFPR TC031**, `.cmdproj` JSON, and HTML5/MD/PDF reports).
- **Dynamic Bilingual I18N (`source/language/`):** Live hot-switching between **Português (BR)** (`R$`) and **English (US)** (`$`) via `.qm` catalogs with automatic Windows 11 Light/Dark synchronization across all tabs, reports, charts, and the **9-tab About window** (`CLC / Licensing`, `Copyright`, and `Full Licenses` dropdown for 11 licenses).
- **100% Local-First Privacy:** Zero telemetry, zero network requests, persisting only `QSettings("AppsCpp", "ConcreteMixDesign")` preferences and local execution logs under `%LOCALAPPDATA%\ConcreteMixDesigner\logs\` ([`docs/privacy-policy.md`](docs/privacy-policy.md#enus)).

---

### 3. Visual Interface Gallery — English (United States) (`images/interfaces/en_US/`)

Actual captures of every tab and interactive diagram in **Concrete Mix Designer** localized in **English (US)** stored under [`images/interfaces/en_US/`](images/interfaces/en_US) *(for the Portuguese UI gallery, see [Section 3 under Português (Brasil)](#galeria-ptbr))*:

<a id="galeria-enus"></a>

<p align="center">
  <img src="images/interfaces/en_US/Captura%20de%20tela%202026-10-26%20053905.png" alt="Tab 1 — Design Parameters and Mandatory Durability Standard Limits (en-US)" width="680"><br>
  <strong>Figure 1 (en-US) — Tab 1: Design Parameters &amp; Mandatory Normative Durability Limits</strong><br>
  <em>Configuration of characteristic compressive strength (<code>fck = 25.0 MPa</code>), jobsite preparation <code>Condition A (Sd = 4.0 MPa)</code>, and automatic target dosage strength calculation (<code>fcj = fck + 1.65&middot;Sd = 31.600 MPa</code>), paired with <code>Class II - Moderate (Urban)</code> environmental exposure, Reinforced Concrete (RC), target slump of <code>100 mm</code> (<code>S100</code> consistency class — NBR 8953), mandatory NBR 12655:2022 / NBR 6118:2023 durability limits (<code>w/c &le; 0.60</code>, <code>C25</code>, minimum cement <code>&ge; 280 kg/m<sup>3</sup></code>, nominal cover <code>25 mm / 30 mm</code>, aggregate size vs. cover check), and NBR 7212:2025 scale tolerances. The bottom status bar displays the green <code>NBR 12655: COMPLIANT</code> badge.</em>
</p>

<p align="center">
  <img src="images/interfaces/en_US/Captura%20de%20tela%202026-10-26%20053915.png" alt="Tab 2 — Material Properties and Coarse Aggregate Blend Optimization (en-US)" width="680"><br>
  <strong>Figure 2 (en-US) — Tab 2: Materials &amp; Coarse Aggregate Blend Optimization Test</strong><br>
  <em>Physical-mechanical input table (NBR 7211:2022) for Portland Cement (<code>CP II-F-32</code>), Fine Aggregate (field moisture <code>3.50 %</code>), Coarse Aggregate 1, and Coarse Aggregate 2 (specific gravity, bulk density, moisture content, and unit prices in <code>$/kg</code>). Below, the binary coarse-aggregate packing test (NBR 7211 / NM 45, <code>15.04 L</code> mold volume, <code>12.00 kg</code> G2 base mass) evaluates blends from <code>100% / 0%</code> down to <code>50% / 50%</code> and highlights the optimal <code>70% Gravel 2 / 30% Gravel 1</code> ratio achieving maximum compacted unit weight (<code>1.689 kg/dm<sup>3</sup></code>).</em>
</p>

<p align="center">
  <img src="images/interfaces/en_US/Captura%20de%20tela%202026-10-26%20053927.png" alt="Tab 3 — Pilot Mortar Content Optimization Sequence (en-US)" width="680"><br>
  <strong>Figure 3 (en-US) — Tab 3: Pilot Mortar Content (&alpha;) &amp; Sequence of Additions in Mixer</strong><br>
  <em>IPT/EPUSP pilot mix parameters (<code>Pilot Mix m = 5.00</code>, fixed gravel mass <code>38.68 kg</code>, water mass <code>5.37 kg</code>) paired with the incremental laboratory mixer addition table spanning <code>&alpha; = 40%</code> to <code>68%</code> in <code>2%</code> steps. The adopted <code>&alpha; = 52%</code> row is highlighted in green with its corresponding pilot mix ratio <code>1 : 2.12 : 2.88</code>, cumulative weights, and incremental sand (<code>+2.69 kg</code>) and cement (<code>+0.54 kg</code>) additions ensuring cohesion without bleeding.</em>
</p>

<p align="center">
  <img src="images/interfaces/en_US/Captura%20de%20tela%202026-10-26%20053938.png" alt="Tab 4 — Experimental Mixes and Complete 4-Quadrant IPT Diagram (en-US)" width="680"><br>
  <strong>Figure 4 (en-US) — Tab 4: Experimental Mixes &amp; Complete 4-Quadrant Diagram (IPT)</strong><br>
  <em>Laboratory data for three trial mixes — Rich (<code>1:4.0</code>, <code>w/c = 0.4400</code>, <code>fcj = 40.5 MPa</code>), Medium (<code>1:5.0</code>, <code>w/c = 0.5200</code>, <code>32.8 MPa</code>), and Lean (<code>1:6.0</code>, <code>w/c = 0.6200</code>, <code>25.2 MPa</code>) — alongside the fitted Abrams, Lyse, and Molinari equations (with <code>R<sup>2</sup></code> metrics and target values <code>w/c = 0.534</code>, <code>m = 5.08</code>, <code>C = 360.0 kg/m<sup>3</sup></code>) and the interactive 2&times;2 Qt Charts grid with chart image export.</em>
</p>

<p align="center">
  <img src="images/interfaces/en_US/Captura%20de%20tela%202026-10-26%20053947.png" alt="Tab 4 — 1st Quadrant: Abrams' Law Full View (en-US)" width="680"><br>
  <strong>Figure 5 (en-US) — Tab 4 (1st Quadrant Sub-Tab): Abrams' Law (fcj &times; w/c)</strong><br>
  <em>Expanded view of the fitted Abrams regression curve (<code>fcj = 112.5000&middot;w/c<sup>2</sup> &minus; 204.2500&middot;w/c + 108.5900</code>, <code>R<sup>2</sup> = 1.000</code>), displaying the three experimental cylinder test points and dashed green guide lines mapping target dosage strength (<code>fcj = 31.60 MPa</code>) to the optimal water-cement ratio <code>w/c = 0.534</code>.</em>
</p>

<p align="center">
  <img src="images/interfaces/en_US/Captura%20de%20tela%202026-10-26%20053957.png" alt="Tab 4 — 2nd Quadrant: Lyse's Law Full View (en-US)" width="680"><br>
  <strong>Figure 6 (en-US) — Tab 4 (2nd Quadrant Sub-Tab): Lyse's Law (m &times; w/c)</strong><br>
  <em>Expanded view of Lyse's linear regression line (<code>m = 11.0656&middot;w/c &minus; 0.8279</code>, <code>R<sup>2</sup> = 0.996</code>) correlating water-cement ratio with total dry aggregate-to-cement ratio <code>m (1:m)</code>. Dashed green guides mark the optimal dry mix ratio <code>m = 5.08</code> at <code>w/c = 0.534</code>.</em>
</p>

<p align="center">
  <img src="images/interfaces/en_US/Captura%20de%20tela%202026-10-26%20054008.png" alt="Tab 4 — 3rd Quadrant: Molinari's Law Full View (en-US)" width="680"><br>
  <strong>Figure 7 (en-US) — Tab 4 (3rd Quadrant Sub-Tab): Molinari's Law (m &times; C)</strong><br>
  <em>Expanded view of Molinari's quadratic curve (<code>m = 0.000036&middot;C<sup>2</sup> &minus; 0.0421&middot;C + 15.6273</code>, <code>R<sup>2</sup> = 1.000</code>) linking dry mix proportion <code>m</code> to cement consumption <code>C (kg/m<sup>3</sup>)</code>. Green dashed projections trace <code>m = 5.08</code> to the resulting cement content <code>C = 360.0 kg/m<sup>3</sup></code>.</em>
</p>

<p align="center">
  <img src="images/interfaces/en_US/Captura%20de%20tela%202026-10-26%20054025.png" alt="Tab 4 — 4th Quadrant: Cement Consumption versus Cost per m³ (en-US)" width="680"><br>
  <strong>Figure 8 (en-US) — Tab 4 (4th Quadrant Sub-Tab): Consumption &times; Cost per m<sup>3</sup></strong><br>
  <em>Expanded view of the cost sensitivity curve plotting estimated concrete cost per cubic meter in <code>$/m<sup>3</sup></code> across cement consumptions from <code>250</code> to <code>499 kg/m<sup>3</sup></code>, with green dashed crosshairs pinpointing the selected design point at <code>C = 360.0 kg/m<sup>3</sup></code>.</em>
</p>

<p align="center">
  <img src="images/interfaces/en_US/Captura%20de%20tela%202026-10-26%20054224.png" alt="Tab 4 — 5th Sub-Tab: Unified Classic IPT Nomogram Abrams/Lyse/Molinari (en-US)" width="680"><br>
  <strong>Figure 9 (en-US) — Tab 4 (5th Sub-Tab): Unified Classic IPT Nomogram (Abrams / Lyse / Molinari)</strong><br>
  <em>Classic IPT/EPUSP unified orthogonal nomogram simultaneously combining Abrams' exponential law (<code>y = 129.16e<sup>&minus;2.636x</sup></code>, <code>R<sup>2</sup> = 1.0000</code>), Lyse's line (<code>y = 11.066x &minus; 0.828</code>, <code>R<sup>2</sup> = 0.9959</code>), and Molinari's curve (<code>m = 0.000036&middot;C<sup>2</sup> &minus; 0.0421&middot;C + 15.63</code>, <code>R<sup>2</sup> = 1.0000</code>), traced by the red dashed projection path (<code>fcj 31.6 MPa &rarr; w/c 0.534 &rarr; m 5.081 &rarr; C 360 kg/m<sup>3</sup></code>).</em>
</p>

<p align="center">
  <img src="images/interfaces/en_US/Captura%20de%20tela%202026-10-26%20054233.png" alt="Tab 5 — Final Mix Proportioning, Consumption per m³, Laboratory Batch and Measuring Boxes (en-US)" width="680"><br>
  <strong>Figure 10 (en-US) — Tab 5: Final Mix Proportioning, 1 m<sup>3</sup> Consumption, Lab Batch &amp; Jobsite Measuring Boxes</strong><br>
  <em>Summary cards displaying the dry unit mix (<code>1 : 2.16 : 2.92</code>), wet unit mix (<code>1 : 2.24 : 2.95</code>), cement consumption (<code>360.0 kg/m<sup>3</sup></code>), <code>w/c = 0.534</code> (effective water <code>154.5 L/m<sup>3</sup></code>), and green <code>Exact Analytical (Abrams)</code> diagnostic badge. Also shown: per-cubic-meter material breakdown with moisture corrections (<code>3.5%</code> sand, <code>1.0%</code> gravel), laboratory mixer batch scaling (<code>29.00 Liters</code>), and <code>50 kg</code> cement-bag jobsite batching with <code>35&times;45 cm</code> wooden measuring boxes (<code>1 box h = 48.0 cm</code> wet sand, <code>2 boxes h = 32.3 cm</code> wet gravel, and <code>21.5 L</code> water to add).</em>
</p>

<p align="center">
  <img src="images/interfaces/en_US/Captura%20de%20tela%202026-10-26%20054243.png" alt="Tab 6 — Cost Analysis and Analytical vs. Manual Nomogram Comparison (en-US)" width="680"><br>
  <strong>Figure 11 (en-US) — Tab 6: Cost Analysis &amp; Analytical vs. Manual Nomogram Comparison</strong><br>
  <em>Jobsite volume input (<code>30.00 m<sup>3</sup></code>) and editable manual nomogram readings (<code>w/c = 0.5340</code>, <code>m = 5.08</code>, <code>C = 360.0 kg/m<sup>3</sup></code>) driving a side-by-side comparison table across <code>Analytical Solution (Equations)</code>, <code>Manual Reading (Nomogram)</code>, and <code>Difference (|&Delta;|)</code> for unit material costs, total jobsite cost, total cement mass (<code>10801 kg</code> vs. <code>10800 kg</code>), and <code>50 kg</code> cement bags (<code>217</code> vs. <code>216 bags</code>), summarized in currency-adapted impact cards (<code>$</code>) at the bottom.</em>
</p>

<p align="center">
  <img src="images/interfaces/en_US/Captura%20de%20tela%202026-10-26%20054253.png" alt="Tab 7 — Technical Report and Multi-Format Export Preview (en-US)" width="680"><br>
  <strong>Figure 12 (en-US) — Tab 7: Technical Report &amp; Experimental Concrete Mix Design Preview</strong><br>
  <em>Formatted HTML preview of the <code>TECHNICAL REPORT &amp; EXPERIMENTAL CONCRETE MIX DESIGN</code> (Scenario 1 — Urban C25 Concrete CAA II, CP II-F-32, Slump 100 mm, <code>30 m<sup>3</sup></code> jobsite) showing mix highlights with the <code>NBR 12655 COMPLIANT</code> badge, design parameters and durability table with ABNT NBR citations, fitted 4-quadrant equations, and mathematical traceability diagnostics, topped by one-click export controls (<code>Copy Text</code>, <code>Save Markdown (.md)...</code>, <code>Save HTML...</code>, and <code>Export Official PDF...</code>).</em>
</p>

---

### 4. Dual-Licensing Model

| Distribution Channel | Governing License | Key Freedoms & Terms |
|---|---|---|
| **Public Source Code (GitHub / Forks)** | **GNU AGPLv3** ([`docs/licensing.md`](docs/licensing.md#enus)) | Full freedom to inspect, modify, rebuild, redistribute, and freely alter [`source/assets/`](source/assets) documents in downstream forks, under AGPLv3 copyleft and author attribution. |
| **Official Microsoft Store Package** | **Official EULA + CLC** ([`EULA_en_US.txt`](source/assets/EULA/EULA_en_US.txt), [`CLC_en_US.txt`](source/assets/CLC/CLC_en_US.txt)) | Free signed Store distribution with clean uninstall (Policy 10.2.7). Paid commercial repackaging of the signed Store binary is prohibited. **Microsoft Corporation provides no support and holds no liability for `source/assets/` files.** |

---

### 5. Repository Layout

```text
Concrete_Mix_Designer_cpp/
├── CMakeLists.txt                   # C++23 / Qt 6.12.0 build (GUI, CLI, Mocks & Test Suite)
├── CMakePresets.json                # qt6-mingw, qt6-msvc, suite-mingw & suite-msvc presets
├── main.cpp                         # Qt Widgets GUI entry point
├── source/
│   ├── windows_app_icon.rc.in       # Win32 resource & icon template
│   ├── core/                        # Calculation engine, ABNT NBR checks, regressions & SQLite :memory:
│   ├── cli/                         # Headless concrete-mix-designer-cli.exe tool
│   ├── gui/                         # 7-tab MainWindow, QuadChartWidget, IptUnifiedDiagramWidget & dialogs
│   ├── language/                    # GerenciadorTraducao + .ts/.qm translation catalogs (pt_BR, en_US)
│   ├── utils/                       # ApplicationPathUtils, LogManager, CaminhoPersistenteUtils
│   └── assets/                      # Embedded text docs (ABOUT, CLC, COPYRIGHT, EULA, LICENSES, MANUAL, NOTICES, PRIVACY_POLICY, RELEASE) & icons
├── tests/                           # 9-target modular CTest engineering suite
├── mocks/                           # find_VersionEditor.exe and Qt .ts -> .qm translators
├── cmake/                           # Build scripts, clean external distribution generator & process helper
├── library/                         # AppxManifest.xml, Inno Setup scripts (.iss) & spec prompt
├── images/interfaces/               # Screenshot gallery in pt_BR/ and en_US/
└── docs/                            # Architecture, CLI, Compliance, Licensing, Privacy, Releases & Audit
```

---

### 6. Building, Testing & Clean External Packaging

- **Requirements:** Qt **6.12.0** (`Core`, `Gui`, `Widgets`, `Charts`, `Sql`, `OpenGL`, `OpenGLWidgets`, `Test`), CMake 3.24+, Ninja, **MSVC 14.52 + Windows SDK `10.0.28000.0`** and/or **MinGW-w64 GCC 15.1.0**.
- **VS Code Tasks (`.vscode/tasks.json`):**
  1. `Qt6: Compilar + Executar Concrete (MinGW)`
  2. `Qt6: Compilar + Executar Concrete (MSVC)`
  3. `Qt6: Compilar + Executar Mocks (MinGW)`
  4. `Qt6: Compilar + Executar Mocks (MSVC)`
  5. `Qt6: Compilar e Executar SUITE (MinGW)`
  6. `Qt6: Compilar e Executar SUITE (MSVC)`
- **Clean External Distribution & Inno Setup Installers:** [`cmake/gerar_distribuicoes_limpas.ps1`](cmake/gerar_distribuicoes_limpas.ps1) stages strictly required binaries/plugins into `D:\MISCELANEAS\VSCode\Concrete_Mix_Designer` using **`app-local` CRT DLLs without `vc_redist.x64.exe`**, feeding [`library/Concrete_Mix_Designer.iss`](library/Concrete_Mix_Designer.iss). The `tests/` suite builds solely inside `build/build_suite_{mingw,msvc}` and is **never** deployed to external distributions.

---

### 7. Documentation Index

| Document | Scope |
|---|---|
| [`MANUAL.md`](MANUAL.md#part-2-operations-manual--civil-engineering-guide-english---us) | Full user guide & civil engineering formulas |
| [`OBSERVACAO.md`](OBSERVACAO.md#enus) | Engineering notes, UFPR TC031 audit & screenshot gallery (`images/interfaces/`) |
| [`docs/licensing.md`](docs/licensing.md#enus) | Dual-Licensing terms (AGPLv3 + Store EULA/CLC) |
| [`docs/privacy-policy.md`](docs/privacy-policy.md#enus) | Local-First privacy & clean uninstall (Policy 10.2.7) |
| [`docs/architecture.md`](docs/architecture.md#enus) | Layered C++23 / Qt 6.12.0 software architecture |
| [`docs/cli-reference.md`](docs/cli-reference.md#enus) | Complete CLI & `find_VersionEditor` command reference |
| [`docs/compliance.md`](docs/compliance.md#enus) / [`docs/matriz_rastreabilidade_normativa.md`](docs/matriz_rastreabilidade_normativa.md#enus) | ABNT NBR compliance & traceability matrix |
| [`docs/security-model.md`](docs/security-model.md#enus) / [`SECURITY.md`](SECURITY.md#enus) | Security model, sanitization & disclosure policy |
| [`docs/technical-audit-and-roadmap.md`](docs/technical-audit-and-roadmap.md#enus) | Continuous technical audit & roadmap |
| [`docs/release-v2026.11.22.0.md`](docs/release-v2026.11.22.0.md#enus) | Current release notes |
| [`CONTRIBUTING.md`](CONTRIBUTING.md#enus) / [`CODE_OF_CONDUCT.md`](CODE_OF_CONDUCT.md#enus) | Contribution guidelines & community standards |

</details>
