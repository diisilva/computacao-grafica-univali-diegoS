"""Aula 11 — Jogo Pong 2D totalmente jogável.

Conceitos aplicados:
- Loop de jogo com atualização em timer fixo.
- Entrada de teclado contínua para movimento fluido.
- Oponente controlado por IA para jogo solo.
- Colisão bola-parede e bola-raquete com resposta física simples.
- Sistema de pontuação, pausa, reinício, velocidade global e estado de vitória.

Controles:
- Jogador esquerdo: W / S ou SETAS ↑ / ↓
- + / -: aumentar ou reduzir velocidade geral
- ESPAÇO: pausar/retomar
- R: reiniciar partida
- ESC: sair

Como executar:
    python3 11_jogo_pong.py
"""

from __future__ import annotations

from dataclasses import dataclass

from OpenGL.GL import (
    GL_COLOR_BUFFER_BIT,
    GL_LINE_LOOP,
    GL_LINES,
    GL_MODELVIEW,
    GL_PROJECTION,
    GL_QUADS,
    glBegin,
    glClear,
    glClearColor,
    glColor3f,
    glEnd,
    glLineWidth,
    glLoadIdentity,
    glMatrixMode,
    glRasterPos2f,
    glVertex2f,
    glViewport,
)
from OpenGL.GLU import gluOrtho2D
from OpenGL.GLUT import (
    GLUT_ACTION_GLUTMAINLOOP_RETURNS,
    GLUT_ACTION_ON_WINDOW_CLOSE,
    GLUT_BITMAP_HELVETICA_18,
    GLUT_DOUBLE,
    GLUT_KEY_DOWN,
    GLUT_KEY_UP,
    GLUT_RGB,
    glutBitmapCharacter,
    glutCreateWindow,
    glutDisplayFunc,
    glutInit,
    glutInitDisplayMode,
    glutInitWindowPosition,
    glutInitWindowSize,
    glutKeyboardFunc,
    glutKeyboardUpFunc,
    glutMainLoop,
    glutPostRedisplay,
    glutReshapeFunc,
    glutSpecialFunc,
    glutSpecialUpFunc,
    glutSetOption,
    glutSwapBuffers,
    glutTimerFunc,
)

WINDOW_WIDTH = 1200
WINDOW_HEIGHT = 800
WORLD_LEFT = 0.0
WORLD_RIGHT = 100.0
WORLD_BOTTOM = 0.0
WORLD_TOP = 100.0
WIN_SCORE = 10
MIN_GAME_SPEED = 0.5
MAX_GAME_SPEED = 2.5


@dataclass
class Paddle:
    x: float
    y: float
    width: float = 2.0
    height: float = 18.0
    speed: float = 1.45

    def top(self) -> float:
        return self.y + self.height / 2

    def bottom(self) -> float:
        return self.y - self.height / 2


@dataclass
class Ball:
    x: float
    y: float
    radius: float
    velocity_x: float
    velocity_y: float


left_paddle = Paddle(x=6.0, y=50.0)
right_paddle = Paddle(x=94.0, y=50.0)
ball = Ball(x=50.0, y=50.0, radius=1.4, velocity_x=0.78, velocity_y=0.52)

left_score = 0
right_score = 0
paused = False
winner_text = ""
game_speed = 1.0

key_w_pressed = False
key_s_pressed = False
key_up_pressed = False
key_down_pressed = False

ai_dead_zone = 1.1
ai_speed = 1.18


def configure_close_behavior() -> None:
    try:
        glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS)
    except Exception:
        pass


def draw_text(x: float, y: float, text: str) -> None:
    glRasterPos2f(x, y)
    for character in text:
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, ord(character))


def draw_filled_rect(center_x: float, center_y: float, width: float, height: float) -> None:
    half_width = width / 2
    half_height = height / 2
    glBegin(GL_QUADS)
    glVertex2f(center_x - half_width, center_y - half_height)
    glVertex2f(center_x + half_width, center_y - half_height)
    glVertex2f(center_x + half_width, center_y + half_height)
    glVertex2f(center_x - half_width, center_y + half_height)
    glEnd()


