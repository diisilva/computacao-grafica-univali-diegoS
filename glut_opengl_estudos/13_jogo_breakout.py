"""Aula 13 — Jogo Breakout (Arkanoid) totalmente jogável.

Conceitos aplicados:
- Loop de jogo com timer fixo.
- Colisão bola-parede, bola-raquete e bola-bloco.
- Sistema de vidas, pontuação, pausa, reinício e condição de vitória.
- Movimento contínuo da raquete por estados de teclado.

Controles:
- Mover raquete: A/D ou setas ←/→
- ESPAÇO: pausar/retomar
- R: reiniciar partida
- ESC: sair

Como executar:
    python3 13_jogo_breakout.py
"""

from __future__ import annotations

from dataclasses import dataclass

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
    GLUT_KEY_LEFT,
    GLUT_KEY_RIGHT,
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
    glutSpecialUpFunc,
    glutSwapBuffers,
    glutTimerFunc,
)

WORLD_WIDTH = 100.0
WORLD_HEIGHT = 100.0
WINDOW_WIDTH = 1100
WINDOW_HEIGHT = 780
MAX_LIVES = 3


@dataclass
class Paddle:
    x: float
    y: float
    width: float
    height: float
    speed: float


@dataclass
class Ball:
    x: float
    y: float
    radius: float
    velocity_x: float
    velocity_y: float


@dataclass
class Brick:
    x: float
    y: float
    width: float
    height: float
    hp: int


paddle = Paddle(x=50.0, y=8.0, width=18.0, height=2.8, speed=1.9)
ball = Ball(x=50.0, y=14.0, radius=1.2, velocity_x=0.66, velocity_y=0.82)
bricks: list[Brick] = []

score = 0
lives = MAX_LIVES
paused = False
game_over = False
victory = False

move_left_pressed = False
move_right_pressed = False


def configure_close_behavior() -> None:
    try:
        glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS)
    except Exception:
        pass


def draw_text(x: float, y: float, text: str) -> None:
    glRasterPos2f(x, y)
    for character in text:
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, ord(character))


def draw_rect(x: float, y: float, width: float, height: float) -> None:
    half_width = width / 2
    half_height = height / 2
    glBegin(GL_QUADS)
    glVertex2f(x - half_width, y - half_height)
    glVertex2f(x + half_width, y - half_height)
    glVertex2f(x + half_width, y + half_height)
    glVertex2f(x - half_width, y + half_height)
    glEnd()


def create_bricks() -> list[Brick]:
    created: list[Brick] = []
    rows = 6
    cols = 10
    brick_width = 8.2
    brick_height = 3.2
    start_x = 9.0
    start_y = 80.0
    gap_x = 0.8
    gap_y = 0.8

    for row in range(rows):
        for col in range(cols):
            x = start_x + col * (brick_width + gap_x)
            y = start_y - row * (brick_height + gap_y)
            hp = 2 if row < 2 else 1
            created.append(Brick(x=x, y=y, width=brick_width, height=brick_height, hp=hp))

    return created


def reset_ball() -> None:
    from random import choice

    ball.x = paddle.x
    ball.y = 14.0
    ball.velocity_x = choice([-0.66, 0.66])
    ball.velocity_y = 0.82


def restart_game() -> None:
    global bricks, score, lives, paused, game_over, victory

    bricks = create_bricks()
    score = 0
    lives = MAX_LIVES
    paused = False
    game_over = False
    victory = False

    paddle.x = 50.0
    reset_ball()


def clamp_paddle() -> None:
    half_width = paddle.width / 2
    if paddle.x < half_width:
        paddle.x = half_width
    if paddle.x > WORLD_WIDTH - half_width:
        paddle.x = WORLD_WIDTH - half_width


def intersects_rect(cx: float, cy: float, radius: float, rx: float, ry: float, rw: float, rh: float) -> bool:
    rect_left = rx - rw / 2
    rect_right = rx + rw / 2
    rect_bottom = ry - rh / 2
    rect_top = ry + rh / 2

    closest_x = max(rect_left, min(cx, rect_right))
    closest_y = max(rect_bottom, min(cy, rect_top))

    dx = cx - closest_x
    dy = cy - closest_y
    return dx * dx + dy * dy <= radius * radius


