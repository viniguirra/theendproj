#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "pico/cyw43_arch.h"
#include "lwip/apps/mqtt.h"

// Definição dos pinos
#define POT_PIN 26          // Pino GP26 para o potenciômetro (entrada analógica)
#define LED_NORMAL_PIN 16   // Pino GP16 para o LED verde (status normal)
#define LED_ALERTA_PIN 17   // Pino GP17 para o LED vermelho (alerta)
#define BUZZER_PIN 18       // Pino GP18 para o buzzer

// Configuração do Wi-Fi
#define WIFI_SSID "SUA_REDE_WIFI"
#define WIFI_PASSWORD "SUA_SENHA_WIFI"

// Configuração do MQTT
#define MQTT_BROKER "test.mosquitto.org"
#define MQTT_TOPIC "pico/temperatura"

// Variáveis globais
float temperatura = 0.0;
bool alerta = false;

// Função para inicializar o ADC (leitura do potenciômetro)
void init_adc() {
    adc_init();
    adc_gpio_init(POT_PIN);
    adc_select_input(0);  // Seleciona o canal ADC0 (GP26)
}

// Função para ler o valor do potenciômetro e converter para temperatura
float ler_temperatura() {
    uint16_t valor_adc = adc_read();  // Lê o valor do ADC (0-4095)
    temperatura = (valor_adc / 4095.0) * 30.0 + 10.0;  // Converte para uma faixa de 10°C a 40°C
    printf("Temperatura: %.2f°C\n", temperatura);
    return temperatura;
}

// Função para verificar anomalias (temperatura > 30°C)
void verificar_anomalias() {
    if (temperatura > 30.0) {
        alerta = true;
        gpio_put(LED_ALERTA_PIN, 1);  // Acende o LED vermelho
        gpio_put(LED_NORMAL_PIN, 0);   // Apaga o LED verde
        pwm_set_gpio_level(BUZZER_PIN, 32768);  // Ativa o buzzer (50% de duty cycle)
    } else {
        alerta = false;
        gpio_put(LED_ALERTA_PIN, 0);  // Apaga o LED vermelho
        gpio_put(LED_NORMAL_PIN, 1);   // Acende o LED verde
        pwm_set_gpio_level(BUZZER_PIN, 0);  // Desativa o buzzer
    }
}

// Função para conectar ao Wi-Fi
void conectar_wifi() {
    if (cyw43_arch_init()) {
        printf("Erro ao inicializar o Wi-Fi\n");
        return;
    }
    cyw43_arch_enable_sta_mode();
    printf("Conectando ao Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
        printf("Erro ao conectar ao Wi-Fi\n");
        return;
    }
    printf("Conectado ao Wi-Fi\n");
}

// Função para enviar dados ao servidor MQTT
void enviar_dados_mqtt() {
    char mensagem[100];
    snprintf(mensagem, sizeof(mensagem), "{\"temperatura\": %.2f, \"alerta\": %s}", temperatura, alerta ? "true" : "false");
    printf("Enviando dados: %s\n", mensagem);

    // Aqui você pode implementar a lógica para enviar os dados ao servidor MQTT
    // usando a biblioteca MQTT do lwIP ou outra biblioteca de sua preferência.
}

// Função principal
int main() {
    stdio_init_all();  // Inicializa a comunicação serial (USB/UART)

    // Inicializa os GPIOs
    gpio_init(LED_NORMAL_PIN);
    gpio_init(LED_ALERTA_PIN);
    gpio_set_dir(LED_NORMAL_PIN, GPIO_OUT);
    gpio_set_dir(LED_ALERTA_PIN, GPIO_OUT);

    // Inicializa o PWM para o buzzer
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    pwm_set_wrap(slice_num, 65535);
    pwm_set_enabled(slice_num, true);

    // Inicializa o ADC
    init_adc();

    // Conecta ao Wi-Fi
    conectar_wifi();

    // Loop principal
    while (true) {
        ler_temperatura();  // Lê a temperatura
        verificar_anomalias();  // Verifica se há anomalias
        enviar_dados_mqtt();  // Envia os dados ao servidor MQTT
        sleep_ms(2000);  // Aguarda 2 segundos antes da próxima leitura
    }

    return 0;
}
