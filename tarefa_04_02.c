#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "ws2812.pio.h"


// Constantes
#define I2C_PORT i2c1 // Define a porta I2C
#define I2C_SDA 14 // Pino SDA
#define I2C_SCL 15 // Pino SCL
#define endereco 0x3C // Endereço do I2C
ssd1306_t ssd; // Inicializa a estrutura do display

// GPIO
#define BOTAO_A 5 // Pino do botão A
#define BOTAO_B 6 // Pino do botão B
#define MATRIZ 7 // Pino da matriz de LEDs
#define NUM_LEDS 25 // Número de LEDs da matriz
#define LED_VERDE 11 // Pino do LED verde
#define LED_AZUL 12 // Pino do LED azul

static volatile uint32_t last_time = 0; // Armazena o tempo do último clique nos botões (debounce)

// Definição de pixel GRB
struct pixel_t {
    uint8_t G, R, B; // Três valores de 8-bits compõem um pixel.
};

typedef struct pixel_t pixel_t;
typedef pixel_t npLED_t; // Mudança de nome de "struct pixel_t" para "npLED_t" por clareza.

// Declaração do buffer de pixels que formam a matriz.
npLED_t leds[NUM_LEDS];

// Variáveis para uso da máquina PIO.
PIO np_pio;
uint sm;

/**
 * Atribui uma cor RGB a um LED.
 */
void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b) {
    leds[index].R = r;
    leds[index].G = g;
    leds[index].B = b;
}

/**
 * Limpa o buffer de pixels.
 */
void npClear() {
    for (uint i = 0; i < NUM_LEDS; ++i) {
        npSetLED(i, 0, 0, 0);
    }
}

/**
 * Escreve os dados do buffer nos LEDs.
 */
void npWrite() {
    for (uint i = 0; i < NUM_LEDS; ++i) {
        pio_sm_put_blocking(np_pio, sm, leds[i].G);
        pio_sm_put_blocking(np_pio, sm, leds[i].R);
        pio_sm_put_blocking(np_pio, sm, leds[i].B);
    }
}

// Função para converter a posição da matriz para uma posição do vetor.
int getIndex(int x, int y) {
    // Se a linha for par (0, 2, 4), percorremos da esquerda para a direita.
    // Se a linha for ímpar (1, 3), percorremos da direita para a esquerda.
    if (y % 2 == 0) {
        return 24-(y * 5 + x); // Linha par (esquerda para direita).
    } else {
        return 24-(y * 5 + (4 - x)); // Linha ímpar (direita para esquerda).
    }
}

