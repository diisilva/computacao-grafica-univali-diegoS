# Trabalho: Shaders na Godot 4
**Disciplina:** Computação Gráfica — UNIVALI  
**Autor:** Diego Silva  

---

## Visão Geral

Este trabalho implementa **3 shaders diferentes** na Godot 4 usando a linguagem **GDShader (shader_type spatial)**. Cada shader é aplicado em um objeto 3D diferente na mesma cena.

| Shader | Objeto | Efeito |
|---|---|---|
| `shader_01_oceano_ondulante.gdshader` | PlaneMesh (plano subdivido) | Ondas de água animadas |
| `shader_02_dissolucao_lava.gdshader` | SphereMesh (esfera) | Superfície de lava com dissolução por ruído |
| `shader_03_cristal_pulsante.gdshader` | BoxMesh (caixa/cristal) | Cristal que pulsa e emite luz |

---

## Cobertura dos Requisitos

| Requisito | Shader que cobre |
|---|---|
| ✅ 3 shaders diferentes em objetos diferentes | Todos os 3 |
| ✅ 2+ com `vertex` alterando posição ou normal | Shader 1 (posição Y), Shader 2 (posição + normal), Shader 3 (posição) |
| ✅ 2+ com `fragment` | Todos os 3 |
| ✅ 1+ com `fragment` alterando `METALLIC` ou `ROUGHNESS` | Shader 1 (ROUGHNESS + METALLIC), Shader 2 (ROUGHNESS), Shader 3 (METALLIC + ROUGHNESS) |
| ✅ 1+ com `TIME` | Shader 1 e Shader 3 |
| ✅ 1+ com textura e/ou ruído | Shader 2 (sampler2D de ruído) |
| ✅ 1+ com `varying` (vertex → fragment) | Shader 1 (`varying float fator_altura`), Shader 3 (`varying float fator_pulso`) |
| ✅ 1+ com `uniform` | Todos os 3 |

---

## Pré-requisitos

- **Godot Engine 4.x** (recomendado 4.2 ou superior)  
  Download: https://godotengine.org/download  
- Os arquivos `.gdshader` desta pasta são copiados diretamente para dentro do projeto Godot.

---

## Passo a Passo — Criação do Projeto no Godot

### 1. Criar o projeto

1. Abra o **Godot Engine 4**
2. Clique em **New Project**
3. Dê o nome `trabalho_shaders`, escolha uma pasta e clique em **Create & Edit**

---

### 2. Configurar a cena

1. No menu superior clique em **Scene → New Scene**
2. Clique em **3D Scene** — isso cria um nó `Node3D` raiz
3. Renomeie o nó raiz para `CenaShaders` (clique com botão direito → Rename)

---

### 3. Adicionar iluminação e câmera

Na barra de cena (painel superior-esquerdo), adicione os seguintes nós filhos ao `CenaShaders`:

- **DirectionalLight3D** — iluminação principal
  - `rotation_degrees`: X = -45, Y = 30
  - `shadow_enabled`: true
- **Camera3D** — câmera da cena
  - `position`: X = 0, Y = 4, Z = 10
  - `rotation_degrees`: X = -20

---

### 4. Adicionar os 3 objetos com MeshInstance3D

Para **cada objeto**, clique com botão direito no `CenaShaders` → **Add Child Node** → `MeshInstance3D`.

#### Objeto 1 — Plano do Oceano

- **Nó:** `MeshInstance3D`, renomeie para `OceanoPlano`
- **Mesh:** clique em `[empty]` ao lado de Mesh → `New PlaneMesh`
  - `size`: X = 10, Z = 10
  - `subdivide_width`: 50
  - `subdivide_depth`: 50
- **Position:** X = -5, Y = 0, Z = 0

#### Objeto 2 — Esfera de Lava

- **Nó:** `MeshInstance3D`, renomeie para `EsferaLava`
- **Mesh:** `New SphereMesh`
  - `radius`: 1.5
  - `height`: 3.0
  - `radial_segments`: 64
  - `rings`: 32
- **Position:** X = 3, Y = 1.5, Z = 0

#### Objeto 3 — Cristal

- **Nó:** `MeshInstance3D`, renomeie para `CristalPulsante`
- **Mesh:** `New BoxMesh`
  - `size`: X = 1.5, Y = 2.5, Z = 1.5
- **Position:** X = 6.5, Y = 1.5, Z = 0

---

### 5. Copiar os arquivos .gdshader para o projeto

1. No **Explorador de Arquivos do Windows**, navegue até:  
   `D:\UNivali\comp_grafica\computacao-grafica-univali-diegoS\trabalhos\trabalho_shaders_godot\shaders\`
2. Copie os 3 arquivos `.gdshader`
3. Cole dentro da pasta `res://` do projeto Godot (arraste para o painel **FileSystem** no Godot, ou cole direto na pasta do projeto)

---

### 6. Aplicar os shaders nos materiais

Para **cada MeshInstance3D**, siga os passos:

1. Selecione o nó (ex: `OceanoPlano`)
2. No painel **Inspector**, em **Surface Material Override**, clique em `[empty]` → **New ShaderMaterial**
3. Clique no `ShaderMaterial` que foi criado
4. No campo **Shader**, clique em `[empty]` → **Load** → selecione o arquivo `.gdshader` correspondente

#### Tabela de associação

| Nó | Arquivo do Shader |
|---|---|
| `OceanoPlano` | `shader_01_oceano_ondulante.gdshader` |
| `EsferaLava` | `shader_02_dissolucao_lava.gdshader` |
| `CristalPulsante` | `shader_03_cristal_pulsante.gdshader` |