def update_game() -> None:
    global score, lives, game_over, victory

    if game_over or victory:
        return

    if move_left_pressed:
        paddle.x -= paddle.speed
    if move_right_pressed:
        paddle.x += paddle.speed
    clamp_paddle()

    ball.x += ball.velocity_x
    ball.y += ball.velocity_y

    if ball.x - ball.radius <= 0.0 or ball.x + ball.radius >= WORLD_WIDTH:
        ball.velocity_x *= -1

    if ball.y + ball.radius >= WORLD_HEIGHT:
        ball.velocity_y *= -1

    if ball.y - ball.radius <= 0.0:
        lives -= 1
        if lives <= 0:
            game_over = True
        else:
            reset_ball()
        return

    if intersects_rect(ball.x, ball.y, ball.radius, paddle.x, paddle.y, paddle.width, paddle.height):
        hit_offset = (ball.x - paddle.x) / (paddle.width / 2)
        ball.velocity_y = abs(ball.velocity_y)
        ball.velocity_x += 0.11 * hit_offset
        ball.velocity_x = max(-1.4, min(1.4, ball.velocity_x))

    for brick in list(bricks):
        if not intersects_rect(ball.x, ball.y, ball.radius, brick.x, brick.y, brick.width, brick.height):
            continue

        overlap_left = abs((ball.x + ball.radius) - (brick.x - brick.width / 2))
        overlap_right = abs((brick.x + brick.width / 2) - (ball.x - ball.radius))
        overlap_bottom = abs((ball.y + ball.radius) - (brick.y - brick.height / 2))
        overlap_top = abs((brick.y + brick.height / 2) - (ball.y - ball.radius))
        min_overlap = min(overlap_left, overlap_right, overlap_bottom, overlap_top)

        if min_overlap in (overlap_left, overlap_right):
            ball.velocity_x *= -1
        else:
            ball.velocity_y *= -1

        brick.hp -= 1
        if brick.hp <= 0:
            bricks.remove(brick)
            score += 10
        else:
            score += 4
        break

    if not bricks:
        victory = True


def display() -> None:
    glClear(GL_COLOR_BUFFER_BIT)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()

    glColor3f(0.2, 0.7, 1.0)
    draw_rect(paddle.x, paddle.y, paddle.width, paddle.height)

    glColor3f(1.0, 0.85, 0.3)
    draw_rect(ball.x, ball.y, ball.radius * 2, ball.radius * 2)

    for brick in bricks:
        if brick.hp == 2:
            glColor3f(1.0, 0.45, 0.45)
        else:
            glColor3f(0.9, 0.8, 0.35)
        draw_rect(brick.x, brick.y, brick.width, brick.height)

    glColor3f(0.85, 0.92, 1.0)
    draw_text(2.0, 96.0, f"Score: {score}")
    draw_text(22.0, 96.0, f"Vidas: {lives}")
    draw_text(46.0, 96.0, "A/D ou Setas | ESPACO pausa | R reinicia | ESC sai")

    if paused and not (game_over or victory):
        glColor3f(1.0, 1.0, 0.6)
        draw_text(44.0, 49.0, "PAUSADO")

    if game_over:
        glColor3f(1.0, 0.55, 0.55)
        draw_text(41.0, 50.0, "GAME OVER")
        draw_text(36.0, 45.0, "Pressione R para jogar novamente")

    if victory:
        glColor3f(0.7, 1.0, 0.7)
        draw_text(42.0, 50.0, "VOCE VENCEU!")
        draw_text(36.0, 45.0, "Pressione R para jogar novamente")

    glutSwapBuffers()


def reshape(width: int, height: int) -> None:
    if height == 0:
        height = 1
    glViewport(0, 0, width, height)
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    glOrtho(0.0, WORLD_WIDTH, 0.0, WORLD_HEIGHT, -1.0, 1.0)


def keyboard_down(key: bytes, _: int, __: int) -> None:
    global move_left_pressed, move_right_pressed, paused

    if key in (b"a", b"A"):
        move_left_pressed = True
    elif key in (b"d", b"D"):
        move_right_pressed = True
    elif key == b" ":
        if not (game_over or victory):
            paused = not paused
    elif key in (b"r", b"R"):
        restart_game()
    elif key == b"\x1b":
        raise SystemExit("ESC")


def keyboard_up(key: bytes, _: int, __: int) -> None:
    global move_left_pressed, move_right_pressed

    if key in (b"a", b"A"):
        move_left_pressed = False
    elif key in (b"d", b"D"):
        move_right_pressed = False


def special_down(key: int, _: int, __: int) -> None:
    global move_left_pressed, move_right_pressed

    if key == GLUT_KEY_LEFT:
        move_left_pressed = True
    elif key == GLUT_KEY_RIGHT:
        move_right_pressed = True


def special_up(key: int, _: int, __: int) -> None:
    global move_left_pressed, move_right_pressed

    if key == GLUT_KEY_LEFT:
        move_left_pressed = False
    elif key == GLUT_KEY_RIGHT:
        move_right_pressed = False


def tick(_: int) -> None:
    if not paused:
        update_game()
    glutPostRedisplay()
    glutTimerFunc(16, tick, 0)


def main() -> None:
    glutInit()
    configure_close_behavior()
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB)
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT)
    glutCreateWindow(b"Aula 13 - Jogo Breakout")
    glClearColor(0.04, 0.04, 0.07, 1.0)

    restart_game()

    glutDisplayFunc(display)
    glutReshapeFunc(reshape)
    glutKeyboardFunc(keyboard_down)
    glutKeyboardUpFunc(keyboard_up)
    glutSpecialFunc(special_down)
    glutSpecialUpFunc(special_up)
    glutTimerFunc(0, tick, 0)

    print("Breakout iniciado: A/D, setas, ESPACO, R, ESC")
    glutMainLoop()


if __name__ == "__main__":
    main()
