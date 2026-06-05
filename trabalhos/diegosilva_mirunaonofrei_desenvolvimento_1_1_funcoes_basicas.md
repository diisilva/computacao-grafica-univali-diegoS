# Trabalho 1.1 - Funcoes Basicas 3D (Cubo Wireframe)

Arquivo fonte:
- `diegosilva_mirunaonofrei_desenvolvimento_1_1_funcoes_basicas.cpp`

## Codigos de inspiracao (aulas)
Este trabalho foi inspirado principalmente nos seguintes codigos-base da disciplina:
- `exemplos_aulas/C01_Exemplo.cpp` (estrutura inicial com GLUT e callbacks);
- `exemplos_aulas/Aula_2` (organizacao do projeto e fluxo de renderizacao);
- `exemplos_aulas/Aula_3/C03_teste3d.cpp` (contexto 3D e uso de projecao);
- `exemplos_aulas/Aula_4/ComputacaoGrafica - Funcoes2D.cpp` (funcoes de transformacao manual);
- `exemplos_aulas/Aula_5/C04_carregarobj.cpp` (padrao de estruturacao e manipulacao geometrica).

## Objetivo atendido
Este codigo adapta a ideia do projeto base 2D para 3D, implementando:
- descricao direta de vertices e arestas de um cubo;
- desenho em 3D usando `GL_LINES` (wireframe);
- funcoes de translacao, escala e rotacao em 3D;
- transformacoes realizadas em torno do centro do poligono;
- interacao por teclado para aplicar as operacoes;
- sem uso de `glTranslate`, `glRotate` e `glScale`.

## Ajustes solicitados (revisao atual)
Foram avaliados os tres ajustes pedidos, sempre respeitando o enunciado do professor.

1. Fundo preto
- Implementado.
- O fundo passou para preto (`glClearColor(0,0,0,1)`).
- Para manter boa visualizacao, as arestas foram coloridas com cores vibrantes.

2. "Cada face do cubo com uma cor vibrante"
- Nao implementado literalmente por face preenchida.
- Justificativa tecnica: o trabalho exige wireframe com `GL_LINES` desenhando apenas arestas.
- Pintar cada face de forma fiel normalmente exige preenchimento de faces (`GL_QUADS`, `GL_TRIANGLES`, etc.), o que descaracteriza o requisito de wireframe puro.
- Alternativa compativel implementada: cores vibrantes nas arestas (sem preenchimento).

3. Wrap-around nas bordas
- Implementado no plano XY.
- Quando o cubo sai totalmente pela esquerda/direita ou por cima/baixo, ele reaparece no lado oposto.
- O calculo usa projeção perspectiva atual (FOV + aspecto da janela + distancia em Z do cubo) e o tamanho atual do cubo (considerando transformacoes ja aplicadas aos vertices).

## Como o codigo foi estruturado
Foi mantido um estilo semelhante aos exemplos das aulas:
- uso de `struct` para reunir os dados do objeto;
- funcoes separadas para criar, desenhar e transformar;
- callbacks GLUT (`display`, `reshape`, `keyboard`, `keyboard_special`, `redraw`);
- variavel global do objeto desenhado (cubo) e timer de redesenho.
- passagem por referencia constante no desenho para evitar copia desnecessaria do objeto.
- funcao auxiliar `aplicar_wraparound_xy` para manter o comportamento de reaparecimento lateral/superior.

A `struct Poligono3D` guarda:
- `centro`: ponto de referencia do cubo;
- `escala` e `rotacao`: estado logico acumulado;
- `vertices`: lista de pontos 3D;
- `arestas`: pares de indices que conectam os vertices.

## Escolhas e justificativas
1. Vertices do cubo definidos diretamente
- O enunciado permite descrever os vertices sem calculo complexo.
- Foi usado `lado / 2` para formar os 8 vertices ao redor do centro inicial.

2. Wireframe apenas com arestas
- O desenho e feito com `glBegin(GL_LINES)`.
- Cada aresta conecta dois vertices por indice, sem preencher faces.

