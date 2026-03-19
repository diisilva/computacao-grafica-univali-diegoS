"""Aula 14 — Jogo Tetris 2D com hardening (robusto e previsível).

Conceitos aplicados:
- Simulação em grade (board 10x20) com peças tetromino.
- Colisão e travamento de peça no tabuleiro.
- Rotação, movimentação lateral, soft drop e hard drop.
- Limpeza de linhas, score, nível e velocidade dinâmica.
- Estados de jogo: pausa, reinício e game over.

Controles:
- A/D ou setas ←/→: mover peça
- W ou seta ↑: rotacionar
- S ou seta ↓: soft drop
- ESPAÇO: hard drop
- P: pausar/retomar
- R: reiniciar
- ESC: sair

Como executar:
    python3 14_jogo_tetris.py

Hardening aplicado:
- Validação de estado da peça ativa antes de usar em update/render.
- Debounce de hard drop para evitar repetição por key-repeat.
- Reinício seguro (restart_requested) aplicado no início do frame lógico.
- Limite de passos lógicos por tick para evitar "espiral" em lag.
- Tick baseado em tempo decorrido (GLUT elapsed time) com acumulador.
"""

from __future__ import annotations

from dataclasses import dataclass
from random import Random

from OpenGL.GL import (
    GL_COLOR_BUFFER_BIT,
    GL_LINE_LOOP,
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
    glutKeyboardUpFunc,
    glutMainLoop,
    glutPostRedisplay,
    glutReshapeFunc,
    glutSetOption,
    glutSpecialFunc,
    glutSwapBuffers,
    glutGet,
    GLUT_ELAPSED_TIME,
    glutTimerFunc,
)

BOARD_WIDTH = 10
BOARD_HEIGHT = 20
WINDOW_WIDTH = 1000
WINDOW_HEIGHT = 900
BASE_TICK_MS = 520
MIN_TICK_MS = 110
MAX_LOGIC_STEPS_PER_TICK = 5

PIECES: dict[str, list[list[tuple[int, int]]]] = {
    "I": [
        [(0, 1), (1, 1), (2, 1), (3, 1)],
        [(2, 0), (2, 1), (2, 2), (2, 3)],
    ],
    "O": [
        [(1, 1), (2, 1), (1, 2), (2, 2)],
    ],
    "T": [
        [(1, 1), (0, 2), (1, 2), (2, 2)],
        [(1, 1), (1, 2), (2, 2), (1, 3)],
        [(0, 2), (1, 2), (2, 2), (1, 3)],
        [(1, 1), (0, 2), (1, 2), (1, 3)],
    ],
    "S": [
        [(1, 1), (2, 1), (0, 2), (1, 2)],
        [(1, 1), (1, 2), (2, 2), (2, 3)],
    ],
    "Z": [
        [(0, 1), (1, 1), (1, 2), (2, 2)],
        [(2, 1), (1, 2), (2, 2), (1, 3)],
    ],
    "J": [
        [(0, 1), (0, 2), (1, 2), (2, 2)],
        [(1, 1), (2, 1), (1, 2), (1, 3)],
        [(0, 2), (1, 2), (2, 2), (2, 3)],
        [(1, 1), (1, 2), (0, 3), (1, 3)],
    ],
    "L": [
        [(2, 1), (0, 2), (1, 2), (2, 2)],
        [(1, 1), (1, 2), (1, 3), (2, 3)],
        [(0, 2), (1, 2), (2, 2), (0, 3)],
        [(0, 1), (1, 1), (1, 2), (1, 3)],
    ],
}

COLORS = {
    "I": (0.3, 0.9, 1.0),
    "O": (1.0, 0.9, 0.3),
    "T": (0.8, 0.4, 1.0),
    "S": (0.4, 1.0, 0.45),
    "Z": (1.0, 0.4, 0.4),
    "J": (0.35, 0.55, 1.0),
    "L": (1.0, 0.65, 0.35),
}


@dataclass
class ActivePiece:
    shape: str
    rotation: int
    x: int
    y: int


board: list[list[str | None]] = [[None for _ in range(BOARD_WIDTH)] for _ in range(BOARD_HEIGHT)]
current_piece: ActivePiece | None = None
next_shape = "I"
rng = Random()

score = 0
lines_cleared_total = 0
level = 1
paused = False
game_over = False
hard_drop_armed = True
restart_requested = False

last_tick_ms = 0
logic_accumulator_ms = 0


def configure_close_behavior() -> None:
    try:
        glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS)
    except Exception:
        pass


def draw_text(x: float, y: float, text: str) -> None:
    glRasterPos2f(x, y)
    for character in text:
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, ord(character))


def piece_cells(piece: ActivePiece) -> list[tuple[int, int]]:
    rotations = PIECES[piece.shape]
    shape_cells = rotations[piece.rotation % len(rotations)]
    return [(piece.x + col, piece.y + row) for col, row in shape_cells]


