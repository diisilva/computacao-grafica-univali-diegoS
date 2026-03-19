"""
Aula 02 — Primitivas e sistema de coordenadas NDC

Conceitos:
- NDC (Normalized Device Coordinates): eixo X e Y em [-1, 1].
- Primitivas básicas: pontos, linhas e triângulos.
- Espessura de linha e tamanho de ponto.

Como executar:
    python3 02_primitivas_e_sistema_de_coordenadas.py
"""

from OpenGL.GL import (
    GL_COLOR_BUFFER_BIT,
    GL_LINES,
    GL_POINTS,
    GL_TRIANGLES,
    glBegin,
    glClear,
    glClearColor,
    glColor3f,
    glEnd,
    glFlush,
    glLineWidth,
    glPointSize,
    glVertex2f,
)
from OpenGL.GLUT import GLUT_RGB, GLUT_SINGLE, glutCreateWindow, glutDisplayFunc, glutInit, glutInitDisplayMode, glutInitWindowSize, glutMainLoop


def draw_axes() -> None:
    """Desenha eixos X e Y para referência visual."""
    glLineWidth(2.0)
    glBegin(GL_LINES)
    glColor3f(0.7, 0.7, 0.7)
    glVertex2f(-1.0, 0.0)
    glVertex2f(1.0, 0.0)
    glVertex2f(0.0, -1.0)
    glVertex2f(0.0, 1.0)
    glEnd()


def draw_primitives() -> None:
    """Mostra três tipos de primitivas no mesmo frame."""
    glPointSize(10.0)
    glBegin(GL_POINTS)
    glColor3f(1.0, 0.9, 0.2)
    glVertex2f(-0.7, 0.6)
    glVertex2f(-0.5, 0.45)
    glVertex2f(-0.3, 0.7)
    glEnd()

    glLineWidth(4.0)
    glBegin(GL_LINES)
    glColor3f(0.2, 0.9, 1.0)
    glVertex2f(-0.1, 0.6)
    glVertex2f(0.4, 0.75)
    glVertex2f(-0.1, 0.4)
    glVertex2f(0.4, 0.2)
    glEnd()

    glBegin(GL_TRIANGLES)
    glColor3f(0.9, 0.3, 0.3)
    glVertex2f(0.55, 0.2)
    glColor3f(0.3, 0.9, 0.4)
    glVertex2f(0.9, 0.2)
    glColor3f(0.3, 0.4, 1.0)
    glVertex2f(0.72, 0.7)
    glEnd()


def display() -> None:
    """Desenha eixos + primitivas."""
    glClear(GL_COLOR_BUFFER_BIT)
    draw_axes()
    draw_primitives()
    glFlush()


def main() -> None:
    """Entry point da aula 02."""
    glutInit()
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB)
    glutInitWindowSize(900, 600)
    glutCreateWindow(b"Aula 02 - Primitivas e NDC")
    glClearColor(0.08, 0.08, 0.1, 1.0)
    glutDisplayFunc(display)
    glutMainLoop()


if __name__ == "__main__":
    main()
