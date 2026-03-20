// Biblioteca padrão para entrada e saída no console (std::cout).
#include <iostream>
// Biblioteca do FreeGLUT/OpenGL para criar janela, tratar input e desenhar.
#include <GL/freeglut.h>
// Biblioteca padrão para usar vetor dinâmico (std::vector).
#include <vector>

// Um vértice 2D representado por (x, y).
using vertice = std::pair<double, double>;
// Lista de vértices do polígono.
using lista_vertices = std::vector<vertice>;
// Uma aresta representada por dois índices de vértices (origem, destino).
using aresta = std::pair<int, int>;
// Lista de arestas do polígono.
using lista_arestas = std::vector<aresta>;

// Estrutura que agrupa os dados de um polígono regular.
struct Poligono {
	// Comprimento do lado do polígono.
	double tamanhoLado;
	// Quantidade de lados (e vértices) do polígono.
	int numLados;
	// Posição lógica de referência (não está sendo usada no desenho final).
	vertice posicao;
	// Escala lógica em X e Y (função escalar ainda está vazia).
	vertice escala;
	// Rotação lógica em radianos/graus (função rotacionar ainda está vazia).
	double rotacao;
	// Coordenadas dos vértices calculadas no mundo 2D.
	lista_vertices vertices;
	// Conexões entre vértices para formar as linhas do contorno.
	lista_arestas arestas;
};

// Declaração da função que cria um polígono regular a partir de posição, lado e número de lados.
Poligono criar_poligono(double posicao_x, double posicao_y, double tamanho_lado, int num_lados);
// Declaração da função que desenha o polígono na tela.
void desenhar(Poligono poligono);
// Declaração da função de movimentação (ainda não implementada).
void movimentar(Poligono& poligono, double distancia, double angulo);
// Declaração da função de escala (ainda não implementada).
void escalar(Poligono& poligono, double escala_x, double escala_y);
// Declaração da função de rotação (ainda não implementada).
void rotacionar(Poligono& poligono, double angulo);
// Callback de desenho da janela.
void display();
// Callback de timer para forçar novo desenho periódico.
void redraw(int value);
// Callback de teclado comum (teclas ASCII).
void keyboard(unsigned char key, int x, int y);
// Callback de teclado especial (setas, F-keys, etc.).
void keyboard_special(int key, int x, int y);

// Instância global do polígono que será desenhado e manipulado.
Poligono pentagono;
// Intervalo do timer em milissegundos (menor valor = mais atualizações de tela).
int delay = 10;

int main(int argc, char** argv) {

	// Cria um pentágono centrado próximo de (128,128), lado 30 e 5 lados.
	pentagono = criar_poligono(128, 128, 30, 5);

	// Inicializa o FreeGLUT com argumentos da linha de comando.
	glutInit(&argc, argv);

	// Define tamanho da janela em pixels.
	glutInitWindowSize(512, 512);

	// Cria a janela com título.
	glutCreateWindow("Desenhando uma linha");
	// Define cor de fundo branca em RGBA.
	glClearColor(1.0, 1.0, 1.0, 1.0);
	// Define projeção ortográfica 2D: x de 0..256 e y de 0..256.
	glOrtho(0, 256, 0, 256, -1, 1);

	// Registra função de desenho.
	glutDisplayFunc(display);
	// Registra função para teclado comum.
	glutKeyboardFunc(keyboard);
	// Registra função para teclas especiais (setas).
	glutSpecialFunc(keyboard_special);
	// Registra timer inicial para atualização contínua.
	glutTimerFunc(10, redraw, 0);

	// Inicia loop principal de eventos da biblioteca.
	glutMainLoop();

	// Retorno formal do programa (na prática, glutMainLoop não retorna em implementações clássicas).
	return 0;
}


void display(void) {
	// Limpa buffer de cor com a cor definida em glClearColor.
	glClear(GL_COLOR_BUFFER_BIT);

	// Chama função que desenha o polígono atual.
	desenhar(pentagono);

	// Força envio dos comandos para a pipeline gráfica (single buffer).
	glFlush();
}

void keyboard(unsigned char key, int x, int y) {
	// Mostra no console qual tecla ASCII foi pressionada.
	std::cout << key;
	// Decide ação com base na tecla pressionada.
	switch (key) {
	// ESC (código 27) encerra o programa.
	case 27:
		exit(0);
		break;
	// Espaço aplica escala (função ainda vazia).
	case ' ':
		escalar(pentagono, 1.1, 1.1);
		break;
	}
}

void keyboard_special(int key, int x, int y) {
	// Mostra no console o código numérico da tecla especial pressionada.
	std::cout << key;
	// Decide ação para teclas especiais.
	switch (key) {
	// Seta para baixo move em 270° (para baixo no plano cartesiano usado).
	case GLUT_KEY_DOWN:
		movimentar(pentagono, 10, (270 / 180.0) * 3.1416);
		break;

	// Seta para cima move em 90°.
	case GLUT_KEY_UP:
		movimentar(pentagono, 10, (90 / 180.0) * 3.1416);
		break;

	// Seta para direita move em 0°.
	case GLUT_KEY_RIGHT:
		movimentar(pentagono, 10, (0 / 180.0) * 3.1416);
		break;

	// Seta para esquerda move em 180°.
	case GLUT_KEY_LEFT:
		movimentar(pentagono, 10, (180 / 180.0) * 3.1416);
		break;

	}
}

