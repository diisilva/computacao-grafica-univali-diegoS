/*
Aula 06 — Double buffering e medição simples de FPS (C++)

Conceitos:
- `GLUT_DOUBLE`: desenha no back buffer e troca com `glutSwapBuffers`.
- `glutIdleFunc`: animação contínua sem timer explícito.
- Janela temporal para cálculo de FPS médio.
*/

#include <GL/glut.h>
#include <chrono>
#include <iostream>
#include "cpp_glut_utils.hpp"

namespace {

float angleDeg = 0.0f;
int frameCount = 0;
auto startTime = std::chrono::steady_clock::now();

void drawRotatingQuad() {
    glBegin(GL_QUADS);
    glColor3f(1.0f, 0.6f, 0.2f); glVertex2f(-0.3f, -0.3f);
    glColor3f(1.0f, 0.6f, 0.2f); glVertex2f(0.3f, -0.3f);
    glColor3f(0.2f, 0.8f, 1.0f); glVertex2f(0.3f, 0.3f);
    glColor3f(0.2f, 0.8f, 1.0f); glVertex2f(-0.3f, 0.3f);
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glRotatef(angleDeg, 0.0f, 0.0f, 1.0f);
    drawRotatingQuad();
    glutSwapBuffers();

    frameCount++;
    const auto now = std::chrono::steady_clock::now();
    const std::chrono::duration<double> elapsed = now - startTime;
    if (elapsed.count() >= 1.0) {
        const double fps = static_cast<double>(frameCount) / elapsed.count();
        std::cout << "FPS medio: " << fps << '\n';
        frameCount = 0;
        startTime = now;
    }
}

void idle() {
    angleDeg += 0.35f;
    if (angleDeg >= 360.0f) angleDeg -= 360.0f;
    glutPostRedisplay();
}

} // namespace

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    aula::configureCloseBehavior();
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(900, 600);
    glutCreateWindow("Aula 06 - Double Buffer e FPS (C++)");

    glClearColor(0.04f, 0.04f, 0.06f, 1.0f);
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutMainLoop();
    return 0;
}
