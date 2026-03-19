/*
Aula 14 — Jogo Tetris (C++)

Conceitos aplicados:
- Tabuleiro em grade 10x20.
- Peças tetromino com rotações pré-definidas.
- Colisão, travamento de peça e limpeza de linhas.
- Score, nível e velocidade dinâmica.

Observação didática:
Este código mantém uma implementação clara (não super-otimizada)
para facilitar leitura por quem está aprendendo.
*/

#include <GL/glut.h>
#include <algorithm>
#include <array>
#include <cstdlib>
#include <map>
#include <random>
#include <string>
#include <vector>
#include "cpp_glut_utils.hpp"

namespace {

constexpr int BOARD_W = 10;
constexpr int BOARD_H = 20;
constexpr int WINDOW_W = 1000;
constexpr int WINDOW_H = 900;
constexpr int BASE_TICK_MS = 520;
constexpr int MIN_TICK_MS = 110;
constexpr int MAX_LOGIC_STEPS_PER_TICK = 5;

using Cell = std::pair<int, int>; // (coluna, linha)
using Rotation = std::vector<Cell>;
using PieceRotations = std::vector<Rotation>;

struct ActivePiece {
    char shape;
    int rotation;
    int x;
    int y;
};

std::map<char, PieceRotations> pieces = {
    {'I', {{{0,1},{1,1},{2,1},{3,1}}, {{2,0},{2,1},{2,2},{2,3}}}},
    {'O', {{{1,1},{2,1},{1,2},{2,2}}}},
    {'T', {{{1,1},{0,2},{1,2},{2,2}}, {{1,1},{1,2},{2,2},{1,3}}, {{0,2},{1,2},{2,2},{1,3}}, {{1,1},{0,2},{1,2},{1,3}}}},
    {'S', {{{1,1},{2,1},{0,2},{1,2}}, {{1,1},{1,2},{2,2},{2,3}}}},
    {'Z', {{{0,1},{1,1},{1,2},{2,2}}, {{2,1},{1,2},{2,2},{1,3}}}},
    {'J', {{{0,1},{0,2},{1,2},{2,2}}, {{1,1},{2,1},{1,2},{1,3}}, {{0,2},{1,2},{2,2},{2,3}}, {{1,1},{1,2},{0,3},{1,3}}}},
    {'L', {{{2,1},{0,2},{1,2},{2,2}}, {{1,1},{1,2},{1,3},{2,3}}, {{0,2},{1,2},{2,2},{0,3}}, {{0,1},{1,1},{1,2},{1,3}}}}
};

std::map<char, std::array<float,3>> colors = {
    {'I',{0.3f,0.9f,1.0f}}, {'O',{1.0f,0.9f,0.3f}}, {'T',{0.8f,0.4f,1.0f}},
    {'S',{0.4f,1.0f,0.45f}}, {'Z',{1.0f,0.4f,0.4f}}, {'J',{0.35f,0.55f,1.0f}}, {'L',{1.0f,0.65f,0.35f}}
};

std::vector<std::vector<char>> board(BOARD_H, std::vector<char>(BOARD_W, 0));
ActivePiece current{'I',0,3,16};
char nextShape = 'I';
std::mt19937 rng{std::random_device{}()};

int score = 0;
int linesTotal = 0;
int level = 1;
bool paused = false;
bool gameOver = false;
bool hardDropArmed = true;
bool restartRequested = false;

int lastTickMs = 0;
int logicAccumulatorMs = 0;

std::vector<char> keys = {'I','O','T','S','Z','J','L'};

bool currentPieceValid() {
    return pieces.find(current.shape) != pieces.end() && !pieces[current.shape].empty();
}

char randomShape() {
    std::uniform_int_distribution<int> dist(0, static_cast<int>(keys.size()) - 1);
    return keys[dist(rng)];
}

std::vector<Cell> pieceCells(const ActivePiece& p) {
    const auto& rots = pieces[p.shape];
    const auto& cells = rots[p.rotation % static_cast<int>(rots.size())];
    std::vector<Cell> out;
    out.reserve(cells.size());
    for (const auto& c : cells) out.push_back({p.x + c.first, p.y + c.second});
    return out;
}

bool collides(const ActivePiece& p) {
    for (const auto& [c, r] : pieceCells(p)) {
        if (c < 0 || c >= BOARD_W || r < 0) return true;
        if (r >= BOARD_H) continue;
        if (board[r][c] != 0) return true;
    }
    return false;
}

void spawnPiece() {
    current = {nextShape, 0, 3, 16};
    nextShape = randomShape();
    if (collides(current)) gameOver = true;
}

void clearLines() {
    std::vector<std::vector<char>> kept;
    for (const auto& row : board) {
        bool full = std::all_of(row.begin(), row.end(), [](char c){ return c != 0; });
        if (!full) kept.push_back(row);
    }

    const int cleared = BOARD_H - static_cast<int>(kept.size());
    if (cleared <= 0) return;

    while (static_cast<int>(kept.size()) < BOARD_H) {
        kept.push_back(std::vector<char>(BOARD_W, 0));
    }
    std::reverse(kept.begin(), kept.end());
    board = kept;

    linesTotal += cleared;
    level = 1 + linesTotal / 10;
    const int bonus = (cleared == 1 ? 100 : cleared == 2 ? 300 : cleared == 3 ? 500 : 800);
    score += bonus * level;
}

void lockPiece() {
    for (const auto& [c, r] : pieceCells(current)) {
        if (r >= 0 && r < BOARD_H) board[r][c] = current.shape;
    }
    clearLines();
    spawnPiece();
}

void restartGame() {
    for (auto& row : board) std::fill(row.begin(), row.end(), 0);
    score = 0;
    linesTotal = 0;
    level = 1;
    paused = false;
    gameOver = false;
    hardDropArmed = true;
    restartRequested = false;
    lastTickMs = glutGet(GLUT_ELAPSED_TIME);
    logicAccumulatorMs = 0;
    nextShape = randomShape();
    spawnPiece();
}

bool movePiece(int dx, int dy) {
    if (!currentPieceValid()) return false;
    ActivePiece t = current;
    t.x += dx;
    t.y += dy;
    if (collides(t)) return false;
    current = t;
    return true;
}

void rotatePiece() {
    if (!currentPieceValid()) return;
    const auto& rots = pieces[current.shape];
    ActivePiece t = current;
    t.rotation = (t.rotation + 1) % static_cast<int>(rots.size());

    for (int kick : {0, -1, 1, -2, 2}) {
        ActivePiece c = t;
        c.x += kick;
        if (!collides(c)) { current = c; return; }
    }
}

void hardDrop() {
    if (!currentPieceValid()) return;
    while (movePiece(0, -1)) {}
    lockPiece();
}

void updateGame() {
    if (restartRequested) {
        restartGame();
        return;
    }
    if (gameOver || paused || !currentPieceValid()) return;
    if (!movePiece(0, -1)) lockPiece();
}

void drawCell(int c, int r, const std::array<float,3>& color) {
    const float x = 1.0f + static_cast<float>(c);
    const float y = 1.0f + static_cast<float>(r);
    glColor3f(color[0], color[1], color[2]);
    glBegin(GL_QUADS);
    glVertex2f(x + 0.06f, y + 0.06f);
    glVertex2f(x + 0.94f, y + 0.06f);
    glVertex2f(x + 0.94f, y + 0.94f);
    glVertex2f(x + 0.06f, y + 0.94f);
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glColor3f(0.35f, 0.4f, 0.5f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(1.0f, 1.0f);
    glVertex2f(1.0f + BOARD_W, 1.0f);
    glVertex2f(1.0f + BOARD_W, 1.0f + BOARD_H);
    glVertex2f(1.0f, 1.0f + BOARD_H);
    glEnd();

    for (int r = 0; r < BOARD_H; ++r) {
        for (int c = 0; c < BOARD_W; ++c) {
            char cell = board[r][c];
            if (cell) drawCell(c, r, colors[cell]);
        }
    }

    if (currentPieceValid()) {
        for (const auto& [c, r] : pieceCells(current)) {
            if (r >= 0 && r < BOARD_H) drawCell(c, r, colors[current.shape]);
        }
    }

    const float ux = 13.0f;
    glColor3f(0.85f, 0.92f, 1.0f);
    aula::drawBitmapText(ux, 20.0f, "AULA 14 - TETRIS");
    aula::drawBitmapText(ux, 18.4f, "Score: " + std::to_string(score));
    aula::drawBitmapText(ux, 17.0f, "Linhas: " + std::to_string(linesTotal));
    aula::drawBitmapText(ux, 15.6f, "Nivel: " + std::to_string(level));
    aula::drawBitmapText(ux, 13.4f, std::string("Proxima: ") + nextShape);

    if (paused && !gameOver) {
        glColor3f(1.0f, 1.0f, 0.6f);
        aula::drawBitmapText(4.1f, 10.4f, "PAUSADO");
    }
    if (gameOver) {
        glColor3f(1.0f, 0.6f, 0.6f);
        aula::drawBitmapText(3.0f, 10.4f, "GAME OVER");
    }

    glutSwapBuffers();
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 22.0, 0.0, 22.0, -1.0, 1.0);
}

void keyboard(unsigned char key, int, int) {
    if (key == 27) std::exit(0);
    if (key == 'r' || key == 'R') { restartRequested = true; return; }
    if (gameOver) return;
    if (key == 'p' || key == 'P') { paused = !paused; return; }
    if (paused) return;

    if (key == 'a' || key == 'A') movePiece(-1, 0);
    else if (key == 'd' || key == 'D') movePiece(1, 0);
    else if (key == 's' || key == 'S') { if (!movePiece(0, -1)) lockPiece(); }
    else if (key == 'w' || key == 'W') rotatePiece();
    else if (key == ' ' && hardDropArmed) {
        hardDropArmed = false;
        hardDrop();
    }
}

void keyboardUp(unsigned char key, int, int) {
    if (key == ' ') hardDropArmed = true;
}

void special(int key, int, int) {
    if (gameOver || paused) return;
    if (key == GLUT_KEY_LEFT) movePiece(-1, 0);
    else if (key == GLUT_KEY_RIGHT) movePiece(1, 0);
    else if (key == GLUT_KEY_DOWN) { if (!movePiece(0, -1)) lockPiece(); }
    else if (key == GLUT_KEY_UP) rotatePiece();
}

void tick(int) {
    const int nowMs = glutGet(GLUT_ELAPSED_TIME);
    const int elapsedMs = std::max(0, nowMs - lastTickMs);
    lastTickMs = nowMs;

    logicAccumulatorMs += elapsedMs;

    const int interval = std::max(MIN_TICK_MS, BASE_TICK_MS - (level - 1) * 35);

    int steps = 0;
    while (logicAccumulatorMs >= interval && steps < MAX_LOGIC_STEPS_PER_TICK) {
        updateGame();
        logicAccumulatorMs -= interval;
        steps++;
    }

    if (steps == MAX_LOGIC_STEPS_PER_TICK && logicAccumulatorMs > interval * 2) {
        logicAccumulatorMs = interval * 2;
    }

    glutPostRedisplay();
    glutTimerFunc(interval, tick, 0);
}

} // namespace

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    aula::configureCloseBehavior();
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_W, WINDOW_H);
    glutCreateWindow("Aula 14 - Jogo Tetris (C++)");

    glClearColor(0.03f, 0.03f, 0.06f, 1.0f);
    restartGame();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(special);
    glutTimerFunc(BASE_TICK_MS, tick, 0);
    glutMainLoop();
    return 0;
}