def draw_ball(ball_obj: Ball) -> None:
    glBegin(GL_LINE_LOOP)
    segments = 28
    from math import cos, sin, tau

    for index in range(segments):
        angle = tau * index / segments
        glVertex2f(ball_obj.x + ball_obj.radius * cos(angle), ball_obj.y + ball_obj.radius * sin(angle))
    glEnd()


def clamp_paddle(paddle: Paddle) -> None:
    half_height = paddle.height / 2
    if paddle.y < WORLD_BOTTOM + half_height:
        paddle.y = WORLD_BOTTOM + half_height
    if paddle.y > WORLD_TOP - half_height:
        paddle.y = WORLD_TOP - half_height


def reset_ball(direction: int) -> None:
    from random import choice

    ball.x = 50.0
    ball.y = 50.0
    ball.velocity_x = 0.78 * direction
    ball.velocity_y = choice([-0.52, 0.52])


def update_right_paddle_ai() -> None:
    target_y = ball.y
    diff = target_y - right_paddle.y

    if abs(diff) <= ai_dead_zone:
        return

    step = ai_speed * game_speed
    if diff > 0:
        right_paddle.y += min(step, diff)
    else:
        right_paddle.y -= min(step, -diff)


def check_ball_paddle_collision(paddle: Paddle, moving_left: bool) -> None:
    if moving_left:
        hit_x = ball.x - ball.radius <= paddle.x + paddle.width / 2
        inside_x = ball.x >= paddle.x
    else:
        hit_x = ball.x + ball.radius >= paddle.x - paddle.width / 2
        inside_x = ball.x <= paddle.x

    inside_y = paddle.bottom() <= ball.y <= paddle.top()

    if hit_x and inside_x and inside_y:
        paddle_center_distance = (ball.y - paddle.y) / (paddle.height / 2)
        ball.velocity_x *= -1.04
        ball.velocity_y += 0.25 * paddle_center_distance

        max_speed = 2.1
        if ball.velocity_x > max_speed:
            ball.velocity_x = max_speed
        if ball.velocity_x < -max_speed:
            ball.velocity_x = -max_speed


def update_game() -> None:
    global left_score, right_score, winner_text

    if winner_text:
        return

    if key_w_pressed or key_up_pressed:
        left_paddle.y += left_paddle.speed * game_speed
    if key_s_pressed or key_down_pressed:
        left_paddle.y -= left_paddle.speed * game_speed

    update_right_paddle_ai()

    clamp_paddle(left_paddle)
    clamp_paddle(right_paddle)

    ball.x += ball.velocity_x * game_speed
    ball.y += ball.velocity_y * game_speed

    if ball.y + ball.radius >= WORLD_TOP or ball.y - ball.radius <= WORLD_BOTTOM:
        ball.velocity_y *= -1

    check_ball_paddle_collision(left_paddle, moving_left=True)
    check_ball_paddle_collision(right_paddle, moving_left=False)

    if ball.x < WORLD_LEFT:
        right_score += 1
        if right_score >= WIN_SCORE:
            winner_text = "Jogador da direita venceu!"
        reset_ball(direction=1)

    if ball.x > WORLD_RIGHT:
        left_score += 1
        if left_score >= WIN_SCORE:
            winner_text = "Jogador da esquerda venceu!"
        reset_ball(direction=-1)