def current_piece_is_valid() -> bool:
    if current_piece is None:
        return False
    if current_piece.shape not in PIECES:
        return False
    rotations = PIECES[current_piece.shape]
    return len(rotations) > 0


def collides(piece: ActivePiece) -> bool:
    for col, row in piece_cells(piece):
        if col < 0 or col >= BOARD_WIDTH:
            return True
        if row < 0:
            return True
        if row >= BOARD_HEIGHT:
            continue
        if board[row][col] is not None:
            return True
    return False


def spawn_piece() -> None:
    global current_piece, next_shape, game_over

    shape = next_shape
    next_shape = rng.choice(list(PIECES.keys()))
    current_piece = ActivePiece(shape=shape, rotation=0, x=3, y=16)

    if collides(current_piece):
        game_over = True


def lock_piece() -> None:
    global current_piece

    assert current_piece is not None
    for col, row in piece_cells(current_piece):
        if 0 <= row < BOARD_HEIGHT:
            board[row][col] = current_piece.shape
    clear_lines()
    spawn_piece()


def clear_lines() -> None:
    global score, lines_cleared_total, level

    new_rows = [row for row in board if any(cell is None for cell in row)]
    cleared = BOARD_HEIGHT - len(new_rows)
    if cleared <= 0:
        return

    for _ in range(cleared):
        new_rows.append([None for _ in range(BOARD_WIDTH)])

    new_rows.reverse()
    for row_idx in range(BOARD_HEIGHT):
        board[row_idx] = new_rows[row_idx]

    lines_cleared_total += cleared
    level = 1 + lines_cleared_total // 10

    bonus = {1: 100, 2: 300, 3: 500, 4: 800}.get(cleared, 1200)
    score += bonus * level


def restart_game() -> None:
    global score, lines_cleared_total, level, paused, game_over, next_shape
    global hard_drop_armed, restart_requested, last_tick_ms, logic_accumulator_ms

    for row in range(BOARD_HEIGHT):
        for col in range(BOARD_WIDTH):
            board[row][col] = None

    score = 0
    lines_cleared_total = 0
    level = 1
    paused = False
    game_over = False
    hard_drop_armed = True
    restart_requested = False
    last_tick_ms = glutGet(GLUT_ELAPSED_TIME)
    logic_accumulator_ms = 0
    next_shape = rng.choice(list(PIECES.keys()))
    spawn_piece()


def move_piece(dx: int, dy: int) -> bool:
    if not current_piece_is_valid():
        return False

    test_piece = ActivePiece(
        shape=current_piece.shape,
        rotation=current_piece.rotation,
        x=current_piece.x + dx,
        y=current_piece.y + dy,
    )

    if collides(test_piece):
        return False

    current_piece.x = test_piece.x
    current_piece.y = test_piece.y
    return True


def rotate_piece() -> None:
    if not current_piece_is_valid():
        return

    rotations = PIECES[current_piece.shape]
    test_piece = ActivePiece(
        shape=current_piece.shape,
        rotation=(current_piece.rotation + 1) % len(rotations),
        x=current_piece.x,
        y=current_piece.y,
    )

    for kick_x in (0, -1, 1, -2, 2):
        candidate = ActivePiece(
            shape=test_piece.shape,
            rotation=test_piece.rotation,
            x=test_piece.x + kick_x,
            y=test_piece.y,
        )
        if not collides(candidate):
            current_piece.rotation = candidate.rotation
            current_piece.x = candidate.x
            return


def hard_drop() -> None:
    if not current_piece_is_valid():
        return

    while move_piece(0, -1):
        pass
    lock_piece()


def update_game() -> None:
    global restart_requested

    if restart_requested:
        restart_game()
        return

    if game_over or paused or not current_piece_is_valid():
        return

    moved = move_piece(0, -1)
    if not moved:
        lock_piece()


def draw_cell(col: int, row: int, color: tuple[float, float, float]) -> None:
    x = 1.0 + col
    y = 1.0 + row

    glColor3f(*color)
    glBegin(GL_QUADS)
    glVertex2f(x + 0.06, y + 0.06)
    glVertex2f(x + 0.94, y + 0.06)
    glVertex2f(x + 0.94, y + 0.94)
    glVertex2f(x + 0.06, y + 0.94)
    glEnd()


def draw_board_frame() -> None:
    glColor3f(0.35, 0.4, 0.5)
    glBegin(GL_LINE_LOOP)
    glVertex2f(1.0, 1.0)
    glVertex2f(1.0 + BOARD_WIDTH, 1.0)
    glVertex2f(1.0 + BOARD_WIDTH, 1.0 + BOARD_HEIGHT)
    glVertex2f(1.0, 1.0 + BOARD_HEIGHT)
    glEnd()


