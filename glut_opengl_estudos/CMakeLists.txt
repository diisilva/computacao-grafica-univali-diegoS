cmake_minimum_required(VERSION 3.16)
project(glut_opengl_estudos_cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

set(AULAS_CPP
    01_janela_e_triangulo.cpp
    02_primitivas_e_sistema_de_coordenadas.cpp
    03_transformacoes_animacao_timer.cpp
    04_entrada_teclado_mouse.cpp
    05_reshape_viewport_projecao.cpp
    06_double_buffer_fps.cpp
    07_mini_projeto_sistema_solar_2d.cpp
    08_scene_graph_hierarquico_2d.cpp
    09_picking_colisoes_aabb.cpp
    10_sistema_particulas_blending.cpp
    11_jogo_pong.cpp
    12_jogo_snake.cpp
    13_jogo_breakout.cpp
    14_jogo_tetris.cpp
    15_jogo_galaga.cpp
)

foreach(src IN LISTS AULAS_CPP)
    get_filename_component(target_name ${src} NAME_WE)
    add_executable(${target_name} ${src})
    target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(${target_name} PRIVATE OpenGL::GL GLUT::GLUT)
endforeach()

add_executable(executar_aula_cpp executar_aula.cpp)
