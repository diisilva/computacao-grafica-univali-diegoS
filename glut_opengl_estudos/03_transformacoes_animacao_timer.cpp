/*
Aula 03 — Transformações 2D e animação com timer (C++)

Conceitos:
- `glTranslatef`, `glRotatef`, `glScalef`.
- Pilha de matriz: `glPushMatrix`/`glPopMatrix`.
- Atualização temporal com `glutTimerFunc`.
*/

#include <GL/glut.h>
#include "cpp_glut_utils.hpp"

namespace {

float angleDegrees = 0.0f;

void drawTriangle() {
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.2f, 0.2f); glVertex2f(0.0f, 0.3f);
    glColor3f(0.2f, 1.0f, 0.2f); glVertex2f(-0.3f, -0.25f);
    glColor3f(0.2f, 0.4f, 1.0f); glVertex2f(0.3f, -0.25f);
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glPushMatrix();
    glTranslatef(0.45f, 0.0f, 0.0f);
    glRotatef(angleDegrees, 0.0f, 0.0f, 1.0f);
    glScalef(1.2f, 1.2f, 1.0f);
    drawTriangle();
    glPopMatrix();

    glutSwapBuffers();
}

void updateTimer(int) {
    angleDegrees += 1.5f;
    if (angleDegrees >= 360.0f) angleDegrees -= 360.0f;

    glutPostRedisplay();
    glutTimerFunc(16, updateTimer, 0);
}

} // namespace

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    aula::configureCloseBehavior();
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(900, 600);
    glutCreateWindow("Aula 03 - Transformacoes e Timer (C++)");

    glClearColor(0.03f, 0.03f, 0.05f, 1.0f);
    glutDisplayFunc(display);
    glutTimerFunc(0, updateTimer, 0);
    glutMainLoop();
    return 0;
}
