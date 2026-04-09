// Trabalho 1.1 - Funcoes Basicas 3D
// Autor: Diego Silva / Miruna Onofrei
// Objetivo: modelar um cubo 3D em wireframe e aplicar transformacoes
// geometricas diretamente nos vertices (translacao, escala e rotacao).
// O OpenGL legado e usado para projecao e rasterizacao, sem chamadas
// de modelagem como glTranslate/glRotate/glScale.

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <GL/freeglut.h>

// vertice3d representa um ponto no espaco cartesiano: [x, y, z].
using vertice3d = std::vector<double>;
// Lista de vertices da malha.
using lista_vertices = std::vector<vertice3d>;
// Aresta definida por dois indices de vertices.
using aresta = std::pair<int, int>;
// Conjunto de arestas usado no desenho wireframe.
using lista_arestas = std::vector<aresta>;

struct Poligono3D {
    // Centro geometrico adotado como pivô das transformacoes.
    std::vector<double> centro;
    // Escala acumulada por eixo (controle de estado).
    std::vector<double> escala;
    // Rotacao acumulada por eixo, em radianos.
    std::vector<double> rotacao;
    // Geometria corrente apos transformacoes incrementais.
    lista_vertices vertices;
    // Conectividade entre vertices para desenhar arestas.
    lista_arestas arestas;
};

// Observacoes de implementacao:
// - A modelagem e feita por vertices e arestas (sem faces preenchidas).
// - O desenho final usa GL_LINES.
// - As transformacoes geometricas sao calculadas no proprio codigo.

// Cria um cubo com centro e lado especificados.
Poligono3D criar_cubo(double centro_x, double centro_y, double centro_z, double lado);
// Desenha as arestas do objeto no modo GL_LINES.
void desenhar(const Poligono3D& poligono);
// Aplica translacao ao centro e a todos os vertices.
void movimentar(Poligono3D& poligono, double delta_x, double delta_y, double delta_z);
// Aplica escala por eixo em torno do centro geometrico.
void escalar(Poligono3D& poligono, double escala_x, double escala_y, double escala_z);
// Aplica rotacao em torno de um eixo (x, y ou z) usando o centro como pivô.
void rotacionar(Poligono3D& poligono, double angulo, char eixo);
// Wrap-around no plano XY considerando projecao perspectiva e tamanho projetado.
void aplicar_wraparound_xy(Poligono3D& poligono);

// Callbacks da aplicacao GLUT.
void display();
void reshape(GLsizei width, GLsizei height);
void redraw(int value);
void keyboard(unsigned char key, int x, int y);
void keyboard_special(int key, int x, int y);

// Objeto global manipulado pelos eventos de entrada.
Poligono3D cubo;
// Intervalo do timer (aprox. 60 FPS).
int delay = 16;
// Dimensoes correntes da janela para calculo de aspecto.
int janela_largura = 900;
int janela_altura = 600;
// Campo de visao vertical da projecao perspectiva.
const double fov_y_graus = 60.0;

int main(int argc, char** argv) {
    // Cubo inicial posicionado no semiespaco de z negativo para ficar visivel.
    cubo = criar_cubo(0.0, 0.0, -6.0, 2.0);

    glutInit(&argc, argv);
    // Double buffer + RGB + depth buffer para visualizacao 3D.
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(900, 600);
    glutCreateWindow("Trabalho 1.1 - Funcoes Basicas 3D");

    // Parametros de estado grafico.
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_DEPTH_TEST);
    glPointSize(3.0);

    // Registro de callbacks (modelo orientado a eventos).
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(keyboard_special);
    glutTimerFunc(delay, redraw, 0);

    glutMainLoop();
    return 0;
}

void display() {
    // Limpa buffers de cor e profundidade a cada quadro.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    // Matriz modelview neutra; vertices ja estao no estado transformado.
    glLoadIdentity();

    // Desenha o estado corrente da geometria.
    desenhar(cubo);

    // Troca de buffers para exibicao suave.
    glutSwapBuffers();
}