3. Transformacoes manuais (sem funcoes prontas do OpenGL)
- Translacao: soma `delta_x`, `delta_y`, `delta_z` no centro e em todos os vertices.
- Escala: para cada vertice, desloca para o referencial do centro, aplica fator e retorna.
- Rotacao: para cada vertice, desloca para o centro, aplica matriz do eixo (x/y/z), retorna.
- Isso garante respeito ao centro do poligono, como pedido.

4. Controles de teclado
- Translacao: `W A S D` e setas para plano X/Y.
- Profundidade: `Q/E` ou `PageUp/PageDown` no eixo Z.
- Escala: `+` aumenta, `-` diminui.
- Rotacao: `x y z` para sentido positivo, `X Y Z` para sentido negativo.
- `ESC` fecha o programa.

## Compilacao e execucao

### Windows (MSYS2 + MinGW-w64) - passo a passo recomendado

1. Instale o MSYS2 pelo site oficial: `https://www.msys2.org/`.
2. Abra o atalho **MSYS2 UCRT64** (nao use CMD/PowerShell para compilar neste metodo).
3. Atualize o ambiente e instale compilador + FreeGLUT:

```bash
pacman -Syu
pacman -S --needed mingw-w64-ucrt-x86_64-toolchain mingw-w64-ucrt-x86_64-freeglut
```

4. No mesmo terminal `UCRT64`, compile (ajuste o caminho conforme sua pasta no Windows):

```bash
cd /c/UNivali/comp_grafica/computacao-grafica-univali-diegoS/trabalhos
g++ diegosilva_mirunaonofrei_desenvolvimento_1_1_funcoes_basicas.cpp -o trabalho1_1.exe -lfreeglut -lopengl32 -lglu32
```

5. Execute:

```bash
./trabalho1_1.exe
```

### Windows (Visual Studio Build Tools) - alternativa

1. Instale **Visual Studio 2022 Build Tools** com o workload `Desktop development with C++`.
2. Abra o terminal **x64 Native Tools Command Prompt for VS 2022**.
3. Garanta que o FreeGLUT esteja configurado (include/lib) e compile:

```bat
cd /d D:\UNivali\comp_grafica\computacao-grafica-univali-diegoS\trabalhos
cl /EHsc diegosilva_mirunaonofrei_desenvolvimento_1_1_funcoes_basicas.cpp freeglut.lib opengl32.lib glu32.lib
```

4. Execute o binario gerado (`diegosilva_mirunaonofrei_desenvolvimento_1_1_funcoes_basicas.exe`).

### Ubuntu (22.04+ / 24.04+) - passo a passo

1. Instale compilador e bibliotecas OpenGL/GLUT:

```bash
sudo apt update
sudo apt install -y g++ freeglut3-dev libglu1-mesa-dev mesa-common-dev
```

2. Entre na pasta do trabalho:

```bash
cd /media/diego/dados/UNivali/comp_grafica/computacao-grafica-univali-diegoS/trabalhos
```

3. Compile:

```bash
g++ diegosilva_mirunaonofrei_desenvolvimento_1_1_funcoes_basicas.cpp -o trabalho1_1 -lglut -lGL -lGLU
```

4. Execute:

```bash
./trabalho1_1
```

Se abrir a janela com o cubo em wireframe, a compilacao e execucao estao corretas.

## Observacoes finais
- O cubo inicia com centro em `(0, 0, -6)` para ficar visivel na projecao perspectiva.
- O `reshape` usa `gluPerspective` para manter visualizacao 3D proporcional.
- O `display` reseta apenas a `ModelView` com `glLoadIdentity`, sem transformacoes geometricas prontas.

## Validação contra o enunciado
- Implementado: fundo preto e ajuste visual das arestas; compativel, pois nao altera a natureza wireframe com `GL_LINES`.
- Implementado: wrap-around em X/Y por logica manual; compativel, pois apenas move vertices/centro sem `glTranslate`.
- Nao implementado: coloracao literal de cada face com preenchimento; nao compativel com a exigencia de desenhar somente arestas em wireframe.
- Mantido: cubo 3D, transformacoes manuais (translacao/rotacao/escala), respeito ao centro do poligono e sem `glRotate`/`glScale`/`glTranslate`.
