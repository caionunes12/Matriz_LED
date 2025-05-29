#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "pico/bootrom.h"
#include "animacoes.h"

//arquivo .pio
#include "pio_matrix.pio.h"

//número de LEDs
#define NUM_PIXELS 25

//pino de saída
#define OUT_PIN 7

//botão de interupção
const uint button_0 = 5;
const uint button_1 = 6;

// Variáveis para debounce
uint32_t last_time_0 = 0;
uint32_t last_time_1 = 0;
int a = 0;
int b = 0;

// Variáveis para controle das animações
int frame_atual_0 = 0;
int frame_atual_1 = 0;
bool animacao_ativa_0 = false;
bool animacao_ativa_1 = false;

//vetor para criar imagem na matriz de led - LEDs apagados
double desenho[25] =   {0.0, 0.0, 0.0, 0.0, 0.0,
                        0.0, 0.0, 0.0, 0.0, 0.0, 
                        0.0, 0.0, 0.0, 0.0, 0.0,
                        0.0, 0.0, 0.0, 0.0, 0.0,
                        0.0, 0.0, 0.0, 0.0, 0.0};

//rotina para definição da intensidade de cores do led
uint32_t matrix_rgb(double b, double r, double g)
{
  unsigned char R, G, B;
  R = r * 255;
  G = g * 255;
  B = b * 255;
  return (G << 24) | (R << 16) | (B << 8);
}

//rotina para acionar a matrix de leds - ws2812b
void desenho_pio(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b){

    for (int16_t i = 0; i < NUM_PIXELS; i++) {
        if(animacao_ativa_0) {
            // Coração pulsante em vermelho
            valor_led = matrix_rgb(0.0, desenho[24-i], 0.0);  // Vermelho
        }
        else if(animacao_ativa_1) {
            // Estrela girando em azul
            valor_led = matrix_rgb(desenho[24-i], 0.0, 0.0);  // Azul
        }
        else {
            // LEDs apagados
            valor_led = matrix_rgb(0.0, 0.0, 0.0);
        }
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}

// Função de Interrupção com Debounce para o botão0
void trata_botao_0(uint gpio, uint32_t events)
{
    //obtem o tempo atual em microssegundos
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    printf("A = %d\n", a); 

    //verifica se passou tempo suficiente desde o ultimo evento
    if (current_time - last_time_0 > 2000)    // 2000 ms de debounce
    {
        last_time_0 = current_time;          // Atualiza o tempo do ultimo evento
        printf("mudanca de estado do led. A = %d\n", a);

        a++;                                //Incrementa a variável de verificação
        animacao_ativa_0 = true;            // Ativa a animação
        frame_atual_0 = 0;                  // Reinicia o frame
    }
}

// Função de Interrupção com Debounce para o botão1
void trata_botao_1(uint gpio, uint32_t events)
{
    //obtem o tempo atual em microssegundos
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    printf("B = %d\n", b); 

    //verifica se passou tempo suficiente desde o ultimo evento
    if (current_time - last_time_1 > 2000)    // 2000 ms de debounce
    {
        last_time_1 = current_time;          // Atualiza o tempo do ultimo evento
        printf("mudanca de estado do led. B = %d\n", b);

        b++;                                //Incrementa a variável de verificação
        animacao_ativa_1 = true;            // Ativa a animação
        frame_atual_1 = 0;                  // Reinicia o frame
    }
}

//função principal
int main()
{
    PIO pio = pio0; 
    bool ok;
    uint16_t i;
    uint32_t valor_led;
    double r = 0.0, b = 0.0 , g = 0.0;

    //coloca a frequência de clock para 128 MHz, facilitando a divisão pelo clock
    ok = set_sys_clock_khz(128000, false);

    // Inicializa todos os códigos stdio padrão que estão ligados ao binário.
    stdio_init_all();

    printf("iniciando a transmissão PIO\n");

    //configurações da PIO
    uint offset = pio_add_program(pio, &pio_matrix_program);
    uint sm = pio_claim_unused_sm(pio, true);
    pio_matrix_program_init(pio, sm, offset, OUT_PIN);

    //inicializar o botão de interrupção - GPIO5
    gpio_init(button_0);
    gpio_set_dir(button_0, GPIO_IN);
    gpio_pull_up(button_0);

    //inicializar o botão de interrupção - GPIO6
    gpio_init(button_1);
    gpio_set_dir(button_1, GPIO_IN);
    gpio_pull_up(button_1);

    // Inicializa com todos os LEDs apagados
    desenho_pio(desenho, valor_led, pio, sm, r, g, b);

    //Interrupção da Gpio Botão ativada
    gpio_set_irq_enabled_with_callback(button_0, GPIO_IRQ_EDGE_FALL, 1, &trata_botao_0);
    gpio_set_irq_enabled_with_callback(button_1, GPIO_IRQ_EDGE_FALL, 1, &trata_botao_1);

    while (true) {
        if(animacao_ativa_0) {
            // Mostra o frame atual da animação do botão 0 (Coração)
            desenho_pio(animacao_botao0.frames[frame_atual_0], valor_led, pio, sm, 1.0, 0.0, 0.0);  // Vermelho
            frame_atual_0 = (frame_atual_0 + 1) % 5;  // Avança para o próximo frame
            if(frame_atual_0 == 0) {
                animacao_ativa_0 = false;  // Desativa a animação após completar o ciclo
            }
        }
        else if(animacao_ativa_1) {
            // Mostra o frame atual da animação do botão 1 (Estrela)
            desenho_pio(animacao_botao1.frames[frame_atual_1], valor_led, pio, sm, 0.0, 0.0, 1.0);  // Azul
            frame_atual_1 = (frame_atual_1 + 1) % 5;  // Avança para o próximo frame
            if(frame_atual_1 == 0) {
                animacao_ativa_1 = false;  // Desativa a animação após completar o ciclo
            }
        }
        else {
            // Se nenhuma animação está ativa, mantém os LEDs apagados
            desenho_pio(desenho, valor_led, pio, sm, 0.0, 0.0, 0.0);
        }

        sleep_ms(200);  // Ajuste este valor para controlar a velocidade da animação
    }
}