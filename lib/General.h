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
#include "lib/ssd1306.h"
#include "pico/bootrom.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define ADDRESS 0x3C
#define ADC_PIN 28 // GPIO para o voltímetro
#define BUTTON_B 6
#define LED_MATRIX 7 // GPIO para a matriz de LEDs

// Estrutura para armazenar referências do PIO
typedef struct
{
    PIO ref;              // Referência ao PIO (pio0 ou pio1)
    uint offset;          // Offset do programa carregado
    uint state_machine;   // Máquina de estado usada
} refs;

// Função para inicializar a configuração do sistema (clocks, I/O, etc.)
void init_system_config();

// Função para configurar o GPIO (entrada ou saída com pull-up opcional)
void configure_gpio(uint gpio, bool is_input, bool use_pull_up);

// Função para configurar o o display 
void configure_display(ssd1306_t *ssd);

refs init_pio();

#endif