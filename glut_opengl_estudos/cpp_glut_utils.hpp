#pragma once

// Utilitários comuns para as aulas C++ com GLUT/OpenGL (modo imediato).
// A ideia deste header é reduzir duplicação e deixar os exemplos principais
// mais fáceis de ler para quem está começando.

#include <GL/glut.h>
#include <algorithm>
#include <string>

namespace aula {

inline void configureCloseBehavior() {
    // Em algumas implementações (freeglut), esse modo faz o main loop retornar
    // quando a janela fecha, em vez de encerrar o processo abruptamente.
#ifdef GLUT_ACTION_ON_WINDOW_CLOSE
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
#endif
}

inline void drawBitmapText(float x, float y, const std::string& text, void* font = GLUT_BITMAP_HELVETICA_18) {
    glRasterPos2f(x, y);
    for (unsigned char ch : text) {
        glutBitmapCharacter(font, ch);
    }
}

template <typename T>
inline T clamp(T value, T low, T high) {
    return std::max(low, std::min(high, value));
}

} // namespace aula
