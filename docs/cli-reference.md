# Referência de Linha de Comando e Build / CLI & Build Reference — Concrete Mix Design C++

[Português (Brasil)](#português-brasil) | [English (United States)](#english-united-states)

---

## Português (Brasil)

### 1. Executáveis Gerados pelo Projeto

O sistema de build CMake gera três executáveis nativos de 64 bits (`x64`) para Microsoft Windows (desenvolvidos exclusivamente para fins de estudo acadêmico, podendo conter possíveis erros e não servindo como software definitivo para tomadas de decisão em produção ou ambientes de trabalho sem conferência manual obrigatória e validação em ferramentas consolidadas no mercado):

1. **`ConcreteMixDesign.exe` (Aplicativo Gráfico Principal — `WIN32_EXECUTABLE`):**
   Inicializa o banco de dados normativo SQLite em memória (`NormativeDatabase::instance().initialize()`), carrega o idioma (`Configurações > Idioma`) e os metadados persistidos do usuário (`QSettings("AppsCpp", "ConcreteMixDesign")`) e abre a interface gráfica completa de 7 abas sem janela de console auxiliar.
2. **`concrete-mix-designer-cli.exe` (Interface de Linha de Comando Profissional CLI):**
   Utilitário CLI standalone em console nativo que permite automatizar dosagens ABNT NBR 6118 / NBR 8953 / NBR 12655, executar cálculos volumétricos de $1\text{ m}^3$, auditar conformidade normativa, consultar tabelas normativas, rodar predefinições industriais (C20 a C50), emitir relatórios técnicos nos formatos Texto, JSON, Markdown e HTML, e disparar diagnósticos e suítes de testes automatizados com códigos de saída padronizados.
3. **`verify_calculations.exe` (Suíte de Verificação Analítica e Normativa via Testes Unitários):**
   Executável de console automatizado (definido em [`tests/verify_calculations.cpp`](../tests/verify_calculations.cpp)) que valida em linha de comando a integridade do banco de dados normativo ABNT NBR, o ajuste das equações de Abrams, Lyse e Molinari, o fechamento volumétrico de $1\text{ m}^3$, os cálculos de amassadas e custos e a reprodução exata dos cenários de referência (incluindo o **Cenário 5** da planilha UFPR TC031).

---

### 2. Manual da CLI (`concrete-mix-designer-cli.exe`)

#### 2.1. Sintaxe Geral
```powershell
concrete-mix-designer-cli.exe [comando] [opcoes...]
```

#### 2.2. Opções Globais
| Opção | Descrição | Valores / Padrão |
| :--- | :--- | :--- |
| `-h`, `--help` | Exibe a ajuda e sintaxe detalhada dos comandos. | — |
| `-v`, `--version` | Exibe a versão oficial do binário e do kernel de cálculo. | — |
| `--lang <pt\|en>` | Define o idioma de saída das mensagens e relatórios. | `pt` (padrão) ou `en` |
| `--format <formato>` | Formato da saída gerada. | `text` (padrão), `json`, `markdown`, `html` |
| `--out <arquivo>` | Redireciona a saída para um arquivo em disco. | Caminho relativo ou absoluto |
| `--dark` | Aplica estilo de cores escuro em relatórios HTML gerados. | — |
| `--no-banner` | Suprime a exibição do banner ASCII de cabeçalho. | — |

#### 2.3. Comandos Disponíveis

##### A. `design` — Dimensionamento e dosagem completa de traço
Executa o fluxo normativo completo: determinação de $f_{cj}$, relação $a/c$ (curva de Abrams ou limites de durabilidade NBR 6118), consumo de água pelo abatimento (slump), proporção agregado miúdo/graúdo e fechamento volumétrico de $1\text{ m}^3$.
```powershell
# Exemplo básico: fck 30 MPa, abatimento 120 mm, CAA III (industrial)
concrete-mix-designer-cli.exe design --fck 30 --slump 120 --caa III --cond A

# Exemplo com saída JSON e salvamento em arquivo
concrete-mix-designer-cli.exe design --fck 35 --slump 100 --caa II --format json --out mix_c35.json
```
Parâmetros do comando `design`:
- `--fck <valor>`: Resistência característica à compressão aos 28 dias em MPa (padrão: 25.0).
- `--slump <valor>`: Abatimento do tronco de cone em mm (padrão: 100.0).
- `--caa <I|II|III|IV>`: Classe de Agressividade Ambiental NBR 6118 (padrão: II).
- `--cond <A|B|C>`: Condição de preparo da NBR 12655 (A: $s_d=4.0$, B: $s_d=5.5$, C: $s_d=7.0$).
- `--cement-rho <valor>`: Massa específica do cimento em $\text{g/cm}^3$ (padrão: 3.10).
- `--sand-rho <valor>`: Massa específica da areia em $\text{g/cm}^3$ (padrão: 2.62).
- `--gravel-rho <valor>`: Massa específica da brita em $\text{g/cm}^3$ (padrão: 2.68).
- `--price-cement <v>`: Custo do cimento por kg (padrão: 0.70).
- `--price-sand <v>`: Custo da areia por kg (padrão: 0.08).
- `--price-gravel <v>`: Custo da brita por kg (padrão: 0.09).

##### B. `calculate` (ou `calc`) — Fechamento volumétrico para traço fornecido
Calcula os consumos unitários por metro cúbico ($C$, $A$, $B$, $Ag$), teor de argamassa ($\alpha$) e custo total por $\text{m}^3$.
```powershell
# Traço 1 : 1.80 : 2.70 com a/c = 0.48
concrete-mix-designer-cli.exe calc --m 4.5 --ac 0.48 --alpha 52.0
```

##### C. `verify` (ou `audit`) — Auditoria de conformidade normativa
Analisa um traço em relação aos requisitos das normas ABNT NBR 6118:2023, NBR 8953:2015 e NBR 12655:2022.
```powershell
concrete-mix-designer-cli.exe verify --fck 30 --caa III --ac 0.48 --cement-content 360
```

##### D. `report` — Geração de relatório técnico completo
Gera documentação técnica oficial de engenharia nos formatos texto, Markdown ou HTML profissional.
```powershell
# Relatório completo em HTML com tema escuro em inglês
concrete-mix-designer-cli.exe report --project "Ponte Sul" --engineer "Dr. Silva" --fck 35 --slump 100 --caa II --format html --dark --out relatorio_ponte.html --lang en
```

##### E. `standards` — Consulta ao banco de dados normativo ABNT NBR
Exibe tabelas normativas integradas em SQLite:
```powershell
# Listar classes de agressividade ambiental (NBR 6118 Tabela 6.1 e 7.1)
concrete-mix-designer-cli.exe standards caa

# Listar tipos normalizados de cimento Portland (NBR 16697)
concrete-mix-designer-cli.exe standards cements

# Listar condições de preparo e desvios-padrão (NBR 12655)
concrete-mix-designer-cli.exe standards prep
```

##### F. `presets` — Demonstração e execução de traços de referência
Permite consultar e executar diretamente traços típicos padronizados:
```powershell
# Listar todos os presets disponíveis
concrete-mix-designer-cli.exe presets

# Executar simulação do preset 3 (C30 Estrutural)
concrete-mix-designer-cli.exe presets --run 3
```

##### G. `test` — Diagnóstico interno e verificação analítica
Dispara a bateria de validação analítica das regressões e fechamentos de massa:
```powershell
concrete-mix-designer-cli.exe test
```

---

### 3. Comandos de Compilação e Empacotamento

#### 3.1. Compilação Automatizada com MSVC 64-bit
```powershell
cmd.exe /c "cmake\build_msvc.bat"
```

#### 3.2. Compilação Automatizada com MinGW 64-bit
```powershell
cmd.exe /c "cmake\build_mingw.bat"
```

#### 3.3. Geração de Distribuições Limpas e Portáteis
```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File cmake\gerar_distribuicoes_limpas.ps1
```

---

## English (United States)

### 1. Project Generated Executables

The CMake build system produces three native 64-bit (`x64`) executables for Microsoft Windows (developed exclusively for academic study purposes, which may contain possible errors and do not serve as definitive software for decision-making in production or work environments without mandatory manual verification and validation in consolidated market tools):

1. **`ConcreteMixDesign.exe` (Main Graphical Application — `WIN32_EXECUTABLE`):**
   Initializes the in-memory SQLite normative database (`NormativeDatabase::instance().initialize()`), loads the user's persisted language (`Settings > Language`) and project metadata (`QSettings("AppsCpp", "ConcreteMixDesign")`), and launches the complete 7-tab graphical interface without an auxiliary console window.
2. **`concrete-mix-designer-cli.exe` (Professional Command Line Interface CLI):**
   Native standalone console CLI tool that automates concrete mix designs under ABNT NBR 6118 / NBR 8953 / NBR 12655, performs $1\text{ m}^3$ volumetric closures, audits normative durability criteria, queries standards database tables, executes industrial presets (C20 to C50), generates technical engineering reports in Plain Text, JSON, Markdown, and HTML formats, and triggers diagnostic tests with standardized exit codes.
3. **`verify_calculations.exe` (CLI Analytical and Normative Unit Test Verification Suite):**
   Automated console executable (defined in [`tests/verify_calculations.cpp`](../tests/verify_calculations.cpp)) that validates via command line the integrity of the ABNT NBR normative database, curve fitting of Abrams', Lyse's, and Molinari's equations, $1\text{ m}^3$ volumetric closure, batch and cost calculations, and exact reproduction of reference scenarios (including **Scenario 5** from the UFPR TC031 spreadsheet).

---

### 2. CLI User Manual (`concrete-mix-designer-cli.exe`)

#### 2.1. General Syntax
```powershell
concrete-mix-designer-cli.exe [command] [options...]
```

#### 2.2. Global Options
| Option | Description | Values / Default |
| :--- | :--- | :--- |
| `-h`, `--help` | Show command line syntax and help reference. | — |
| `-v`, `--version` | Display application binary and calculation engine version. | — |
| `--lang <pt\|en>` | Output language for text messages and reports. | `pt` (default) or `en` |
| `--format <format>` | Output format. | `text` (default), `json`, `markdown`, `html` |
| `--out <file>` | Write output directly to a file on disk. | Relative or absolute path |
| `--dark` | Enable dark theme styling in exported HTML reports. | — |
| `--no-banner` | Suppress ASCII banner header. | — |

#### 2.3. Available Commands

##### A. `design` — Automated Mix Design Workflow
Executes full design calculations: determines target compressive strength $f_{cj}$, water-cement ratio $w/c$ (Abrams curve or NBR 6118 durability limits), water demand by slump, sand/gravel proportion, and $1\text{ m}^3$ volumetric closure.
```powershell
# Basic example: fck 30 MPa, slump 120 mm, CAA III (industrial)
concrete-mix-designer-cli.exe design --fck 30 --slump 120 --caa III --cond A

# Example with JSON output and saving to file
concrete-mix-designer-cli.exe design --fck 35 --slump 100 --caa II --format json --out mix_c35.json
```
Parameters for the `design` command:
- `--fck <value>`: 28-day characteristic compressive strength in MPa (default: 25.0).
- `--slump <value>`: Slump cone consistency in mm (default: 100.0).
- `--caa <I|II|III|IV>`: Environmental Aggressiveness Class per NBR 6118 (default: II).
- `--cond <A|B|C>`: Preparation condition per NBR 12655 (A: $s_d=4.0$, B: $s_d=5.5$, C: $s_d=7.0$).
- `--cement-rho <value>`: Specific gravity of cement in $\text{g/cm}^3$ (default: 3.10).
- `--sand-rho <value>`: Specific gravity of sand in $\text{g/cm}^3$ (default: 2.62).
- `--gravel-rho <value>`: Specific gravity of gravel in $\text{g/cm}^3$ (default: 2.68).
- `--price-cement <v>`: Unit cost of cement per kg (default: 0.70).
- `--price-sand <v>`: Unit cost of sand per kg (default: 0.08).
- `--price-gravel <v>`: Unit cost of gravel per kg (default: 0.09).

##### B. `calculate` (or `calc`) — Volumetric Closure for Given Mix
Calculates unit consumptions per cubic meter ($C$, $W$, Sand, Gravel), mortar ratio ($\alpha$), and total cost per $\text{m}^3$.
```powershell
# Mix 1 : 1.80 : 2.70 with w/c = 0.48
concrete-mix-designer-cli.exe calc --m 4.5 --ac 0.48 --alpha 52.0
```

##### C. `verify` (or `audit`) — Normative Compliance Audit
Audits a concrete mix against ABNT NBR 6118:2023, NBR 8953:2015, and NBR 12655:2022 durability and strength limits.
```powershell
concrete-mix-designer-cli.exe verify --fck 30 --caa III --ac 0.48 --cement-content 360
```

##### D. `report` — Comprehensive Technical Report Generation
Generates complete engineering documentation in text, Markdown, or HTML formats with dynamic currency support.
```powershell
# Complete HTML report with dark theme in English
concrete-mix-designer-cli.exe report --project "Main Bridge" --engineer "Dr. Silva" --fck 35 --slump 100 --caa II --format html --dark --out bridge_report.html --lang en
```

##### E. `standards` — Query Normative Database
Inspects built-in ABNT NBR SQLite database tables:
```powershell
# List environmental aggressiveness classes (NBR 6118 Table 6.1 and 7.1)
concrete-mix-designer-cli.exe standards caa

# List standardized Portland cement types (NBR 16697)
concrete-mix-designer-cli.exe standards cements

# List preparation conditions and standard deviations (NBR 12655)
concrete-mix-designer-cli.exe standards prep
```

##### F. `presets` — Industry Mix Presets
Lists or executes standardized industrial mix designs:
```powershell
# List all available presets
concrete-mix-designer-cli.exe presets

# Run simulation for preset 3 (C30 Structural)
concrete-mix-designer-cli.exe presets --run 3
```

##### G. `test` — Analytical Regression and Closure Diagnostic
Runs embedded analytical regression checks:
```powershell
concrete-mix-designer-cli.exe test
```

---

### 3. Build and Packaging Commands

#### 3.1. Automated Build with MSVC 64-bit
```powershell
cmd.exe /c "cmake\build_msvc.bat"
```

#### 3.2. Automated Build with MinGW 64-bit
```powershell
cmd.exe /c "cmake\build_mingw.bat"
```

#### 3.3. Clean and Portable Distribution Generation
```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File cmake\gerar_distribuicoes_limpas.ps1
```
