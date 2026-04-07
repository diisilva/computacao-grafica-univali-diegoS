// Trabalho 1.1 - Funcoes Basicas 3D
// Autor: Diego Silva / Miruna Onofrei
// Objetivo: desenhar um cubo em wireframe (GL_LINES) e aplicar
// translacao, escala e rotacao manualmente em 3D, sem glTranslate,
// glRotate ou glScale.

#include <iostream>
#include <vector>
#include <cmath>
#include <GL/freeglut.h>

// vertice3d: [x, y, z]
using vertice3d = std::vector<double>;
// lista de vertices do objeto
using lista_vertices = std::vector<vertice3d>;
// aresta: par de indices na lista de vertices
using aresta = std::pair<int, int>;
// lista de arestas para desenhar no modo wireframe
using lista_arestas = std::vector<aresta>;

struct Poligono3D {
    // Centro geometrico usado como pivô das transformacoes.
    std::vector<double> centro;
    // Estado acumulado de escala (controle logico).
    std::vector<double> escala;
    // Estado acumulado de rotacao por eixo (radianos).
    std::vector<double> rotacao;
    // Geometria atual do objeto apos transformacoes.
    lista_vertices vertices;
    // Conectividade entre vertices (wireframe).
    lista_arestas arestas;
};

// Cria cubo com centro e tamanho de lado informados.
Poligono3D criar_cubo(double centro_x, double centro_y, double centro_z, double lado);
// Desenha as arestas do objeto com GL_LINES.
void desenhar(Poligono3D poligono);
// Aplica translacao em todos os vertices e no centro.
void movimentar(Poligono3D& poligono, double delta_x, double delta_y, double delta_z);
// Aplica escala por eixo usando o centro do objeto como referencia.
void escalar(Poligono3D& poligono, double escala_x, double escala_y, double escala_z);
// Aplica rotacao por eixo (x, y ou z) usando o centro como pivô.
void rotacionar(Poligono3D& poligono, double angulo, char eixo);

// Callbacks GLUT.
void display();
void reshape(GLsizei width, GLsizei height);
void redraw(int value);
void keyboard(unsigned char key, int x, int y);
void keyboard_special(int key, int x, int y);

// Objeto global manipulado pelos eventos.
Poligono3D cubo;
// Intervalo de atualizacao da tela (aprox. 60 FPS).
int delay = 16;

int main(int argc, char** argv) {
    // Cubo inicial centrado no eixo Z negativo para ficar visivel em perspectiva.
    cubo = criar_cubo(0.0, 0.0, -6.0, 2.0);

    glutInit(&argc, argv);
    // Double buffer + RGB + depth buffer para visualizacao 3D correta.
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(900, 600);
    glutCreateWindow("Trabalho 1.1 - Funcoes Basicas 3D");

    // Fundo branco e teste de profundidade ativo.
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_DEPTH_TEST);
    glPointSize(3.0);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(keyboard_special);
    glutTimerFunc(delay, redraw, 0);

    glutMainLoop();
    return 0;
}

void display() {
    // Limpa tela e profundidade a cada quadro.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    // Sem transformacoes prontas de modelagem: apenas identidade.
    glLoadIdentity();

    desenhar(cubo);

    // Troca de buffers para evitar flicker.
    glutSwapBuffers();
}

