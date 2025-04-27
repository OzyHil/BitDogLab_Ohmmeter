#ifndef LED_MATRIX_H
#define LED_MATRIX_H

#include "General.h"

#define NUM_PIXELS 25 // Total de LEDs na matriz (5x5)

// Estrutura para representar uma cor RGB
typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} led_color;

// Converte uma estrutura de cor RGB para um valor 32 bits
uint32_t rgb_matrix(led_color color);

// Função para desenhar as cores do resistor, com base nos dígitos
void draw_resistor_color(refs pio, resistor_digits digits);

// Função para obter o nome da cor com base no dígito
const char* get_color_name(int digit);

#endif