void reshape(GLsizei width, GLsizei height) {
    if (height == 0) {
        height = 1;
    }

    janela_largura = static_cast<int>(width);
    janela_altura = static_cast<int>(height);

    // aspecto = largura / altura, necessario para evitar distorcao.
    GLfloat aspecto = static_cast<GLfloat>(width) / static_cast<GLfloat>(height);

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Projecao perspectiva: fov_y = 60 graus, near = 0.1, far = 100.
    gluPerspective(60.0f, aspecto, 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
    (void)x;
    (void)y;
    std::cout << "Tecla: " << key << "\n";

    // Mapeamento dos controles:
    // - W/S/A/D : translacao no plano XY
    // - Q/E     : translacao no eixo Z
    // - +/-     : escala uniforme
    // - x,y,z   : rotacao positiva por eixo
    // - X,Y,Z   : rotacao negativa por eixo
    // - ESC     : encerra o programa
    switch (key) {
    // ESC: encerra o programa.
    case 27:
        exit(0);
        break;

    // Translacao no plano XY e ao longo de Z.
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

    // Rotacoes por eixo (minuscula: positiva, maiuscula: negativa).
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
    (void)x;
    (void)y;
    std::cout << "Tecla especial: " << key << "\n";

    // Teclas especiais para translacao alternativa:
    // - Setas: movimento em X/Y
    // - PageUp/PageDown: movimento em Z
    switch (key) {
    // Translacao com teclas especiais.
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
    (void)value;
    // Solicita novo quadro e reagenda o timer para taxa de atualizacao constante.
    glutPostRedisplay();
    glutTimerFunc(delay, redraw, 0);
}

Poligono3D criar_cubo(double centro_x, double centro_y, double centro_z, double lado) {
    Poligono3D novo_poligono;

    novo_poligono.centro = { centro_x, centro_y, centro_z };
    novo_poligono.escala = { 1.0, 1.0, 1.0 };
    novo_poligono.rotacao = { 0.0, 0.0, 0.0 };

    // Metade do lado define deslocamentos simetricos em torno do centro.
    double metade = lado / 2.0;

    // Vertices do cubo em ordem consistente com as arestas definidas abaixo.
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

    // Arestas do cubo: base, topo e conexoes verticais.
    novo_poligono.arestas = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        {4, 5}, {5, 6}, {6, 7}, {7, 4},
        {0, 4}, {1, 5}, {2, 6}, {3, 7}
    };

    return novo_poligono;
}

void movimentar(Poligono3D& poligono, double delta_x, double delta_y, double delta_z) {
    // Atualiza o centro, que e o pivô das transformacoes seguintes.
    poligono.centro[0] += delta_x;
    poligono.centro[1] += delta_y;
    poligono.centro[2] += delta_z;

    // Aplica o mesmo deslocamento a todos os vertices.
    for (int i = 0; i < static_cast<int>(poligono.vertices.size()); i++) {
        poligono.vertices[i][0] += delta_x;
        poligono.vertices[i][1] += delta_y;
        poligono.vertices[i][2] += delta_z;
    }

    // Wrap-around no plano XY quando o objeto sai da area visivel.
    aplicar_wraparound_xy(poligono);
}

void escalar(Poligono3D& poligono, double escala_x, double escala_y, double escala_z) {
    // Atualiza o estado acumulado de escala por eixo.
    poligono.escala[0] *= escala_x;
    poligono.escala[1] *= escala_y;
    poligono.escala[2] *= escala_z;

    // Escala em torno do centro C: v' = C + S * (v - C).
    for (int i = 0; i < static_cast<int>(poligono.vertices.size()); i++) {
        // 1) Converte para referencial local do objeto.
        poligono.vertices[i][0] -= poligono.centro[0];
        poligono.vertices[i][1] -= poligono.centro[1];
        poligono.vertices[i][2] -= poligono.centro[2];

        // 2) Aplica escala anisotropica por eixo.
        poligono.vertices[i][0] *= escala_x;
        poligono.vertices[i][1] *= escala_y;
        poligono.vertices[i][2] *= escala_z;

        // 3) Retorna ao referencial global.
        poligono.vertices[i][0] += poligono.centro[0];
        poligono.vertices[i][1] += poligono.centro[1];
        poligono.vertices[i][2] += poligono.centro[2];
    }
}

void rotacionar(Poligono3D& poligono, double angulo, char eixo) {
    // Trigonometria pre-calculada para evitar recomputacao por vertice.
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

    // Rotacao incremental em torno do centro do objeto.
    // Etapas: local -> rotacao no eixo escolhido -> global.
    for (int i = 0; i < static_cast<int>(poligono.vertices.size()); i++) {
        // Coordenadas no referencial local (centro na origem).
        double x = poligono.vertices[i][0] - poligono.centro[0];
        double y = poligono.vertices[i][1] - poligono.centro[1];
        double z = poligono.vertices[i][2] - poligono.centro[2];

        double novo_x = x;
        double novo_y = y;
        double novo_z = z;

        if (eixo == 'x') {
            // Rotacao no plano YZ (eixo X).
            // [y']   [ cos -sin ] [y]
            // [z'] = [ sin  cos ] [z]
            novo_y = y * cos_a - z * sin_a;
            novo_z = y * sin_a + z * cos_a;
        }

        if (eixo == 'y') {
            // Rotacao no plano XZ (eixo Y).
            // [x']   [ cos  sin ] [x]
            // [z'] = [-sin  cos ] [z]
            novo_x = x * cos_a + z * sin_a;
            novo_z = -x * sin_a + z * cos_a;
        }

        if (eixo == 'z') {
            // Rotacao no plano XY (eixo Z).
            // [x']   [ cos -sin ] [x]
            // [y'] = [ sin  cos ] [y]
            novo_x = x * cos_a - y * sin_a;
            novo_y = x * sin_a + y * cos_a;
        }

        // Reconverte para o referencial global.
        poligono.vertices[i][0] = novo_x + poligono.centro[0];
        poligono.vertices[i][1] = novo_y + poligono.centro[1];
        poligono.vertices[i][2] = novo_z + poligono.centro[2];
    }
}

void desenhar(const Poligono3D& poligono) {
    // Paleta fixa para facilitar leitura espacial das arestas durante a interacao.
    const float cores_arestas[12][3] = {
        {1.0f, 0.2f, 0.2f}, {1.0f, 0.5f, 0.2f}, {1.0f, 0.9f, 0.2f}, {0.5f, 1.0f, 0.2f},
        {0.2f, 1.0f, 0.2f}, {0.2f, 1.0f, 0.8f}, {0.2f, 0.8f, 1.0f}, {0.2f, 0.4f, 1.0f},
        {0.5f, 0.2f, 1.0f}, {0.9f, 0.2f, 1.0f}, {1.0f, 0.2f, 0.8f}, {1.0f, 0.2f, 0.5f}
    };

    // Wireframe puro: cada aresta e emitida como um segmento GL_LINES.
    glBegin(GL_LINES);
    for (int i = 0; i < static_cast<int>(poligono.arestas.size()); i++) {
        int v_origem = poligono.arestas[i].first;
        int v_destino = poligono.arestas[i].second;

        glColor3f(cores_arestas[i][0], cores_arestas[i][1], cores_arestas[i][2]);

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

void aplicar_wraparound_xy(Poligono3D& poligono) {
    if (janela_altura <= 0) {
        return;
    }

    double aspecto = static_cast<double>(janela_largura) / static_cast<double>(janela_altura);
    if (aspecto <= 0.0) {
        aspecto = 1.0;
    }

    // Distancia ao observador (camera na origem, olhando para -Z).
    // Para objetos visiveis no semiespaco negativo, distancia = -z do centro.
    double distancia_camera = -poligono.centro[2];
    if (distancia_camera < 0.2) {
        distancia_camera = 0.2;
    }

    // Converte FOV para radianos para uso em tan().
    double fov_y_rad = fov_y_graus * M_PI / 180.0;
    double metade_visivel_y = distancia_camera * tan(fov_y_rad / 2.0);
    double metade_visivel_x = metade_visivel_y * aspecto;

    // Semi-extensoes projetadas no plano XY, estimadas por vertices atuais.
    // Assim o limite acompanha escala e rotacao aplicadas ao cubo.
    double raio_x = 0.0;
    double raio_y = 0.0;
    for (int i = 0; i < static_cast<int>(poligono.vertices.size()); i++) {
        double dx = std::abs(poligono.vertices[i][0] - poligono.centro[0]);
        double dy = std::abs(poligono.vertices[i][1] - poligono.centro[1]);
        raio_x = std::max(raio_x, dx);
        raio_y = std::max(raio_y, dy);
    }

    // Limites de saida total, com margem equivalente ao tamanho do objeto.
    double limite_esq = -metade_visivel_x - raio_x;
    double limite_dir = metade_visivel_x + raio_x;
    double limite_inf = -metade_visivel_y - raio_y;
    double limite_sup = metade_visivel_y + raio_y;

    double delta_x_wrap = 0.0;
    double delta_y_wrap = 0.0;

    // Calcula deslocamento de teleporte entre bordas opostas.
    if (poligono.centro[0] < limite_esq) {
        delta_x_wrap = (limite_dir - limite_esq);
    } else if (poligono.centro[0] > limite_dir) {
        delta_x_wrap = -(limite_dir - limite_esq);
    }

    if (poligono.centro[1] < limite_inf) {
        delta_y_wrap = (limite_sup - limite_inf);
    } else if (poligono.centro[1] > limite_sup) {
        delta_y_wrap = -(limite_sup - limite_inf);
    }

    if (delta_x_wrap == 0.0 && delta_y_wrap == 0.0) {
        return;
    }

    // Aplica o mesmo deslocamento ao centro e aos vertices,
    // preservando forma, escala e orientacao atuais.
    poligono.centro[0] += delta_x_wrap;
    poligono.centro[1] += delta_y_wrap;

    for (int i = 0; i < static_cast<int>(poligono.vertices.size()); i++) {
        poligono.vertices[i][0] += delta_x_wrap;
        poligono.vertices[i][1] += delta_y_wrap;
    }
}
