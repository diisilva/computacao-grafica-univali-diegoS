// Trabalho 1.1 - Funcoes Basicas 3D
// Autor: Diego Silva / Miruna Onofrei
// Objetivo: desenhar um cubo em wireframe (GL_LINES) e aplicar
// translacao, escala e rotacao manualmente em 3D, sem glTranslate,
// glRotate ou glScale.
//
// V2 EXPERIMENTAL (demonstracao visual):
// - Versao separada da entrega oficial.
// - Ao tocar/sair da borda em X/Y, o cubo nao atravessa imediatamente.
// - Primeiro ele gira 360 graus automaticamente.
// - So depois reaparece no lado oposto.
//
// Resumo didatico da arquitetura:
// 1) A geometria (vertices + arestas) fica na struct Poligono3D.
// 2) As transformacoes sao feitas manualmente sobre os vertices.
// 3) O desenho usa apenas GL_LINES (wireframe puro).
// 4) O callback de teclado aplica translacao, escala e rotacao.
// 5) O timer atualiza a transicao de borda com giro automatico de 360 graus.

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdlib>
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

struct LimitesTela {
    // Limites visiveis da janela no mundo para o Z atual do cubo.
    double min_x;
    double max_x;
    double min_y;
    double max_y;
    // Semi-extensoes atuais do cubo (considera escala e rotacao).
    double raio_x;
    double raio_y;
};

// Observacao importante de conformidade academica:
// - O trabalho permanece em wireframe com GL_LINES.
// - Nao ha preenchimento de faces.
// - Nao sao usadas transformacoes prontas de modelagem do OpenGL.

// Cria cubo com centro e tamanho de lado informados.
Poligono3D criar_cubo(double centro_x, double centro_y, double centro_z, double lado);
// Desenha as arestas do objeto com GL_LINES.
void desenhar(const Poligono3D& poligono);
// Aplica translacao em todos os vertices e no centro.
void movimentar(Poligono3D& poligono, double delta_x, double delta_y, double delta_z);
// Aplica escala por eixo usando o centro do objeto como referencia.
void escalar(Poligono3D& poligono, double escala_x, double escala_y, double escala_z);
// Aplica rotacao por eixo (x, y ou z) usando o centro como pivô.
void rotacionar(Poligono3D& poligono, double angulo, char eixo);
// Calcula os limites visiveis da tela no espaco de mundo para o cubo atual.
LimitesTela calcular_limites_tela(const Poligono3D& poligono);
// Detecta toque/saida de borda no plano XY.
bool detectar_saida_xy(const Poligono3D& poligono, char& direcao_saida);
// Inicia o estado de transicao de borda.
void iniciar_transicao_borda(char direcao_saida);
// Atualiza o giro automatico da transicao no timer.
void atualizar_transicao_borda();
// Reaparece no lado oposto ao concluir o giro.
void reaparecer_no_lado_oposto(Poligono3D& poligono, char direcao_saida);

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
// Dimensoes atuais da janela para calculo de aspecto.
int janela_largura = 900;
int janela_altura = 600;
// Campo de visao vertical usado na projecao perspectiva.
const double fov_y_graus = 60.0;
// Constante para comparacoes de ponto flutuante.
const double epsilon = 1e-9;

// Estado da transicao de borda (V2 experimental).
bool em_transicao_borda = false;
char direcao_transicao = 'n';
char eixo_rotacao_transicao = 'z';
double giro_acumulado_transicao = 0.0;
double passo_giro_transicao = 0.08;
double giro_total_transicao = 2.0 * M_PI;

int main(int argc, char** argv) {
    // Cubo inicial centrado no eixo Z negativo para ficar visivel em perspectiva.
    cubo = criar_cubo(0.0, 0.0, -6.0, 2.0);

    glutInit(&argc, argv);
    // Double buffer + RGB + depth buffer para visualizacao 3D correta.
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(900, 600);
    glutCreateWindow("Trabalho 1.1 - Funcoes Basicas 3D - V2 Experimental");

    // Fundo preto e teste de profundidade ativo.
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_DEPTH_TEST);
    glPointSize(3.0);

    // Registro de callbacks GLUT (loop orientado a eventos).
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

    // Desenho do estado atual do objeto (ja transformado manualmente).
    desenhar(cubo);

    // Troca de buffers para evitar flicker.
    glutSwapBuffers();
}

