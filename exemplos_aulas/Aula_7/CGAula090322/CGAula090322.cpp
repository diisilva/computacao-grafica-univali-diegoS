#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <glew.h>
#include <GL/glut.h>
#include <chrono>
#include <fstream>
using namespace std;

GLuint v, f;
GLuint p;

GLuint VAO, VBO;

static unsigned int texturas[2];

float intensidadeLuzAmbiente = 0.1; 
float intensidadeLuzDifusa = 0.9;
float corLuz[3] = { 1, 1, 1 };
float posLuz[3] = { 0, 1, 0.25 };


struct BitMapFile
{
    int sizeX;
    int sizeY;
    unsigned char* data;
};

// Funciona somente com bmp de 24 bits
BitMapFile* getBMPData(string filename)
{
    BitMapFile* bmp = new BitMapFile;
    unsigned int size, offset, headerSize;

    // Ler o arquivo de entrada
    ifstream infile(filename.c_str(), ios::binary);

    // Pegar o ponto inicial de leitura
    infile.seekg(10);
    infile.read((char*)&offset, 4);

    // Pegar o tamanho do cabeçalho do bmp.
    infile.read((char*)&headerSize, 4);

    // Pegar a altura e largura da imagem no cabeçalho do bmp.
    infile.seekg(18);
    infile.read((char*)&bmp->sizeX, 4);
    infile.read((char*)&bmp->sizeY, 4);

    // Alocar o buffer para a imagem.
    size = bmp->sizeX * bmp->sizeY * 24;
    bmp->data = new unsigned char[size];

    // Ler a informação da imagem.
    infile.seekg(offset);
    infile.read((char*)bmp->data, size);

    // Reverte a cor de bgr para rgb
    int temp;
    for (int i = 0; i < size; i += 3)
    {
        temp = bmp->data[i];
        bmp->data[i] = bmp->data[i + 2];
        bmp->data[i + 2] = temp;
    }

    return bmp;
}

void loadExternalTextures(std::string nome_arquivo, int id_textura)
{
    BitMapFile* image[1];

    image[0] = getBMPData(nome_arquivo);

    glBindTexture(GL_TEXTURE_2D, texturas[id_textura]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[0]->sizeX, image[0]->sizeY, 0,
        GL_RGB, GL_UNSIGNED_BYTE, image[0]->data);
}

char* readStringFromFile(char* fn) {

    FILE* fp;
    char* content = NULL;
    int count = 0;

    if (fn != NULL) {
        fopen_s(&fp, fn, "rt");

        if (fp != NULL) {

            fseek(fp, 0, SEEK_END);
            count = ftell(fp);
            rewind(fp);

            if (count > 0) {
                content = (char*)malloc(sizeof(char) * (count + 1));
                count = fread(content, sizeof(char), count, fp);
                content[count] = '\0';
            }
            fclose(fp);
        }
    }
    return content;
}


void setShaders() {

    char* vs = NULL, * fs = NULL, * fs2 = NULL;


    glewInit();

    v = glCreateShader(GL_VERTEX_SHADER);
    f = glCreateShader(GL_FRAGMENT_SHADER);

    char vertex_shader[] = "simples.vert";
    char fragment_shader[] = "simples.frag";
    vs = readStringFromFile(vertex_shader);
    fs = readStringFromFile(fragment_shader);

    const char* vv = vs;
    const char* ff = fs;

    glShaderSource(v, 1, &vv, NULL);
    glShaderSource(f, 1, &ff, NULL);

    free(vs); free(fs);

    glCompileShader(v);
    glCompileShader(f);

    int result;
    glGetShaderiv(v, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int tam;
        glGetShaderiv(v, GL_INFO_LOG_LENGTH, &tam);
        char* mensagem = new char[tam];
        glGetShaderInfoLog(v, tam, &tam, mensagem);
        std::cout << mensagem << std::endl;
    }

    glGetShaderiv(f, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int tam;
        glGetShaderiv(f, GL_INFO_LOG_LENGTH, &tam);
        char* mensagem = new char[tam];
        glGetShaderInfoLog(f, tam, &tam, mensagem);
        std::cout << mensagem << std::endl;
    }

    p = glCreateProgram();
    glAttachShader(p, v);
    glAttachShader(p, f);

    glLinkProgram(p);


    float vertices[] = {
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f,
       -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
       -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f

    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glUseProgram(p);

}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();

    glTranslatef(0, 0, -10);




    glColor3f(1, 0, 0);
    glPushMatrix();
    glScaled(2, 2, 2);


    int id_int_luz_amb = glGetUniformLocation(p, "luz_int_amb");
    glUniform1f(id_int_luz_amb, intensidadeLuzAmbiente);

    int id_int_luz_dif = glGetUniformLocation(p, "luz_int_dif");
    glUniform1f(id_int_luz_dif, intensidadeLuzDifusa);

    int id_cor_luz = glGetUniformLocation(p, "cor_luz");
    glUniform3f(id_cor_luz, corLuz[0], corLuz[1], corLuz[2]);

    int id_pos_luz = glGetUniformLocation(p, "pos_luz");
    glUniform3f(id_pos_luz, posLuz[0], posLuz[1], posLuz[2]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texturas[0]);
    int sp_texture = glGetUniformLocation(p, "textura");
    glUniform1i(sp_texture, 0);

    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, texturas[1]);
    int sp_texture_normal = glGetUniformLocation(p, "textura_normal");
    glUniform1i(sp_texture_normal, 1);


    glBindTexture(GL_TEXTURE_2D, texturas[1]);
    glBindVertexArray(VAO);
    glDrawArrays(GL_QUADS, 0, 4);



    glPopMatrix();

    glFlush();
    glutSwapBuffers();
    glutPostRedisplay();
}


void key(unsigned char k, int x, int y)
{
    if (k == 'q') exit(0);
    if (k == 'z')
        intensidadeLuzAmbiente -= 0.05;
    else if(k == 'x')
        intensidadeLuzAmbiente += 0.05;

    if (k == 'a')
        posLuz[0] -= 0.05;
    else if (k == 'd')
        posLuz[0] += 0.05;

    if (k == 'w')
        posLuz[1] += 0.05;
    else if (k == 's')
        posLuz[1] -= 0.05;
}

void reshape(int width, int height)
{
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0f, (float)640 / (float)480, 0.1f, 1000.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void setupRC() {

    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLfloat diffuseLight[] = { 0.7, 0.7, 0.7, 1.0 };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glEnable(GL_LIGHT0);

    GLfloat ambientLight[] = { 0.05, 0.05, 0.05, 1.0 };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);

    GLfloat specularLight[] = { 0.7, 0.7, 0.7, 1.0 };
    GLfloat spectre[] = { 1.0, 1.0, 1.0, 1.0 };

    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    glMaterialfv(GL_FRONT, GL_SPECULAR, spectre);
    glMateriali(GL_FRONT, GL_SHININESS, 128);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    glEnable(GL_LIGHTING);
}

void init()
{
    glClearColor(0, 0, 0, 1);
    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    setupRC();

    glGenTextures(2, texturas);
    loadExternalTextures("Textures/brickwall.bmp", 0);
    loadExternalTextures("Textures/brickwall_normal.bmp", 1);
    glEnable(GL_TEXTURE_2D);


    //comente essa linha para desabilitar os shaders
    setShaders();
}



int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(640, 480);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Shader");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(key);

    glutMainLoop();

    return 0;
}