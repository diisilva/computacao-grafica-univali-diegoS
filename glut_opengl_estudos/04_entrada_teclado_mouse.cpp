/*
Aula 04 — Interação com teclado e mouse (C++)

Conceitos:
- Callback de teclado (`glutKeyboardFunc`).
- Callback de mouse (`glutMouseFunc`).
- Estado global simples para aplicações interativas.

Controles:
- WASD: move o quadrado.
- R/G/B: altera cor.
- Clique esquerdo: reposiciona no ponto clicado.
- ESC: sair.
*/

#include <GL/glut.h>
#include <cstdlib>
#include "cpp_glut_utils.hpp"

namespace {

float positionX = 0.0f;
float positionY = 0.0f;
float colorR = 0.9f;
float colorG = 0.4f;
float colorB = 0.2f;

void drawSquare() {
    glBegin(GL_QUADS);
    glVertex2f(-0.15f, -0.15f);
    glVertex2f( 0.15f, -0.15f);
    glVertex2f( 0.15f,  0.15f);
    glVertex2f(-0.15f,  0.15f);
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    glTranslatef(positionX, positionY, 0.0f);
    glColor3f(colorR, colorG, colorB);
    drawSquare();

    glFlush();
}

void keyboard(unsigned char key, int, int) {
    constexpr float step = 0.05f;

    switch (key) {
        case 'w': case 'W': positionY += step; break;
        case 's': case 'S': positionY -= step; break;
        case 'a': case 'A': positionX -= step; break;
        case 'd': case 'D': positionX += step; break;
        case 'r': case 'R': colorR = 1.0f; colorG = 0.2f; colorB = 0.2f; break;
        case 'g': case 'G': colorR = 0.2f; colorG = 1.0f; colorB = 0.2f; break;
        case 'b': case 'B': colorR = 0.2f; colorG = 0.4f; colorB = 1.0f; break;
        case 27: std::exit(0); // ESC
    }

    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) return;

    // Converte pixel para NDC em [-1, 1].
    const int width = glutGet(GLUT_WINDOW_WIDTH);
    const int height = glutGet(GLUT_WINDOW_HEIGHT);

    positionX = 2.0f * (static_cast<float>(x) / static_cast<float>(width)) - 1.0f;
    positionY = 1.0f - 2.0f * (static_cast<float>(y) / static_cast<float>(height));

    glutPostRedisplay();
}

} // namespace

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    aula::configureCloseBehavior();
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(900, 600);
    glutCreateWindow("Aula 04 - Teclado e Mouse (C++)");

    glClearColor(0.07f, 0.07f, 0.1f, 1.0f);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);

    glutMainLoop();
    return 0;
}
