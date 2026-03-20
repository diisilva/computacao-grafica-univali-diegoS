// Biblioteca padrão de entrada/saída em C++ (não é essencial aqui, mas comum em exemplos).
#include <iostream>
// FreeGLUT: criação de janela, loop de eventos e chamadas OpenGL básicas.
#include <GL/freeglut.h>

// Declaração antecipada da função de desenho (callback de renderização).
void display();
// Declaração antecipada da função de teclado (callback de input).
void keyboard(unsigned char key, int x, int y);

int main(int argc, char** argv) {
	// Inicializa o GLUT com os argumentos recebidos na linha de comando.
	glutInit(&argc, argv);
	
	// Define o tamanho inicial da janela (largura=500, altura=250) em pixels.
	glutInitWindowSize(500, 250);

	// Cria a janela e define o título exibido na barra.
	glutCreateWindow("Desenhando uma linha");
	// Define a cor de fundo (branco), no formato RGBA.
	glClearColor(1.0, 1.0, 1.0, 1.0);
	// Define projeção ortográfica 2D: x de 0 a 256 e y de 0 a 256.
	glOrtho(0, 256, 0, 256, -1, 1);

	// Registra a função que será chamada para desenhar cada frame.
	glutDisplayFunc(display);
	// Registra a função que será chamada quando uma tecla for pressionada.
	glutKeyboardFunc(keyboard);

	// Inicia o loop principal de eventos do GLUT (janela, input, redraw).
	glutMainLoop();

	// Retorno formal; normalmente não é alcançado enquanto o loop estiver ativo.
	return 0;
}


void display(void) {
	// Limpa o buffer de cor usando a cor definida por glClearColor.
	glClear(GL_COLOR_BUFFER_BIT);
	// Define a cor atual de desenho para preto (RGB).
	glColor3f(0.0, 0.0, 0.0);

	// Inicia o envio de vértices para desenhar linhas.
	glBegin(GL_LINES);
	// Primeiro ponto da linha.
	glVertex2i(10, 10);
	// Segundo ponto da linha.
	glVertex2i(245, 245);
	// Finaliza o bloco de desenho iniciado por glBegin.
	glEnd();

	// Garante execução dos comandos OpenGL no pipeline (single buffer).
	glFlush();
}

void keyboard(unsigned char key, int x, int y) {
	// Seleciona ação com base na tecla pressionada.
	switch (key) {
	// Código ASCII 27 = tecla ESC.
	case 27:                                         
		// Encerra o programa imediatamente.
		exit(0);
		break;
	}
}