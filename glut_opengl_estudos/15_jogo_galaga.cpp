/*
Aula 15 — Jogo Galaga (C++)

Conceitos aplicados:
- Entidades: jogador, inimigos e projéteis.
- Progressão por ondas com aumento de dificuldade.
- Colisão AABB, vidas, score e game over.

Controles:
- A/D ou setas: mover nave
- Espaço: atirar
- + / -: velocidade geral
- P: pausa
- R: reiniciar
- ESC: sair
*/

#include <GL/glut.h>
#include <algorithm>
#include <cstdlib>
#include <random>
#include <string>
#include <vector>
#include "cpp_glut_utils.hpp"

namespace {

constexpr float WORLD_W = 100.0f;
constexpr float WORLD_H = 100.0f;
constexpr int WINDOW_W = 1200;
constexpr int WINDOW_H = 850;
constexpr float MIN_GAME_SPEED = 0.6f;
constexpr float MAX_GAME_SPEED = 2.4f;
constexpr int LOGIC_STEP_MS = 16;
constexpr int MAX_LOGIC_STEPS_PER_TICK = 5;
constexpr int MAX_PLAYER_BULLETS = 6;
constexpr int MAX_ENEMY_BULLETS = 64;

struct Player { float x, y, w, h, speed; };
struct Enemy { float x, y, w, h; bool alive = true; };
struct Bullet { float x, y, vy; bool fromPlayer; };

Player player{50.0f, 8.0f, 5.0f, 3.0f, 1.35f};
std::vector<Enemy> enemies;
std::vector<Bullet> playerBullets;
std::vector<Bullet> enemyBullets;

int score = 0;
int lives = 3;
int level = 1;
bool paused = false;
bool gameOver = false;
bool waveCleared = false;
float gameSpeed = 1.0f;

bool moveLeft = false;
bool moveRight = false;
int enemyDirection = 1;
float enemySpeed = 0.18f;
int shootCooldown = 0;
bool shootKeyArmed = true;
bool restartRequested = false;

int lastTickMs = 0;
int logicAccumulatorMs = 0;
std::mt19937 rng{std::random_device{}()};

bool intersects(float ax, float ay, float aw, float ah, float bx, float by, float bw, float bh) {
    const float al = ax - aw * 0.5f, ar = ax + aw * 0.5f;
    const float ab = ay - ah * 0.5f, at = ay + ah * 0.5f;
    const float bl = bx - bw * 0.5f, br = bx + bw * 0.5f;
    const float bb = by - bh * 0.5f, bt = by + bh * 0.5f;
    return !(ar < bl || al > br || at < bb || ab > bt);
}

void drawRect(float cx, float cy, float w, float h) {
    const float hw = w * 0.5f;
    const float hh = h * 0.5f;
    glBegin(GL_QUADS);
    glVertex2f(cx - hw, cy - hh);
    glVertex2f(cx + hw, cy - hh);
    glVertex2f(cx + hw, cy + hh);
    glVertex2f(cx - hw, cy + hh);
    glEnd();
}

std::vector<Enemy*> aliveEnemies() {
    std::vector<Enemy*> out;
    for (auto& e : enemies) if (e.alive) out.push_back(&e);
    return out;
}

void buildWave() {
    enemies.clear();
    const int rows = 5, cols = 9;
    const float spacingX = 8.6f, spacingY = 6.0f;
    const float startX = 14.0f, startY = 86.0f;

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            enemies.push_back({startX + c * spacingX, startY - r * spacingY, 4.7f, 3.0f, true});
        }
    }

    enemyDirection = 1;
    enemySpeed = 0.18f + (level - 1) * 0.03f;
    waveCleared = false;
}

void restartGame() {
    score = 0;
    lives = 3;
    level = 1;
    paused = false;
    gameOver = false;
    gameSpeed = 1.0f;
    playerBullets.clear();
    enemyBullets.clear();
    shootCooldown = 0;
    shootKeyArmed = true;
    restartRequested = false;
    lastTickMs = glutGet(GLUT_ELAPSED_TIME);
    logicAccumulatorMs = 0;
    player.x = 50.0f;
    buildWave();
}

