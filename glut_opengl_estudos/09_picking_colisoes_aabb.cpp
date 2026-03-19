/*
Aula 09 — Picking e colisões AABB (C++)

Objetivo pedagógico:
- Implementar detecção de clique sobre objetos (picking 2D).
- Implementar colisão Axis-Aligned Bounding Box (AABB).
- Explicar claramente conversão de coordenadas de tela para mundo.
*/

#include <GL/glut.h>
#include <string>
#include <vector>
#include "cpp_glut_utils.hpp"

namespace {

struct Box {
    float x;
    float y;
    float w;
    float h;
    float r;
    float g;
    float b;
    bool selected = false;
};

std::vector<Box> boxes = {
    {20.0f, 65.0f, 16.0f, 12.0f, 0.9f, 0.4f, 0.4f, false},
    {52.0f, 52.0f, 20.0f, 14.0f, 0.4f, 0.8f, 1.0f, false},
    {78.0f, 30.0f, 14.0f, 18.0f, 0.5f, 1.0f, 0.6f, false}
};

int selectedIndex = -1;

void drawRect(const Box& box) {
    const float hw = box.w * 0.5f;
    const float hh = box.h * 0.5f;

    if (box.selected) {
        // Objeto selecionado fica mais claro para feedback visual.
        glColor3f(aula::clamp(box.r + 0.25f, 0.0f, 1.0f), aula::clamp(box.g + 0.25f, 0.0f, 1.0f), aula::clamp(box.b + 0.25f, 0.0f, 1.0f));
    } else {
        glColor3f(box.r, box.g, box.b);
    }

    glBegin(GL_QUADS);
    glVertex2f(box.x - hw, box.y - hh);
    glVertex2f(box.x + hw, box.y - hh);
    glVertex2f(box.x + hw, box.y + hh);
    glVertex2f(box.x - hw, box.y + hh);
    glEnd();
}

bool pointInAabb(float px, float py, const Box& b) {
    const float left = b.x - b.w * 0.5f;
    const float right = b.x + b.w * 0.5f;
    const float bottom = b.y - b.h * 0.5f;
    const float top = b.y + b.h * 0.5f;
    return px >= left && px <= right && py >= bottom && py <= top;
}

bool intersects(const Box& a, const Box& b) {
    const float aLeft = a.x - a.w * 0.5f;
    const float aRight = a.x + a.w * 0.5f;
    const float aBottom = a.y - a.h * 0.5f;
    const float aTop = a.y + a.h * 0.5f;

    const float bLeft = b.x - b.w * 0.5f;
    const float bRight = b.x + b.w * 0.5f;
    const float bBottom = b.y - b.h * 0.5f;
    const float bTop = b.y + b.h * 0.5f;

    return !(aRight < bLeft || aLeft > bRight || aTop < bBottom || aBottom > bTop);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    for (const auto& b : boxes) drawRect(b);

    // Mostra colisão entre primeira e segunda caixas.
    const bool collision = intersects(boxes[0], boxes[1]);
    glColor3f(collision ? 1.0f : 0.8f, collision ? 0.5f : 0.9f, collision ? 0.5f : 1.0f);
    aula::drawBitmapText(2.0f, 96.0f, collision ? "AABB: COLISAO" : "AABB: sem colisao");

    glutSwapBuffers();
}

void reshape(int width, int height) {
    if (height == 0) height = 1;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 100.0, 0.0, 100.0, -1.0, 1.0);
}

void special(int key, int, int) {
    if (selectedIndex < 0) return;

    constexpr float step = 1.8f;
    if (key == GLUT_KEY_LEFT) boxes[selectedIndex].x -= step;
    else if (key == GLUT_KEY_RIGHT) boxes[selectedIndex].x += step;
    else if (key == GLUT_KEY_UP) boxes[selectedIndex].y += step;
    else if (key == GLUT_KEY_DOWN) boxes[selectedIndex].y -= step;

    glutPostRedisplay();
}

void keyboard(unsigned char key, int, int) {
    if (key == 27) std::exit(0);
}

void mouse(int button, int state, int x, int y) {
    if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) return;

    const int width = glutGet(GLUT_WINDOW_WIDTH);
    const int height = glutGet(GLUT_WINDOW_HEIGHT);

    // Conversão de pixel para mundo ortográfico [0,100]x[0,100].
    const float worldX = 100.0f * static_cast<float>(x) / static_cast<float>(width);
    const float worldY = 100.0f * (1.0f - static_cast<float>(y) / static_cast<float>(height));

    selectedIndex = -1;
    for (std::size_t i = 0; i < boxes.size(); ++i) {
        boxes[i].selected = pointInAabb(worldX, worldY, boxes[i]);
        if (boxes[i].selected) selectedIndex = static_cast<int>(i);
    }

    glutPostRedisplay();
}

} // namespace

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    aula::configureCloseBehavior();
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1050, 760);
    glutCreateWindow("Aula 09 - Picking e AABB (C++)");

    glClearColor(0.04f, 0.04f, 0.07f, 1.0f);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutMouseFunc(mouse);
    glutMainLoop();
    return 0;
}
