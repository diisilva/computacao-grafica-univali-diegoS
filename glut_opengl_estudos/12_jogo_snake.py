"""Aula 12 — Jogo Snake (cobrinha) totalmente jogável.

Conceitos aplicados:
- Simulação em grade com passo temporal discreto.
- Fila de segmentos para corpo da cobra.
- Geração de comida em posições válidas.
- Controle de direção com bloqueio de inversão instantânea.
- Estados de jogo: ativo, pausa, game over e reinício.

Controles:
- Direção: setas ou WASD
- + / -: aumentar/reduzir velocidade manual
- ESPAÇO: pausar/retomar
- R: reiniciar
- ESC: sair

Como executar:
    python3 12_jogo_snake.py
"""

from __future__ import annotations

from dataclasses import dataclass
from random import Random

from OpenGL.GL import (
    GL_COLOR_BUFFER_BIT,
    GL_MODELVIEW,
    GL_PROJECTION,
    GL_QUADS,
    glBegin,
    glClear,
    glClearColor,
    glColor3f,
    glEnd,
    glLoadIdentity,
    glMatrixMode,
    glOrtho,
    glRasterPos2f,
    glVertex2f,
    glViewport,
)
from OpenGL.GLUT import (
    GLUT_ACTION_GLUTMAINLOOP_RETURNS,
    GLUT_ACTION_ON_WINDOW_CLOSE,
    GLUT_BITMAP_HELVETICA_18,
    GLUT_DOUBLE,
    GLUT_KEY_DOWN,
    GLUT_KEY_LEFT,
    GLUT_KEY_RIGHT,
    GLUT_KEY_UP,
    GLUT_RGB,
    glutBitmapCharacter,
    glutCreateWindow,
    glutDisplayFunc,
    glutInit,
    glutInitDisplayMode,
    glutInitWindowSize,
    glutKeyboardFunc,
    glutMainLoop,
    glutPostRedisplay,
    glutReshapeFunc,
    glutSetOption,
    glutSpecialFunc,
    glutSwapBuffers,
    glutTimerFunc,
)

GRID_COLS = 28
GRID_ROWS = 28
WINDOW_WIDTH = 900
WINDOW_HEIGHT = 900
BASE_TICK_MS = 105
MIN_SPEED_FACTOR = 0.6
MAX_SPEED_FACTOR = 2.4


@dataclass(frozen=True)
class GridPoint:
    col: int
    row: int


class SnakeGame:
    def __init__(self) -> None:
        self.random = Random()
        self.high_score = 0
        self.reset()

    def reset(self) -> None:
        center = GridPoint(GRID_COLS // 2, GRID_ROWS // 2)
        self.snake: list[GridPoint] = [
            center,
            GridPoint(center.col - 1, center.row),
            GridPoint(center.col - 2, center.row),
        ]
        self.direction = GridPoint(1, 0)
        self.next_direction = GridPoint(1, 0)
        self.food = self._spawn_food()
        self.score = 0
        self.game_over = False
        self.paused = False

    def _spawn_food(self) -> GridPoint:
        occupied = set(self.snake)
        while True:
            candidate = GridPoint(self.random.randrange(GRID_COLS), self.random.randrange(GRID_ROWS))
            if candidate not in occupied:
                return candidate

    def _is_opposite(self, new_direction: GridPoint) -> bool:
        return (new_direction.col == -self.direction.col) and (new_direction.row == -self.direction.row)

    def try_set_direction(self, new_direction: GridPoint) -> None:
        if self._is_opposite(new_direction):
            return
        self.next_direction = new_direction

    def update(self) -> None:
        if self.paused or self.game_over:
            return

        self.direction = self.next_direction
        head = self.snake[0]
        new_head = GridPoint(head.col + self.direction.col, head.row + self.direction.row)

        out_of_bounds = (
            new_head.col < 0
            or new_head.col >= GRID_COLS
            or new_head.row < 0
            or new_head.row >= GRID_ROWS
        )
        if out_of_bounds or new_head in self.snake:
            self.game_over = True
            self.high_score = max(self.high_score, self.score)
            return

        self.snake.insert(0, new_head)
        if new_head == self.food:
            self.score += 1
            self.high_score = max(self.high_score, self.score)
            self.food = self._spawn_food()
        else:
            self.snake.pop()

    def draw_cell(self, point: GridPoint) -> None:
        left = float(point.col)
        right = float(point.col + 1)
        bottom = float(point.row)
        top = float(point.row + 1)

        margin = 0.08
        glBegin(GL_QUADS)
        glVertex2f(left + margin, bottom + margin)
        glVertex2f(right - margin, bottom + margin)
        glVertex2f(right - margin, top - margin)
        glVertex2f(left + margin, top - margin)
        glEnd()

    def draw_text(self, x: float, y: float, text: str) -> None:
        glRasterPos2f(x, y)
        for character in text:
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, ord(character))

    def render(self) -> None:
        glClear(GL_COLOR_BUFFER_BIT)
        glMatrixMode(GL_MODELVIEW)
        glLoadIdentity()

        glColor3f(0.18, 0.95, 0.45)
        for index, segment in enumerate(self.snake):
            if index == 0:
                glColor3f(0.35, 1.0, 0.55)
            else:
                glColor3f(0.18, 0.9, 0.45)
            self.draw_cell(segment)

        glColor3f(1.0, 0.3, 0.3)
        self.draw_cell(self.food)

        glColor3f(0.8, 0.9, 1.0)
        self.draw_text(0.3, GRID_ROWS - 0.8, f"Score: {self.score}")
        self.draw_text(6.0, GRID_ROWS - 0.8, f"High: {self.high_score}")
        self.draw_text(12.5, GRID_ROWS - 0.8, f"Vel: {manual_speed_factor:.1f}x | Setas/WASD | +/- | ESPACO | R | ESC")

        if self.paused and not self.game_over:
            glColor3f(1.0, 1.0, 0.5)
            self.draw_text(11.5, 14.2, "PAUSADO")

        if self.game_over:
            glColor3f(1.0, 0.75, 0.5)
            self.draw_text(9.2, 14.2, "GAME OVER")
            self.draw_text(6.9, 12.7, "Pressione R para reiniciar")

        glutSwapBuffers()


