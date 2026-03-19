/*
Aula 12 — Jogo Snake (C++)

Conceitos aplicados:
- Simulação em grade (grid) com atualização discreta.
- Estrutura de dados de corpo da cobra com `std::deque`.
- Spawn de comida em célula livre.
- Controle de direção com bloqueio de inversão imediata.

Controles:
- Direção: setas ou WASD
- + / -: velocidade manual
- Espaço: pausa
- R: reiniciar
- ESC: sair
*/

#include <GL/glut.h>
#include <algorithm>
#include <cstdlib>
#include <deque>
#include <random>
#include <set>
#include <string>
#include "cpp_glut_utils.hpp"

namespace {

constexpr int GRID_COLS = 28;
constexpr int GRID_ROWS = 28;
constexpr int WINDOW_WIDTH = 900;
constexpr int WINDOW_HEIGHT = 900;
constexpr int BASE_TICK_MS = 105;
constexpr float MIN_SPEED_FACTOR = 0.6f;
constexpr float MAX_SPEED_FACTOR = 2.4f;

struct GridPoint {
    int col;
    int row;
    bool operator<(const GridPoint& other) const {
        if (col != other.col) return col < other.col;
        return row < other.row;
    }
    bool operator==(const GridPoint& other) const {
        return col == other.col && row == other.row;
    }
};

class SnakeGame {
public:
    SnakeGame() : rng(std::random_device{}()) { reset(); }

    void reset() {
        const GridPoint center{GRID_COLS / 2, GRID_ROWS / 2};
        snake.clear();
        snake.push_front(center);
        snake.push_back({center.col - 1, center.row});
        snake.push_back({center.col - 2, center.row});

        direction = {1, 0};
        nextDirection = {1, 0};
        food = spawnFood();
        score = 0;
        gameOver = false;
        paused = false;
    }

    void trySetDirection(const GridPoint& newDir) {
        if (newDir.col == -direction.col && newDir.row == -direction.row) return;
        nextDirection = newDir;
    }

    void update() {
        if (paused || gameOver) return;

        direction = nextDirection;
        const GridPoint head = snake.front();
        const GridPoint newHead{head.col + direction.col, head.row + direction.row};

        const bool outOfBounds =
            newHead.col < 0 || newHead.col >= GRID_COLS ||
            newHead.row < 0 || newHead.row >= GRID_ROWS;

        if (outOfBounds || contains(snake, newHead)) {
            gameOver = true;
            highScore = std::max(highScore, score);
            return;
        }

        snake.push_front(newHead);
        if (newHead == food) {
            score++;
            highScore = std::max(highScore, score);
            food = spawnFood();
        } else {
            snake.pop_back();
        }
    }