void startNextWave() {
    level++;
    playerBullets.clear();
    enemyBullets.clear();
    buildWave();
}

void movePlayer() {
    const float step = player.speed * gameSpeed;
    if (moveLeft) player.x -= step;
    if (moveRight) player.x += step;
    const float hw = player.w * 0.5f;
    player.x = aula::clamp(player.x, hw, WORLD_W - hw);
}

void updateEnemySwarm() {
    auto alive = aliveEnemies();
    if (alive.empty()) return;

    const float stepX = enemySpeed * gameSpeed * static_cast<float>(enemyDirection);
    bool hitWall = false;

    for (auto* e : alive) {
        e->x += stepX;
        if (e->x + e->w * 0.5f >= WORLD_W - 1.0f || e->x - e->w * 0.5f <= 1.0f) hitWall = true;
    }

    if (hitWall) {
        enemyDirection *= -1;
        for (auto* e : alive) e->y -= 2.4f;
    }
}

void spawnEnemyBullet() {
    auto alive = aliveEnemies();
    if (alive.empty()) return;
    if (static_cast<int>(enemyBullets.size()) >= MAX_ENEMY_BULLETS) return;

    std::uniform_real_distribution<float> p(0.0f, 1.0f);
    if (p(rng) < 0.03f * gameSpeed) {
        std::uniform_int_distribution<int> idx(0, static_cast<int>(alive.size()) - 1);
        Enemy* s = alive[idx(rng)];
        enemyBullets.push_back({s->x, s->y - s->h * 0.5f - 0.8f, -0.95f, false});
    }
}

void updateBullets() {
    for (auto& b : playerBullets) b.y += b.vy * gameSpeed;
    for (auto& b : enemyBullets) b.y += b.vy * gameSpeed;

    playerBullets.erase(std::remove_if(playerBullets.begin(), playerBullets.end(), [](const Bullet& b){ return b.y > WORLD_H + 2.0f || b.y < -2.0f; }), playerBullets.end());
    enemyBullets.erase(std::remove_if(enemyBullets.begin(), enemyBullets.end(), [](const Bullet& b){ return b.y > WORLD_H + 2.0f || b.y < -2.0f; }), enemyBullets.end());
}

void handleCollisions() {
    std::vector<Bullet> keptPlayer;
    for (const auto& b : playerBullets) {
        bool hit = false;
        for (auto& e : enemies) {
            if (!e.alive) continue;
            if (intersects(b.x, b.y, 1.1f, 2.0f, e.x, e.y, e.w, e.h)) {
                e.alive = false;
                score += 100;
                hit = true;
                break;
            }
        }
        if (!hit) keptPlayer.push_back(b);
    }
    playerBullets.swap(keptPlayer);

    std::vector<Bullet> keptEnemy;
    for (const auto& b : enemyBullets) {
        if (intersects(b.x, b.y, 1.2f, 2.0f, player.x, player.y, player.w, player.h)) {
            lives--;
            if (lives <= 0) gameOver = true;
            continue;
        }
        keptEnemy.push_back(b);
    }
    enemyBullets.swap(keptEnemy);

    for (auto* e : aliveEnemies()) {
        if (intersects(e->x, e->y, e->w, e->h, player.x, player.y, player.w, player.h)) {
            gameOver = true;
            return;
        }
        if (e->y - e->h * 0.5f <= player.y + player.h * 0.5f) {
            gameOver = true;
            return;
        }
    }

    if (aliveEnemies().empty() && !waveCleared && !gameOver) waveCleared = true;
}

