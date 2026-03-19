/*
Aula 05 — Reshape, viewport e projeção ortográfica (C++)

Conceitos:
- `reshape(w, h)` é chamado quando a janela muda de tamanho.
- `glViewport` define a região de render em pixels.
- `gluOrtho2D` ajusta projeção para manter proporção visual.
*/

#include <GL/glut.h>
#include <GL/glu.h>
#include "cpp_glut_utils.hpp"

namespace {

void drawSquare() {
    glBegin(GL_QUADS);
    glColor3f(0.2f, 0.75f, 1.0f);
    glVertex2f(-0.5f, -0.5f);
    glVertex2f( 0.5f, -0.5f);
    glVertex2f( 0.5f,  0.5f);
    glVertex2f(-0.5f,  0.5f);
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    drawSquare();
    glFlush();
}

void reshape(int width, int height) {
    if (height == 0) height = 1;

    const float aspect = static_cast<float>(width) / static_cast<float>(height);

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (aspect >= 1.0f) {
        // Janela mais larga: expandimos eixo X.
        gluOrtho2D(-aspect, aspect, -1.0, 1.0);
    } else {
        // Janela mais alta: expandimos eixo Y.
        const float inv = 1.0f / aspect;
        gluOrtho2D(-1.0, 1.0, -inv, inv);
    }
}

} // namespace

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    aula::configureCloseBehavior();
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(900, 600);
    glutCreateWindow("Aula 05 - Reshape e Viewport (C++)");

    glClearColor(0.05f, 0.05f, 0.07f, 1.0f);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMainLoop();
    return 0;
}