void reshape(GLsizei width, GLsizei height) {
    if (height == 0) {
        height = 1;
    }

    GLfloat aspecto = static_cast<GLfloat>(width) / static_cast<GLfloat>(height);

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Projecao perspectiva para reforcar visual 3D.
    gluPerspective(60.0f, aspecto, 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
    std::cout << "Tecla: " << key << "\n";
    switch (key) {
    // ESC encerra o programa.
    case 27:
        exit(0);
        break;

    // Translacao no plano XY e profundidade no eixo Z.
    case 'w':
        movimentar(cubo, 0.0, 0.1, 0.0);
        break;
    case 's':
        movimentar(cubo, 0.0, -0.1, 0.0);
        break;
    case 'a':
        movimentar(cubo, -0.1, 0.0, 0.0);
        break;
    case 'd':
        movimentar(cubo, 0.1, 0.0, 0.0);
        break;
    case 'q':
        movimentar(cubo, 0.0, 0.0, -0.1);
        break;
    case 'e':
        movimentar(cubo, 0.0, 0.0, 0.1);
        break;

    // Escala uniforme.
    case '+':
    case '=':
        escalar(cubo, 1.1, 1.1, 1.1);
        break;
    case '-':
    case '_':
        escalar(cubo, 0.9, 0.9, 0.9);
        break;

    // Rotacoes por eixo (minuscula: sentido positivo, maiuscula: negativo).
    case 'x':
        rotacionar(cubo, 0.08, 'x');
        break;
    case 'X':
        rotacionar(cubo, -0.08, 'x');
        break;
    case 'y':
        rotacionar(cubo, 0.08, 'y');
        break;
    case 'Y':
        rotacionar(cubo, -0.08, 'y');
        break;
    case 'z':
        rotacionar(cubo, 0.08, 'z');
        break;
    case 'Z':
        rotacionar(cubo, -0.08, 'z');
        break;
    }
}

void keyboard_special(int key, int x, int y) {
    std::cout << "Tecla especial: " << key << "\n";
    switch (key) {
    // Duplicacao de translacao para facilitar navegacao com setas.
    case GLUT_KEY_UP:
        movimentar(cubo, 0.0, 0.1, 0.0);
        break;
    case GLUT_KEY_DOWN:
        movimentar(cubo, 0.0, -0.1, 0.0);
        break;
    case GLUT_KEY_LEFT:
        movimentar(cubo, -0.1, 0.0, 0.0);
        break;
    case GLUT_KEY_RIGHT:
        movimentar(cubo, 0.1, 0.0, 0.0);
        break;
    case GLUT_KEY_PAGE_UP:
        movimentar(cubo, 0.0, 0.0, -0.1);
        break;
    case GLUT_KEY_PAGE_DOWN:
        movimentar(cubo, 0.0, 0.0, 0.1);
        break;
    }
}

void redraw(int value) {
    // Solicita novo quadro e reagenda o timer.
    glutPostRedisplay();
    glutTimerFunc(delay, redraw, 0);
}

Poligono3D criar_cubo(double centro_x, double centro_y, double centro_z, double lado) {
    Poligono3D novo_poligono;

    novo_poligono.centro = { centro_x, centro_y, centro_z };
    novo_poligono.escala = { 1.0, 1.0, 1.0 };
    novo_poligono.rotacao = { 0.0, 0.0, 0.0 };

    double metade = lado / 2.0;

    // Vertices descritos diretamente (conforme pedido no enunciado).
    novo_poligono.vertices = {
        { centro_x - metade, centro_y - metade, centro_z - metade }, // 0
        { centro_x + metade, centro_y - metade, centro_z - metade }, // 1
        { centro_x + metade, centro_y + metade, centro_z - metade }, // 2
        { centro_x - metade, centro_y + metade, centro_z - metade }, // 3
        { centro_x - metade, centro_y - metade, centro_z + metade }, // 4
        { centro_x + metade, centro_y - metade, centro_z + metade }, // 5
        { centro_x + metade, centro_y + metade, centro_z + metade }, // 6
        { centro_x - metade, centro_y + metade, centro_z + metade }  // 7
    };

    // Arestas do cubo em wireframe: base, topo e ligacoes verticais.
    novo_poligono.arestas = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        {4, 5}, {5, 6}, {6, 7}, {7, 4},
        {0, 4}, {1, 5}, {2, 6}, {3, 7}
    };

    return novo_poligono;
}

void movimentar(Poligono3D& poligono, double delta_x, double delta_y, double delta_z) {
    // Atualiza o centro para manter consistencia com as proximas transformacoes.
    poligono.centro[0] += delta_x;
    poligono.centro[1] += delta_y;
    poligono.centro[2] += delta_z;

    // Move todos os vertices com o mesmo deslocamento.
    for (int i = 0; i < static_cast<int>(poligono.vertices.size()); i++) {
        poligono.vertices[i][0] += delta_x;
        poligono.vertices[i][1] += delta_y;
        poligono.vertices[i][2] += delta_z;
    }
}