def display() -> None:
    glClear(GL_COLOR_BUFFER_BIT)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()

    draw_board_frame()

    for row in range(BOARD_HEIGHT):
        for col in range(BOARD_WIDTH):
            cell = board[row][col]
            if cell is not None:
                draw_cell(col, row, COLORS[cell])

    if current_piece_is_valid():
        for col, row in piece_cells(current_piece):
            if 0 <= row < BOARD_HEIGHT:
                draw_cell(col, row, COLORS[current_piece.shape])

    ui_x = 13.0
    glColor3f(0.85, 0.92, 1.0)
    draw_text(ui_x, 20.0, "AULA 14 - TETRIS")
    draw_text(ui_x, 18.4, f"Score: {score}")
    draw_text(ui_x, 17.0, f"Linhas: {lines_cleared_total}")
    draw_text(ui_x, 15.6, f"Nivel: {level}")
    draw_text(ui_x, 13.4, f"Proxima: {next_shape}")

    draw_text(ui_x, 10.8, "A/D ou <-/->")
    draw_text(ui_x, 9.8, "W ou UP: rotacao")
    draw_text(ui_x, 8.8, "S ou DOWN: descer")
    draw_text(ui_x, 7.8, "ESPACO: hard drop")
    draw_text(ui_x, 6.8, "P: pausa | R: reset")

    if paused and not game_over:
        glColor3f(1.0, 1.0, 0.6)
        draw_text(4.1, 10.4, "PAUSADO")

    if game_over:
        glColor3f(1.0, 0.6, 0.6)
        draw_text(3.0, 10.4, "GAME OVER")
        draw_text(2.5, 9.2, "Pressione R para reiniciar")

    glutSwapBuffers()


def reshape(width: int, height: int) -> None:
    if height == 0:
        height = 1

    glViewport(0, 0, width, height)
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    glOrtho(0.0, 22.0, 0.0, 22.0, -1.0, 1.0)


def keyboard(key: bytes, _: int, __: int) -> None:
    global paused, hard_drop_armed, restart_requested

    if key == b"\x1b":
        raise SystemExit("ESC")

    if key in (b"r", b"R"):
        restart_requested = True
        return

    if game_over:
        return

    if key in (b"p", b"P"):
        paused = not paused
        return

    if paused:
        return

    if key in (b"a", b"A"):
        move_piece(-1, 0)
    elif key in (b"d", b"D"):
        move_piece(1, 0)
    elif key in (b"s", b"S"):
        if not move_piece(0, -1):
            lock_piece()
    elif key in (b"w", b"W"):
        rotate_piece()
    elif key == b" ":
        if hard_drop_armed:
            hard_drop_armed = False
            hard_drop()


def keyboard_up(key: bytes, _: int, __: int) -> None:
    global hard_drop_armed
    if key == b" ":
        hard_drop_armed = True


def special(key: int, _: int, __: int) -> None:
    if game_over or paused:
        return

    if key == GLUT_KEY_LEFT:
        move_piece(-1, 0)
    elif key == GLUT_KEY_RIGHT:
        move_piece(1, 0)
    elif key == GLUT_KEY_DOWN:
        if not move_piece(0, -1):
            lock_piece()
    elif key == GLUT_KEY_UP:
        rotate_piece()


def tick(_: int) -> None:
    global last_tick_ms, logic_accumulator_ms

    now_ms = glutGet(GLUT_ELAPSED_TIME)
    elapsed_ms = max(0, now_ms - last_tick_ms)
    last_tick_ms = now_ms

    logic_accumulator_ms += elapsed_ms
    interval = max(MIN_TICK_MS, BASE_TICK_MS - (level - 1) * 35)

    steps = 0
    while logic_accumulator_ms >= interval and steps < MAX_LOGIC_STEPS_PER_TICK:
        update_game()
        logic_accumulator_ms -= interval
        steps += 1

    if steps == MAX_LOGIC_STEPS_PER_TICK and logic_accumulator_ms > interval * 2:
        logic_accumulator_ms = interval * 2

    glutPostRedisplay()
    glutTimerFunc(interval, tick, 0)


def main() -> None:
    glutInit()
    configure_close_behavior()
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB)
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT)
    glutCreateWindow(b"Aula 14 - Jogo Tetris")
    glClearColor(0.03, 0.03, 0.06, 1.0)

    restart_game()

    glutDisplayFunc(display)
    glutReshapeFunc(reshape)
    glutKeyboardFunc(keyboard)
    glutKeyboardUpFunc(keyboard_up)
    glutSpecialFunc(special)
    glutTimerFunc(BASE_TICK_MS, tick, 0)

    print("Tetris iniciado: A/D, W, S, setas, ESPACO, P, R, ESC")
    glutMainLoop()


if __name__ == "__main__":
    main()
