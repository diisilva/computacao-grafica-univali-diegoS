"""Aula 09 — Picking por mouse + colisão AABB em tempo real.

Exemplo avançado de engenharia de gameplay 2D com OpenGL clássico:
- Entidades atualizadas com passo temporal fixo por timer.
- Picking de entidade via clique do mouse (NDC).
- Detecção de colisão AABB e realce visual.

Controles:
- Clique esquerdo: seleciona caixa
- R: reset das posições
- ESC: sair
"""

from __future__ import annotations

from dataclasses import dataclass
from random import Random

from OpenGL.GL import (
    GL_COLOR_BUFFER_BIT,
    GL_LINE_LOOP,
    GL_MODELVIEW,
    GL_QUADS,
    glBegin,
    glClear,
    glClearColor,
    glColor3f,
    glEnd,
    glLoadIdentity,
    glMatrixMode,
    glRasterPos2f,
    glVertex2f,
)
from OpenGL.GLUT import (
    GLUT_ACTION_GLUTMAINLOOP_RETURNS,
    GLUT_ACTION_ON_WINDOW_CLOSE,
    GLUT_BITMAP_HELVETICA_12,
    GLUT_DOUBLE,
    GLUT_DOWN,
    GLUT_LEFT_BUTTON,
    GLUT_RGB,
    GLUT_WINDOW_HEIGHT,
    GLUT_WINDOW_WIDTH,
    glutBitmapCharacter,
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
    glutSetOption,
    glutSwapBuffers,
    glutTimerFunc,
)


@dataclass
class BoxEntity:
    """Representa uma caixa 2D com velocidade, cor e estado de seleção."""

    x: float
    y: float
    half_size: float
    vx: float
    vy: float
    color: tuple[float, float, float]
    selected: bool = False

    def min_x(self) -> float:
        return self.x - self.half_size

    def max_x(self) -> float:
        return self.x + self.half_size

    def min_y(self) -> float:
        return self.y - self.half_size

    def max_y(self) -> float:
        return self.y + self.half_size

    def contains(self, px: float, py: float) -> bool:
        """Retorna True se o ponto em NDC estiver dentro da caixa."""
        return self.min_x() <= px <= self.max_x() and self.min_y() <= py <= self.max_y()


