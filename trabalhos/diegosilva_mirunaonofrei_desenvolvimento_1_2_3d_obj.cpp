// M1.2 - 3D e Arquivos .obj
// Diego Silva / Miruna Onofrei
//
// Carrega .obj (v, vt, vn, f) e renderiza com 3 luzes independentes.
//
// Controles:
//   WASD / QE        - translacao
//   x/X  y/Y  z/Z    - rotacao por eixo
//   +/-              - escala
//   1 / 2 / 3        - toggle luzes
//   R                - reset  |  ESC - sair
//   arrastar (L.btn) - orbitar

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <array>
#include <string>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <GL/freeglut.h>

// -- structs do modelo OBJ --------------------------------------------------

// indices v/vt/vn de um vertice de face; -1 quando ausente
struct IndiceVertice {
    int iv  = -1;
    int ivt = -1;
    int ivn = -1;
};

struct Face {
    IndiceVertice v[3];
};

struct ModeloObj {
    std::vector<std::array<float, 3>> vertices;
    std::vector<std::array<float, 2>> texcoords;
    std::vector<std::array<float, 3>> normais;
    std::vector<Face>                 faces;

    GLuint display_list = 0;

    // calculados em normalizar_modelo() para auto-enquadramento
    std::array<float, 3> centro_aabb = { 0.0f, 0.0f, 0.0f };
    float escala_normalizacao = 1.0f;
};

struct Luz {
    GLenum  id;
    bool    ativa;
    GLfloat ambiente[4];
    GLfloat difusa[4];
    GLfloat especular[4];
    GLfloat posicao[4]; // w=1: pontual, w=0: direcional
};

struct EstadoCena {
    float rot_x   = 0.0f;
    float rot_y   = 0.0f;
    float rot_z   = 0.0f;
    float escala  = 1.0f;
    float trans_x = 0.0f;
    float trans_y = 0.0f;
    float trans_z = 0.0f;
};

ModeloObj modelo;
EstadoCena cena;

// key light (branca), fill light (vermelha), rim light (azul)
Luz luzes[3] = {
    {
        GL_LIGHT0, true,
        { 0.20f, 0.20f, 0.20f, 1.0f },  // ambiente
        { 1.00f, 1.00f, 1.00f, 1.0f },  // difusa
        { 1.00f, 1.00f, 1.00f, 1.0f },  // especular
        { 4.0f,  8.0f,  6.0f,  1.0f }
    },
    {
        GL_LIGHT1, true,
        { 0.10f, 0.04f, 0.04f, 1.0f },
        { 0.80f, 0.20f, 0.15f, 1.0f },
        { 0.60f, 0.10f, 0.10f, 1.0f },
        { -7.0f, 2.0f,  4.0f,  1.0f }
    },
    {
        GL_LIGHT2, true,
        { 0.04f, 0.04f, 0.12f, 1.0f },
        { 0.15f, 0.25f, 0.90f, 1.0f },
        { 0.10f, 0.15f, 0.80f, 1.0f },
        { 0.0f, -4.0f, -10.0f, 1.0f }
    }
};

bool arrastando        = false;
int  mouse_x_anterior  = 0;
int  mouse_y_anterior  = 0;

int janela_largura = 900;
int janela_altura  = 600;
const int delay_ms = 16; // ~60 fps

// -- prototipos --------------------------------------------------------------
IndiceVertice        parse_indice(const std::string& token);
std::array<float, 3> calcular_normal_face(const std::array<float, 3>& a,
                                          const std::array<float, 3>& b,
                                          const std::array<float, 3>& c);
void carregar_obj(const std::string& caminho);
void normalizar_modelo();
void compilar_display_list();
void configurar_luzes();
void desenhar_hud();
void display();
void reshape(GLsizei w, GLsizei h);
void redraw(int valor);
void keyboard(unsigned char key, int x, int y);
void keyboard_special(int key, int x, int y);
void mouse_botao(int button, int state, int x, int y);
void mouse_movimento(int x, int y);

