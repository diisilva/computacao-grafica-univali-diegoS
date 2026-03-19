/*
Aula 11 — Jogo Pong 2D totalmente jogável (C++)

Conceitos aplicados:
- Loop de jogo em timer fixo (16ms ≈ 60 FPS).
- Entrada contínua por estado de teclas pressionadas/soltas.
- IA simples controlando a raquete da direita.
- Colisão bola-parede e bola-raquete com resposta ajustada.
- Pontuação, pausa, reinício e ajuste de velocidade global.

Controles:
- Jogador esquerdo: W/S ou setas ↑/↓
- + / -: velocidade geral
- Espaço: pausar
- R: reiniciar
- ESC: sair
*/

#include <GL/glut.h>
#include <GL/glu.h>
#include <cmath>
#include <cstdlib>
#include <random>
#include <string>
#include "cpp_glut_utils.hpp"

namespace {

constexpr int WINDOW_WIDTH = 1200;
constexpr int WINDOW_HEIGHT = 800;
constexpr float WORLD_LEFT = 0.0f;
constexpr float WORLD_RIGHT = 100.0f;
constexpr float WORLD_BOTTOM = 0.0f;
constexpr float WORLD_TOP = 100.0f;
constexpr int WIN_SCORE = 10;
constexpr float MIN_GAME_SPEED = 0.5f;
constexpr float MAX_GAME_SPEED = 2.5f;

struct Paddle {
    float x;
    float y;
    float width = 2.0f;
    float height = 18.0f;
    float speed = 1.45f;

    float top() const { return y + height * 0.5f; }
    float bottom() const { return y - height * 0.5f; }
};

struct Ball {
    float x;
    float y;
    float radius;
    float vx;
    float vy;
};

Paddle leftPaddle{6.0f, 50.0f};
Paddle rightPaddle{94.0f, 50.0f};
Ball ball{50.0f, 50.0f, 1.4f, 0.78f, 0.52f};

int leftScore = 0;
int rightScore = 0;
bool paused = false;
std::string winnerText;
float gameSpeed = 1.0f;

bool keyW = false;
bool keyS = false;
bool keyUp = false;
bool keyDown = false;

std::mt19937 rng{std::random_device{}()};

void drawFilledRect(float cx, float cy, float w, float h) {
    const float hw = w * 0.5f;
    const float hh = h * 0.5f;
    glBegin(GL_QUADS);
    glVertex2f(cx - hw, cy - hh);
    glVertex2f(cx + hw, cy - hh);
    glVertex2f(cx + hw, cy + hh);
    glVertex2f(cx - hw, cy + hh);
    glEnd();
}

void drawBall() {
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 28; ++i) {
        const float a = 2.0f * 3.1415926f * static_cast<float>(i) / 28.0f;
        glVertex2f(ball.x + ball.radius * std::cos(a), ball.y + ball.radius * std::sin(a));
    }
    glEnd();
}

void clampPaddle(Paddle& p) {
    const float hh = p.height * 0.5f;
    p.y = aula::clamp(p.y, WORLD_BOTTOM + hh, WORLD_TOP - hh);
}

void resetBall(int direction) {
    std::uniform_int_distribution<int> dist(0, 1);
    ball.x = 50.0f;
    ball.y = 50.0f;
    ball.vx = 0.78f * static_cast<float>(direction);
    ball.vy = dist(rng) ? 0.52f : -0.52f;
}

void updateRightPaddleAI() {
    const float target = ball.y;
    const float diff = target - rightPaddle.y;
    const float deadZone = 1.1f;
    const float aiSpeed = 1.18f * gameSpeed;

    if (std::fabs(diff) <= deadZone) return;

    if (diff > 0.0f) rightPaddle.y += std::min(aiSpeed, diff);
    else rightPaddle.y -= std::min(aiSpeed, -diff);
}

void checkCollision(Paddle& p, bool movingLeft) {
    bool hitX = false;
    bool insideX = false;

    if (movingLeft) {
        hitX = ball.x - ball.radius <= p.x + p.width * 0.5f;
        insideX = ball.x >= p.x;
    } else {
        hitX = ball.x + ball.radius >= p.x - p.width * 0.5f;
        insideX = ball.x <= p.x;
    }

    const bool insideY = ball.y >= p.bottom() && ball.y <= p.top();
    if (!(hitX && insideX && insideY)) return;

    const float relative = (ball.y - p.y) / (p.height * 0.5f);
    ball.vx *= -1.04f;
    ball.vy += 0.25f * relative;
    ball.vx = aula::clamp(ball.vx, -2.1f, 2.1f);
}

void restartGame() {
    leftScore = 0;
    rightScore = 0;
    winnerText.clear();
    leftPaddle.y = 50.0f;
    rightPaddle.y = 50.0f;
    gameSpeed = 1.0f;
    resetBall(+1);
}

