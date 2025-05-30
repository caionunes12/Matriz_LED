#ifndef ANIMACOES_H
#define ANIMACOES_H

// Estrutura para armazenar uma animação
typedef struct {
    double frames[15][25];  // 5 frames de 25 LEDs cada
} Animacao;

// Declaração das animações
extern Animacao animacao_botao0;  // Animação do botão 0 - Coração pulsante
extern Animacao animacao_botao1;  // Animação do botão 1 - Estrela girando

#endif 