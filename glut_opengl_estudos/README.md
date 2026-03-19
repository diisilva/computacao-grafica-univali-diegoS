# Trilha de Estudos — GLUT + OpenGL (Python)

Material didático progressivo com **15 scripts** e **15 notebooks pareados**.

## Estrutura

1. `01_janela_e_triangulo.py` + `01_janela_e_triangulo.ipynb`
2. `02_primitivas_e_sistema_de_coordenadas.py` + `02_primitivas_e_sistema_de_coordenadas.ipynb`
3. `03_transformacoes_animacao_timer.py` + `03_transformacoes_animacao_timer.ipynb`
4. `04_entrada_teclado_mouse.py` + `04_entrada_teclado_mouse.ipynb`
5. `05_reshape_viewport_projecao.py` + `05_reshape_viewport_projecao.ipynb`
6. `06_double_buffer_fps.py` + `06_double_buffer_fps.ipynb`
7. `07_mini_projeto_sistema_solar_2d.py` + `07_mini_projeto_sistema_solar_2d.ipynb`
8. `08_scene_graph_hierarquico_2d.py` + `08_scene_graph_hierarquico_2d.ipynb`
9. `09_picking_colisoes_aabb.py` + `09_picking_colisoes_aabb.ipynb`
10. `10_sistema_particulas_blending.py` + `10_sistema_particulas_blending.ipynb`
11. `11_jogo_pong.py` + `11_jogo_pong.ipynb`
12. `12_jogo_snake.py` + `12_jogo_snake.ipynb`
13. `13_jogo_breakout.py` + `13_jogo_breakout.ipynb`
14. `14_jogo_tetris.py` + `14_jogo_tetris.ipynb`
15. `15_jogo_galaga.py` + `15_jogo_galaga.ipynb`

## Pré-requisitos

- Python 3.10+
- `PyOpenGL` e `PyOpenGL_accelerate`
- FreeGLUT instalado no sistema

## Instalação rápida

```bash
cd ~/projects_ubuntu
source .venv/bin/activate
python3 -m pip install -r comp_grafica/glut_opengl_estudos/requirements_glut_estudos.txt
```

## Como executar scripts

```bash
cd ~/projects_ubuntu/comp_grafica/glut_opengl_estudos
python3 01_janela_e_triangulo.py
```

## Runner de aulas

```bash
cd ~/projects_ubuntu/comp_grafica/glut_opengl_estudos
python3 executar_aula.py --list
python3 executar_aula.py --aula 4
```

## Como usar notebooks

Abra os notebooks no VS Code/Jupyter e execute as células em ordem.

> Observação: em notebooks, fechar janela GLUT pode encerrar kernel dependendo da build do GLUT. Para estabilidade máxima, prefira rodar os `.py`.