void updateGame() {
    if (!winnerText.empty()) return;

    if (keyW || keyUp) leftPaddle.y += leftPaddle.speed * gameSpeed;
    if (keyS || keyDown) leftPaddle.y -= leftPaddle.speed * gameSpeed;

    updateRightPaddleAI();
    clampPaddle(leftPaddle);
    clampPaddle(rightPaddle);

    ball.x += ball.vx * gameSpeed;
    ball.y += ball.vy * gameSpeed;

    if (ball.y + ball.radius >= WORLD_TOP || ball.y - ball.radius <= WORLD_BOTTOM) {
        ball.vy *= -1.0f;
    }

    checkCollision(leftPaddle, true);
    checkCollision(rightPaddle, false);

    if (ball.x < WORLD_LEFT) {
        rightScore++;
        if (rightScore >= WIN_SCORE) winnerText = "Jogador da direita venceu!";
        resetBall(+1);
    }

    if (ball.x > WORLD_RIGHT) {
        leftScore++;
        if (leftScore >= WIN_SCORE) winnerText = "Jogador da esquerda venceu!";
        resetBall(-1);
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glColor3f(0.3f, 0.35f, 0.4f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    for (int row = 0; row < 100; row += 6) {
        glVertex2f(50.0f, static_cast<float>(row));
        glVertex2f(50.0f, static_cast<float>(row + 3));
    }
    glEnd();

    glColor3f(0.9f, 0.9f, 0.95f);
    drawFilledRect(leftPaddle.x, leftPaddle.y, leftPaddle.width, leftPaddle.height);
    drawFilledRect(rightPaddle.x, rightPaddle.y, rightPaddle.width, rightPaddle.height);

    glColor3f(1.0f, 0.8f, 0.3f);
    drawBall();

    glColor3f(0.85f, 0.9f, 1.0f);
    aula::drawBitmapText(24.0f, 95.0f, std::to_string(leftScore));
    aula::drawBitmapText(74.0f, 95.0f, std::to_string(rightScore));
    aula::drawBitmapText(2.0f, 6.0f, "Velocidade: " + std::to_string(gameSpeed).substr(0, 3) + "x");
    aula::drawBitmapText(2.0f, 3.0f, "W/S ou SETAS | +/- vel | ESPACO | R | ESC");

    if (paused && winnerText.empty()) {
        glColor3f(1.0f, 1.0f, 0.5f);
        aula::drawBitmapText(44.0f, 52.0f, "PAUSADO");
    }

    if (!winnerText.empty()) {
        glColor3f(0.7f, 1.0f, 0.7f);
        aula::drawBitmapText(31.0f, 52.0f, winnerText);
        aula::drawBitmapText(28.0f, 47.0f, "Pressione R para nova partida");
    }

    glutSwapBuffers();
}

void reshape(int width, int height) {
    if (height == 0) height = 1;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(WORLD_LEFT, WORLD_RIGHT, WORLD_BOTTOM, WORLD_TOP);
}

void keyDownFn(unsigned char key, int, int) {
    if (key == 'w' || key == 'W') keyW = true;
    else if (key == 's' || key == 'S') keyS = true;
    else if (key == ' ') paused = !paused;
    else if (key == '+') gameSpeed = aula::clamp(gameSpeed + 0.1f, MIN_GAME_SPEED, MAX_GAME_SPEED);
    else if (key == '-') gameSpeed = aula::clamp(gameSpeed - 0.1f, MIN_GAME_SPEED, MAX_GAME_SPEED);
    else if (key == 'r' || key == 'R') restartGame();
    else if (key == 27) std::exit(0);
}

void keyUpFn(unsigned char key, int, int) {
    if (key == 'w' || key == 'W') keyW = false;
    else if (key == 's' || key == 'S') keyS = false;
}

void specialDownFn(int key, int, int) {
    if (key == GLUT_KEY_UP) keyUp = true;
    else if (key == GLUT_KEY_DOWN) keyDown = true;
}

void specialUpFn(int key, int, int) {
    if (key == GLUT_KEY_UP) keyUp = false;
    else if (key == GLUT_KEY_DOWN) keyDown = false;
}

void tick(int) {
    if (!paused) updateGame();
    glutPostRedisplay();
    glutTimerFunc(16, tick, 0);
}

} // namespace

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    aula::configureCloseBehavior();
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowPosition(120, 80);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Aula 11 - Jogo Pong (C++)");

    glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyDownFn);
    glutKeyboardUpFunc(keyUpFn);
    glutSpecialFunc(specialDownFn);
#ifdef FREEGLUT
    glutSpecialUpFunc(specialUpFn);
#endif
    glutTimerFunc(0, tick, 0);
    glutMainLoop();
    return 0;
}