// Função que guarda os frames dos números
void digito_matriz(char digito){
    npClear();
    switch (digito){
        case '0': {
            // Frame 0
            int frame0[5][5][3] = {
                {{0, 0, 0}, {100, 100, 100}, {100, 100, 100}, {100, 100, 100}, {0, 0, 0}},
                {{0, 0, 0}, {100, 100, 100}, {0, 0, 0}, {100, 100, 100}, {0, 0, 0}},    
                {{0, 0, 0}, {100, 100, 100}, {0, 0, 0}, {100, 100, 100}, {0, 0, 0}},
                {{0, 0, 0}, {100, 100, 100}, {0, 0, 0}, {100, 100, 100}, {0, 0, 0}},
                {{0, 0, 0}, {100, 100, 100}, {100, 100, 100}, {100, 100, 100}, {0, 0, 0}}
            };
            for (int linha = 0; linha < 5; linha++)
            {
                for (int coluna = 0; coluna < 5; coluna++)
                {
                int posicao = getIndex(linha, coluna);
                npSetLED(posicao, frame0[coluna][linha][0], frame0[coluna][linha][1], frame0[coluna][linha][2]);
                }
            };
            npWrite();
            break;
        }
        case '1':{
            // Frame 1
            int frame1[5][5][3] = {
                {{0, 0, 0}, {0, 0, 0}, {100, 100, 100}, {0, 0, 0}, {0, 0, 0}},
                {{0, 0, 0}, {100, 100, 100}, {100, 100, 100}, {0, 0, 0}, {0, 0, 0}},
                {{0, 0, 0}, {0, 0, 0}, {100, 100, 100}, {0, 0, 0}, {0, 0, 0}},
                {{0, 0, 0}, {0, 0, 0}, {100, 100, 100}, {0, 0, 0}, {0, 0, 0}},
                {{0, 0, 0}, {100, 100, 100}, {100, 100, 100}, {100, 100, 100}, {0, 0, 0}}
            };
            for (int linha = 0; linha < 5; linha++)
            {
                for (int coluna = 0; coluna < 5; coluna++)
                {
                int posicao = getIndex(linha, coluna);
                npSetLED(posicao, frame1[coluna][linha][0], frame1[coluna][linha][1], frame1[coluna][linha][2]);
                }
            };
            npWrite();
            break;
        }
        case '2':{
            // Frame 2
            int frame2[5][5][3] = {
                {{0, 0, 0}, {100, 100, 100}, {100, 100, 100}, {100, 100, 100}, {0, 0, 0}},
                {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {100, 100, 100}, {0, 0, 0}},        
                {{0, 0, 0}, {100, 100, 100}, {100, 100, 100}, {100, 100, 100}, {0, 0, 0}},
                {{0, 0, 0}, {100, 100, 100}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
                {{0, 0, 0}, {100, 100, 100}, {100, 100, 100}, {100, 100, 100}, {0, 0, 0}}
            };
            for (int linha = 0; linha < 5; linha++)
            {
                for (int coluna = 0; coluna < 5; coluna++)
                {
                int posicao = getIndex(linha, coluna);
                npSetLED(posicao, frame2[coluna][linha][0], frame2[coluna][linha][1], frame2[coluna][linha][2]);
                }
            };
            npWrite();
            break;
        }
        case '3':{
            // Frame 3
            int frame3[5][5][3] = {
                {{0, 0, 0}, {100, 100, 100}, {100, 100, 100}, {100, 100, 100}, {0, 0, 0}},
                {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {100, 100, 100}, {0, 0, 0}},
                {{0, 0, 0}, {100, 100, 100}, {100, 100, 100}, {100, 100, 100}, {0, 0, 0}},
                {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {100, 100, 100}, {0, 0, 0}},
                {{0, 0, 0}, {100, 100, 100}, {100, 100, 100}, {100, 100, 100}, {0, 0, 0}}
            };
            for (int linha = 0; linha < 5; linha++)
            {
                for (int coluna = 0; coluna < 5; coluna++)
                {
                int posicao = getIndex(linha, coluna);
                npSetLED(posicao, frame3[coluna][linha][0], frame3[coluna][linha][1], frame3[coluna][linha][2]);
                }
            };
            npWrite();
            break;
        }
        case '4':{
            // Frame 4
            int frame4[5][5][3] = {
                {{0, 0, 0}, {100, 100, 100}, {0, 0, 0}, {100, 100, 100}, {0, 0, 0}},
                {{0, 0, 0}, {100, 100, 100}, {0, 0, 0}, {100, 100, 100}, {0, 0, 0}},    
                {{0, 0, 0}, {100, 100, 100}, {100, 100, 100}, {100, 100, 100}, {0, 0, 0}},
                {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {100, 100, 100}, {0, 0, 0}},
                {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {100, 100, 100}, {0, 0, 0}} 
            };
            for (int linha = 0; linha < 5; linha++)
            {
                for (int coluna = 0; coluna < 5; coluna++)
                {
                int posicao = getIndex(linha, coluna);
                npSetLED(posicao, frame4[coluna][linha][0], frame4[coluna][linha][1], frame4[coluna][linha][2]);
                }
            };
            npWrite();
            break;
        }
        case '5':{
            // Frame 5
            int frame5[5][5][3] = {
                {{0, 0, 0}, {100, 100, 100}, {100, 100, 100}, {100, 100, 100}, {0, 0, 0}},
                {{0, 0, 0}, {100, 100, 100}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
                {{0, 0, 0}, {100, 100, 100}, {100, 100, 100}, {100, 100, 100}, {0, 0, 0}},
                {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {100, 100, 100}, {0, 0, 0}},
                {{0, 0, 0}, {100, 100, 100}, {100, 100, 100}, {100, 100, 100}, {0, 0, 0}}
            };
            for (int linha = 0; linha < 5; linha++)
            {
                for (int coluna = 0; coluna < 5; coluna++)
                {
                int posicao = getIndex(linha, coluna);
                npSetLED(posicao, frame5[coluna][linha][0], frame5[coluna][linha][1], frame5[coluna][linha][2]);
                }
            };
            npWrite();
            break;
        }
        case '6':{
            // Frame 6
            int frame6[5][5][3] = {
                {{0, 0, 0}, {100, 100, 100}, {100, 100, 100}, {100, 100, 100}, {0, 0, 0}},
                {{0, 0, 0}, {100, 100, 100}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},        
                {{0, 0, 0}, {100, 100, 100}, {100, 100, 100}, {100, 100, 100}, {0, 0, 0}},
                {{0, 0, 0}, {100, 100, 100}, {0, 0, 0}, {100, 100, 100}, {0, 0, 0}},    
                {{0, 0, 0}, {100, 100, 100}, {100, 100, 100}, {100, 100, 100}, {0, 0, 0}}
            };
            for (int linha = 0; linha < 5; linha++)
            {
                for (int coluna = 0; coluna < 5; coluna++)
                {
                int posicao = getIndex(linha, coluna);
                npSetLED(posicao, frame6[coluna][linha][0], frame6[coluna][linha][1], frame6[coluna][linha][2]);
                }
            };
            npWrite();
            break;
        }
        case '7':{
            // Frame 7
            int frame7[5][5][3] = {
                {{0, 0, 0}, {100, 100, 100}, {100, 100, 100}, {100, 100, 100}, {0, 0, 0}},
                {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {100, 100, 100}, {0, 0, 0}},
                {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {100, 100, 100}, {0, 0, 0}},
                {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {100, 100, 100}, {0, 0, 0}},
                {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {100, 100, 100}, {0, 0, 0}} 
            };
            for (int linha = 0; linha < 5; linha++)
            {
                for (int coluna = 0; coluna < 5; coluna++)
                {
                int posicao = getIndex(linha, coluna);
                npSetLED(posicao, frame7[coluna][linha][0], frame7[coluna][linha][1], frame7[coluna][linha][2]);
                }
            };
            npWrite();
            break;
        }
        case '8':{
            // Frame 8
            int frame8[5][5][3] = {
                {{0, 0, 0}, {100, 100, 100}, {100, 100, 100}, {100, 100, 100}, {0, 0, 0}},
                {{0, 0, 0}, {100, 100, 100}, {0, 0, 0}, {100, 100, 100}, {0, 0, 0}},    
                {{0, 0, 0}, {100, 100, 100}, {100, 100, 100}, {100, 100, 100}, {0, 0, 0}},
                {{0, 0, 0}, {100, 100, 100}, {0, 0, 0}, {100, 100, 100}, {0, 0, 0}},
                {{0, 0, 0}, {100, 100, 100}, {100, 100, 100}, {100, 100, 100}, {0, 0, 0}}
            };
            for (int linha = 0; linha < 5; linha++)
            {
                for (int coluna = 0; coluna < 5; coluna++)
                {
                int posicao = getIndex(linha, coluna);
                npSetLED(posicao, frame8[coluna][linha][0], frame8[coluna][linha][1], frame8[coluna][linha][2]);
                }
            };
            npWrite();
            break;
        }
        case '9':{
            // Frame 9
            int frame9[5][5][3] = {
                {{0, 0, 0}, {100, 100, 100}, {100, 100, 100}, {100, 100, 100}, {0, 0, 0}},
                {{0, 0, 0}, {100, 100, 100}, {0, 0, 0}, {100, 100, 100}, {0, 0, 0}},    
                {{0, 0, 0}, {100, 100, 100}, {100, 100, 100}, {100, 100, 100}, {0, 0, 0}},
                {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {100, 100, 100}, {0, 0, 0}},       
                {{0, 0, 0}, {100, 100, 100}, {100, 100, 100}, {100, 100, 100}, {0, 0, 0}}
            };
            for (int linha = 0; linha < 5; linha++)
            {
                for (int coluna = 0; coluna < 5; coluna++)
                {
                int posicao = getIndex(linha, coluna);
                npSetLED(posicao, frame9[coluna][linha][0], frame9[coluna][linha][1], frame9[coluna][linha][2]);
                }
            };
            npWrite();
            break;
        }
        default:
            break;
    }
}

// Função de callback de interrupção
void gpio_irq_handler(uint gpio, uint32_t events){

    // Debounce
    uint32_t current_time = to_us_since_boot(get_absolute_time()); // Pega o tempo atual e transforma em us

    if(current_time - last_time > 200000){
        last_time = current_time; // Atualização de tempo do último clique

        if(gpio == BOTAO_A){
            gpio_put(LED_VERDE, !gpio_get(LED_VERDE));
            if(gpio_get(LED_VERDE) == 1){
                printf("O LED Verde foi aceso!\n");
                ssd1306_draw_string(&ssd, "LED Verde  Lig", 8, 28);
                ssd1306_send_data(&ssd);
            }else{
                printf("O LED Verde foi apagado!\n");
                ssd1306_draw_string(&ssd, "LED Verde  Des", 8, 28);
                ssd1306_send_data(&ssd);
            }
        }else if(gpio == BOTAO_B){
            gpio_put(LED_AZUL, !gpio_get(LED_AZUL));
            if(gpio_get(LED_AZUL) == 1){
                printf("O LED Azul foi aceso!\n");
                ssd1306_draw_string(&ssd, "LED Azul   Lig", 8, 52);
                ssd1306_send_data(&ssd); 
            }else{
                printf("O LED Azul foi apagado!\n");
                ssd1306_draw_string(&ssd, "LED Azul   Des", 8, 52);
                ssd1306_send_data(&ssd);
            }
        }
    }
}

int main()
{
    stdio_init_all(); // Inicia o monitor serial

    i2c_init(I2C_PORT, 400 * 1000); // Inicializa o I2C usando 400kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Define o pino SDA como I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Define o pino SCL como I2C
    gpio_pull_up(I2C_SDA); // Ativa o pull up para o pino SDA
    gpio_pull_up(I2C_SCL); // Ativa o pull up para o pino SCL
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd); // Configura o display
    ssd1306_send_data(&ssd); // Envia os dados para o display

    ssd1306_fill(&ssd, false); // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_send_data(&ssd); // Envia os dados para o display

    gpio_init(BOTAO_A); // Inicia o botão A
    gpio_set_dir(BOTAO_A, GPIO_IN); // Define o botão Acomo entrada
    gpio_pull_up(BOTAO_A); // Ativa o pull up para o botão A

    gpio_init(BOTAO_B); // Inicia o botão B
    gpio_set_dir(BOTAO_B, GPIO_IN); // Define o botão B como entrada
    gpio_pull_up(BOTAO_B); // Ativa o pull up para o botão B

    gpio_init(LED_VERDE); // Inicia o LED Verde
    gpio_set_dir(LED_VERDE, GPIO_OUT); // Define o LED Verde como saída

    gpio_init(LED_AZUL); // Inicia o LED Azul
    gpio_set_dir(LED_AZUL, GPIO_OUT); // Define o LED Azul como saída

    np_pio = pio0;
    sm = pio_claim_unused_sm(np_pio, true);
    uint offset = pio_add_program(pio0, &ws2818b_program);
    ws2818b_program_init(np_pio, sm, offset, MATRIZ, 800000);
    npClear(); // Para limpar o buffer dos LEDs

    // Desenha inicialmente o display
    ssd1306_draw_string(&ssd, "Char", 32, 4);
    ssd1306_draw_string(&ssd, "LED Verde  Des", 8, 28);
    ssd1306_draw_string(&ssd, "LED Azul   Des", 8, 52);
    ssd1306_send_data(&ssd);

    // Interrupção
    gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BOTAO_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    while (true) {
        if(stdio_usb_connected()){
            // Recebe o caractere digitado mostra no monitor serial e desenha no display
            char c;
            if (scanf("%c", &c) == 1){
                printf("Caractere: '%c'\n", c);
                ssd1306_draw_char(&ssd, c, 80, 4);
                ssd1306_send_data(&ssd);
                if(c >= '0' && c <= '9'){
                    digito_matriz(c);
                }
            }
        }
        sleep_ms(40);
    }
}