---

### 7. Configurar o uniform de ruído do Shader 2

O Shader 2 (lava) usa um `sampler2D` de ruído. É necessário criar a textura dentro do Godot:

1. Selecione o nó `EsferaLava`
2. No **Inspector**, clique no `ShaderMaterial` → clique no shader carregado
3. Nos parâmetros do shader, você verá o campo **ruido** (aparece como slot de textura)
4. Clique no campo `ruido` → **New NoiseTexture2D**
5. Clique na `NoiseTexture2D` criada e configure:
   - Clique em **Noise** → **New FastNoiseLite**
   - `noise_type`: Cellular (ou Simplex — experimente!)
   - `frequency`: 0.05
   - `width` e `height`: 512
6. Ajuste o `limiar_dissolucao` para 0.3 para ver o efeito de dissolução

---

### 8. Ajustar os uniforms de cada shader

Após aplicar cada shader, os parâmetros ficam visíveis no **Inspector** do ShaderMaterial. Sugestões de valores iniciais:

**Shader 1 — Oceano:**
| Parâmetro | Valor sugerido |
|---|---|
| amplitude | 0.4 |
| frequencia | 1.5 |
| velocidade | 1.2 |
| cor_rasa | Azul claro (#0099CC) |
| cor_profunda | Azul escuro (#001A66) |

**Shader 2 — Lava:**
| Parâmetro | Valor sugerido |
|---|---|
| limiar_dissolucao | 0.25 |
| cor_quente | Laranja (#FF4400) |
| cor_fria | Vermelho escuro (#330000) |
| ruido | NoiseTexture2D (como descrito acima) |

**Shader 3 — Cristal:**
| Parâmetro | Valor sugerido |
|---|---|
| velocidade_pulso | 2.0 |
| intensidade_pulso | 0.12 |
| cor_cristal | Ciano transparente (#88DDFF com alpha 0.85) |
| cor_emissao | Azul elétrico (#4499FF) |

---

### 9. Salvar a cena

- **Ctrl+S** → salvar como `cena_shaders.tscn`

---

### 10. Rodar o projeto

- Pressione **F5** ou clique no botão ▶ **Play**
- A câmera mostrará os 3 objetos com seus shaders rodando em tempo real

---

## Estrutura de Arquivos do Projeto Godot

Após a configuração, o projeto terá a estrutura:

```
trabalho_shaders/          (pasta do projeto Godot)
├── project.godot
├── cena_shaders.tscn
├── shader_01_oceano_ondulante.gdshader
├── shader_02_dissolucao_lava.gdshader
└── shader_03_cristal_pulsante.gdshader
```

---

## Descrição Técnica dos Shaders

### Shader 1 — Oceano Ondulante

**Conceitos utilizados:**
- `varying float fator_altura` — passa a altura calculada do vértice para o fragment
- `uniform` — parâmetros de amplitude, frequência, velocidade e cores
- `TIME` — animação contínua das ondas
- `VERTEX.y +=` — deslocamento da posição dos vértices no eixo Y
- `ROUGHNESS`, `METALLIC` — simula superfície de água (pouca rugosidade)
- `EMISSION` — brilho leve nas cristas das ondas

O shader calcula dois sinusoides defasados (X e Z) para criar um padrão de onda 2D.  
A altura resultante é passada via `varying` para o fragment, que a usa para misturar  
a cor rasa (azul claro) com a cor profunda (azul escuro).

---

### Shader 2 — Dissolução de Lava

**Conceitos utilizados:**
- `uniform sampler2D ruido` — textura de ruído gerada pela NoiseTexture2D do Godot
- `discard` — descarta fragmentos com base no valor do ruído, criando dissolução
- `NORMAL +=` — perturba as normais no vertex para dar aspecto irregular
- `VERTEX +=` — deslocamento de posição baseado no ruído
- `EMISSION` — brilho laranja intenso nas bordas da dissolução
- `ROUGHNESS` — superfície rugosa de rocha/lava

O valor de ruído amostrado é comparado com o `limiar_dissolucao`: pixels abaixo  
do limiar são descartados com `discard`. A borda da dissolução recebe emissão  
laranja forte, criando o efeito de lava derretendo a rocha.

---

### Shader 3 — Cristal Pulsante

**Conceitos utilizados:**
- `varying float fator_pulso` — passa o fator de pulsação do vertex para o fragment
- `TIME` — oscilação contínua com `sin(TIME * velocidade_pulso)`
- `VERTEX += NORMAL *` — expande/contrai a malha ao longo das normais
- `METALLIC = 0.9` — material altamente reflexivo (cristal metálico)
- `ROUGHNESS` — varia com o pulso (mais suave nas cristas)
- `EMISSION` — emissão intensa que pulsa sincronizada com o vertex

O fator de pulso é calculado uma vez no `vertex()` e repassado via `varying`  
ao `fragment()`, garantindo sincronização perfeita entre deformação e emissão.

---

## Dicas e Resolução de Problemas

| Problema | Solução |
|---|---|
| Shader 1 sem ondas visíveis | Verifique se o PlaneMesh tem `subdivide_width` e `subdivide_depth` ≥ 30 |
| Shader 2 sem dissolução | Verifique se o `ruido` uniform tem uma `NoiseTexture2D` atribuída |
| Shader 3 sem transparência | Em `ShaderMaterial`, o `render_mode blend_mix` é necessário — já está no shader |
| Objetos totalmente pretos | Adicione um `DirectionalLight3D` ou `WorldEnvironment` na cena |
| Shader não compila | Verifique erros no painel **Output** do Godot (parte inferior da tela) |
