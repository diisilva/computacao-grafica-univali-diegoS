/*
Runner simples para compilar e executar as aulas C++ por número.

Uso:
  ./executar_aula_cpp --list
  ./executar_aula_cpp --aula 3

Observações para estagiários:
- Este runner compila a aula sob demanda usando `g++`.
- O binário é gerado em `.build_cpp/` para evitar poluir a pasta raiz.
- Em produção, prefira CMake/Ninja para build incremental mais eficiente.
*/

#include <array>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

namespace {

const std::map<int, std::string> AULAS = {
    {1, "01_janela_e_triangulo.cpp"},
    {2, "02_primitivas_e_sistema_de_coordenadas.cpp"},
    {3, "03_transformacoes_animacao_timer.cpp"},
    {4, "04_entrada_teclado_mouse.cpp"},
    {5, "05_reshape_viewport_projecao.cpp"},
    {6, "06_double_buffer_fps.cpp"},
    {7, "07_mini_projeto_sistema_solar_2d.cpp"},
    {8, "08_scene_graph_hierarquico_2d.cpp"},
    {9, "09_picking_colisoes_aabb.cpp"},
    {10, "10_sistema_particulas_blending.cpp"},
    {11, "11_jogo_pong.cpp"},
    {12, "12_jogo_snake.cpp"},
    {13, "13_jogo_breakout.cpp"},
    {14, "14_jogo_tetris.cpp"},
    {15, "15_jogo_galaga.cpp"}
};

void printUsage() {
    std::cout << "Uso:\n"
              << "  ./executar_aula_cpp --list\n"
              << "  ./executar_aula_cpp --aula N\n";
}

void listLessons() {
    std::cout << "Aulas C++ disponiveis:\n";
    for (const auto& [n, file] : AULAS) {
        std::cout << "  " << n << ": " << file << '\n';
    }
}

int runLesson(int lessonNumber) {
    const auto it = AULAS.find(lessonNumber);
    if (it == AULAS.end()) {
        std::cerr << "Aula invalida: " << lessonNumber << '\n';
        return 1;
    }

    const std::filesystem::path base = std::filesystem::current_path();
    const std::filesystem::path source = base / it->second;
    if (!std::filesystem::exists(source)) {
        std::cerr << "Arquivo nao encontrado: " << source << '\n';
        return 1;
    }

    const std::filesystem::path buildDir = base / ".build_cpp";
    std::filesystem::create_directories(buildDir);

    const std::string binaryName = "aula_" + std::to_string(lessonNumber);
    const std::filesystem::path binary = buildDir / binaryName;

    std::ostringstream cmd;
    cmd << "g++ -std=c++17 \"" << source.string() << "\" -o \"" << binary.string() << "\" -lglut -lGL -lGLU";

    std::cout << "Compilando: " << source.filename().string() << '\n';
    if (std::system(cmd.str().c_str()) != 0) {
        std::cerr << "Falha na compilacao.\n";
        return 1;
    }

    std::cout << "Executando aula " << lessonNumber << "...\n";
    return std::system(("\"" + binary.string() + "\"").c_str());
}

} // namespace

int main(int argc, char** argv) {
    if (argc < 2) {
        printUsage();
        return 1;
    }

    const std::string arg1 = argv[1];
    if (arg1 == "--list") {
        listLessons();
        return 0;
    }

    if (arg1 == "--aula") {
        if (argc < 3) {
            std::cerr << "Informe o numero da aula.\n";
            return 1;
        }
        const int lesson = std::atoi(argv[2]);
        return runLesson(lesson);
    }

    printUsage();
    return 1;
}
