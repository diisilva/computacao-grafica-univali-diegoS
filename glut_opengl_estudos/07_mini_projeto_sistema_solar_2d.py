"""
Aula 07 — Mini projeto: sistema solar 2D completo

Conceitos integrados:
- Sistema solar com 8 planetas em órbitas distintas.
- Campo de estrelas no fundo da cena.
- Legendas de identificação dos planetas.
- Controle interativo de velocidade, pausa e exibição de rótulos.

Atalhos:
- + : aumentar velocidade
- - : diminuir velocidade
- ESPAÇO: pausar/retomar
- L : mostrar/ocultar rótulos
- ESC: sair

Como executar:
    python3 07_mini_projeto_sistema_solar_2d.py
"""

from math import cos, sin, tau

from OpenGL.GL import (
    GL_COLOR_BUFFER_BIT,
    GL_LINE_LOOP,
    GL_POINTS,
    GL_POLYGON,
    glBegin,
    glClear,
    glClearColor,
    glColor3f,
    glEnd,
    glPointSize,
    glPushMatrix,
    glPopMatrix,
    glRasterPos2f,
    glTranslatef,
    glVertex2f,
)
from OpenGL.GLUT import (
    GLUT_ACTION_GLUTMAINLOOP_RETURNS,
    GLUT_ACTION_ON_WINDOW_CLOSE,
    GLUT_BITMAP_HELVETICA_12,
    GLUT_DOUBLE,
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
    glutSetOption,
    glutSwapBuffers,
    glutTimerFunc,
)

sim_time = 0.0
speed = 1.0
paused = False
show_labels = True

PLANETS = [
    {"name": "Mercurio", "orbit": 0.14, "radius": 0.010, "speed": 4.8, "color": (0.70, 0.70, 0.70)},
    {"name": "Venus", "orbit": 0.20, "radius": 0.016, "speed": 3.5, "color": (0.90, 0.75, 0.45)},
    {"name": "Terra", "orbit": 0.27, "radius": 0.017, "speed": 3.0, "color": (0.20, 0.55, 1.00)},
    {"name": "Marte", "orbit": 0.34, "radius": 0.014, "speed": 2.4, "color": (0.90, 0.35, 0.25)},
    {"name": "Jupiter", "orbit": 0.48, "radius": 0.040, "speed": 1.3, "color": (0.85, 0.65, 0.45)},
    {"name": "Saturno", "orbit": 0.62, "radius": 0.033, "speed": 1.0, "color": (0.90, 0.82, 0.55)},
    {"name": "Urano", "orbit": 0.76, "radius": 0.024, "speed": 0.7, "color": (0.55, 0.85, 0.90)},
    {"name": "Netuno", "orbit": 0.88, "radius": 0.023, "speed": 0.55, "color": (0.30, 0.45, 0.95)},
]

STARS = [
    (-0.95, 0.95), (-0.82, 0.75), (-0.60, 0.90), (-0.40, 0.70), (-0.20, 0.96),
    (0.05, 0.88), (0.28, 0.74), (0.52, 0.93), (0.74, 0.79), (0.92, 0.91),
    (-0.90, 0.45), (-0.65, 0.52), (-0.42, 0.40), (-0.15, 0.55), (0.16, 0.46),
    (0.43, 0.51), (0.70, 0.44), (0.90, 0.36), (-0.85, 0.10), (-0.58, 0.20),
    (-0.30, 0.12), (-0.05, 0.18), (0.20, 0.08), (0.48, 0.16), (0.77, 0.12),
    (-0.94, -0.24), (-0.66, -0.15), (-0.39, -0.29), (-0.12, -0.18), (0.14, -0.25),
    (0.38, -0.12), (0.64, -0.22), (0.90, -0.30), (-0.78, -0.60), (-0.50, -0.52),
    (-0.24, -0.72), (0.02, -0.64), (0.28, -0.77), (0.56, -0.58), (0.84, -0.70),
]


def configure_close_behavior() -> None:
    """Evita `exit()` abrupto quando a janela fecha (se suportado pelo GLUT)."""
    try:
        glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS)
    except Exception:
        pass


def draw_text(x: float, y: float, text: str) -> None:
    """Desenha texto 2D em posição NDC com fonte bitmap do GLUT."""
    glRasterPos2f(x, y)
    for char in text:
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, ord(char))


def draw_circle(radius: float, color: tuple[float, float, float], segments: int = 40) -> None:
    """Desenha círculo preenchido aproximado por polígono."""
    glColor3f(*color)
    glBegin(GL_POLYGON)
    for i in range(segments):
        t = tau * i / segments
        glVertex2f(radius * cos(t), radius * sin(t))
    glEnd()


def draw_orbit(radius: float) -> None:
    """Desenha linha circular para representar órbita de um planeta."""
    glColor3f(0.18, 0.18, 0.25)
    glBegin(GL_LINE_LOOP)
    for i in range(120):
        t = tau * i / 120
        glVertex2f(radius * cos(t), radius * sin(t))
    glEnd()


def draw_stars() -> None:
    """Desenha estrelas fixas no fundo."""
    glPointSize(2.0)
    glColor3f(0.95, 0.95, 1.0)
    glBegin(GL_POINTS)
    for sx, sy in STARS:
        glVertex2f(sx, sy)
    glEnd()


def display() -> None:
    """Renderiza sistema solar completo com rótulos opcionais."""
    glClear(GL_COLOR_BUFFER_BIT)
    draw_stars()

    for planet in PLANETS:
        draw_orbit(planet["orbit"])

    draw_circle(0.08, (1.0, 0.85, 0.15), segments=60)

    for planet in PLANETS:
        angle = sim_time * planet["speed"]
        x = planet["orbit"] * cos(angle)
        y = planet["orbit"] * sin(angle)

        glPushMatrix()
        glTranslatef(x, y, 0.0)
        draw_circle(planet["radius"], planet["color"], segments=28)
        glPopMatrix()

        if show_labels:
            glColor3f(0.85, 0.9, 1.0)
            draw_text(x + planet["radius"] + 0.01, y + planet["radius"] + 0.01, planet["name"])

    glColor3f(1.0, 1.0, 0.6)
    draw_text(-0.96, -0.95, "Controles: +  -  ESPACO  L  ESC")
    glutSwapBuffers()


def update(_: int) -> None:
    """Atualiza tempo da simulação e agenda o próximo frame."""
    global sim_time
    if not paused:
        sim_time += 0.01 * speed
    glutPostRedisplay()
    glutTimerFunc(16, update, 0)


def keyboard(key: bytes, _: int, __: int) -> None:
    """Controla velocidade, pausa, rótulos e saída."""
    global speed, paused, show_labels

    if key == b"+":
        speed = min(speed + 0.2, 8.0)
    elif key == b"-":
        speed = max(speed - 0.2, 0.2)
    elif key == b" ":
        paused = not paused
    elif key in (b"l", b"L"):
        show_labels = not show_labels
    elif key == b"\x1b":
        raise SystemExit("Encerrado com ESC")

    print(f"speed={speed:.1f} | paused={paused} | labels={show_labels}")


def main() -> None:
    """Entry point da aula 07."""
    glutInit()
    configure_close_behavior()
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB)
    glutInitWindowSize(1200, 900)
    glutCreateWindow(b"Aula 07 - Sistema Solar 2D Completo")
    glClearColor(0.01, 0.01, 0.05, 1.0)

    glutDisplayFunc(display)
    glutKeyboardFunc(keyboard)
    glutTimerFunc(0, update, 0)

    print("Controles: +, -, ESPACO, L, ESC")
    glutMainLoop()


if __name__ == "__main__":
    main()
