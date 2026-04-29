# Trabalho M1.2 - 3D e Arquivos .obj

Arquivo fonte:
- `diegosilva_mirunaonofrei_desenvolvimento_1_2_3d_obj.cpp`

## Codigos de inspiracao (aulas)

- `exemplos_aulas/Aula_5/C04_carregarobj.cpp` — estrutura do parser de .obj e uso de Display Lists;
- `exemplos_aulas/C05_iluminacao.cpp` — configuracao de `GL_LIGHTING`, `glLightfv`, materiais;
- `exemplos_aulas/C06_Texturas.cpp` — leitura de arquivos externos, `glTexCoord2f`;
- `diegosilva_mirunaonofrei_desenvolvimento_1_1_funcoes_basicas.cpp` — arquitetura de callbacks GLUT, timer, reshape, estilo de codigo.

## Objetivo atendido

Este trabalho expande o M1.1 para os seguintes requisitos:

| Requisito | Status |
|---|---|
| Eventos de teclado e mouse para translacao, rotacao e escala | Implementado |
| Uso de `glTranslate`, `glRotate`, `glScale` | Implementado |
| Leitura de .obj com vertices, normais, texcoords e faces | Implementado |
| Renderizacao como faces preenchidas (`GL_TRIANGLES`) | Implementado |
| Normais consideradas na iluminacao por vertice | Implementado |
| Pelo menos 3 luzes com componentes ambiente, difusa e especular | Implementado (3 luzes) |
| Luzes independentemente ativaveis/desativaveis | Implementado (teclas 1, 2, 3) |

## Como compilar e executar

### Dependencia
- **freeglut** instalado no sistema (headers em `GL/freeglut.h`, biblioteca `freeglut.lib` ou `libfreeglut.a`).

### Linux / macOS
```bash
g++ diegosilva_mirunaonofrei_desenvolvimento_1_2_3d_obj.cpp \
    -o trabalho1_2 -lGL -lGLU -lglut -lm
./trabalho1_2 data/mba1.obj          # ou outro .obj como argumento
```

### Windows (MSYS2 — MINGW64 ou UCRT64)

Abra o terminal correto para o ambiente instalado:
- **MSYS2 MINGW64**: instale `mingw-w64-x86_64-gcc` e `mingw-w64-x86_64-freeglut`
- **MSYS2 UCRT64**: instale `mingw-w64-ucrt-x86_64-gcc` e `mingw-w64-ucrt-x86_64-freeglut` *(ja instalados)*

```bash
# MINGW64 (se ainda nao instalou)
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-freeglut

# compilar (bash: continuacao de linha com \, nao ^)
g++ diegosilva_mirunaonofrei_desenvolvimento_1_2_3d_obj.cpp \
    -o trabalho1_2.exe -lfreeglut -lopengl32 -lglu32
./trabalho1_2.exe data/mba1.obj
```

### Windows (MSVC via Developer Prompt)
```bash
cl diegosilva_mirunaonofrei_desenvolvimento_1_2_3d_obj.cpp ^
   /I"C:\freeglut\include" ^
   /link /LIBPATH:"C:\freeglut\lib\x64" freeglut.lib opengl32.lib glu32.lib
trabalho1_2.exe data\mba1.obj
```

O caminho do `.obj` pode ser passado como argumento; sem argumento carrega `data/mba1.obj` por padrao.

### Modelos disponiveis em `data/`

| Arquivo | Descricao |
|---|---|
| `mba1.obj` | modelo usado no exemplo das aulas (`C04_carregarobj.cpp`) — padrao |
| `elepham.obj` | elefante |
| `porsche.obj` | carro Porsche |
| `radar.obj` | antena de radar |
| `teddy.obj` | urso de pelucia |

Exemplo trocando o modelo:
```bash
trabalho1_2.exe data\porsche.obj
```

---

## Controles interativos

| Tecla / Acao | Efeito |
|---|---|
| `W` / `S` | Translacao Y+ / Y- |
| `A` / `D` | Translacao X- / X+ |
| `Q` / `E` | Translacao Z- / Z+ |
| Setas | Translacao X/Y |
| `Page Up` / `Page Down` | Translacao Z |
| `x` / `X` | Rotacao em X (+/-) |
| `y` / `Y` | Rotacao em Y (+/-) |
| `z` / `Z` | Rotacao em Z (+/-) |
| `+` / `-` | Escala uniforme (+/-) |
| `1` | Liga/desliga Luz 0 (branca) |
| `2` | Liga/desliga Luz 1 (vermelha) |
| `3` | Liga/desliga Luz 2 (azul) |
| `R` | Reseta todas as transformacoes |
| Arrastar (botao esq.) | Orbita o modelo (yaw/pitch) |
| `ESC` | Encerra o programa |

---

## Como o codigo foi estruturado

