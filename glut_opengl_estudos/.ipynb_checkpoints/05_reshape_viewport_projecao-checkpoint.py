"""
Aula 05 — Reshape, viewport e projeção ortográfica

Conceitos:
- `reshape(w, h)` é chamado quando a janela muda de tamanho.
- `glViewport` define a região de renderização em pixels.
- Projeção ortográfica com `gluOrtho2D` para preservar proporção.

Como executar:
    python3 05_reshape_viewport_projecao.py
"""

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
    glVertex2f,
    glViewport,
)
from OpenGL.GLU import gluOrtho2D
from OpenGL.GLUT import GLUT_RGB, GLUT_SINGLE, glutCreateWindow, glutDisplayFunc, glutInit, glutInitDisplayMode, glutInitWindowSize, glutMainLoop, glutReshapeFunc


def draw_square() -> None:
    """Desenha quadrado central."""
    glBegin(GL_QUADS)
    glColor3f(0.2, 0.75, 1.0)
    glVertex2f(-0.5, -0.5)
    glVertex2f(0.5, -0.5)
    glVertex2f(0.5, 0.5)
    glVertex2f(-0.5, 0.5)
    glEnd()


def display() -> None:
    """Renderiza cena estática."""
    glClear(GL_COLOR_BUFFER_BIT)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()
    draw_square()


def reshape(width: int, height: int) -> None:
    """Ajusta viewport e projeção para manter proporção visual.

    Se a janela ficar mais larga, ampliamos X.
    Se ficar mais alta, ampliamos Y.
    """
    if height == 0:
        height = 1

    aspect = width / height

    glViewport(0, 0, width, height)
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()

    if aspect >= 1.0:
        gluOrtho2D(-aspect, aspect, -1.0, 1.0)
    else:
        inv = 1.0 / aspect
        gluOrtho2D(-1.0, 1.0, -inv, inv)


def main() -> None:
    """Entry point da aula 05."""
    glutInit()
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB)
    glutInitWindowSize(900, 600)
    glutCreateWindow(b"Aula 05 - Reshape e Viewport")
    glClearColor(0.05, 0.05, 0.07, 1.0)

    glutDisplayFunc(display)
    glutReshapeFunc(reshape)
    glutMainLoop()


if __name__ == "__main__":
    main()
