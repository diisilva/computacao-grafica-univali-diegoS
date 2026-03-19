/*
Aula 02 — Primitivas e sistema de coordenadas NDC (C++)

Objetivo pedagógico:
- Visualizar eixos em NDC (Normalized Device Coordinates: [-1, 1]).
- Desenhar pontos, linhas e triângulos no mesmo frame.
- Entender espessura de linha e tamanho de ponto.
*/

#include <GL/glut.h>
#include "cpp_glut_utils.hpp"

namespace {

void drawAxes() {
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glColor3f(0.7f, 0.7f, 0.7f);
    glVertex2f(-1.0f, 0.0f); glVertex2f(1.0f, 0.0f);
    glVertex2f(0.0f, -1.0f); glVertex2f(0.0f, 1.0f);
    glEnd();
}

void drawPrimitives() {
    glPointSize(10.0f);
    glBegin(GL_POINTS);
    glColor3f(1.0f, 0.9f, 0.2f);
    glVertex2f(-0.7f, 0.6f);
    glVertex2f(-0.5f, 0.45f);
    glVertex2f(-0.3f, 0.7f);
    glEnd();

    glLineWidth(4.0f);
    glBegin(GL_LINES);
    glColor3f(0.2f, 0.9f, 1.0f);
    glVertex2f(-0.1f, 0.6f); glVertex2f(0.4f, 0.75f);
    glVertex2f(-0.1f, 0.4f); glVertex2f(0.4f, 0.2f);
    glEnd();

    glBegin(GL_TRIANGLES);
    glColor3f(0.9f, 0.3f, 0.3f); glVertex2f(0.55f, 0.2f);
    glColor3f(0.3f, 0.9f, 0.4f); glVertex2f(0.9f, 0.2f);
    glColor3f(0.3f, 0.4f, 1.0f); glVertex2f(0.72f, 0.7f);
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawAxes();
    drawPrimitives();
    glFlush();
}

} // namespace

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    aula::configureCloseBehavior();
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(900, 600);
    glutCreateWindow("Aula 02 - Primitivas e NDC (C++)");

    glClearColor(0.08f, 0.08f, 0.1f, 1.0f);
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
