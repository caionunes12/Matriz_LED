#ifndef ANIMACOES_H
#define ANIMACOES_H

// Estrutura para armazenar uma animação
typedef struct {
    double frames[5][25];  // 5 frames de 25 LEDs cada
} Animacao;

// Declaração das animações
extern Animacao animacao_botao0;
extern Animacao animacao_botao1;

#endif 