    void render(float manualSpeedFactor) const {
        glClear(GL_COLOR_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        int index = 0;
        for (const auto& segment : snake) {
            if (index == 0) glColor3f(0.35f, 1.0f, 0.55f);
            else glColor3f(0.18f, 0.9f, 0.45f);
            drawCell(segment);
            index++;
        }

        glColor3f(1.0f, 0.3f, 0.3f);
        drawCell(food);

        glColor3f(0.8f, 0.9f, 1.0f);
        aula::drawBitmapText(0.3f, GRID_ROWS - 0.8f, "Score: " + std::to_string(score));
        aula::drawBitmapText(6.0f, GRID_ROWS - 0.8f, "High: " + std::to_string(highScore));
        aula::drawBitmapText(12.5f, GRID_ROWS - 0.8f, "Vel: " + std::to_string(manualSpeedFactor).substr(0, 3) + "x");

        if (paused && !gameOver) {
            glColor3f(1.0f, 1.0f, 0.5f);
            aula::drawBitmapText(11.5f, 14.2f, "PAUSADO");
        }

        if (gameOver) {
            glColor3f(1.0f, 0.75f, 0.5f);
            aula::drawBitmapText(9.2f, 14.2f, "GAME OVER");
            aula::drawBitmapText(6.9f, 12.7f, "Pressione R para reiniciar");
        }

        glutSwapBuffers();
    }

    int score = 0;
    int highScore = 0;
    bool gameOver = false;
    bool paused = false;

private:
    std::mt19937 rng;
    std::deque<GridPoint> snake;
    GridPoint direction{1, 0};
    GridPoint nextDirection{1, 0};
    GridPoint food{0, 0};

    static bool contains(const std::deque<GridPoint>& body, const GridPoint& p) {
        return std::find(body.begin(), body.end(), p) != body.end();
    }

    GridPoint spawnFood() {
        std::set<GridPoint> occupied(snake.begin(), snake.end());
        std::uniform_int_distribution<int> colDist(0, GRID_COLS - 1);
        std::uniform_int_distribution<int> rowDist(0, GRID_ROWS - 1);

        while (true) {
            GridPoint candidate{colDist(rng), rowDist(rng)};
            if (!occupied.count(candidate)) return candidate;
        }
    }

    static void drawCell(const GridPoint& p) {
        const float left = static_cast<float>(p.col);
        const float right = static_cast<float>(p.col + 1);
        const float bottom = static_cast<float>(p.row);
        const float top = static_cast<float>(p.row + 1);
        constexpr float margin = 0.08f;

        glBegin(GL_QUADS);
        glVertex2f(left + margin, bottom + margin);
        glVertex2f(right - margin, bottom + margin);
        glVertex2f(right - margin, top - margin);
        glVertex2f(left + margin, top - margin);
        glEnd();
    }
};

SnakeGame game;
float manualSpeedFactor = 1.0f;

void display() { game.render(manualSpeedFactor); }

void reshape(int width, int height) {
    if (height == 0) height = 1;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, GRID_COLS, 0.0, GRID_ROWS, -1.0, 1.0);
}

void keyboard(unsigned char key, int, int) {
    if (key == 'w' || key == 'W') game.trySetDirection({0, 1});
    else if (key == 's' || key == 'S') game.trySetDirection({0, -1});
    else if (key == 'a' || key == 'A') game.trySetDirection({-1, 0});
    else if (key == 'd' || key == 'D') game.trySetDirection({1, 0});
    else if (key == ' ') {
        if (!game.gameOver) game.paused = !game.paused;
    }
    else if (key == '+') manualSpeedFactor = aula::clamp(manualSpeedFactor + 0.1f, MIN_SPEED_FACTOR, MAX_SPEED_FACTOR);
    else if (key == '-') manualSpeedFactor = aula::clamp(manualSpeedFactor - 0.1f, MIN_SPEED_FACTOR, MAX_SPEED_FACTOR);
    else if (key == 'r' || key == 'R') {
        game.reset();
        manualSpeedFactor = 1.0f;
    }
    else if (key == 27) std::exit(0);
}

void special(int key, int, int) {
    if (key == GLUT_KEY_UP) game.trySetDirection({0, 1});
    else if (key == GLUT_KEY_DOWN) game.trySetDirection({0, -1});
    else if (key == GLUT_KEY_LEFT) game.trySetDirection({-1, 0});
    else if (key == GLUT_KEY_RIGHT) game.trySetDirection({1, 0});
}

void tick(int) {
    game.update();
    glutPostRedisplay();

    const int speedBonus = std::min(game.score, 12);
    const int baseInterval = std::max(55, BASE_TICK_MS - speedBonus * 3);
    const int nextInterval = std::max(35, static_cast<int>(static_cast<float>(baseInterval) / manualSpeedFactor));
    glutTimerFunc(nextInterval, tick, 0);
}

} // namespace

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    aula::configureCloseBehavior();
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Aula 12 - Jogo Snake (C++)");

    glClearColor(0.03f, 0.03f, 0.05f, 1.0f);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutTimerFunc(BASE_TICK_MS, tick, 0);
    glutMainLoop();
    return 0;
}