void reshape(GLsizei width, GLsizei height) {
    if (height == 0) {
        height = 1;
    }

    janela_largura = static_cast<int>(width);
    janela_altura = static_cast<int>(height);

    // aspecto = largura / altura, usado para evitar distorcao horizontal.
    GLfloat aspecto = static_cast<GLfloat>(width) / static_cast<GLfloat>(height);

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Projecao perspectiva para reforcar visual 3D.
    // Parametros: fov_y = 60 graus, near = 0.1, far = 100.
    gluPerspective(60.0f, aspecto, 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
    (void)x;
    (void)y;
    std::cout << "Tecla: " << key << "\n";

    // Mapeamento de controles:
    // - W/S/A/D : translacao no plano XY
    // - Q/E     : translacao no eixo Z
    // - +/-     : escala uniforme
    // - x,y,z   : rotacao positiva por eixo
    // - X,Y,Z   : rotacao negativa por eixo
    // - ESC     : encerra o programa
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
    (void)x;
    (void)y;
    std::cout << "Tecla especial: " << key << "\n";

    // Teclas especiais duplicam a translacao para facilitar uso continuo:
    // - Setas: movimento em X/Y
    // - PageUp/PageDown: movimento em Z
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
    (void)value;
    // Atualiza a animacao da transicao de borda no loop de timer.
    atualizar_transicao_borda();

    // Solicita novo quadro e reagenda o timer.
    // Isso mantem o loop de redesenho constante (aprox. 60 FPS).
    glutPostRedisplay();
    glutTimerFunc(delay, redraw, 0);
}

Poligono3D criar_cubo(double centro_x, double centro_y, double centro_z, double lado) {
    Poligono3D novo_poligono;

    novo_poligono.centro = { centro_x, centro_y, centro_z };
    novo_poligono.escala = { 1.0, 1.0, 1.0 };
    novo_poligono.rotacao = { 0.0, 0.0, 0.0 };

    // Usa metade do lado para posicionar os 8 vertices em torno do centro.
    // Assim, o centro geometrico do cubo coincide com (centro_x, centro_y, centro_z).
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
    // Enquanto estiver em transicao de borda, bloqueia translacao em X/Y
    // para manter o giro automatico estavel.
    if (em_transicao_borda && (std::abs(delta_x) > epsilon || std::abs(delta_y) > epsilon)) {
        return;
    }

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

    // Na V2, o wrap nao e imediato.
    // Ao tocar/sair da borda, inicia o estado de giro de 360 graus.
    if (!em_transicao_borda) {
        char direcao_saida = 'n';
        if (detectar_saida_xy(poligono, direcao_saida)) {
            iniciar_transicao_borda(direcao_saida);
        }
    }
}

void escalar(Poligono3D& poligono, double escala_x, double escala_y, double escala_z) {
    // Acumula escala logica aplicada.
    poligono.escala[0] *= escala_x;
    poligono.escala[1] *= escala_y;
    poligono.escala[2] *= escala_z;

    // Escala em torno do centro do poligono (pivô local).
    // Formula por vertice: v' = C + S * (v - C)
    // onde C = centro e S = matriz diagonal de escala (sx, sy, sz).
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

    // Rotacao em torno do centro do objeto.
    // Etapas por vertice: (1) leva para sistema local, (2) rotaciona,
    // (3) retorna para sistema global.
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
            // [y']   [ cos -sin ] [y]
            // [z'] = [ sin  cos ] [z]
            novo_y = y * cos_a - z * sin_a;
            novo_z = y * sin_a + z * cos_a;
        }

        if (eixo == 'y') {
            // Rotacao em torno do eixo Y (plano XZ).
            // [x']   [ cos  sin ] [x]
            // [z'] = [-sin  cos ] [z]
            novo_x = x * cos_a + z * sin_a;
            novo_z = -x * sin_a + z * cos_a;
        }

        if (eixo == 'z') {
            // Rotacao em torno do eixo Z (plano XY).
            // [x']   [ cos -sin ] [x]
            // [y'] = [ sin  cos ] [y]
            novo_x = x * cos_a - y * sin_a;
            novo_y = x * sin_a + y * cos_a;
        }

        // Volta para o sistema global.
        poligono.vertices[i][0] = novo_x + poligono.centro[0];
        poligono.vertices[i][1] = novo_y + poligono.centro[1];
        poligono.vertices[i][2] = novo_z + poligono.centro[2];
    }
}

