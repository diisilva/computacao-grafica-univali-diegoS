"""
Aula 06 — Double buffering e medição simples de FPS

Conceitos:
- `GLUT_DOUBLE`: renderiza em back buffer e troca com `glutSwapBuffers`.
- `glutIdleFunc`: atualiza continuamente a cena.
- FPS médio por janela temporal.

Como executar:
    python3 06_double_buffer_fps.py
"""

from time import perf_counter

from OpenGL.GL import (
    GL_COLOR_BUFFER_BIT,
    GL_QUADS,
    glBegin,
    glClear,
    glClearColor,
    glColor3f,
    glEnd,
    glLoadIdentity,
    glRotatef,
    glVertex2f,
)
from OpenGL.GLUT import GLUT_DOUBLE, GLUT_RGB, glutCreateWindow, glutDisplayFunc, glutIdleFunc, glutInit, glutInitDisplayMode, glutInitWindowSize, glutMainLoop, glutPostRedisplay, glutSwapBuffers

angle = 0.0
frame_count = 0
start_time = perf_counter()


def draw_rotating_quad() -> None:
    """Desenha um quadrado rotacionando ao redor do centro."""
    glBegin(GL_QUADS)
    glColor3f(1.0, 0.6, 0.2)
    glVertex2f(-0.3, -0.3)
    glVertex2f(0.3, -0.3)
    glColor3f(0.2, 0.8, 1.0)
    glVertex2f(0.3, 0.3)
    glVertex2f(-0.3, 0.3)
    glEnd()


def display() -> None:
    """Renderiza um frame e atualiza medição de FPS."""
    global frame_count, start_time

    glClear(GL_COLOR_BUFFER_BIT)
    glLoadIdentity()
    glRotatef(angle, 0.0, 0.0, 1.0)
    draw_rotating_quad()
    glutSwapBuffers()

    frame_count += 1
    elapsed = perf_counter() - start_time
    if elapsed >= 1.0:
        fps = frame_count / elapsed
        print(f"FPS médio: {fps:.1f}")
        frame_count = 0
        start_time = perf_counter()


def idle() -> None:
    """Atualiza ângulo e solicita novo frame."""
    global angle
    angle = (angle + 0.35) % 360.0
    glutPostRedisplay()


def main() -> None:
    """Entry point da aula 06."""
    glutInit()
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB)
    glutInitWindowSize(900, 600)
    glutCreateWindow(b"Aula 06 - Double Buffer e FPS")
    glClearColor(0.04, 0.04, 0.06, 1.0)

    glutDisplayFunc(display)
    glutIdleFunc(idle)
    glutMainLoop()


if __name__ == "__main__":
    main()
