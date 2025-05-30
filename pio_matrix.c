#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "pico/bootrom.h"
#include "hardware/pwm.h"
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

//pino do buzzer
const uint BUZZER_PIN = 21;

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
void desenho_pio(double *desenho, PIO pio, uint sm, double r, double g, double b) {
    for (int16_t i = 0; i < NUM_PIXELS; i++) {
        uint32_t valor_led = matrix_rgb(
            b * desenho[24 - i],
            r * desenho[24 - i],
            g * desenho[24 - i]
        );
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}

// Função de Interrupção com Debounce para o botão0
void trata_button_0(uint gpio, uint32_t events)
{
    //obtem o tempo atual em microssegundos
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    printf("A = %d\n", a); 

    //verifica se passou tempo suficiente desde o ultimo evento
    if (current_time - last_time_0 > 5000000)    // 500ms de debounce
    {
        last_time_0 = current_time;          // Atualiza o tempo do ultimo evento
        printf("mudanca de estado do led. A = %d\n", a);

        a++;                                //Incrementa a variável de verificação
      
    }
}

// Função de Interrupção com Debounce para o botão1
void trata_button_1(uint gpio, uint32_t events)
{
    //obtem o tempo atual em microssegundos
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    printf("B = %d\n", b); 

    //verifica se passou tempo suficiente desde o ultimo evento
    if (current_time - last_time_1 > 500000)    // 500ms de debounce
    {
        last_time_1 = current_time;          // Atualiza o tempo do ultimo evento
        printf("mudanca de estado do led. B = %d\n", b);

        b++;                                //Incrementa a variável de verificação
        
    }
}

// Função para tocar uma nota
void tocar_nota(int duracao_ms, int frequencia_hz) {
    int periodo_us = 1000000 / frequencia_hz;
    int ciclos = (duracao_ms * 1000) / periodo_us;
    
    for(int i = 0; i < ciclos; i++) {
        gpio_put(BUZZER_PIN, 1);
        sleep_us(periodo_us/2);
        gpio_put(BUZZER_PIN, 0);
        sleep_us(periodo_us/2);
    }
}

// Função para tocar a melodia da Animação 01
void tocar_melodia_coracao() {
    // Notas: C4, E4, G4, C5
    tocar_nota(100, 262); // C4
    sleep_ms(50);
    tocar_nota(100, 330); // E4
    sleep_ms(50);
    tocar_nota(100, 392); // G4
    sleep_ms(50);
    tocar_nota(200, 523); // C5
}

// Função para tocar a melodia da Animação 02
void tocar_melodia_estrela() {
    // Notas: G4, B4, D5, G5
    tocar_nota(100, 392); // G4
    sleep_ms(50);
    tocar_nota(100, 494); // B4
    sleep_ms(50);
    tocar_nota(100, 587); // D5
    sleep_ms(50);
    tocar_nota(200, 784); // G5
}

//função principal
int main()
{
    PIO pio = pio0; 
    bool ok;
    uint16_t i;
    double r = 0.0, b = 0.0, g = 0.0;

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

    //inicializar o buzzer - GPIO21
    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);

    // Inicializa com todos os LEDs apagados
    desenho_pio(desenho, pio, sm, 0.0, 0.0, 0.0);

    //Interrupção da Gpio Botão ativada
    gpio_set_irq_enabled_with_callback(button_0, GPIO_IRQ_EDGE_FALL, true, &trata_button_0);
    gpio_set_irq_enabled_with_callback(button_1, GPIO_IRQ_EDGE_FALL, true, &trata_button_1);

   while (true) {
    // Leitura dos botões
    if (gpio_get(button_0) == 0) {
        animacao_ativa_0 = true;
    }
    if (gpio_get(button_1) == 0) {
        animacao_ativa_1 = true;
    }

    bool desenhou_alguma_animacao = false;

    // Animação do botão 0 (Coração pulsante vermelho)
    if (animacao_ativa_0) {
        desenho_pio(animacao_botao0.frames[frame_atual_0], pio, sm, 1.0, 0.0, 0.0);
        if (frame_atual_0 == 0) {
            tocar_melodia_coracao();
        }
        frame_atual_0++;
        desenhou_alguma_animacao = true;
        if (frame_atual_0 >= 20) {
            frame_atual_0 = 0;
            animacao_ativa_0 = false;
        }
    }

    // Animação do botão 1 (Estrela girando azul)
    if (animacao_ativa_1) {
        desenho_pio(animacao_botao1.frames[frame_atual_1], pio, sm, 0.0, 0.0, 1.0);
        if (frame_atual_1 == 0) {
            tocar_melodia_estrela();
        }
        frame_atual_1++;
        desenhou_alguma_animacao = true;
        if (frame_atual_1 >= 20) {
            frame_atual_1 = 0;
            animacao_ativa_1 = false;
        }
    }

    // Nenhuma animação - mantém apagado
    if (!desenhou_alguma_animacao) {
        desenho_pio(desenho, pio, sm, 0.0, 0.0, 0.0);
        gpio_put(BUZZER_PIN, 0);
    }

    sleep_ms(200);
  }
}