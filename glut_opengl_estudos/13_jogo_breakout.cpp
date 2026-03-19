/*
Aula 13 — Jogo Breakout (C++)

Conceitos aplicados:
- Colisão bola x parede, raquete e blocos.
- Vida, pontuação e estados de vitória/derrota.
- Controle contínuo com teclado normal + teclas especiais.
*/

#include <GL/glut.h>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <random>
#include <string>
#include <vector>
#include "cpp_glut_utils.hpp"

namespace {

constexpr float WORLD_W = 100.0f;
constexpr float WORLD_H = 100.0f;
constexpr int WINDOW_W = 1100;
constexpr int WINDOW_H = 780;
constexpr int MAX_LIVES = 3;

struct Paddle { float x, y, w, h, speed; };
struct Ball { float x, y, radius, vx, vy; };
struct Brick { float x, y, w, h; int hp; };

Paddle paddle{50.0f, 8.0f, 18.0f, 2.8f, 1.9f};
Ball ball{50.0f, 14.0f, 1.2f, 0.66f, 0.82f};
std::vector<Brick> bricks;

int score = 0;
int lives = MAX_LIVES;
bool paused = false;
bool gameOver = false;
bool victory = false;

bool moveLeft = false;
bool moveRight = false;
std::mt19937 rng{std::random_device{}()};

void drawRect(float x, float y, float w, float h) {
    const float hw = w * 0.5f;
    const float hh = h * 0.5f;
    glBegin(GL_QUADS);
    glVertex2f(x - hw, y - hh);
    glVertex2f(x + hw, y - hh);
    glVertex2f(x + hw, y + hh);
    glVertex2f(x - hw, y + hh);
    glEnd();
}

std::vector<Brick> createBricks() {
    std::vector<Brick> out;
    const int rows = 6, cols = 10;
    const float bw = 8.2f, bh = 3.2f;
    const float sx = 9.0f, sy = 80.0f;
    const float gx = 0.8f, gy = 0.8f;

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            const float x = sx + c * (bw + gx);
            const float y = sy - r * (bh + gy);
            const int hp = (r < 2) ? 2 : 1;
            out.push_back({x, y, bw, bh, hp});
        }
    }
    return out;
}

void resetBall() {
    std::uniform_int_distribution<int> dist(0, 1);
    ball.x = paddle.x;
    ball.y = 14.0f;
    ball.vx = dist(rng) ? 0.66f : -0.66f;
    ball.vy = 0.82f;
}

void restartGame() {
    bricks = createBricks();
    score = 0;
    lives = MAX_LIVES;
    paused = false;
    gameOver = false;
    victory = false;
    paddle.x = 50.0f;
    resetBall();
}

void clampPaddle() {
    const float hw = paddle.w * 0.5f;
    paddle.x = aula::clamp(paddle.x, hw, WORLD_W - hw);
}

bool intersectsRect(float cx, float cy, float r, float rx, float ry, float rw, float rh) {
    const float left = rx - rw * 0.5f;
    const float right = rx + rw * 0.5f;
    const float bottom = ry - rh * 0.5f;
    const float top = ry + rh * 0.5f;

    const float closestX = std::max(left, std::min(cx, right));
    const float closestY = std::max(bottom, std::min(cy, top));

    const float dx = cx - closestX;
    const float dy = cy - closestY;
    return dx * dx + dy * dy <= r * r;
}

