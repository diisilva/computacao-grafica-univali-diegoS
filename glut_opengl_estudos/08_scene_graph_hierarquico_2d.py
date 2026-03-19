"""Aula 08 — Scene Graph 2D com arquitetura orientada a nós.

Este exemplo apresenta um design mais profissional para OpenGL clássico:
- Estrutura em árvore (scene graph) com transformações hierárquicas.
- Separação de responsabilidades entre atualização e renderização.
- Controle de tempo, pause e velocidade de animação.

Controles:
- ESPAÇO: pausa/retoma animação
- + / - : aumenta/diminui velocidade global
- ESC: sair
"""

from __future__ import annotations

from dataclasses import dataclass, field
from math import cos, sin, tau
from typing import Callable

from OpenGL.GL import (
    GL_COLOR_BUFFER_BIT,
    GL_LINE_LOOP,
    GL_MODELVIEW,
    GL_POLYGON,
    glBegin,
    glClear,
    glClearColor,
    glColor3f,
    glEnd,
    glLoadIdentity,
    glMatrixMode,
    glPopMatrix,
    glPushMatrix,
    glRasterPos2f,
    glRotatef,
    glScalef,
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


@dataclass
class Transform2D:
    """Transformação local de um nó da cena."""

    tx: float = 0.0
    ty: float = 0.0
    rotation_deg: float = 0.0
    sx: float = 1.0
    sy: float = 1.0


@dataclass
class SceneNode:
    """Nó de cena com transformação local e filhos.

    Attributes:
        name: Nome lógico do nó para depuração/estudo.
        transform: Transformação local aplicada antes do desenho.
        draw_fn: Função de desenho opcional para o nó.
        children: Lista de nós filhos (hierarquia).
    """

    name: str
    transform: Transform2D = field(default_factory=Transform2D)
    draw_fn: Callable[[], None] | None = None
    children: list["SceneNode"] = field(default_factory=list)

    def render(self) -> None:
        """Renderiza o nó e seus filhos com pilha de matrizes."""
        glPushMatrix()
        glTranslatef(self.transform.tx, self.transform.ty, 0.0)
        glRotatef(self.transform.rotation_deg, 0.0, 0.0, 1.0)
        glScalef(self.transform.sx, self.transform.sy, 1.0)

        if self.draw_fn is not None:
            self.draw_fn()

        for child in self.children:
            child.render()

        glPopMatrix()


class SceneGraphDemo:
    """Demo de scene graph com dois corpos orbitando um núcleo."""

    def __init__(self) -> None:
        self.time = 0.0
        self.speed = 1.0
        self.paused = False

        self.root = SceneNode("root")
        self.pivot_outer = SceneNode("pivot_outer")
        self.pivot_inner = SceneNode("pivot_inner")

        self.core = SceneNode("core", draw_fn=self._draw_core)
        self.outer_planet = SceneNode("outer_planet", draw_fn=self._draw_outer)
        self.inner_planet = SceneNode("inner_planet", draw_fn=self._draw_inner)

        self.root.children.extend([self.core, self.pivot_outer])
        self.pivot_outer.children.extend([self.outer_planet, self.pivot_inner])
        self.pivot_inner.children.append(self.inner_planet)

    def _draw_circle(self, radius: float, color: tuple[float, float, float], segments: int = 40) -> None:
        glColor3f(*color)
        glBegin(GL_POLYGON)
        for i in range(segments):
            angle = tau * i / segments
            glVertex2f(radius * cos(angle), radius * sin(angle))
        glEnd()

    def _draw_orbit(self, radius: float, color: tuple[float, float, float] = (0.2, 0.2, 0.25)) -> None:
        glColor3f(*color)
        glBegin(GL_LINE_LOOP)
        for i in range(96):
            angle = tau * i / 96
            glVertex2f(radius * cos(angle), radius * sin(angle))
        glEnd()

    def _draw_text(self, x: float, y: float, text: str) -> None:
        glColor3f(0.9, 0.9, 0.95)
        glRasterPos2f(x, y)
        for char in text:
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, ord(char))

    def _draw_core(self) -> None:
        self._draw_circle(0.10, (1.0, 0.85, 0.2), segments=60)

    def _draw_outer(self) -> None:
        self._draw_circle(0.035, (0.25, 0.6, 1.0), segments=36)

    def _draw_inner(self) -> None:
        self._draw_circle(0.015, (0.85, 0.85, 0.95), segments=24)

    def update(self) -> None:
        """Atualiza parâmetros da hierarquia com base no tempo global."""
        if not self.paused:
            self.time += 0.012 * self.speed

        self.pivot_outer.transform.rotation_deg = self.time * 80.0
        self.outer_planet.transform.tx = 0.52

        self.pivot_inner.transform.rotation_deg = self.time * 240.0
        self.pivot_inner.transform.tx = 0.0
        self.inner_planet.transform.tx = 0.14

    def render(self) -> None:
        """Renderiza cena com órbitas auxiliares e scene graph."""
        glClear(GL_COLOR_BUFFER_BIT)
        glMatrixMode(GL_MODELVIEW)
        glLoadIdentity()

        self._draw_orbit(0.52)
        glPushMatrix()
        glRotatef(self.pivot_outer.transform.rotation_deg, 0.0, 0.0, 1.0)
        glTranslatef(0.52, 0.0, 0.0)
        self._draw_orbit(0.14, color=(0.25, 0.25, 0.30))
        glPopMatrix()

        self.root.render()
        self._draw_text(-0.95, -0.92, f"speed={self.speed:.1f} | paused={self.paused}")
        glutSwapBuffers()


demo = SceneGraphDemo()


def configure_close_behavior() -> None:
    """Configura GLUT para tentar retornar ao fechar a janela."""
    try:
        glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS)
    except Exception:
        pass


def display() -> None:
    demo.render()


def keyboard(key: bytes, _: int, __: int) -> None:
    if key == b" ":
        demo.paused = not demo.paused
    elif key == b"+":
        demo.speed = min(demo.speed + 0.2, 6.0)
    elif key == b"-":
        demo.speed = max(demo.speed - 0.2, 0.2)
    elif key == b"\x1b":
        raise SystemExit("ESC")


def tick(_: int) -> None:
    demo.update()
    glutPostRedisplay()
    glutTimerFunc(16, tick, 0)


def main() -> None:
    glutInit()
    configure_close_behavior()
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB)
    glutInitWindowSize(1100, 800)
    glutCreateWindow(b"Aula 08 - Scene Graph 2D")
    glClearColor(0.03, 0.03, 0.06, 1.0)

    glutDisplayFunc(display)
    glutKeyboardFunc(keyboard)
    glutTimerFunc(0, tick, 0)

    print("Controles: ESPACO, +, -, ESC")
    glutMainLoop()


if __name__ == "__main__":
    main()
