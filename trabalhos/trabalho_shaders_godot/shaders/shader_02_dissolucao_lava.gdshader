// Shader 2: Dissolução de Lava
// Objeto alvo: SphereMesh com radial_segments >= 64 e rings >= 32
// Requisitos cobertos:
//   - vertex():     altera NORMAL e VERTEX (deslocamento pulsante com TIME)
//   - fragment():   altera ROUGHNESS, usa discard para dissolução animada
//   - textura/ruído: sampler2D ruido (NoiseTexture2D da Godot)
//   - TIME:         pulso de expansão e fluxo de lava animados
//   - uniform:       limiar_dissolucao, velocidade_pulso, cor_quente, cor_fria, ruido

shader_type spatial;

// ---- Uniforms ----

// Define o ponto de corte da dissolução (0.0 = sem dissolução, 1.0 = totalmente dissolvido)
uniform float limiar_dissolucao : hint_range(0.0, 1.0) = 0.25;

// Velocidade do pulso de expansão/contração da esfera
uniform float velocidade_pulso : hint_range(0.1, 5.0) = 1.4;

// Intensidade do pulso de expansão
uniform float intensidade_pulso : hint_range(0.0, 0.3) = 0.08;

// Cor das regiões de alta temperatura (bordas da lava)
uniform vec4 cor_quente : source_color = vec4(1.0, 0.28, 0.0, 1.0);

// Cor das regiões de baixa temperatura (rocha solidificada)
uniform vec4 cor_fria : source_color = vec4(0.18, 0.02, 0.02, 1.0);

// Textura de ruído — configure como NoiseTexture2D no Inspector
uniform sampler2D ruido : filter_linear_mipmap, repeat_enable;

// ---- Função vertex: roda uma vez por vértice ----

void vertex() {
    // UV animado: desloca as coordenadas ao longo do tempo criando fluxo de lava
    vec2 uv_animado = UV + vec2(TIME * 0.05, TIME * 0.03);

    // Amostra o ruído com UV animado — a superfície "flui"
    float n = textureLod(ruido, uv_animado, 0.0).r;

    // Pulso global: expande e contrai a esfera inteira com TIME
    float pulso = sin(TIME * velocidade_pulso) * 0.5 + 0.5;

    // Perturba a normal com base no ruído para superfície irregular
    NORMAL += NORMAL * (n - 0.5) * 0.4;

    // Deslocamento irregular do ruído + pulso global ao longo da normal
    VERTEX += NORMAL * ((n - 0.5) * 0.08 + pulso * intensidade_pulso);
}

// ---- Função fragment: roda uma vez por pixel (fragmento) ----

void fragment() {
    // UV animado igual ao vertex para o padrão de dissolução também fluir
    vec2 uv_animado = UV + vec2(TIME * 0.05, TIME * 0.03);
    float n = texture(ruido, uv_animado).r;

    // Descarta fragmentos abaixo do limiar — cria buracos que se movem
    if (n < limiar_dissolucao) {
        discard;
    }

    // Transição suave entre cor quente e fria com base no ruído
    float fator_calor = smoothstep(
        limiar_dissolucao + 0.05,
        limiar_dissolucao + 0.35,
        n
    );
    ALBEDO = mix(cor_quente.rgb, cor_fria.rgb, fator_calor);

    // Borda incandescente pulsa em intensidade com o TIME
    float borda = smoothstep(limiar_dissolucao, limiar_dissolucao + 0.12, n);
    float pulso_emissao = sin(TIME * velocidade_pulso) * 0.5 + 0.5;
    EMISSION = cor_quente.rgb * (1.0 - borda) * (2.5 + pulso_emissao * 1.5);

    // Superfície de rocha/lava: alta rugosidade, sem reflexo metálico
    ROUGHNESS = 0.85;
    METALLIC = 0.0;
}
