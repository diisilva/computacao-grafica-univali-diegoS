// Shader 1: Oceano Ondulante
// Objeto alvo: PlaneMesh com subdivide_width e subdivide_depth >= 50
// Requisitos cobertos:
//   - vertex():   altera VERTEX.y (posição dos vértices)
//   - fragment(): altera ROUGHNESS e METALLIC
//   - TIME:       anima as ondas continuamente
//   - varying:    fator_altura passa a altura calculada do vertex para o fragment
//   - uniform:    amplitude, frequencia, velocidade, cor_rasa, cor_profunda

shader_type spatial;

// ---- Uniforms: visíveis e ajustáveis no Inspector da Godot ----

// Altura máxima das ondas (metros)
uniform float amplitude : hint_range(0.0, 3.0) = 0.4;

// Quantidade de ondas por unidade de espaço
uniform float frequencia : hint_range(0.1, 10.0) = 1.5;

// Velocidade de deslocamento das ondas no tempo
uniform float velocidade : hint_range(0.1, 5.0) = 1.2;

// Cor da água rasa (cristas das ondas)
uniform vec4 cor_rasa : source_color = vec4(0.0, 0.6, 0.8, 1.0);

// Cor da água profunda (vales das ondas)
uniform vec4 cor_profunda : source_color = vec4(0.0, 0.08, 0.35, 1.0);

// ---- Varying: variável compartilhada entre vertex() e fragment() ----

// Armazena a altura normalizada do vértice (0.0 = vale, 1.0 = crista)
varying float fator_altura;

// ---- Função vertex: roda uma vez por vértice ----

void vertex() {
    // Calcula onda no eixo X usando seno, deslocada no tempo
    float onda_x = sin(VERTEX.x * frequencia + TIME * velocidade);

    // Calcula onda no eixo Z usando cosseno com velocidade ligeiramente diferente
    // A diferença de frequência (0.7) evita padrão repetitivo previsível
    float onda_z = cos(VERTEX.z * frequencia + TIME * velocidade * 0.7);

    // Combina as duas ondas e aplica a amplitude
    float deslocamento = (onda_x + onda_z) * 0.5 * amplitude;

    // Desloca o vértice verticalmente (eixo Y no espaço 3D)
    VERTEX.y += deslocamento;

    // Normaliza o deslocamento para intervalo [0.0, 1.0]
    // Divide pelo amplitude total e desloca para não ficar negativo
    fator_altura = (deslocamento / amplitude) * 0.5 + 0.5;
}

// ---- Função fragment: roda uma vez por pixel (fragmento) ----

void fragment() {
    // Mistura a cor profunda com a cor rasa com base na altura do vértice
    // fator_altura=0.0 → cor_profunda | fator_altura=1.0 → cor_rasa
    ALBEDO = mix(cor_profunda.rgb, cor_rasa.rgb, fator_altura);

    // Água tem superfície quase especular: baixíssima rugosidade
    // Cristas ficam levemente mais rugosas que os vales
    ROUGHNESS = 0.05 + fator_altura * 0.1;

    // Reflexo metálico leve — simula reflexo do céu na água
    METALLIC = 0.2;

    // Emissão sutil nas cristas das ondas (brilho de luz solar)
    EMISSION = cor_rasa.rgb * fator_altura * 0.12;
}
