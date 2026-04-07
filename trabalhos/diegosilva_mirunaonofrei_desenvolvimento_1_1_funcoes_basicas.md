# Trabalho 1.1 - Funcoes Basicas 3D (Cubo Wireframe)

Arquivo fonte:
- `diegosilva_mirunaonofrei_desenvolvimento_1_1_funcoes_basicas.cpp`

## Objetivo atendido
Este codigo adapta a ideia do projeto base 2D para 3D, implementando:
- descricao direta de vertices e arestas de um cubo;
- desenho em 3D usando `GL_LINES` (wireframe);
- funcoes de translacao, escala e rotacao em 3D;
- transformacoes realizadas em torno do centro do poligono;
- interacao por teclado para aplicar as operacoes;
- sem uso de `glTranslate`, `glRotate` e `glScale`.

## Como o codigo foi estruturado
Foi mantido um estilo semelhante aos exemplos das aulas:
- uso de `struct` para reunir os dados do objeto;
- funcoes separadas para criar, desenhar e transformar;
- callbacks GLUT (`display`, `reshape`, `keyboard`, `keyboard_special`, `redraw`);
- variavel global do objeto desenhado (cubo) e timer de redesenho.

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

### Pre-requisito (importante)
O comando `g++` so funciona se voce tiver um compilador C++ instalado e configurado no `PATH`.

Se aparecer erro como:
- `g++ : O termo 'g++' nao e reconhecido...`

isso significa que o compilador ainda nao esta disponivel no terminal atual.

### Opcao rapida recomendada no Windows
Instale o **Visual Studio 2022 Build Tools** com o workload:
- `Desktop development with C++`

Depois abra o terminal **x64 Native Tools Command Prompt for VS 2022** e compile por la.

Comandos (exemplo):
```bat
cd /d D:\UNivali\comp_grafica\computacao-grafica-univali-diegoS\trabalhos
cl /EHsc diegosilva_mirunaonofrei_desenvolvimento_1_1_funcoes_basicas.cpp freeglut.lib opengl32.lib glu32.lib
```

Observacao:
- Se o `freeglut.lib` nao for encontrado, ajuste os diretorios de include/lib do FreeGLUT no ambiente/projeto.

### Opcao 1: MinGW (g++) com FreeGLUT
Ajuste os caminhos dos includes/libs conforme sua instalacao do FreeGLUT.

Exemplo (PowerShell):
```powershell
g++ .\diegosilva_mirunaonofrei_desenvolvimento_1_1_funcoes_basicas.cpp -o .\trabalho1_1.exe -lfreeglut -lopengl32 -lglu32
.\trabalho1_1.exe
```

Se o terminal ainda nao reconhecer `g++` apos instalar MinGW:
- feche e abra o PowerShell novamente;
- valide com `g++ --version`;
- confirme que a pasta `bin` do MinGW esta no `PATH`.

### Opcao 2: Visual Studio
1. Crie um projeto C++ vazio.
2. Adicione o arquivo `diegosilva_mirunaonofrei_desenvolvimento_1_1_funcoes_basicas.cpp`.
3. Configure os diretorios de include/lib do FreeGLUT.
4. Adicione nas dependencias de link:
- `freeglut.lib`
- `opengl32.lib`
- `glu32.lib`
5. Compile e execute.

## Observacoes finais
- O cubo inicia com centro em `(0, 0, -6)` para ficar visivel na projecao perspectiva.
- O `reshape` usa `gluPerspective` para manter visualizacao 3D proporcional.
- O `display` reseta apenas a `ModelView` com `glLoadIdentity`, sem transformacoes geometricas prontas.
