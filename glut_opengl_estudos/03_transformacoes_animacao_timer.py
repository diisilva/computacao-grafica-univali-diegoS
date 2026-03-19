"""
Aula 03 — Transformações 2D e animação com timer

Conceitos:
- Matrizes de transformação (translação, rotação, escala).
- Pilha de matriz (`glPushMatrix`/`glPopMatrix`).
- Atualização temporal com `glutTimerFunc`.

Como executar:
    python3 03_transformacoes_animacao_timer.py
"""

from OpenGL.GL import (
    GL_COLOR_BUFFER_BIT,
    GL_MODELVIEW,
    GL_TRIANGLES,
    glBegin,
    glClear,
    glClearColor,
    glColor3f,
    glEnd,
    glLoadIdentity,
    glMatrixMode,
    glPopMatrix,
    glPushMatrix,
    glRotatef,
    glScalef,
    glTranslatef,
    glVertex2f,
)
from OpenGL.GLUT import GLUT_DOUBLE, GLUT_RGB, glutCreateWindow, glutDisplayFunc, glutInit, glutInitDisplayMode, glutInitWindowSize, glutMainLoop, glutPostRedisplay, glutSwapBuffers, glutTimerFunc

angle_degrees = 0.0


def draw_triangle() -> None:
    """Desenha triângulo centrado na origem local."""
    glBegin(GL_TRIANGLES)
    glColor3f(1.0, 0.2, 0.2)
    glVertex2f(0.0, 0.3)
    glColor3f(0.2, 1.0, 0.2)
    glVertex2f(-0.3, -0.25)
    glColor3f(0.2, 0.4, 1.0)
    glVertex2f(0.3, -0.25)
    glEnd()


def display() -> None:
    """Renderiza cena animada.

    Ordem das transformações no código:
    1) translação para direita
    2) rotação contínua
    3) escala leve
    """
    glClear(GL_COLOR_BUFFER_BIT)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()

    glPushMatrix()
    glTranslatef(0.45, 0.0, 0.0)
    glRotatef(angle_degrees, 0.0, 0.0, 1.0)
    glScalef(1.2, 1.2, 1.0)
    draw_triangle()
    glPopMatrix()

    glutSwapBuffers()


def update_timer(_: int) -> None:
    """Atualiza estado de animação e agenda próximo frame."""
    global angle_degrees
    angle_degrees = (angle_degrees + 1.5) % 360.0
    glutPostRedisplay()
    glutTimerFunc(16, update_timer, 0)


def main() -> None:
    """Entry point da aula 03."""
    glutInit()
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB)
    glutInitWindowSize(900, 600)
    glutCreateWindow(b"Aula 03 - Transformacoes e Timer")
    glClearColor(0.03, 0.03, 0.05, 1.0)
    glutDisplayFunc(display)
    glutTimerFunc(0, update_timer, 0)
    glutMainLoop()


if __name__ == "__main__":
    main()