// parse token de face: v / v/vt / v//vn / v/vt/vn  (1-based -> 0-based)
IndiceVertice parse_indice(const std::string& token) {
    IndiceVertice idx;
    std::istringstream ss(token);
    std::string parte;
    int coluna = 0;
    while (std::getline(ss, parte, '/')) {
        if (!parte.empty()) {
            int val = std::stoi(parte) - 1;
            if (coluna == 0) idx.iv  = val;
            if (coluna == 1) idx.ivt = val;
            if (coluna == 2) idx.ivn = val;
        }
        ++coluna;
    }
    return idx;
}

// produto vetorial ab x ac normalizado; fallback quando vn nao existe
std::array<float, 3> calcular_normal_face(const std::array<float, 3>& a,
                                          const std::array<float, 3>& b,
                                          const std::array<float, 3>& c) {
    float ab[3] = { b[0] - a[0], b[1] - a[1], b[2] - a[2] };
    float ac[3] = { c[0] - a[0], c[1] - a[1], c[2] - a[2] };
    std::array<float, 3> n = {
        ab[1] * ac[2] - ab[2] * ac[1],
        ab[2] * ac[0] - ab[0] * ac[2],
        ab[0] * ac[1] - ab[1] * ac[0]
    };
    float len = std::sqrt(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
    if (len > 1e-6f) {
        n[0] /= len;
        n[1] /= len;
        n[2] /= len;
    }
    return n;
}

// le v/vt/vn/f do .obj; faces com N>3 vertices sao trianguladas por fan
void carregar_obj(const std::string& caminho) {
    std::ifstream arquivo(caminho);
    if (!arquivo.is_open()) {
        std::cerr << "Erro: nao foi possivel abrir o arquivo: " << caminho << "\n";
        std::exit(1);
    }

    std::string linha;
    while (std::getline(arquivo, linha)) {
        if (linha.empty() || linha[0] == '#') {
            continue;
        }

        std::istringstream ss(linha);
        std::string tipo;
        ss >> tipo;

        if (tipo == "v") {
            std::array<float, 3> v;
            ss >> v[0] >> v[1] >> v[2];
            modelo.vertices.push_back(v);
        }
        else if (tipo == "vt") {
            std::array<float, 2> vt = { 0.0f, 0.0f };
            ss >> vt[0] >> vt[1];
            modelo.texcoords.push_back(vt);
        }
        else if (tipo == "vn") {
            std::array<float, 3> vn;
            ss >> vn[0] >> vn[1] >> vn[2];
            modelo.normais.push_back(vn);
        }
        else if (tipo == "f") {
            std::vector<IndiceVertice> verts;
            std::string token;
            while (ss >> token) {
                verts.push_back(parse_indice(token));
            }
            // fan: [0,i,i+1] para i = 1..N-2
            for (int i = 1; i + 1 < static_cast<int>(verts.size()); ++i) {
                Face face;
                face.v[0] = verts[0];
                face.v[1] = verts[i];
                face.v[2] = verts[i + 1];
                modelo.faces.push_back(face);
            }
        }
        // mtllib, usemtl, g, o, s: ignorados
    }

    arquivo.close();

    std::cout << "OBJ carregado: "
              << modelo.vertices.size()  << " vertices, "
              << modelo.normais.size()   << " normais, "
              << modelo.texcoords.size() << " texcoords, "
              << modelo.faces.size()     << " faces triangulares\n";
}

// calcula AABB e escala de normalizacao para enquadramento automatico
void normalizar_modelo() {
    if (modelo.vertices.empty()) return;

    std::array<float, 3> vmin = modelo.vertices[0];
    std::array<float, 3> vmax = modelo.vertices[0];

    for (const auto& v : modelo.vertices) {
        for (int k = 0; k < 3; ++k) {
            vmin[k] = std::min(vmin[k], v[k]);
            vmax[k] = std::max(vmax[k], v[k]);
        }
    }

    for (int k = 0; k < 3; ++k) {
        modelo.centro_aabb[k] = (vmin[k] + vmax[k]) * 0.5f;
    }

    float maior_dim = 0.0f;
    for (int k = 0; k < 3; ++k) {
        maior_dim = std::max(maior_dim, vmax[k] - vmin[k]);
    }
    // cabe em cubo unitario; guarda divisao por zero
    modelo.escala_normalizacao = (maior_dim > 1e-6f) ? (2.0f / maior_dim) : 1.0f;

    std::cout << "AABB centro: ("
              << modelo.centro_aabb[0] << ", "
              << modelo.centro_aabb[1] << ", "
              << modelo.centro_aabb[2] << ")  "
              << "fator de escala: " << modelo.escala_normalizacao << "\n";
}

// compila toda a geometria em display list; emite normal, texcoord, posicao
void compilar_display_list() {
    modelo.display_list = glGenLists(1);
    glNewList(modelo.display_list, GL_COMPILE);

    glBegin(GL_TRIANGLES);
    for (const Face& face : modelo.faces) {
        // sem vn no .obj: usa normal da face calculada (flat shading)
        std::array<float, 3> normal_face = { 0.0f, 0.0f, 1.0f };
        bool usa_normal_calculada = (face.v[0].ivn < 0);

        if (usa_normal_calculada) {
            bool valido = (face.v[0].iv >= 0 &&
                           face.v[1].iv >= 0 &&
                           face.v[2].iv >= 0 &&
                           face.v[0].iv < static_cast<int>(modelo.vertices.size()) &&
                           face.v[1].iv < static_cast<int>(modelo.vertices.size()) &&
                           face.v[2].iv < static_cast<int>(modelo.vertices.size()));
            if (valido) {
                normal_face = calcular_normal_face(
                    modelo.vertices[face.v[0].iv],
                    modelo.vertices[face.v[1].iv],
                    modelo.vertices[face.v[2].iv]
                );
            }
        }

        for (int k = 0; k < 3; ++k) {
            const IndiceVertice& idx = face.v[k];

            // normal: prioriza vn do .obj, senao usa a calculada
            if (!usa_normal_calculada &&
                idx.ivn >= 0 &&
                idx.ivn < static_cast<int>(modelo.normais.size())) {
                const auto& n = modelo.normais[idx.ivn];
                glNormal3f(n[0], n[1], n[2]);
            } else {
                glNormal3f(normal_face[0], normal_face[1], normal_face[2]);
            }

            // texcoord: enviada mesmo sem textura ativa
            if (idx.ivt >= 0 &&
                idx.ivt < static_cast<int>(modelo.texcoords.size())) {
                const auto& t = modelo.texcoords[idx.ivt];
                glTexCoord2f(t[0], t[1]);
            }

            if (idx.iv >= 0 &&
                idx.iv < static_cast<int>(modelo.vertices.size())) {
                const auto& v = modelo.vertices[idx.iv];
                glVertex3f(v[0], v[1], v[2]);
            }
        }
    }
    glEnd();

    glEndList();
    std::cout << "Display list compilada: id=" << modelo.display_list << "\n";
}

// configura as 3 luzes; chamado na init e em cada toggle
void configurar_luzes() {
    // LOCAL_VIEWER melhora o highlight especular
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

    glEnable(GL_LIGHTING);

    for (int i = 0; i < 3; ++i) {
        if (luzes[i].ativa) {
            glEnable(luzes[i].id);
        } else {
            glDisable(luzes[i].id);
        }
        glLightfv(luzes[i].id, GL_AMBIENT,  luzes[i].ambiente);
        glLightfv(luzes[i].id, GL_DIFFUSE,  luzes[i].difusa);
        glLightfv(luzes[i].id, GL_SPECULAR, luzes[i].especular);
        // posicao re-emitida por frame em display() -> fixa no espaco do mundo
    }
}

// overlay 2D com estado das luzes e linha de ajuda
void desenhar_hud() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, janela_largura, 0.0, janela_altura);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // cores coincidem com a difusa de cada luz
    const float cores_ativas[3][3] = {
        { 1.0f, 1.0f, 1.0f },
        { 1.0f, 0.4f, 0.4f },
        { 0.4f, 0.6f, 1.0f }
    };
    const float cor_inativa[3] = { 0.4f, 0.4f, 0.4f };

    const char* rotulos[] = {
        "[1] Luz Branca  (key)",
        "[2] Luz Vermelha (fill)",
        "[3] Luz Azul     (rim)"
    };

    for (int i = 0; i < 3; ++i) {
        if (luzes[i].ativa) {
            glColor3fv(cores_ativas[i]);
        } else {
            glColor3fv(cor_inativa);
        }
        glRasterPos2i(10, janela_altura - 18 * (i + 1));
        for (const char* c = rotulos[i]; *c != '\0'; ++c) {
            glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *c);
        }
    }

    glColor3f(0.5f, 0.5f, 0.5f);
    const char* ajuda = "WASD/QE:mover  x/y/z:rodar  +/-:escala  R:reset  ESC:sair";
    glRasterPos2i(10, 8);
    for (const char* c = ajuda; *c != '\0'; ++c) {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *c);
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(0.0, 2.0, 6.0,
              0.0, 0.0, 0.0,
              0.0, 1.0, 0.0);

    // emite posicoes apos gluLookAt e antes das transforms do modelo
    // -> luzes ficam fixas no espaco do mundo
    for (int i = 0; i < 3; ++i) {
        glLightfv(luzes[i].id, GL_POSITION, luzes[i].posicao);
    }

    // subtrai o centro AABB para rotacionar em torno do centro do modelo
    glTranslatef(cena.trans_x, cena.trans_y, cena.trans_z);
    glRotatef(cena.rot_x, 1.0f, 0.0f, 0.0f);
    glRotatef(cena.rot_y, 0.0f, 1.0f, 0.0f);
    glRotatef(cena.rot_z, 0.0f, 0.0f, 1.0f);
    glScalef(cena.escala * modelo.escala_normalizacao,
             cena.escala * modelo.escala_normalizacao,
             cena.escala * modelo.escala_normalizacao);
    glTranslatef(-modelo.centro_aabb[0],
                 -modelo.centro_aabb[1],
                 -modelo.centro_aabb[2]);

    // material cinza neutro para evidenciar as 3 cores de luz
    GLfloat mat_amb[]  = { 0.25f, 0.25f, 0.25f, 1.0f };
    GLfloat mat_dif[]  = { 0.75f, 0.75f, 0.75f, 1.0f };
    GLfloat mat_esp[]  = { 0.90f, 0.90f, 0.90f, 1.0f };
    GLfloat mat_bri    = 64.0f;
    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_dif);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_esp);
    glMaterialf (GL_FRONT, GL_SHININESS, mat_bri);

    if (modelo.display_list != 0) {
        glCallList(modelo.display_list);
    }

    // desativa iluminacao pro HUD (evita modular as cores do texto)
    glDisable(GL_LIGHTING);
    desenhar_hud();
    glEnable(GL_LIGHTING);

    glutSwapBuffers();
}

