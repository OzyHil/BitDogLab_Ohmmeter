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

// Converte uma cor RGB para o formato aceito pela matriz
uint32_t rgb_matrix(led_color color);

void draw_resistor_color(refs pio);
#endif
