"""Aula 10 — Sistema de partículas 2D com blending aditivo.

Exemplo avançado de VFX em OpenGL clássico com foco em arquitetura:
- Emissor configurável (taxa, espalhamento, gravidade, vida útil).
- Atualização estável de partículas com remoção eficiente.
- Render com blending aditivo para efeito de brilho.

Controles:
- ESPAÇO: pausa/retoma
- C: limpar partículas
- + / -: aumentar/reduzir taxa de emissão
- ESC: sair
"""

from __future__ import annotations

from dataclasses import dataclass
from random import Random

from OpenGL.GL import (
    GL_BLEND,
    GL_COLOR_BUFFER_BIT,
    GL_ONE,
    GL_ONE_MINUS_SRC_ALPHA,
    GL_POLYGON,
    GL_SRC_ALPHA,
    glBegin,
    glBlendFunc,
    glClear,
    glClearColor,
    glColor4f,
    glEnable,
    glEnd,
    glLoadIdentity,
    glRasterPos2f,
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
class Particle:
    """Partícula individual do sistema."""

    x: float
    y: float
    vx: float
    vy: float
    life: float
    max_life: float
    size: float


class ParticleEmitter:
    """Emissor de partículas com update e render desacoplados."""

    def __init__(self) -> None:
        self.rng = Random(7)
        self.particles: list[Particle] = []
        self.emit_rate = 35
        self.gravity = -0.0007
        self.paused = False

    def clear(self) -> None:
        """Remove todas as partículas ativas."""
        self.particles.clear()

    def _spawn_one(self) -> None:
        angle = self.rng.uniform(-0.8, 0.8)
        speed = self.rng.uniform(0.008, 0.018)
        vx = speed * angle
        vy = self.rng.uniform(0.010, 0.022)
        life = self.rng.uniform(0.8, 1.4)
        size = self.rng.uniform(0.010, 0.025)
        self.particles.append(Particle(0.0, -0.85, vx, vy, life, life, size))

    def update(self) -> None:
        """Atualiza estado físico e ciclo de vida das partículas."""
        if self.paused:
            return

        for _ in range(self.emit_rate):
            self._spawn_one()

        alive: list[Particle] = []
        for particle in self.particles:
            particle.vy += self.gravity
            particle.x += particle.vx
            particle.y += particle.vy
            particle.life -= 0.016
            if particle.life > 0.0:
                alive.append(particle)

        self.particles = alive

    def _draw_disc(self, x: float, y: float, radius: float, alpha: float) -> None:
        glColor4f(1.0, 0.55, 0.12, alpha)
        glBegin(GL_POLYGON)
        segments = 20
        from math import cos, sin, tau

        for i in range(segments):
            t = tau * i / segments
            glVertex2f(x + radius * cos(t), y + radius * sin(t))
        glEnd()

    def _draw_text(self, x: float, y: float, text: str) -> None:
        glColor4f(0.9, 0.9, 0.95, 1.0)
        glRasterPos2f(x, y)
        for char in text:
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, ord(char))

    def render(self) -> None:
        """Renderiza partículas com alpha baseado na vida restante."""
        glClear(GL_COLOR_BUFFER_BIT)
        glLoadIdentity()

        for particle in self.particles:
            alpha = max(0.0, min(1.0, particle.life / particle.max_life))
            self._draw_disc(particle.x, particle.y, particle.size, alpha)

        self._draw_text(-0.96, -0.94, f"particles={len(self.particles)} | rate={self.emit_rate} | paused={self.paused}")
        glutSwapBuffers()


emitter = ParticleEmitter()


def configure_close_behavior() -> None:
    try:
        glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS)
    except Exception:
        pass


def display() -> None:
    emitter.render()


def keyboard(key: bytes, _: int, __: int) -> None:
    if key == b" ":
        emitter.paused = not emitter.paused
    elif key in (b"c", b"C"):
        emitter.clear()
    elif key == b"+":
        emitter.emit_rate = min(emitter.emit_rate + 5, 120)
    elif key == b"-":
        emitter.emit_rate = max(emitter.emit_rate - 5, 5)
    elif key == b"\x1b":
        raise SystemExit("ESC")


def tick(_: int) -> None:
    emitter.update()
    glutPostRedisplay()
    glutTimerFunc(16, tick, 0)


def main() -> None:
    glutInit()
    configure_close_behavior()
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB)
    glutInitWindowSize(1200, 850)
    glutCreateWindow(b"Aula 10 - Sistema de Particulas com Blending")
    glClearColor(0.02, 0.02, 0.05, 1.0)

    glEnable(GL_BLEND)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE)

    glutDisplayFunc(display)
    glutKeyboardFunc(keyboard)
    glutTimerFunc(0, tick, 0)

    print("Controles: ESPACO, C, +, -, ESC")
    glutMainLoop()


if __name__ == "__main__":
    main()
