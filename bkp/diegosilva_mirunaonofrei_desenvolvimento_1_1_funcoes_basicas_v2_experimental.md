# Trabalho 1.1 - Funcoes Basicas 3D (V2 Experimental)

Arquivos desta V2 (separada da entrega oficial):
- `bkp/diegosilva_mirunaonofrei_desenvolvimento_1_1_funcoes_basicas_v2_experimental.cpp`
- `bkp/diegosilva_mirunaonofrei_desenvolvimento_1_1_funcoes_basicas_v2_experimental.md`

## Importante
Esta versao e **experimental** e foi feita apenas para demonstracao visual/interacao.
Ela **nao substitui** a versao oficial academica que deve permanecer separada em `trabalhos/`.

## Base preservada
Mesmo na V2 experimental, foram preservados os pontos principais da base:
- cubo 3D em wireframe;
- desenho com `GL_LINES` (somente arestas);
- translacao, escala e rotacao manuais nos vertices;
- respeito ao centro do poligono;
- sem uso de `glTranslate`, `glRotate` e `glScale`.

## O que mudou na V2
O wrap-around imediato foi substituido por uma transicao de borda animada:

1. O cubo e movido normalmente por teclado.
2. Quando toca/sai da borda visivel em X ou Y:
   - entra em estado de transicao;
   - a translacao em X/Y fica temporariamente bloqueada;
   - inicia um giro automatico de `360 graus` ao longo de varios frames;
   - ao concluir o giro, reaparece no lado oposto;
   - retorna ao comportamento normal.

## Como funciona a logica de "girar 360 antes de atravessar"

### 1) Estado de transicao
Foram adicionadas variaveis globais para controlar a animacao:
- `em_transicao_borda`;
- `direcao_transicao` (`L`, `R`, `U`, `D`);
- `giro_acumulado_transicao`;
- `passo_giro_transicao`;
- `giro_total_transicao = 2*pi`;
- `eixo_rotacao_transicao`.

### 2) Deteccao de borda
A cada translacao, o programa calcula:
- limites visiveis atuais em mundo (com FOV, aspecto da janela e Z do cubo);
- raio atual do cubo em X/Y (a partir dos vertices transformados).

Com isso, detecta toque/saida de borda de forma coerente com o tamanho atual do objeto.

### 3) Giro no timer (nao no teclado)
No callback `redraw`, a funcao `atualizar_transicao_borda()`:
- aplica pequenos incrementos de rotacao por frame;
- acumula o angulo girado;
- encerra quando atinge `2*pi` radianos.

### 4) Reaparecimento no lado oposto
Ao finalizar os `360 graus`, a funcao `reaparecer_no_lado_oposto()`:
- calcula o destino oposto com base nos limites atuais;
- atualiza centro e vertices com o mesmo deslocamento;
- limpa o estado de transicao.

## Eixo do giro automatico
Nesta V2 foi usado o eixo `Z` para o giro automatico.
Motivo: visualmente fica mais estavel e legivel em tela no contexto atual da projecao.

## Controles
- Translacao em XY: `W`, `A`, `S`, `D` e setas.
- Translacao em Z: `Q`, `E`, `PageUp`, `PageDown`.
- Escala: `+` / `-`.
- Rotacao manual: `x y z` (sentido positivo), `X Y Z` (sentido negativo).
- Sair: `ESC`.

## Compilacao e execucao

### Ubuntu (22.04+ / 24.04+)

```bash
cd /media/diego/dados/UNivali/comp_grafica/computacao-grafica-univali-diegoS/bkp
g++ diegosilva_mirunaonofrei_desenvolvimento_1_1_funcoes_basicas_v2_experimental.cpp -o trabalho1_1_v2 -lglut -lGL -lGLU
./trabalho1_1_v2
```

### Windows (MSYS2 UCRT64)

```bash
cd /c/UNivali/comp_grafica/computacao-grafica-univali-diegoS/bkp
g++ diegosilva_mirunaonofrei_desenvolvimento_1_1_funcoes_basicas_v2_experimental.cpp -o trabalho1_1_v2.exe -lfreeglut -lopengl32 -lglu32
./trabalho1_1_v2.exe
```

### Windows (Visual Studio Build Tools)

```bat
cd /d D:\UNivali\comp_grafica\computacao-grafica-univali-diegoS\bkp
cl /EHsc diegosilva_mirunaonofrei_desenvolvimento_1_1_funcoes_basicas_v2_experimental.cpp freeglut.lib opengl32.lib glu32.lib
```

## Diferenças da V2 experimental
- Esta versao adiciona um comportamento visual extra (giro de 360 graus antes do wrap) que **nao e obrigatorio** no enunciado original.
- A versao oficial para o professor deve continuar separada e sem depender desta customizacao.
- A V2 foi feita apenas para fins de demonstracao/interacao visual.
