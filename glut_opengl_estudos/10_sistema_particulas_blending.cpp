/*
Aula 10 — Sistema de Partículas com Blending Aditivo (C++)

Conceitos:
- Emissão, atualização e descarte de partículas por tempo de vida.
- Blending aditivo (`GL_SRC_ALPHA`, `GL_ONE`) para efeito de brilho.
- Organização orientada a dados para VFX simples.
*/

#include <GL/glut.h>
#include <cmath>
#include <random>
#include <vector>
#include "cpp_glut_utils.hpp"

namespace {

struct Particle {
    float x;
    float y;
    float vx;
    float vy;
    float life;
    float maxLife;
    float size;
};

class ParticleEmitter {
public:
    void update() {
        if (paused) return;

        for (int i = 0; i < emitRate; ++i) spawnOne();

        std::vector<Particle> alive;
        alive.reserve(particles.size());

        for (auto p : particles) {
            p.vy += gravity;
            p.x += p.vx;
            p.y += p.vy;
            p.life -= 0.016f;
            if (p.life > 0.0f) alive.push_back(p);
        }

        particles.swap(alive);
    }

    void render() const {
        glClear(GL_COLOR_BUFFER_BIT);
        glLoadIdentity();

        for (const auto& p : particles) {
            const float alpha = aula::clamp(p.life / p.maxLife, 0.0f, 1.0f);
            drawDisc(p.x, p.y, p.size, alpha);
        }

        glutSwapBuffers();
    }

    void clear() { particles.clear(); }

    int emitRate = 35;
    float gravity = -0.0007f;
    bool paused = false;

private:
    mutable std::mt19937 rng {7};
    std::vector<Particle> particles;

    float randomRange(float lo, float hi) {
        std::uniform_real_distribution<float> dist(lo, hi);
        return dist(rng);
    }

    void spawnOne() {
        const float angle = randomRange(-0.8f, 0.8f);
        const float speed = randomRange(0.008f, 0.018f);

        Particle p;
        p.x = 0.0f;
        p.y = -0.85f;
        p.vx = speed * angle;
        p.vy = randomRange(0.010f, 0.022f);
        p.life = randomRange(0.8f, 1.4f);
        p.maxLife = p.life;
        p.size = randomRange(0.010f, 0.025f);

        particles.push_back(p);
    }

    void drawDisc(float x, float y, float r, float alpha) const {
        glColor4f(1.0f, 0.55f, 0.12f, alpha);
        glBegin(GL_POLYGON);
        for (int i = 0; i < 20; ++i) {
            const float t = 2.0f * 3.1415926f * static_cast<float>(i) / 20.0f;
            glVertex2f(x + r * std::cos(t), y + r * std::sin(t));
        }
        glEnd();
    }
};

ParticleEmitter emitter;

void display() { emitter.render(); }

void keyboard(unsigned char key, int, int) {
    if (key == ' ') emitter.paused = !emitter.paused;
    else if (key == 'c' || key == 'C') emitter.clear();
    else if (key == '+') emitter.emitRate = aula::clamp(emitter.emitRate + 5, 5, 120);
    else if (key == '-') emitter.emitRate = aula::clamp(emitter.emitRate - 5, 5, 120);
    else if (key == 27) std::exit(0);
}

void tick(int) {
    emitter.update();
    glutPostRedisplay();
    glutTimerFunc(16, tick, 0);
}

} // namespace

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    aula::configureCloseBehavior();
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1200, 850);
    glutCreateWindow("Aula 10 - Sistema de Particulas (C++)");

    glClearColor(0.02f, 0.02f, 0.05f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, tick, 0);
    glutMainLoop();
    return 0;
}
