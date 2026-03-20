# Guia da pasta `ComputacaoGrafica`

Este documento descreve **o que existe** nesta pasta, **para que serve** e **como executar**.

---

## 1) O que tem nesta pasta

Arquivos principais encontrados em `exemplos_aulas/ComputacaoGrafica`:

- `ComputacaoGrafica.cpp`
- `ComputacaoGrafica.vcxproj`
- `ComputacaoGrafica.vcxproj.filters`
- `ComputacaoGrafica.vcxproj.user`
- `freeglut/` (headers, libs e binários do FreeGLUT)
- `freeglut.dll`
- `x64/` (saídas de build no Visual Studio)

### Finalidade de cada um

- `ComputacaoGrafica.cpp`: código-fonte C++ do exemplo OpenGL/FreeGLUT.
- `ComputacaoGrafica.vcxproj`: projeto do Visual Studio (configura compilação, includes e libs).
- `ComputacaoGrafica.vcxproj.filters`: organização visual de arquivos dentro do Solution Explorer.
- `ComputacaoGrafica.vcxproj.user`: preferências locais do usuário (normalmente não essenciais para o código em si).
- `freeglut/`: dependências locais do FreeGLUT (inclui `include/GL`, `lib/x64` e `bin/x64`).
- `freeglut.dll`: DLL necessária em runtime no Windows para executar o binário.
- `x64/`: pasta de build (Debug/Release) gerada pelo Visual Studio.

---

## 2) Para que serve este código (`ComputacaoGrafica.cpp`)

O código é um **exemplo didático de Computação Gráfica 2D** com OpenGL no modo imediato (`glBegin/glEnd`) usando FreeGLUT para:

- criar janela e contexto gráfico;
- definir projeção ortográfica;
- criar um polígono regular (neste caso, um pentágono);
- representar o polígono por **lista de vértices** e **lista de arestas**;
- desenhar o contorno do polígono como linhas;
- capturar teclado comum e teclas especiais (setas);
- manter atualização contínua com `glutTimerFunc`.

### Observação didática importante

As funções abaixo existem, mas ainda estão com implementação pendente:

- `movimentar(...)`
- `escalar(...)`
- `rotacionar(...)`

Ou seja: a estrutura está pronta para evoluir o exemplo com transformações geométricas completas.

---

## 3) Como executar no Windows (Visual Studio)

Este é o caminho recomendado para essa pasta, porque já existe `.vcxproj` configurado com FreeGLUT local.

### Passos

1. Abra o arquivo de solução em `exemplos_aulas/ComputacaoGrafica.sln`.
2. Selecione configuração `Debug | x64`.
3. Compile (Build).
4. Execute (Start Without Debugging / `Ctrl+F5`).

O projeto já contém, para `Debug|x64`:

- include em `$(ProjectDir)freeglut\include`
- library dir em `$(ProjectDir)freeglut\lib\x64`
- dependência `freeglut.lib`
- post-build copy da DLL de `freeglut\bin\x64\freeglut.dll`

---

## 4) Como executar no Linux (compilação manual)

Se você quiser compilar este mesmo `.cpp` no Linux, use FreeGLUT/OpenGL do sistema.

### Instalar dependências

```bash
sudo apt update
sudo apt install -y g++ freeglut3-dev libglu1-mesa-dev mesa-common-dev
```

### Compilar e rodar

```bash
cd "/media/diego/Novo volume/UNivali/comp_grafica/computacao-grafica-univali-diegoS/exemplos_aulas/ComputacaoGrafica"
g++ ComputacaoGrafica.cpp -o ComputacaoGrafica -lglut -lGL -lGLU
./ComputacaoGrafica
```

---

## 5) Controles atuais no exemplo

- `ESC`: encerra o programa.
- `ESPAÇO`: chama `escalar(...)` (função ainda não implementada).
- `Setas`: chamam `movimentar(...)` com ângulos em radianos (função ainda não implementada).

---

## 6) O que foi feito para estudo (neste repositório)

O arquivo `ComputacaoGrafica.cpp` foi comentado de forma didática para explicar:

- propósito de cada `#include`;
- tipos (`vertice`, `aresta`, listas);
- estrutura `Poligono`;
- ciclo de vida do GLUT (init, callbacks, loop);
- criação de vértices e arestas do polígono;
- pipeline de desenho com OpenGL imediato;
- pontos pendentes (`movimentar`, `escalar`, `rotacionar`).

---

## 7) Próximo passo recomendado

Para transformar este exemplo em ferramenta completa de aprendizado, o próximo passo é implementar:

1. `movimentar(...)` aplicando deslocamento em todos os vértices;
2. `escalar(...)` em torno do centro geométrico do polígono;
3. `rotacionar(...)` com matriz de rotação 2D em torno de pivô.

Se você quiser, eu implemento essas três funções com comentários didáticos no mesmo estilo.
