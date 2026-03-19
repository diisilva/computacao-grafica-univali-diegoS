/*
Aula 08 — Scene Graph Hierárquico 2D (C++)

Objetivo pedagógico:
- Demonstrar uma arquitetura de cena em árvore (scene graph).
- Separar transformação local, atualização e render recursivo.

Nota didática:
Este exemplo usa lambdas e ponteiros para manter o código compacto,
mas o foco é a ideia de composição hierárquica.
*/

#include <GL/glut.h>
#include <cmath>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "cpp_glut_utils.hpp"

namespace {

struct Transform2D {
    float tx = 0.0f;
    float ty = 0.0f;
    float rotationDeg = 0.0f;
    float sx = 1.0f;
    float sy = 1.0f;
};

struct SceneNode {
    std::string name;
    Transform2D transform;
    std::function<void()> drawFn;
    std::vector<std::unique_ptr<SceneNode>> children;

    explicit SceneNode(std::string n) : name(std::move(n)) {}

    void render() {
        glPushMatrix();
        glTranslatef(transform.tx, transform.ty, 0.0f);
        glRotatef(transform.rotationDeg, 0.0f, 0.0f, 1.0f);
        glScalef(transform.sx, transform.sy, 1.0f);

        if (drawFn) drawFn();
        for (auto& child : children) child->render();

        glPopMatrix();
    }
};

float t = 0.0f;
float speed = 1.0f;
bool paused = false;

std::unique_ptr<SceneNode> root;
SceneNode* pivotOuter = nullptr;
SceneNode* pivotInner = nullptr;

void drawCircle(float radius, float r, float g, float b, int seg = 40) {
    glColor3f(r, g, b);
    glBegin(GL_POLYGON);
    for (int i = 0; i < seg; ++i) {
        const float a = 2.0f * 3.1415926f * static_cast<float>(i) / static_cast<float>(seg);
        glVertex2f(radius * std::cos(a), radius * std::sin(a));
    }
    glEnd();
}

void drawOrbit(float radius) {
    glColor3f(0.2f, 0.2f, 0.25f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 96; ++i) {
        const float a = 2.0f * 3.1415926f * static_cast<float>(i) / 96.0f;
        glVertex2f(radius * std::cos(a), radius * std::sin(a));
    }
    glEnd();
}

void buildScene() {
    root = std::make_unique<SceneNode>("root");

    auto core = std::make_unique<SceneNode>("core");
    core->drawFn = [] { drawCircle(0.10f, 1.0f, 0.85f, 0.2f, 60); };

    auto outer = std::make_unique<SceneNode>("outer");
    outer->transform.tx = 0.52f;
    outer->drawFn = [] { drawCircle(0.035f, 0.25f, 0.6f, 1.0f, 36); };

    auto inner = std::make_unique<SceneNode>("inner");
    inner->transform.tx = 0.14f;
    inner->drawFn = [] { drawCircle(0.015f, 0.85f, 0.85f, 0.95f, 24); };

    auto pivotOuterNode = std::make_unique<SceneNode>("pivotOuter");
    auto pivotInnerNode = std::make_unique<SceneNode>("pivotInner");

    pivotOuter = pivotOuterNode.get();
    pivotInner = pivotInnerNode.get();

    pivotInnerNode->children.push_back(std::move(inner));
    pivotOuterNode->children.push_back(std::move(outer));
    pivotOuterNode->children.push_back(std::move(pivotInnerNode));

    root->children.push_back(std::move(core));
    root->children.push_back(std::move(pivotOuterNode));
}

void update() {
    if (!paused) t += 0.012f * speed;
    if (pivotOuter) pivotOuter->transform.rotationDeg = t * 80.0f;
    if (pivotInner) pivotInner->transform.rotationDeg = t * 240.0f;
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    drawOrbit(0.52f);
    glPushMatrix();
    glRotatef(t * 80.0f, 0.0f, 0.0f, 1.0f);
    glTranslatef(0.52f, 0.0f, 0.0f);
    drawOrbit(0.14f);
    glPopMatrix();

    if (root) root->render();

    glutSwapBuffers();
}

void keyboard(unsigned char key, int, int) {
    if (key == ' ') paused = !paused;
    else if (key == '+') speed = aula::clamp(speed + 0.2f, 0.2f, 6.0f);
    else if (key == '-') speed = aula::clamp(speed - 0.2f, 0.2f, 6.0f);
    else if (key == 27) std::exit(0);
}

void tick(int) {
    update();
    glutPostRedisplay();
    glutTimerFunc(16, tick, 0);
}

} // namespace

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    aula::configureCloseBehavior();
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1100, 800);
    glutCreateWindow("Aula 08 - Scene Graph 2D (C++)");

    glClearColor(0.03f, 0.03f, 0.06f, 1.0f);
    buildScene();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, tick, 0);
    glutMainLoop();
    return 0;
}