void desenhar(const Poligono3D& poligono) {
    // Mantido wireframe puro com GL_LINES.
    // Observacao academica: nao ha preenchimento de faces.
    // Como alternativa compativel, as arestas recebem cores vibrantes.
    const float cores_arestas[12][3] = {
        {1.0f, 0.2f, 0.2f}, {1.0f, 0.5f, 0.2f}, {1.0f, 0.9f, 0.2f}, {0.5f, 1.0f, 0.2f},
        {0.2f, 1.0f, 0.2f}, {0.2f, 1.0f, 0.8f}, {0.2f, 0.8f, 1.0f}, {0.2f, 0.4f, 1.0f},
        {0.5f, 0.2f, 1.0f}, {0.9f, 0.2f, 1.0f}, {1.0f, 0.2f, 0.8f}, {1.0f, 0.2f, 0.5f}
    };

    // Desenha somente arestas (wireframe) sem preencher faces.
    // Cada iteracao emite exatamente dois vertices (origem/destino) de uma aresta.
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

LimitesTela calcular_limites_tela(const Poligono3D& poligono) {
    LimitesTela limites;

    if (janela_altura <= 0) {
        limites.min_x = -1.0;
        limites.max_x = 1.0;
        limites.min_y = -1.0;
        limites.max_y = 1.0;
        limites.raio_x = 0.0;
        limites.raio_y = 0.0;
        return limites;
    }

    double aspecto = static_cast<double>(janela_largura) / static_cast<double>(janela_altura);
    if (aspecto <= 0.0) {
        aspecto = 1.0;
    }

    double distancia_camera = -poligono.centro[2];
    if (distancia_camera < 0.2) {
        distancia_camera = 0.2;
    }

    double fov_y_rad = fov_y_graus * M_PI / 180.0;
    double metade_visivel_y = distancia_camera * tan(fov_y_rad / 2.0);
    double metade_visivel_x = metade_visivel_y * aspecto;

    double raio_x = 0.0;
    double raio_y = 0.0;
    for (int i = 0; i < static_cast<int>(poligono.vertices.size()); i++) {
        double dx = std::abs(poligono.vertices[i][0] - poligono.centro[0]);
        double dy = std::abs(poligono.vertices[i][1] - poligono.centro[1]);
        raio_x = std::max(raio_x, dx);
        raio_y = std::max(raio_y, dy);
    }

    limites.min_x = -metade_visivel_x;
    limites.max_x = metade_visivel_x;
    limites.min_y = -metade_visivel_y;
    limites.max_y = metade_visivel_y;
    limites.raio_x = raio_x;
    limites.raio_y = raio_y;
    return limites;
}

bool detectar_saida_xy(const Poligono3D& poligono, char& direcao_saida) {
    LimitesTela limites = calcular_limites_tela(poligono);

    if (poligono.centro[0] - limites.raio_x <= limites.min_x) {
        direcao_saida = 'L';
        return true;
    }
    if (poligono.centro[0] + limites.raio_x >= limites.max_x) {
        direcao_saida = 'R';
        return true;
    }
    if (poligono.centro[1] + limites.raio_y >= limites.max_y) {
        direcao_saida = 'U';
        return true;
    }
    if (poligono.centro[1] - limites.raio_y <= limites.min_y) {
        direcao_saida = 'D';
        return true;
    }

    direcao_saida = 'n';
    return false;
}

void iniciar_transicao_borda(char direcao_saida) {
    em_transicao_borda = true;
    direcao_transicao = direcao_saida;

    // Eixo Z foi escolhido por estabilidade visual no plano da tela.
    eixo_rotacao_transicao = 'z';

    giro_acumulado_transicao = 0.0;
}

void atualizar_transicao_borda() {
    if (!em_transicao_borda) {
        return;
    }

    double restante = giro_total_transicao - giro_acumulado_transicao;
    double angulo_passo = std::min(passo_giro_transicao, restante);

    rotacionar(cubo, angulo_passo, eixo_rotacao_transicao);
    giro_acumulado_transicao += angulo_passo;

    if (giro_acumulado_transicao + epsilon >= giro_total_transicao) {
        reaparecer_no_lado_oposto(cubo, direcao_transicao);

        em_transicao_borda = false;
        direcao_transicao = 'n';
        giro_acumulado_transicao = 0.0;
    }
}

void reaparecer_no_lado_oposto(Poligono3D& poligono, char direcao_saida) {
    LimitesTela limites = calcular_limites_tela(poligono);

    double destino_x = poligono.centro[0];
    double destino_y = poligono.centro[1];

    if (direcao_saida == 'L') {
        destino_x = limites.max_x - limites.raio_x;
    } else if (direcao_saida == 'R') {
        destino_x = limites.min_x + limites.raio_x;
    } else if (direcao_saida == 'U') {
        destino_y = limites.min_y + limites.raio_y;
    } else if (direcao_saida == 'D') {
        destino_y = limites.max_y - limites.raio_y;
    }

    double delta_x = destino_x - poligono.centro[0];
    double delta_y = destino_y - poligono.centro[1];

    poligono.centro[0] = destino_x;
    poligono.centro[1] = destino_y;

    for (int i = 0; i < static_cast<int>(poligono.vertices.size()); i++) {
        poligono.vertices[i][0] += delta_x;
        poligono.vertices[i][1] += delta_y;
    }
}
