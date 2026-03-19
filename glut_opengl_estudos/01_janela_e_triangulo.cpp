/*
Aula 01 — Primeira janela com GLUT + OpenGL (C++)

Objetivo pedagógico:
- Entender o ciclo base de uma aplicação gráfica com GLUT.
- Entender o callback de renderização (`display`).
- Renderizar um triângulo colorido com interpolação de cores.

Compilação (Linux):
  g++ -std=c++17 01_janela_e_triangulo.cpp -o aula01 -lglut -lGL -lGLU
*/

#include <GL/glut.h>
#include "cpp_glut_utils.hpp"

namespace {

void display() {
    // Limpa color buffer (fundo da janela).
    glClear(GL_COLOR_BUFFER_BIT);

    // Modo imediato: cada vértice recebe uma cor.
    // A GPU interpola as cores no interior do triângulo.
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f); glVertex2f(-0.7f, -0.6f);
    glColor3f(0.0f, 1.0f, 0.0f); glVertex2f( 0.7f, -0.6f);
    glColor3f(0.0f, 0.2f, 1.0f); glVertex2f( 0.0f,  0.7f);
    glEnd();

    glFlush();
}

} // namespace

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    aula::configureCloseBehavior();
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Aula 01 - Triangulo (C++)");

    glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