void escalar(Poligono3D& poligono, double escala_x, double escala_y, double escala_z) {
    // Acumula escala logica aplicada.
    poligono.escala[0] *= escala_x;
    poligono.escala[1] *= escala_y;
    poligono.escala[2] *= escala_z;

    for (int i = 0; i < static_cast<int>(poligono.vertices.size()); i++) {
        // 1) Leva o vertice para o sistema local (centro na origem).
        poligono.vertices[i][0] -= poligono.centro[0];
        poligono.vertices[i][1] -= poligono.centro[1];
        poligono.vertices[i][2] -= poligono.centro[2];

        // 2) Aplica escala por eixo.
        poligono.vertices[i][0] *= escala_x;
        poligono.vertices[i][1] *= escala_y;
        poligono.vertices[i][2] *= escala_z;

        // 3) Retorna para o sistema global.
        poligono.vertices[i][0] += poligono.centro[0];
        poligono.vertices[i][1] += poligono.centro[1];
        poligono.vertices[i][2] += poligono.centro[2];
    }
}

void rotacionar(Poligono3D& poligono, double angulo, char eixo) {
    // cos/sin pre-calculados para reutilizacao no loop.
    double cos_a = cos(angulo);
    double sin_a = sin(angulo);

    if (eixo == 'x') {
        poligono.rotacao[0] += angulo;
    }
    if (eixo == 'y') {
        poligono.rotacao[1] += angulo;
    }
    if (eixo == 'z') {
        poligono.rotacao[2] += angulo;
    }

    for (int i = 0; i < static_cast<int>(poligono.vertices.size()); i++) {
        // Transfere o vertice para o sistema local do objeto.
        double x = poligono.vertices[i][0] - poligono.centro[0];
        double y = poligono.vertices[i][1] - poligono.centro[1];
        double z = poligono.vertices[i][2] - poligono.centro[2];

        double novo_x = x;
        double novo_y = y;
        double novo_z = z;

        if (eixo == 'x') {
            // Rotacao em torno do eixo X (plano YZ).
            novo_y = y * cos_a - z * sin_a;
            novo_z = y * sin_a + z * cos_a;
        }

        if (eixo == 'y') {
            // Rotacao em torno do eixo Y (plano XZ).
            novo_x = x * cos_a + z * sin_a;
            novo_z = -x * sin_a + z * cos_a;
        }

        if (eixo == 'z') {
            // Rotacao em torno do eixo Z (plano XY).
            novo_x = x * cos_a - y * sin_a;
            novo_y = x * sin_a + y * cos_a;
        }

        // Volta para o sistema global.
        poligono.vertices[i][0] = novo_x + poligono.centro[0];
        poligono.vertices[i][1] = novo_y + poligono.centro[1];
        poligono.vertices[i][2] = novo_z + poligono.centro[2];
    }
}

void desenhar(Poligono3D poligono) {
    // Preto para destacar arestas no fundo branco.
    glColor3f(0.0, 0.0, 0.0);

    // Desenha somente arestas (wireframe) sem preencher faces.
    glBegin(GL_LINES);
    for (int i = 0; i < static_cast<int>(poligono.arestas.size()); i++) {
        int v_origem = poligono.arestas[i].first;
        int v_destino = poligono.arestas[i].second;

        glVertex3f(
            static_cast<float>(poligono.vertices[v_origem][0]),
            static_cast<float>(poligono.vertices[v_origem][1]),
            static_cast<float>(poligono.vertices[v_origem][2])
        );

        glVertex3f(
            static_cast<float>(poligono.vertices[v_destino][0]),
            static_cast<float>(poligono.vertices[v_destino][1]),
            static_cast<float>(poligono.vertices[v_destino][2])
        );
    }
    glEnd();
}