O mesmo estilo do M1.1 foi mantido: structs para dados, funcoes separadas para cada responsabilidade, callbacks GLUT e variaveis globais de estado.

### Structs principais

| Struct | Descricao |
|---|---|
| `IndiceVertice` | Tripla de indices (posicao / texcoord / normal), com -1 para ausentes |
| `Face` | Tres `IndiceVertice` formando um triangulo |
| `ModeloObj` | Vetores de vertices, texcoords, normais e faces; Display List; AABB |
| `Luz` | ID OpenGL, flag ativa, e os quatro vetores de cor (amb, dif, esp, pos) |
| `EstadoCena` | Angulos, escala e translacao acumulados pelo usuario |

### Fluxo de carga do modelo

```
carregar_obj()
    -> parse linha a linha (v / vt / vn / f)
    -> faces com N > 3 vertices: fan triangulation
normalizar_modelo()
    -> calcula AABB -> centro e fator de escala para auto-enquadramento
compilar_display_list()
    -> para cada face: emite normal, texcoord e vertice
    -> se ivn == -1: calcula normal da face por produto vetorial (fallback)
```

### Pipeline de iluminacao

- `GL_LIGHTING` ativado globalmente.
- Tres luzes (`GL_LIGHT0/1/2`) com componentes independentes: `GL_AMBIENT`, `GL_DIFFUSE`, `GL_SPECULAR`.
- Posicoes re-emitidas a cada frame **apos** `gluLookAt` e **antes** das transformacoes do modelo, fixando as luzes no espaco do mundo.
- Material do modelo: cinza neutro para evidenciar as tres cores de luz.
- `GL_NORMALIZE` ativado: evita artefatos de iluminacao quando `glScale` altera o comprimento das normais.

---

## Escolhas e justificativas

### 1. Por que `glTranslate / glRotate / glScale` (diferente do M1.1)?

O enunciado do M1.2 permite — e incentiva — o uso dos comandos de modelagem do OpenGL. Alem de simplificar o codigo de transformacao, o uso de `glScale` com `GL_NORMALIZE` garante que as normais continuem corretas para iluminacao, o que seria mais trabalhoso com transformacao manual.

### 2. Fan triangulation para faces poligonais

O formato .obj permite faces com qualquer numero de vertices. A estrategia de fan (vertice 0 compartilhado com cada par consecutivo) e simples, correta para faces convexas e compativel com modelos reais exportados por Blender ou similares.

### 3. Calculo de normais como fallback

Muitos arquivos .obj de exemplos didaticos nao incluem normais (`vn`). A funcao `calcular_normal_face()` computa o produto vetorial das arestas e normaliza o resultado, garantindo que a iluminacao funcione mesmo nesses casos — a normal e emitida para os tres vertices da face (equivale a flat shading dentro de cada triangulo).

### 4. Normalizacao automatica do modelo (AABB)

O arquivo `mba1.obj` do exemplo das aulas precisava de `glScalef(0.4,0.4,0.4)` e `glTranslatef(0,-40,-105)` codificados manualmente. `normalizar_modelo()` calcula o AABB e define um fator de escala para que qualquer modelo fique visivel sem ajuste manual, tornando o codigo reutilizavel com outros .obj.

### 5. Tres luzes com cores distintas

A combinacao key light (branca) + fill light (vermelha) + rim light (azulada) e um padrao clasico de iluminacao de cena 3D. As cores diferentes tornam cada componente visualmente distinguivel ao desativar as luzes individualmente, o que facilita entender o efeito de cada uma.

### 6. Display List

A geometria e compilada uma unica vez em uma Display List OpenGL. Em vez de remontar os `glBegin/glEnd` a cada frame, a GPU reutiliza a lista compilada — o que e relevante para modelos com dezenas de milhares de faces como o `mba1.obj`.

### 7. Orbita por mouse

O arrasto do botao esquerdo acumula deltas de pixel convertidos em graus de rotacao em X (pitch) e Y (yaw). Essa e a forma mais intuitiva de inspecionar um modelo 3D e complementa os controles de teclado.

### 8. HUD de estado

Um overlay 2D (modo ortogonal) exibe quais luzes estao ativas, com a cor correspondente. Isso elimina a necessidade de olhar o console para saber o estado atual.

---

## Limitacoes conhecidas

- **Materiais `.mtl`**: o parser ignora `mtllib` e `usemtl`. Materiais do arquivo nao sao carregados; o modelo usa material fixo cinza.
- **Texturas**: as coordenadas `vt` sao enviadas ao pipeline mas nenhuma textura e carregada. Para habilitar, basta carregar uma imagem com `glTexImage2D` e chamar `glEnable(GL_TEXTURE_2D)` antes do `glCallList`.
- **Faces concavas**: a fan triangulation pode gerar triangulos incorretos para faces nao convexas. Para modelos exportados por ferramentas padrao isso raramente ocorre.
