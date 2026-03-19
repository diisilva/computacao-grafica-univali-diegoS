"""Aula 15 — Jogo Galaga 2D com hardening (robusto e previsível).

Conceitos aplicados:
- Entidades de gameplay (nave, inimigos, projéteis) com estado explícito.
- Loop em timer fixo com progressão por ondas.
- IA simples de enxame inimigo com movimento lateral e descida.
- Colisões AABB e gerenciamento de vidas/pontuação.
- Controle de velocidade da partida para estudo de tuning.

Controles:
- A/D ou setas ←/→: mover nave
- ESPAÇO: atirar
- + / -: aumentar/reduzir velocidade geral
- P: pausar/retomar
- R: reiniciar
- ESC: sair

Como executar:
    python3 15_jogo_galaga.py

Hardening aplicado:
- Validação forte de estados (`paused`, `game_over`, `wave_cleared`, restart).
- Debounce de tiro para evitar rajada por key-repeat do teclado.
- Limite de projéteis para evitar crescimento indevido de listas.
- Tick com acumulador de tempo e limite de passos por frame lógico.
- Limpeza defensiva de entidades fora da área jogável.
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
    glutGet,
    GLUT_ELAPSED_TIME,
    glutTimerFunc,
)

WORLD_WIDTH = 100.0
WORLD_HEIGHT = 100.0
WINDOW_WIDTH = 1200
WINDOW_HEIGHT = 850

MIN_GAME_SPEED = 0.6
MAX_GAME_SPEED = 2.4
LOGIC_STEP_MS = 16
MAX_LOGIC_STEPS_PER_TICK = 5
MAX_PLAYER_BULLETS = 6
MAX_ENEMY_BULLETS = 64


@dataclass
class Player:
    x: float
    y: float
    width: float
    height: float
    speed: float


@dataclass
class Enemy:
    x: float
    y: float
    width: float
    height: float
    alive: bool = True


@dataclass
class Bullet:
    x: float
    y: float
    velocity_y: float
    from_player: bool


player = Player(x=50.0, y=8.0, width=5.0, height=3.0, speed=1.35)
enemies: list[Enemy] = []
player_bullets: list[Bullet] = []
enemy_bullets: list[Bullet] = []

score = 0
lives = 3
level = 1
paused = False
game_over = False
wave_cleared = False
game_speed = 1.0

move_left_pressed = False
move_right_pressed = False

enemy_direction = 1
enemy_speed = 0.18

shoot_cooldown = 0
shoot_key_armed = True
restart_requested = False

last_tick_ms = 0
logic_accumulator_ms = 0
rng = Random()


def configure_close_behavior() -> None:
    try:
        glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS)
    except Exception:
        pass


def draw_text(x: float, y: float, text: str) -> None:
    glRasterPos2f(x, y)
    for character in text:
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, ord(character))


def draw_rect(center_x: float, center_y: float, width: float, height: float) -> None:
    half_w = width / 2
    half_h = height / 2
    glBegin(GL_QUADS)
    glVertex2f(center_x - half_w, center_y - half_h)
    glVertex2f(center_x + half_w, center_y - half_h)
    glVertex2f(center_x + half_w, center_y + half_h)
    glVertex2f(center_x - half_w, center_y + half_h)
    glEnd()


def intersects(a_x: float, a_y: float, a_w: float, a_h: float, b_x: float, b_y: float, b_w: float, b_h: float) -> bool:
    a_left = a_x - a_w / 2
    a_right = a_x + a_w / 2
    a_bottom = a_y - a_h / 2
    a_top = a_y + a_h / 2

    b_left = b_x - b_w / 2
    b_right = b_x + b_w / 2
    b_bottom = b_y - b_h / 2
    b_top = b_y + b_h / 2

    return not (a_right < b_left or a_left > b_right or a_top < b_bottom or a_bottom > b_top)


def build_wave() -> None:
    global enemies, enemy_direction, enemy_speed, wave_cleared

    enemies = []
    rows = 5
    cols = 9
    spacing_x = 8.6
    spacing_y = 6.0
    start_x = 14.0
    start_y = 86.0

    for row in range(rows):
        for col in range(cols):
            enemies.append(Enemy(x=start_x + col * spacing_x, y=start_y - row * spacing_y, width=4.7, height=3.0, alive=True))

    enemy_direction = 1
    enemy_speed = 0.18 + (level - 1) * 0.03
    wave_cleared = False


def restart_game() -> None:
    global score, lives, level, paused, game_over, game_speed, player_bullets, enemy_bullets, shoot_cooldown
    global shoot_key_armed, restart_requested, last_tick_ms, logic_accumulator_ms

    score = 0
    lives = 3
    level = 1
    paused = False
    game_over = False
    game_speed = 1.0
    player_bullets = []
    enemy_bullets = []
    shoot_cooldown = 0
    shoot_key_armed = True
    restart_requested = False
    last_tick_ms = glutGet(GLUT_ELAPSED_TIME)
    logic_accumulator_ms = 0
    player.x = 50.0
    build_wave()


def start_next_wave() -> None:
    global level, player_bullets, enemy_bullets

    level += 1
    player_bullets = []
    enemy_bullets = []
    build_wave()


def alive_enemies() -> list[Enemy]:
    return [enemy for enemy in enemies if enemy.alive]


def move_player() -> None:
    step = player.speed * game_speed
    if move_left_pressed:
        player.x -= step
    if move_right_pressed:
        player.x += step

    half_w = player.width / 2
    player.x = max(half_w, min(WORLD_WIDTH - half_w, player.x))


def update_enemy_swarm() -> None:
    global enemy_direction

    alive = alive_enemies()
    if not alive:
        return

    step_x = enemy_speed * game_speed * enemy_direction
    hit_wall = False

    for enemy in alive:
        enemy.x += step_x
        if enemy.x + enemy.width / 2 >= WORLD_WIDTH - 1.0 or enemy.x - enemy.width / 2 <= 1.0:
            hit_wall = True

    if hit_wall:
        enemy_direction *= -1
        for enemy in alive:
            enemy.y -= 2.4


def spawn_enemy_bullet() -> None:
    shooters = alive_enemies()
    if not shooters:
        return

    if len(enemy_bullets) >= MAX_ENEMY_BULLETS:
        return

    if rng.random() < 0.03 * game_speed:
        shooter = rng.choice(shooters)
        enemy_bullets.append(Bullet(x=shooter.x, y=shooter.y - shooter.height / 2 - 0.8, velocity_y=-0.95, from_player=False))


def update_bullets() -> None:
    global player_bullets, enemy_bullets

    new_player: list[Bullet] = []
    for bullet in player_bullets:
        bullet.y += bullet.velocity_y * game_speed
        if bullet.y <= WORLD_HEIGHT + 2.0:
            new_player.append(bullet)
    player_bullets = [bullet for bullet in new_player if -2.0 <= bullet.y <= WORLD_HEIGHT + 2.0]

    new_enemy: list[Bullet] = []
    for bullet in enemy_bullets:
        bullet.y += bullet.velocity_y * game_speed
        if bullet.y >= -2.0:
            new_enemy.append(bullet)
    enemy_bullets = [bullet for bullet in new_enemy if -2.0 <= bullet.y <= WORLD_HEIGHT + 2.0]


def handle_collisions() -> None:
    global score, lives, game_over, wave_cleared, player_bullets, enemy_bullets

    remaining_player_bullets: list[Bullet] = []
    for bullet in player_bullets:
        hit_enemy = False
        for enemy in enemies:
            if not enemy.alive:
                continue
            if intersects(bullet.x, bullet.y, 1.1, 2.0, enemy.x, enemy.y, enemy.width, enemy.height):
                enemy.alive = False
                score += 100
                hit_enemy = True
                break
        if not hit_enemy:
            remaining_player_bullets.append(bullet)
    player_bullets = remaining_player_bullets

    remaining_enemy_bullets: list[Bullet] = []
    for bullet in enemy_bullets:
        if intersects(bullet.x, bullet.y, 1.2, 2.0, player.x, player.y, player.width, player.height):
            lives -= 1
            if lives <= 0:
                game_over = True
            continue
        remaining_enemy_bullets.append(bullet)
    enemy_bullets = remaining_enemy_bullets

    for enemy in alive_enemies():
        if intersects(enemy.x, enemy.y, enemy.width, enemy.height, player.x, player.y, player.width, player.height):
            game_over = True
            return
        if enemy.y - enemy.height / 2 <= player.y + player.height / 2:
            game_over = True
            return

    if not alive_enemies() and not wave_cleared and not game_over:
        wave_cleared = True


def update_game() -> None:
    global shoot_cooldown

    if restart_requested:
        restart_game()
        return

    if paused or game_over:
        return

    if wave_cleared:
        start_next_wave()

    move_player()
    update_enemy_swarm()
    spawn_enemy_bullet()
    update_bullets()
    handle_collisions()

    shoot_cooldown = max(0, shoot_cooldown - 1)


def display() -> None:
    glClear(GL_COLOR_BUFFER_BIT)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()

    glColor3f(0.35, 0.9, 1.0)
    draw_rect(player.x, player.y, player.width, player.height)

    for enemy in alive_enemies():
        glColor3f(1.0, 0.45, 0.45)
        draw_rect(enemy.x, enemy.y, enemy.width, enemy.height)

    for bullet in player_bullets:
        glColor3f(1.0, 1.0, 0.45)
        draw_rect(bullet.x, bullet.y, 0.8, 1.8)

    for bullet in enemy_bullets:
        glColor3f(1.0, 0.65, 0.3)
        draw_rect(bullet.x, bullet.y, 0.8, 1.8)

    glColor3f(0.85, 0.92, 1.0)
    draw_text(2.0, 96.0, f"Score: {score}")
    draw_text(18.0, 96.0, f"Vidas: {lives}")
    draw_text(33.0, 96.0, f"Nivel: {level}")
    draw_text(48.0, 96.0, f"Vel: {game_speed:.1f}x")
    draw_text(2.0, 3.0, "A/D ou setas | ESPACO atira | +/- vel | P pausa | R reset | ESC")

    if paused and not game_over:
        glColor3f(1.0, 1.0, 0.6)
        draw_text(44.0, 52.0, "PAUSADO")

    if game_over:
        glColor3f(1.0, 0.6, 0.6)
        draw_text(41.0, 52.0, "GAME OVER")
        draw_text(35.0, 47.0, "Pressione R para reiniciar")

    glutSwapBuffers()


def reshape(width: int, height: int) -> None:
    if height == 0:
        height = 1
    glViewport(0, 0, width, height)
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    glOrtho(0.0, WORLD_WIDTH, 0.0, WORLD_HEIGHT, -1.0, 1.0)


def keyboard_down(key: bytes, _: int, __: int) -> None:
    global move_left_pressed, move_right_pressed, paused, game_speed, shoot_key_armed, restart_requested

    if key in (b"a", b"A"):
        move_left_pressed = True
    elif key in (b"d", b"D"):
        move_right_pressed = True
    elif key == b" ":
        can_shoot = (not paused and not game_over and shoot_cooldown == 0 and shoot_key_armed)
        if can_shoot and len(player_bullets) < MAX_PLAYER_BULLETS:
            player_bullets.append(Bullet(x=player.x, y=player.y + player.height / 2 + 0.8, velocity_y=1.35, from_player=True))
            shoot_cooldown = 9
            shoot_key_armed = False
    elif key in (b"p", b"P"):
        if not game_over:
            paused = not paused
    elif key == b"+":
        game_speed = min(MAX_GAME_SPEED, game_speed + 0.1)
    elif key == b"-":
        game_speed = max(MIN_GAME_SPEED, game_speed - 0.1)
    elif key in (b"r", b"R"):
        restart_requested = True
    elif key == b"\x1b":
        raise SystemExit("ESC")


def keyboard_up(key: bytes, _: int, __: int) -> None:
    global move_left_pressed, move_right_pressed, shoot_key_armed

    if key in (b"a", b"A"):
        move_left_pressed = False
    elif key in (b"d", b"D"):
        move_right_pressed = False
    elif key == b" ":
        shoot_key_armed = True


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
    global last_tick_ms, logic_accumulator_ms

    now_ms = glutGet(GLUT_ELAPSED_TIME)
    elapsed_ms = max(0, now_ms - last_tick_ms)
    last_tick_ms = now_ms

    logic_accumulator_ms += elapsed_ms

    steps = 0
    while logic_accumulator_ms >= LOGIC_STEP_MS and steps < MAX_LOGIC_STEPS_PER_TICK:
        update_game()
        logic_accumulator_ms -= LOGIC_STEP_MS
        steps += 1

    if steps == MAX_LOGIC_STEPS_PER_TICK and logic_accumulator_ms > LOGIC_STEP_MS * 2:
        logic_accumulator_ms = LOGIC_STEP_MS * 2

    glutPostRedisplay()
    glutTimerFunc(LOGIC_STEP_MS, tick, 0)


def main() -> None:
    glutInit()
    configure_close_behavior()
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB)
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT)
    glutCreateWindow(b"Aula 15 - Jogo Galaga")
    glClearColor(0.03, 0.03, 0.07, 1.0)

    restart_game()

    glutDisplayFunc(display)
    glutReshapeFunc(reshape)
    glutKeyboardFunc(keyboard_down)
    glutKeyboardUpFunc(keyboard_up)
    glutSpecialFunc(special_down)
    glutSpecialUpFunc(special_up)
    glutTimerFunc(0, tick, 0)

    print("Galaga iniciado: A/D, setas, ESPACO, +/-, P, R, ESC")
    glutMainLoop()


if __name__ == "__main__":
    main()