void redraw(int value) {
	// Solicita ao GLUT que chame novamente o callback display.
	glutPostRedisplay();
	// Agenda próxima chamada do timer para manter atualização contínua.
	glutTimerFunc(delay, redraw, 0);
}

Poligono criar_poligono(double posicao_x, double posicao_y, double tamanho_lado, int num_lados) {
	// Cria variável local do tipo Poligono para preencher e retornar.
	Poligono novo_poligono;
	// Guarda quantidade de lados.
	novo_poligono.numLados = num_lados;
	// Guarda posição de referência (x).
	novo_poligono.posicao.first = posicao_x;
	// Guarda posição de referência (y).
	novo_poligono.posicao.second = posicao_y;
	// Guarda tamanho do lado.
	novo_poligono.tamanhoLado = tamanho_lado;

	// Escala inicial em x = 1 (sem aumento/redução).
	novo_poligono.escala.first = 1;
	// Escala inicial em y = 1.
	novo_poligono.escala.second = 1;

	// Rotação inicial = 0.
	novo_poligono.rotacao = 0;

	// Ângulo acumulado para gerar vértices sucessivos.
	float angulo = 0;
	// Passo angular em radianos: 360/num_lados convertido para rad.
	float passo_angulo = ((360 / float(num_lados)) * 3.1415926536) / 180.0;

	// Apótema do polígono regular, usado para ajustar a posição inicial.
	float apothem = tamanho_lado / (2 * tan(3.1416 / float(num_lados)));
	// Ajusta x para começar no “canto” a partir do centro aproximado.
	posicao_x -= tamanho_lado / 2.0;
	// Ajusta y com base no apótema.
	posicao_y -= apothem;

	// Insere primeiro vértice.
	novo_poligono.vertices.push_back(vertice(posicao_x, posicao_y));
	// Imprime cabeçalho de debug dos vértices.
	std::cout << "Vertices:\n";
	// Imprime primeiro vértice.
	std::cout << 0 << " - " << posicao_x << " - " << posicao_y << "\n";
	// Gera os demais vértices somando deslocamentos com cos/sin.
	for (int i = 1; i < num_lados; i++) {
		// Avança em x conforme tamanho do lado e ângulo atual.
		posicao_x = posicao_x + tamanho_lado * cos(angulo);
		// Avança em y conforme tamanho do lado e ângulo atual.
		posicao_y = posicao_y + tamanho_lado * sin(angulo);
		// Armazena vértice calculado.
		novo_poligono.vertices.push_back(vertice(posicao_x, posicao_y));
		// Exibe vértice no console.
		std::cout << i << " - " << posicao_x << " - " << posicao_y << "\n";
		// Incrementa ângulo para próximo vértice.
		angulo += passo_angulo;
	}

	// Imprime cabeçalho de debug das arestas.
	std::cout << "Arestas:\n";
	// Cria uma aresta entre cada vértice i e o próximo, fechando no final com módulo.
	for (int i = 0; i < num_lados; i++) {
		// Adiciona aresta (i -> (i+1)%num_lados).
		novo_poligono.arestas.push_back(aresta(i, (i + 1) % num_lados));
		// Exibe aresta no console.
		std::cout << i << " - " << (i + 1) % num_lados << "\n";
	}


	// Retorna polígono pronto com vértices e arestas preenchidos.
	return novo_poligono;
}

void movimentar(Poligono& poligono, double distancia, double angulo) {
	// TODO didático: implementar translação dos vértices usando distancia e angulo.
}

void escalar(Poligono& poligono, double escala_x, double escala_y) {
	// TODO didático: implementar escala dos vértices em relação ao centro do polígono.
}

void rotacionar(Poligono& poligono, double angulo) {
	// TODO didático: implementar rotação dos vértices em torno de um pivô.
}

void desenhar(Poligono poligono) {
	// Define cor preta para desenhar as linhas.
	glColor3f(0.0, 0.0, 0.0);
	// Inicia envio de segmentos de reta independentes.
	glBegin(GL_LINES);
	// Percorre lista de arestas para desenhar cada conexão entre dois vértices.
	for (int i = 0; i < poligono.arestas.size(); i++) {
		// Índice do vértice de origem da aresta atual.
		float v_o = poligono.arestas[i].first;
		// Índice do vértice de destino da aresta atual.
		float v_d = poligono.arestas[i].second;
		// Envia coordenada do vértice de origem para OpenGL.
		glVertex2f(poligono.vertices[v_o].first, poligono.vertices[v_o].second);
		// Envia coordenada do vértice de destino para OpenGL.
		glVertex2f(poligono.vertices[v_d].first, poligono.vertices[v_d].second);
	}
	// Finaliza bloco de desenho de linhas.
	glEnd();
}