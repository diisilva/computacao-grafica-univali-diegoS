# Computação Gráfica — UNIVALI (GLUT/OpenGL)

Repositório de estudos práticos de **Computação Gráfica** com foco em **OpenGL no modo imediato** usando:

- **Python + PyOpenGL + GLUT**
- **C++ + FreeGLUT/OpenGL**
- **Notebooks Jupyter** para experimentação

O material é organizado em trilha progressiva (aulas 01 a 15), indo de janela e primitivas até mini-jogos e tópicos como colisão, partículas e scene graph.

---

## 1) Estrutura do repositório

```text
.
├── teste_glut.py
├── teste_glut.ipynb
├── C01_Exemplo.cpp
├── ml_environment_setup.md
├── requirements_recriada.txt
└── glut_opengl_estudos/
    ├── 01_janela_e_triangulo.{py,cpp,ipynb}
    ├── 02_primitivas_e_sistema_de_coordenadas.{py,cpp,ipynb}
    ├── 03_transformacoes_animacao_timer.{py,cpp,ipynb}
    ├── 04_entrada_teclado_mouse.{py,cpp,ipynb}
    ├── 05_reshape_viewport_projecao.{py,cpp,ipynb}
    ├── 06_double_buffer_fps.{py,cpp,ipynb}
    ├── 07_mini_projeto_sistema_solar_2d.{py,cpp,ipynb}
    ├── 08_scene_graph_hierarquico_2d.{py,cpp,ipynb}
    ├── 09_picking_colisoes_aabb.{py,cpp,ipynb}
    ├── 10_sistema_particulas_blending.{py,cpp,ipynb}
    ├── 11_jogo_pong.{py,cpp,ipynb}
    ├── 12_jogo_snake.{py,cpp,ipynb}
    ├── 13_jogo_breakout.{py,cpp,ipynb}
    ├── 14_jogo_tetris.{py,cpp,ipynb}
    ├── 15_jogo_galaga.{py,cpp,ipynb}
    ├── executar_aula.py
    ├── executar_aula.cpp
    ├── cpp_glut_utils.hpp
    ├── CMakeLists.txt
    ├── requirements_glut_estudos.txt
    ├── README.md
    └── README_CPP.md
```

---

## 2) Do que se trata este repo

Este repositório é uma base didática para aprender pipeline gráfico 2D/3D introdutório com GLUT/OpenGL clássico, cobrindo:

- criação de janelas e contexto gráfico;
- desenho de primitivas e sistema de coordenadas;
- transformações, animação por timer e controle de FPS;
- entrada por teclado e mouse;
- viewport, reshape e projeções básicas;
- organização hierárquica de cena (scene graph);
- picking e colisões AABB;
- partículas e blending;
- implementação de jogos clássicos (Pong, Snake, Breakout, Tetris, Galaga).

---

## 3) Conteúdo das aulas (01–15)

1. **Janela e triângulo** — inicialização GLUT e primeiro desenho.
2. **Primitivas e coordenadas** — linhas, polígonos e noções de espaço.
3. **Transformações + timer** — translação/rotação/escala e animação.
4. **Entrada teclado/mouse** — interação em tempo real.
5. **Reshape/viewport/projeção** — adaptação da cena à janela.
6. **Double buffer + FPS** — suavidade de renderização e medição.
7. **Mini projeto sistema solar 2D** — composição e movimento orbital.
8. **Scene graph hierárquico 2D** — herança de transformações.
9. **Picking + colisões AABB** — seleção e detecção de interseções.
10. **Sistema de partículas + blending** — efeitos visuais básicos.
11. **Jogo Pong** — loop de jogo e colisão com paletas.
12. **Jogo Snake** — grade, crescimento e regras de jogo.
13. **Jogo Breakout** — blocos, rebotes e progressão.
14. **Jogo Tetris** — peças, rotação, encaixe e pontuação.
15. **Jogo Galaga** — movimentação/inimigos/projéteis.

Cada aula possui três versões:

- `*.py` (Python)
- `*.cpp` (C++)
- `*.ipynb` (notebook)

---

## 4) Pré-requisitos

### Linux (Ubuntu/Debian)

Instale dependências de sistema:

```bash
sudo apt update
sudo apt install -y python3 python3-venv python3-pip freeglut3-dev libglu1-mesa-dev mesa-common-dev g++ cmake
```

---

## 5) Executando (Python)

### 5.1 Ambiente mínimo para GLUT

```bash
cd "/media/diego/Novo volume/UNivali/comp_grafica/computacao-grafica-univali-diegoS"
python3 -m venv .venv
source .venv/bin/activate
python3 -m pip install --upgrade pip
python3 -m pip install -r glut_opengl_estudos/requirements_glut_estudos.txt
```

### 5.2 Teste rápido da instalação

```bash
python3 teste_glut.py
```

### 5.3 Executar uma aula específica

```bash
cd glut_opengl_estudos
python3 01_janela_e_triangulo.py
python3 11_jogo_pong.py
```

### 5.4 Runner de aulas Python

```bash
cd glut_opengl_estudos
python3 executar_aula.py --list
python3 executar_aula.py --aula 4
python3 executar_aula.py --aula 15
```

---

## 6) Executando (C++)

### 6.1 Exemplo raiz

```bash
cd "/media/diego/Novo volume/UNivali/comp_grafica/computacao-grafica-univali-diegoS"
g++ C01_Exemplo.cpp -o C01_Exemplo -lglut -lGL -lGLU
./C01_Exemplo
```

### 6.2 Runner C++ (compila sob demanda)

```bash
cd glut_opengl_estudos
g++ -std=c++17 executar_aula.cpp -o executar_aula_cpp
./executar_aula_cpp --list
./executar_aula_cpp --aula 11
```

### 6.3 Build completo via CMake

```bash
cd glut_opengl_estudos
cmake -S . -B build
cmake --build build -j
./build/01_janela_e_triangulo
./build/11_jogo_pong
```

---

## 7) Executando notebooks

```bash
cd "/media/diego/Novo volume/UNivali/comp_grafica/computacao-grafica-univali-diegoS"
source .venv/bin/activate
python3 -m pip install jupyterlab ipykernel
jupyter lab
```

Depois, abra os arquivos `*.ipynb` e execute as células em ordem.

> Observação: dependendo da implementação do GLUT no sistema, fechar a janela pode encerrar o kernel do notebook. Para máxima estabilidade, use os arquivos `.py`.

---

## 8) Arquivos auxiliares importantes

- `glut_opengl_estudos/cpp_glut_utils.hpp`: utilitários C++ (texto bitmap, clamp, comportamento de fechamento de janela).
- `requirements_recriada.txt`: ambiente Python maior (inclui pacotes de ML e Jupyter, além de PyOpenGL).
- `ml_environment_setup.md`: guia de setup de ambiente (Linux/Windows) com foco em Python/Jupyter e GPU.

---

## 9) Sugestão de fluxo de estudo

1. Validar instalação com `teste_glut.py`.
2. Seguir aulas em ordem (`01` → `15`) na versão `*.py`.
3. Repetir as mesmas aulas em `*.cpp` para comparar APIs/estilo.
4. Usar os notebooks para testes rápidos e anotações.
5. Explorar os jogos finais para consolidar arquitetura de loop, input, física simples e render.

---

## 10) Próximos passos (evolução)

- Migrar de OpenGL imediato para pipeline moderno (VBO/VAO + GLSL).
- Separar melhor módulos de render, input, física e estado de jogo.
- Adicionar testes lógicos para regras dos jogos (sem depender da janela gráfica).