void reshape(GLsizei w, GLsizei h) {
    if (h == 0) h = 1;
    janela_largura = w;
    janela_altura  = h;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // near/far amplos para qualquer .obj
    gluPerspective(45.0, static_cast<GLdouble>(w) / static_cast<GLdouble>(h),
                   0.01, 2000.0);
    glMatrixMode(GL_MODELVIEW);
}

void redraw(int valor) {
    (void)valor;
    glutPostRedisplay();
    glutTimerFunc(delay_ms, redraw, 0);
}

void keyboard(unsigned char key, int x, int y) {
    (void)x;
    (void)y;

    const float passo_trans = 0.05f;
    const float passo_rot   = 5.0f;
    const float passo_esc   = 0.05f;

    switch (key) {
    // translacao
    case 'w': cena.trans_y += passo_trans; break;
    case 's': cena.trans_y -= passo_trans; break;
    case 'a': cena.trans_x -= passo_trans; break;
    case 'd': cena.trans_x += passo_trans; break;
    case 'q': cena.trans_z -= passo_trans; break;
    case 'e': cena.trans_z += passo_trans; break;

    // rotacao (min: +, mai: -)
    case 'x': cena.rot_x += passo_rot; break;
    case 'X': cena.rot_x -= passo_rot; break;
    case 'y': cena.rot_y += passo_rot; break;
    case 'Y': cena.rot_y -= passo_rot; break;
    case 'z': cena.rot_z += passo_rot; break;
    case 'Z': cena.rot_z -= passo_rot; break;

    // escala
    case '+':
    case '=': cena.escala += passo_esc; break;
    case '-':
    case '_': cena.escala = std::max(0.01f, cena.escala - passo_esc); break;

    // luzes
    case '1':
        luzes[0].ativa = !luzes[0].ativa;
        configurar_luzes();
        std::cout << "Luz 0 (branca): " << (luzes[0].ativa ? "ON" : "OFF") << "\n";
        break;
    case '2':
        luzes[1].ativa = !luzes[1].ativa;
        configurar_luzes();
        std::cout << "Luz 1 (vermelha): " << (luzes[1].ativa ? "ON" : "OFF") << "\n";
        break;
    case '3':
        luzes[2].ativa = !luzes[2].ativa;
        configurar_luzes();
        std::cout << "Luz 2 (azul): " << (luzes[2].ativa ? "ON" : "OFF") << "\n";
        break;

    case 'r':
    case 'R':
        cena = EstadoCena();
        std::cout << "Transformacoes resetadas.\n";
        break;

    case 27: // ESC
        exit(0);
        break;
    }

    glutPostRedisplay();
}