game = SnakeGame()
manual_speed_factor = 1.0


def configure_close_behavior() -> None:
    try:
        glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS)
    except Exception:
        pass


def reshape(width: int, height: int) -> None:
    if height == 0:
        height = 1
    glViewport(0, 0, width, height)
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    glOrtho(0.0, GRID_COLS, 0.0, GRID_ROWS, -1.0, 1.0)


def display() -> None:
    game.render()


def keyboard(key: bytes, _: int, __: int) -> None:
    global manual_speed_factor

    if key in (b"w", b"W"):
        game.try_set_direction(GridPoint(0, 1))
    elif key in (b"s", b"S"):
        game.try_set_direction(GridPoint(0, -1))
    elif key in (b"a", b"A"):
        game.try_set_direction(GridPoint(-1, 0))
    elif key in (b"d", b"D"):
        game.try_set_direction(GridPoint(1, 0))
    elif key == b" ":
        if not game.game_over:
            game.paused = not game.paused
    elif key == b"+":
        manual_speed_factor = min(MAX_SPEED_FACTOR, manual_speed_factor + 0.1)
    elif key == b"-":
        manual_speed_factor = max(MIN_SPEED_FACTOR, manual_speed_factor - 0.1)
    elif key in (b"r", b"R"):
        game.reset()
        manual_speed_factor = 1.0
    elif key == b"\x1b":
        raise SystemExit("ESC")


def special(key: int, _: int, __: int) -> None:
    if key == GLUT_KEY_UP:
        game.try_set_direction(GridPoint(0, 1))
    elif key == GLUT_KEY_DOWN:
        game.try_set_direction(GridPoint(0, -1))
    elif key == GLUT_KEY_LEFT:
        game.try_set_direction(GridPoint(-1, 0))
    elif key == GLUT_KEY_RIGHT:
        game.try_set_direction(GridPoint(1, 0))


def tick(_: int) -> None:
    game.update()
    glutPostRedisplay()

    speed_bonus = min(game.score, 12)
    base_interval = max(55, BASE_TICK_MS - speed_bonus * 3)
    next_interval = max(35, int(base_interval / manual_speed_factor))
    glutTimerFunc(next_interval, tick, 0)


def main() -> None:
    glutInit()
    configure_close_behavior()
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB)
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT)
    glutCreateWindow(b"Aula 12 - Jogo Snake")
    glClearColor(0.03, 0.03, 0.05, 1.0)

    glutDisplayFunc(display)
    glutReshapeFunc(reshape)
    glutKeyboardFunc(keyboard)
    glutSpecialFunc(special)
    glutTimerFunc(BASE_TICK_MS, tick, 0)

    print("Snake iniciado: setas/WASD, ESPACO, R, ESC")
    glutMainLoop()


if __name__ == "__main__":
    main()