void updateGame() {
    if (restartRequested) {
        restartGame();
        return;
    }
    if (paused || gameOver) return;
    if (waveCleared) startNextWave();

    movePlayer();
    updateEnemySwarm();
    spawnEnemyBullet();
    updateBullets();
    handleCollisions();

    shootCooldown = std::max(0, shootCooldown - 1);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glColor3f(0.35f, 0.9f, 1.0f);
    drawRect(player.x, player.y, player.w, player.h);

    for (const auto& e : enemies) {
        if (!e.alive) continue;
        glColor3f(1.0f, 0.45f, 0.45f);
        drawRect(e.x, e.y, e.w, e.h);
    }

    glColor3f(1.0f, 1.0f, 0.45f);
    for (const auto& b : playerBullets) drawRect(b.x, b.y, 0.8f, 1.8f);

    glColor3f(1.0f, 0.65f, 0.3f);
    for (const auto& b : enemyBullets) drawRect(b.x, b.y, 0.8f, 1.8f);

    glColor3f(0.85f, 0.92f, 1.0f);
    aula::drawBitmapText(2.0f, 96.0f, "Score: " + std::to_string(score));
    aula::drawBitmapText(18.0f, 96.0f, "Vidas: " + std::to_string(lives));
    aula::drawBitmapText(33.0f, 96.0f, "Nivel: " + std::to_string(level));
    aula::drawBitmapText(48.0f, 96.0f, "Vel: " + std::to_string(gameSpeed).substr(0,3) + "x");

    if (paused && !gameOver) {
        glColor3f(1.0f, 1.0f, 0.6f);
        aula::drawBitmapText(44.0f, 52.0f, "PAUSADO");
    }

    if (gameOver) {
        glColor3f(1.0f, 0.6f, 0.6f);
        aula::drawBitmapText(41.0f, 52.0f, "GAME OVER");
    }

    glutSwapBuffers();
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, WORLD_W, 0.0, WORLD_H, -1.0, 1.0);
}

void keyDownFn(unsigned char key, int, int) {
    if (key == 'a' || key == 'A') moveLeft = true;
    else if (key == 'd' || key == 'D') moveRight = true;
    else if (key == ' ') {
        const bool canShoot = (!paused && !gameOver && shootCooldown == 0 && shootKeyArmed);
        if (canShoot && static_cast<int>(playerBullets.size()) < MAX_PLAYER_BULLETS) {
            playerBullets.push_back({player.x, player.y + player.h * 0.5f + 0.8f, 1.35f, true});
            shootCooldown = 9;
            shootKeyArmed = false;
        }
    }
    else if (key == 'p' || key == 'P') {
        if (!gameOver) paused = !paused;
    }
    else if (key == '+') gameSpeed = aula::clamp(gameSpeed + 0.1f, MIN_GAME_SPEED, MAX_GAME_SPEED);
    else if (key == '-') gameSpeed = aula::clamp(gameSpeed - 0.1f, MIN_GAME_SPEED, MAX_GAME_SPEED);
    else if (key == 'r' || key == 'R') restartRequested = true;
    else if (key == 27) std::exit(0);
}

void keyUpFn(unsigned char key, int, int) {
    if (key == 'a' || key == 'A') moveLeft = false;
    else if (key == 'd' || key == 'D') moveRight = false;
    else if (key == ' ') shootKeyArmed = true;
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
    const int nowMs = glutGet(GLUT_ELAPSED_TIME);
    const int elapsedMs = std::max(0, nowMs - lastTickMs);
    lastTickMs = nowMs;

    logicAccumulatorMs += elapsedMs;

    int steps = 0;
    while (logicAccumulatorMs >= LOGIC_STEP_MS && steps < MAX_LOGIC_STEPS_PER_TICK) {
        updateGame();
        logicAccumulatorMs -= LOGIC_STEP_MS;
        steps++;
    }

    if (steps == MAX_LOGIC_STEPS_PER_TICK && logicAccumulatorMs > LOGIC_STEP_MS * 2) {
        logicAccumulatorMs = LOGIC_STEP_MS * 2;
    }

    glutPostRedisplay();
    glutTimerFunc(LOGIC_STEP_MS, tick, 0);
}

} // namespace

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    aula::configureCloseBehavior();
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_W, WINDOW_H);
    glutCreateWindow("Aula 15 - Jogo Galaga (C++)");

    glClearColor(0.03f, 0.03f, 0.07f, 1.0f);
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
