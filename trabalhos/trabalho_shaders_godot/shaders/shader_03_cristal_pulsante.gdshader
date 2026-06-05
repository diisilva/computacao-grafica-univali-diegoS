// Shader 3: Cristal Pulsante
// Objeto alvo: BoxMesh (ou qualquer malha com normais bem definidas)
// Requisitos cobertos:
//   - vertex():   altera VERTEX (expande/contrai ao longo das normais com pulso)
//   - fragment(): altera METALLIC e ROUGHNESS conforme o ciclo de pulso
//   - TIME:       oscilação contínua baseada no relógio da engine
//   - varying:    fator_pulso calculado no vertex() e usado no fragment()
//   - uniform:    velocidade_pulso, intensidade_pulso, cor_cristal, cor_emissao

shader_type spatial;

// Modo de renderização: blend_mix para suporte a alpha/transparência leve
// depth_draw_opaque: escreve no z-buffer mesmo com alpha para evitar z-fighting
render_mode blend_mix, depth_draw_opaque, cull_back;

// ---- Uniforms ----

// Velocidade do ciclo de pulso (oscilações por segundo)
uniform float velocidade_pulso : hint_range(0.1, 10.0) = 2.0;

// Intensidade do deslocamento dos vértices durante o pulso
uniform float intensidade_pulso : hint_range(0.0, 0.5) = 0.12;

// Cor base do cristal (suporta alpha para transparência)
uniform vec4 cor_cristal : source_color = vec4(0.5, 0.88, 1.0, 0.88);

// Cor da emissão (brilho que pulsa junto com a deformação)
uniform vec4 cor_emissao : source_color = vec4(0.25, 0.65, 1.0, 1.0);

// ---- Varying: passado do vertex() para o fragment() ----

// Fator de pulso no momento atual [0.0, 1.0]
// 0.0 = contração máxima | 1.0 = expansão máxima
varying float fator_pulso;

// ---- Função vertex: roda uma vez por vértice ----

void vertex() {
    // Calcula o pulso usando seno de TIME, normalizado para [0.0, 1.0]
    // sin() retorna [-1, 1] → * 0.5 + 0.5 mapeia para [0.0, 1.0]
    fator_pulso = sin(TIME * velocidade_pulso) * 0.5 + 0.5;

    // Expande ou contrai o vértice ao longo da sua normal
    // Na expansão máxima (fator=1.0): move intensidade_pulso unidades para fora
    // Na contração (fator=0.0): sem deslocamento
    VERTEX += NORMAL * fator_pulso * intensidade_pulso;
}

// ---- Função fragment: roda uma vez por pixel (fragmento) ----

void fragment() {
    // Cor base do cristal (usa o alpha do uniform para transparência leve)
    ALBEDO = cor_cristal.rgb;
    ALPHA  = cor_cristal.a;

    // Cristal tem reflexo quase perfeito — muito metálico
    METALLIC = 0.9;

    // Rugosidade mínima quando expandido (superfície lisa e brilhante na crista)
    // Levemente mais rugoso na contração (superfície "fria" do cristal)
    ROUGHNESS = 0.04 + (1.0 - fator_pulso) * 0.12;

    // Emissão intensa nas cristas do pulso (o cristal "brilha" quando se expande)
    // Multiplica por 1.8 para intensidade visível mesmo com iluminação ambiente
    EMISSION = cor_emissao.rgb * fator_pulso * 1.8;
}
