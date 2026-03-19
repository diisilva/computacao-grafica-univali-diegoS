"""
Aula 01 — Primeira janela com GLUT + OpenGL

Objetivos didáticos:
1) Entender o papel do GLUT (janela, eventos e loop).
2) Entender o callback `display`.
3) Desenhar um triângulo com cores interpoladas.

Como executar:
    python3 01_janela_e_triangulo.py
"""

from OpenGL.GL import (
    GL_COLOR_BUFFER_BIT,
    GL_DEPTH_BUFFER_BIT,
    GL_TRIANGLES,
    glBegin,
    glClear,
    glClearColor,
    glColor3f,
    glEnd,
    glFlush,
    glVertex2f,
)
from OpenGL.GLUT import (
    GLUT_ACTION_GLUTMAINLOOP_RETURNS,
    GLUT_ACTION_ON_WINDOW_CLOSE,
    GLUT_DEPTH,
    GLUT_RGB,
    GLUT_SINGLE,
    glutCreateWindow,
    glutDisplayFunc,
    glutInit,
    glutInitDisplayMode,
    glutInitWindowSize,
    glutMainLoop,
    glutSetOption,
)


def configure_close_behavior() -> None:
    """Tenta evitar `exit()` do processo ao fechar a janela (quando suportado)."""
    try:
        glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS)
    except Exception:
        pass


def display() -> None:
    """Callback de desenho.

    O OpenGL imediato (`glBegin`/`glEnd`) é simples para estudo.
    Cada vértice recebe uma cor; a GPU interpola no interior do triângulo.
    """
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

    glBegin(GL_TRIANGLES)
    glColor3f(1.0, 0.0, 0.0)
    glVertex2f(-0.7, -0.6)

    glColor3f(0.0, 1.0, 0.0)
    glVertex2f(0.7, -0.6)

    glColor3f(0.0, 0.2, 1.0)
    glVertex2f(0.0, 0.7)
    glEnd()

    glFlush()


def main() -> None:
    """Inicializa contexto OpenGL e inicia o loop de eventos."""
    glutInit()
    configure_close_behavior()
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH)
    glutInitWindowSize(800, 600)
    glutCreateWindow(b"Aula 01 - Triangulo")

    glClearColor(0.05, 0.05, 0.08, 1.0)
    glutDisplayFunc(display)

    print("Feche a janela para encerrar a aula 01.")
    glutMainLoop()


if __name__ == "__main__":
    main()