void updateGame() {
    if (gameOver || victory) return;

    if (moveLeft) paddle.x -= paddle.speed;
    if (moveRight) paddle.x += paddle.speed;
    clampPaddle();

    ball.x += ball.vx;
    ball.y += ball.vy;

    if (ball.x - ball.radius <= 0.0f || ball.x + ball.radius >= WORLD_W) ball.vx *= -1.0f;
    if (ball.y + ball.radius >= WORLD_H) ball.vy *= -1.0f;

    if (ball.y - ball.radius <= 0.0f) {
        lives--;
        if (lives <= 0) gameOver = true;
        else resetBall();
        return;
    }

    if (intersectsRect(ball.x, ball.y, ball.radius, paddle.x, paddle.y, paddle.w, paddle.h)) {
        const float hitOffset = (ball.x - paddle.x) / (paddle.w * 0.5f);
        ball.vy = std::fabs(ball.vy);
        ball.vx += 0.11f * hitOffset;
        ball.vx = aula::clamp(ball.vx, -1.4f, 1.4f);
    }

    for (auto it = bricks.begin(); it != bricks.end(); ++it) {
        if (!intersectsRect(ball.x, ball.y, ball.radius, it->x, it->y, it->w, it->h)) continue;

        ball.vy *= -1.0f;
        it->hp -= 1;

        if (it->hp <= 0) {
            score += 10;
            bricks.erase(it);
        } else {
            score += 4;
        }
        break;
    }

    if (bricks.empty()) victory = true;
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glColor3f(0.2f, 0.7f, 1.0f);
    drawRect(paddle.x, paddle.y, paddle.w, paddle.h);

    glColor3f(1.0f, 0.85f, 0.3f);
    drawRect(ball.x, ball.y, ball.radius * 2.0f, ball.radius * 2.0f);

    for (const auto& br : bricks) {
        if (br.hp == 2) glColor3f(1.0f, 0.45f, 0.45f);
        else glColor3f(0.9f, 0.8f, 0.35f);
        drawRect(br.x, br.y, br.w, br.h);
    }

    glColor3f(0.85f, 0.92f, 1.0f);
    aula::drawBitmapText(2.0f, 96.0f, "Score: " + std::to_string(score));
    aula::drawBitmapText(22.0f, 96.0f, "Vidas: " + std::to_string(lives));
    aula::drawBitmapText(46.0f, 96.0f, "A/D ou Setas | ESPACO pausa | R reinicia | ESC");

    if (paused && !(gameOver || victory)) {
        glColor3f(1.0f, 1.0f, 0.6f);
        aula::drawBitmapText(44.0f, 49.0f, "PAUSADO");
    }

    if (gameOver) {
        glColor3f(1.0f, 0.55f, 0.55f);
        aula::drawBitmapText(41.0f, 50.0f, "GAME OVER");
    }

    if (victory) {
        glColor3f(0.7f, 1.0f, 0.7f);
        aula::drawBitmapText(42.0f, 50.0f, "VOCE VENCEU!");
    }

    glutSwapBuffers();
}

void reshape(int width, int height) {
    if (height == 0) height = 1;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, WORLD_W, 0.0, WORLD_H, -1.0, 1.0);
}

void keyDownFn(unsigned char key, int, int) {
    if (key == 'a' || key == 'A') moveLeft = true;
    else if (key == 'd' || key == 'D') moveRight = true;
    else if (key == ' ') {
        if (!(gameOver || victory)) paused = !paused;
    }
    else if (key == 'r' || key == 'R') restartGame();
    else if (key == 27) std::exit(0);
}

void keyUpFn(unsigned char key, int, int) {
    if (key == 'a' || key == 'A') moveLeft = false;
    else if (key == 'd' || key == 'D') moveRight = false;
}

void specialDownFn(int key, int, int) {
    if (key == GLUT_KEY_LEFT) moveLeft = true;
    else if (key == GLUT_KEY_RIGHT) moveRight = true;
}

void specialUpFn(int key, int, int) {
    if (key == GLUT_KEY_LEFT) moveLeft = false;
    else if (key == GLUT_KEY_RIGHT) moveRight = false;
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
    glutInitWindowSize(WINDOW_W, WINDOW_H);
    glutCreateWindow("Aula 13 - Jogo Breakout (C++)");

    glClearColor(0.04f, 0.04f, 0.07f, 1.0f);
    restartGame();
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