void keyboard_special(int key, int x, int y) {
    (void)x;
    (void)y;

    const float passo = 0.05f;

    switch (key) {
    case GLUT_KEY_UP:        cena.trans_y += passo; break;
    case GLUT_KEY_DOWN:      cena.trans_y -= passo; break;
    case GLUT_KEY_LEFT:      cena.trans_x -= passo; break;
    case GLUT_KEY_RIGHT:     cena.trans_x += passo; break;
    case GLUT_KEY_PAGE_UP:   cena.trans_z -= passo; break;
    case GLUT_KEY_PAGE_DOWN: cena.trans_z += passo; break;
    }

    glutPostRedisplay();
}

void mouse_botao(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        arrastando = (state == GLUT_DOWN);
        mouse_x_anterior = x;
        mouse_y_anterior = y;
    }
}

// orbita: H=yaw, V=pitch (0.4 graus/pixel)
void mouse_movimento(int x, int y) {
    if (!arrastando) return;

    float delta_x = static_cast<float>(x - mouse_x_anterior);
    float delta_y = static_cast<float>(y - mouse_y_anterior);

    cena.rot_y += delta_x * 0.4f;
    cena.rot_x += delta_y * 0.4f;

    mouse_x_anterior = x;
    mouse_y_anterior = y;

    glutPostRedisplay();
}

int main(int argc, char** argv) {
    std::string caminho_obj = "data/mba1.obj";
    if (argc > 1) {
        caminho_obj = argv[1];
    }

    // glGenLists precisa de contexto ativo; compilar_display_list() vem depois de CreateWindow
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(janela_largura, janela_altura);
    glutCreateWindow("M1.2 - 3D e .obj | Diego Silva / Miruna Onofrei");

    glClearColor(0.08f, 0.08f, 0.12f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE); // renormaliza normais apos glScale
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW); // OBJ: CCW = frente

    configurar_luzes();

    carregar_obj(caminho_obj);
    normalizar_modelo();
    compilar_display_list();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(keyboard_special);
    glutMouseFunc(mouse_botao);
    glutMotionFunc(mouse_movimento);
    glutTimerFunc(delay_ms, redraw, 0);

    glutMainLoop();
    return 0;
}