class CollisionPickingDemo:
    """Gerencia simulação, picking e renderização de caixas em colisão."""

    def __init__(self) -> None:
        self.random = Random(42)
        self.entities: list[BoxEntity] = []
        self.colliding_pairs: set[tuple[int, int]] = set()
        self._build_entities()

    def _build_entities(self) -> None:
        self.entities.clear()
        palette = [(1.0, 0.4, 0.4), (0.4, 1.0, 0.5), (0.4, 0.7, 1.0), (1.0, 0.9, 0.4), (0.9, 0.5, 1.0)]
        for i in range(6):
            self.entities.append(
                BoxEntity(
                    x=self.random.uniform(-0.8, 0.8),
                    y=self.random.uniform(-0.6, 0.6),
                    half_size=self.random.uniform(0.07, 0.11),
                    vx=self.random.uniform(-0.006, 0.006),
                    vy=self.random.uniform(-0.006, 0.006),
                    color=palette[i % len(palette)],
                )
            )

    def _aabb_overlap(self, a: BoxEntity, b: BoxEntity) -> bool:
        return not (
            a.max_x() < b.min_x()
            or a.min_x() > b.max_x()
            or a.max_y() < b.min_y()
            or a.min_y() > b.max_y()
        )

    def _solve_wall_bounce(self, entity: BoxEntity) -> None:
        if entity.min_x() < -1.0 or entity.max_x() > 1.0:
            entity.vx *= -1.0
        if entity.min_y() < -1.0 or entity.max_y() > 1.0:
            entity.vy *= -1.0

        entity.x = max(-1.0 + entity.half_size, min(1.0 - entity.half_size, entity.x))
        entity.y = max(-1.0 + entity.half_size, min(1.0 - entity.half_size, entity.y))

    def update(self) -> None:
        """Atualiza posições, resolve paredes e detecta colisões AABB."""
        for entity in self.entities:
            entity.x += entity.vx
            entity.y += entity.vy
            self._solve_wall_bounce(entity)

        self.colliding_pairs.clear()
        n = len(self.entities)
        for i in range(n):
            for j in range(i + 1, n):
                if self._aabb_overlap(self.entities[i], self.entities[j]):
                    self.colliding_pairs.add((i, j))

    def pick_at(self, px: float, py: float) -> None:
        """Seleciona entidade clicada; se nenhuma, limpa seleção."""
        selected_any = False
        for entity in self.entities:
            entity.selected = False

        for entity in reversed(self.entities):
            if entity.contains(px, py):
                entity.selected = True
                selected_any = True
                break

        if not selected_any:
            for entity in self.entities:
                entity.selected = False

    def _draw_text(self, x: float, y: float, text: str) -> None:
        glColor3f(0.9, 0.9, 0.95)
        glRasterPos2f(x, y)
        for char in text:
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, ord(char))

    def _draw_box(self, entity: BoxEntity, colliding: bool) -> None:
        base = entity.color
        if colliding:
            glColor3f(1.0, 0.2, 0.2)
        else:
            glColor3f(*base)

        glBegin(GL_QUADS)
        glVertex2f(entity.min_x(), entity.min_y())
        glVertex2f(entity.max_x(), entity.min_y())
        glVertex2f(entity.max_x(), entity.max_y())
        glVertex2f(entity.min_x(), entity.max_y())
        glEnd()

        if entity.selected:
            glColor3f(1.0, 1.0, 1.0)
            glBegin(GL_LINE_LOOP)
            glVertex2f(entity.min_x() - 0.01, entity.min_y() - 0.01)
            glVertex2f(entity.max_x() + 0.01, entity.min_y() - 0.01)
            glVertex2f(entity.max_x() + 0.01, entity.max_y() + 0.01)
            glVertex2f(entity.min_x() - 0.01, entity.max_y() + 0.01)
            glEnd()

    def render(self) -> None:
        glClear(GL_COLOR_BUFFER_BIT)
        glMatrixMode(GL_MODELVIEW)
        glLoadIdentity()

        colliding_indices = {idx for pair in self.colliding_pairs for idx in pair}
        for i, entity in enumerate(self.entities):
            self._draw_box(entity, i in colliding_indices)

        self._draw_text(-0.96, -0.94, f"colisoes={len(self.colliding_pairs)} | clique para selecionar | R reset")
        glutSwapBuffers()


demo = CollisionPickingDemo()


def configure_close_behavior() -> None:
    try:
        glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS)
    except Exception:
        pass


def display() -> None:
    demo.render()


def keyboard(key: bytes, _: int, __: int) -> None:
    if key in (b"r", b"R"):
        demo._build_entities()
    elif key == b"\x1b":
        raise SystemExit("ESC")


def mouse(button: int, state: int, x: int, y: int) -> None:
    if button != GLUT_LEFT_BUTTON or state != GLUT_DOWN:
        return

    width = glutGet(GLUT_WINDOW_WIDTH)
    height = glutGet(GLUT_WINDOW_HEIGHT)
    px = 2.0 * (x / width) - 1.0
    py = 1.0 - 2.0 * (y / height)
    demo.pick_at(px, py)
    glutPostRedisplay()


def tick(_: int) -> None:
    demo.update()
    glutPostRedisplay()
    glutTimerFunc(16, tick, 0)


def main() -> None:
    glutInit()
    configure_close_behavior()
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB)
    glutInitWindowSize(1200, 850)
    glutCreateWindow(b"Aula 09 - Picking e Colisoes AABB")
    glClearColor(0.04, 0.04, 0.07, 1.0)

    glutDisplayFunc(display)
    glutKeyboardFunc(keyboard)
    glutMouseFunc(mouse)
    glutTimerFunc(0, tick, 0)

    print("Controles: clique, R, ESC")
    glutMainLoop()


if __name__ == "__main__":
    main()