def display() -> None:
    glClear(GL_COLOR_BUFFER_BIT)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()

    glColor3f(0.3, 0.35, 0.4)
    glLineWidth(2.0)
    glBegin(GL_LINES)
    for row in range(0, 100, 6):
        glVertex2f(50.0, row)
        glVertex2f(50.0, row + 3)
    glEnd()

    glColor3f(0.9, 0.9, 0.95)
    draw_filled_rect(left_paddle.x, left_paddle.y, left_paddle.width, left_paddle.height)
    draw_filled_rect(right_paddle.x, right_paddle.y, right_paddle.width, right_paddle.height)

    glColor3f(1.0, 0.8, 0.3)
    draw_ball(ball)

    glColor3f(0.85, 0.9, 1.0)
    draw_text(24.0, 95.0, f"{left_score}")
    draw_text(74.0, 95.0, f"{right_score}")
    draw_text(2.0, 6.0, f"Velocidade: {game_speed:.1f}x")
    draw_text(2.0, 3.0, "W/S ou SETAS | +/- velocidade | ESPACO pausa | R reinicia | ESC sai")

    if paused and not winner_text:
        glColor3f(1.0, 1.0, 0.5)
        draw_text(44.0, 52.0, "PAUSADO")

    if winner_text:
        glColor3f(0.7, 1.0, 0.7)
        draw_text(31.0, 52.0, winner_text)
        draw_text(28.0, 47.0, "Pressione R para nova partida")

    glutSwapBuffers()


def reshape(width: int, height: int) -> None:
    if height == 0:
        height = 1
    glViewport(0, 0, width, height)
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    gluOrtho2D(WORLD_LEFT, WORLD_RIGHT, WORLD_BOTTOM, WORLD_TOP)


def keyboard_down(key: bytes, _: int, __: int) -> None:
    global key_w_pressed, key_s_pressed, paused, left_score, right_score, winner_text, game_speed

    if key in (b"w", b"W"):
        key_w_pressed = True
    elif key in (b"s", b"S"):
        key_s_pressed = True
    elif key == b" ":
        paused = not paused
    elif key == b"+":
        game_speed = min(MAX_GAME_SPEED, game_speed + 0.1)
    elif key == b"-":
        game_speed = max(MIN_GAME_SPEED, game_speed - 0.1)
    elif key in (b"r", b"R"):
        left_score = 0
        right_score = 0
        winner_text = ""
        left_paddle.y = 50.0
        right_paddle.y = 50.0
        game_speed = 1.0
        reset_ball(direction=1)
    elif key == b"\x1b":
        raise SystemExit("ESC")


def keyboard_up(key: bytes, _: int, __: int) -> None:
    global key_w_pressed, key_s_pressed

    if key in (b"w", b"W"):
        key_w_pressed = False
    elif key in (b"s", b"S"):
        key_s_pressed = False


def special_down(key: int, _: int, __: int) -> None:
    global key_up_pressed, key_down_pressed

    if key == GLUT_KEY_UP:
        key_up_pressed = True
    elif key == GLUT_KEY_DOWN:
        key_down_pressed = True


def special_up(key: int, _: int, __: int) -> None:
    global key_up_pressed, key_down_pressed

    if key == GLUT_KEY_UP:
        key_up_pressed = False
    elif key == GLUT_KEY_DOWN:
        key_down_pressed = False


def tick(_: int) -> None:
    if not paused:
        update_game()
    glutPostRedisplay()
    glutTimerFunc(16, tick, 0)


def main() -> None:
    glutInit()
    configure_close_behavior()
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB)
    glutInitWindowPosition(120, 80)
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT)
    glutCreateWindow(b"Aula 11 - Jogo Pong")
    glClearColor(0.05, 0.05, 0.08, 1.0)

    glutDisplayFunc(display)
    glutReshapeFunc(reshape)
    glutKeyboardFunc(keyboard_down)
    glutKeyboardUpFunc(keyboard_up)
    glutSpecialFunc(special_down)
    glutSpecialUpFunc(special_up)
    glutTimerFunc(0, tick, 0)

    print("Pong iniciado: voce joga na esquerda (W/S ou setas). IA joga na direita.")
    glutMainLoop()


if __name__ == "__main__":
    main()
