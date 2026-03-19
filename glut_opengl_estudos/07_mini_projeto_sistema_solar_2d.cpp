/*
Aula 07 — Mini Projeto: Sistema Solar 2D (C++)

Objetivo pedagógico:
- Integrar animação, hierarquia de transformações e HUD textual.
- Trabalhar organização de dados de cena com `struct`.

Controles:
- `+` / `-`: altera velocidade orbital.
- `Espaco`: pausa.
- `L`: mostra/oculta rótulos.
- `ESC`: sair.
*/

#include <GL/glut.h>
#include <cmath>
#include <string>
#include <vector>
#include "cpp_glut_utils.hpp"

namespace {

struct Planet {
    std::string name;
    float orbit;
    float radius;
    float speed;
    float r;
    float g;
    float b;
};

std::vector<Planet> planets = {
    {"Mercurio", 0.14f, 0.010f, 4.8f, 0.70f, 0.70f, 0.70f},
    {"Venus",    0.20f, 0.016f, 3.5f, 0.90f, 0.75f, 0.45f},
    {"Terra",    0.27f, 0.017f, 3.0f, 0.20f, 0.55f, 1.00f},
    {"Marte",    0.34f, 0.014f, 2.4f, 0.90f, 0.35f, 0.25f},
    {"Jupiter",  0.48f, 0.040f, 1.3f, 0.85f, 0.65f, 0.45f},
    {"Saturno",  0.62f, 0.033f, 1.0f, 0.90f, 0.82f, 0.55f},
    {"Urano",    0.76f, 0.024f, 0.7f, 0.55f, 0.85f, 0.90f},
    {"Netuno",   0.88f, 0.023f, 0.55f, 0.30f, 0.45f, 0.95f}
};

float simTime = 0.0f;
float speed = 1.0f;
bool paused = false;
bool showLabels = true;

void drawCircle(float radius, float r, float g, float b, int segments = 40) {
    glColor3f(r, g, b);
    glBegin(GL_POLYGON);
    for (int i = 0; i < segments; ++i) {
        const float t = 2.0f * 3.1415926f * static_cast<float>(i) / static_cast<float>(segments);
        glVertex2f(radius * std::cos(t), radius * std::sin(t));
    }
    glEnd();
}

void drawOrbit(float radius) {
    glColor3f(0.18f, 0.18f, 0.25f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 120; ++i) {
        const float t = 2.0f * 3.1415926f * static_cast<float>(i) / 120.0f;
        glVertex2f(radius * std::cos(t), radius * std::sin(t));
    }
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    for (const auto& p : planets) drawOrbit(p.orbit);

    drawCircle(0.08f, 1.0f, 0.85f, 0.15f, 60);

    for (const auto& p : planets) {
        const float angle = simTime * p.speed;
        const float x = p.orbit * std::cos(angle);
        const float y = p.orbit * std::sin(angle);

        glPushMatrix();
        glTranslatef(x, y, 0.0f);
        drawCircle(p.radius, p.r, p.g, p.b, 28);
        glPopMatrix();

        if (showLabels) {
            glColor3f(0.85f, 0.9f, 1.0f);
            aula::drawBitmapText(x + p.radius + 0.01f, y + p.radius + 0.01f, p.name, GLUT_BITMAP_HELVETICA_12);
        }
    }

    glColor3f(1.0f, 1.0f, 0.6f);
    aula::drawBitmapText(-0.96f, -0.95f, "Controles: +  -  Espaco  L  ESC", GLUT_BITMAP_HELVETICA_12);

    glutSwapBuffers();
}

void update(int) {
    if (!paused) simTime += 0.01f * speed;
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void keyboard(unsigned char key, int, int) {
    if (key == '+') speed = aula::clamp(speed + 0.2f, 0.2f, 8.0f);
    else if (key == '-') speed = aula::clamp(speed - 0.2f, 0.2f, 8.0f);
    else if (key == ' ') paused = !paused;
    else if (key == 'l' || key == 'L') showLabels = !showLabels;
    else if (key == 27) std::exit(0);
}

} // namespace

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    aula::configureCloseBehavior();
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(950, 700);
    glutCreateWindow("Aula 07 - Sistema Solar 2D (C++)");

    glClearColor(0.01f, 0.01f, 0.05f, 1.0f);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, update, 0);
    glutMainLoop();
    return 0;
}
