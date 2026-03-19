"""
Aula 04 — Interação com teclado e mouse

Conceitos:
- Callback de teclado (`glutKeyboardFunc`).
- Callback de mouse (`glutMouseFunc`).
- Estado global simples para aplicações interativas.

Atalhos:
- W/S: mover para cima/baixo
- A/D: mover para esquerda/direita
- R/G/B: trocar cor
- Botão esquerdo do mouse: reposicionar objeto
- ESC: sair

Como executar:
    python3 04_entrada_teclado_mouse.py
"""

from OpenGL.GL import (
    GL_COLOR_BUFFER_BIT,
    GL_QUADS,
    glBegin,
    glClear,
    glClearColor,
    glColor3f,
    glEnd,
    glLoadIdentity,
    glTranslatef,
    glVertex2f,
)
from OpenGL.GLUT import (
    GLUT_DOWN,
    GLUT_LEFT_BUTTON,
    GLUT_RGB,
    GLUT_SINGLE,
    GLUT_WINDOW_HEIGHT,
    GLUT_WINDOW_WIDTH,
    glutCreateWindow,
    glutDisplayFunc,
    glutGet,
    glutInit,
    glutInitDisplayMode,
    glutInitWindowSize,
    glutKeyboardFunc,
    glutMainLoop,
    glutMouseFunc,
    glutPostRedisplay,
)

position_x = 0.0
position_y = 0.0
current_color = (0.9, 0.4, 0.2)


def draw_square() -> None:
    """Desenha quadrado com centro na origem local."""
    glBegin(GL_QUADS)
    glVertex2f(-0.15, -0.15)
    glVertex2f(0.15, -0.15)
    glVertex2f(0.15, 0.15)
    glVertex2f(-0.15, 0.15)
    glEnd()


def display() -> None:
    """Renderiza quadrado conforme estado atual."""
    glClear(GL_COLOR_BUFFER_BIT)
    glLoadIdentity()

    glTranslatef(position_x, position_y, 0.0)
    glColor3f(*current_color)
    draw_square()


def keyboard(key: bytes, _: int, __: int) -> None:
    """Processa teclas e atualiza estado global."""
    global position_x, position_y, current_color

    step = 0.05
    if key == b"w":
        position_y += step
    elif key == b"s":
        position_y -= step
    elif key == b"a":
        position_x -= step
    elif key == b"d":
        position_x += step
    elif key == b"r":
        current_color = (1.0, 0.2, 0.2)
    elif key == b"g":
        current_color = (0.2, 1.0, 0.2)
    elif key == b"b":
        current_color = (0.2, 0.4, 1.0)
    elif key == b"\x1b":
        raise SystemExit("Encerrado pelo usuário (ESC).")

    glutPostRedisplay()


def mouse(button: int, state: int, x: int, y: int) -> None:
    """Converte coordenadas de tela (pixels) para NDC e reposiciona objeto."""
    global position_x, position_y

    if button != GLUT_LEFT_BUTTON or state != GLUT_DOWN:
        return

    width = glutGet(GLUT_WINDOW_WIDTH)
    height = glutGet(GLUT_WINDOW_HEIGHT)
    position_x = 2.0 * (x / width) - 1.0
    position_y = 1.0 - 2.0 * (y / height)
    glutPostRedisplay()


def main() -> None:
    """Entry point da aula 04."""
    glutInit()
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB)
    glutInitWindowSize(900, 600)
    glutCreateWindow(b"Aula 04 - Teclado e Mouse")
    glClearColor(0.07, 0.07, 0.1, 1.0)

    glutDisplayFunc(display)
    glutKeyboardFunc(keyboard)
    glutMouseFunc(mouse)

    print("Use WASD, RGB, clique esquerdo e ESC.")
    glutMainLoop()


if __name__ == "__main__":
    main()
