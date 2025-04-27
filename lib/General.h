#ifndef GENERAL_H
#define GENERAL_H

// Inclusão das bibliotecas padrão e específicas do hardware
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"    // Biblioteca principal para o Raspberry Pi Pico
#include "hardware/adc.h"   // Controle de ADC (Analog to Digital Converter)
#include "hardware/pio.h"   // Programação de E/S PIO (Programmable I/O)
#include "pio_matrix.pio.h" // Programa específico para controle da matriz de LEDs
#include "hardware/clocks.h" // Controle de clocks
#include "hardware/i2c.h"   // Comunicação I2C
#include "ssd1306.h"
#include <math.h>

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define ADDRESS 0x3C
#define ADC_PIN 28 // GPIO para o ohmimetro
#define BUTTON_B 6 // GPIO para o b0tão 6
#define DEBOUNCE_DELAY 300000 // Limiar de tempo para o debounce
#define LED_MATRIX 7 // GPIO para a matriz de LEDs
#define ADC_USB 4095 // Valor do ADC máximo quando alimentado via USB
#define ADC_BATTERY 3135 // Valor do ADC máximo quando alimentado via USB

// Estrutura para armazenar referências do PIO
typedef struct
{
    PIO ref;              // Referência ao PIO (pio0 ou pio1)
    uint offset;          // Offset do programa carregado
    uint state_machine;   // Máquina de estado usada
} refs;

// Estrutura para armazenar os dígitos e o multiplicador
typedef struct {
    int first_digit;    // Primeiro dígito significativo
    int second_digit;   // Segundo dígito significativo
    int multiplier;     // Multiplicador (potência de 10)
} resistor_digits;

// Função para configurar o GPIO (entrada ou saída com pull-up opcional)
void configure_gpio(uint gpio, bool is_input, bool use_pull_up);

// Função para configurar o o display 
void configure_display(ssd1306_t *ssd);

// Função para inicializar o PIO e configurar a máquina de estado com o programa da matriz de LEDs
refs init_pio();

// Função para encontrar o valor de resistor mais próximo da série E24
float find_nearest_e24(float resistance);

// Função para calcular a porcentagem de erro entre o valor medido e o valor E24
float calculate_error_percentage(float measured_value, float e24_value);

// Função para extrair os dígitos do valor da resistência para exibição
resistor_digits extract_digits(float resistance);

// Cáucula a média ponderada de de n "samples" levando (sample * 0,0005) segundos
float calculate_average_adc(uint adc_input, int samples);

// Prencher o display com as informações
void update_resistor_display(ssd1306_t *ssd, bool color, resistor_digits digits, const char *adc, const char *resistance, const char *error);
#endif