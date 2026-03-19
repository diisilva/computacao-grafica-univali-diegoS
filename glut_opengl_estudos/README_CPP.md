# Aulas C++ (GLUT/OpenGL)

Este diretório agora contém equivalentes em C++ para todas as aulas `.py` (01–15), com foco didático e comentários para estudo.

## Pré-requisitos
- Linux
- `g++` com suporte a C++17
- FreeGLUT/OpenGL (`libglut`, `libGL`, `libGLU`)
- CMake (opcional, mas recomendado)

## Opção 1 — Compilar e rodar com o runner C++
```bash
g++ -std=c++17 executar_aula.cpp -o executar_aula_cpp
./executar_aula_cpp --list
./executar_aula_cpp --aula 11
```

## Opção 2 — Build completo com CMake
```bash
cmake -S . -B build
cmake --build build -j
./build/11_jogo_pong
```

## Mapeamento
Cada arquivo Python possui seu par C++:
- `01_janela_e_triangulo.py` -> `01_janela_e_triangulo.cpp`
- ...
- `15_jogo_galaga.py` -> `15_jogo_galaga.cpp`

## Observações pedagógicas
- Os exemplos usam OpenGL imediato (`glBegin`/`glEnd`) por clareza didática.
- O objetivo é ensino de fundamentos; não é uma arquitetura moderna de engine.
- Para evolução profissional, próximos passos: VBO/VAO, shaders GLSL e ECS.
