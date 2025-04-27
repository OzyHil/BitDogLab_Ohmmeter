#include "Led_Matrix.h" // Inclusão da biblioteca para controlar a matriz de LEDs

// Definições de cores RGB
const led_color DARK =   {.red = 0, .green = 0, .blue = 0};
const led_color BROWN =  {.red = 3, .green = 1, .blue = 1};
const led_color RED =    {.red = 3, .green = 0, .blue = 0};
const led_color ORANGE = {.red = 4, .green = 1, .blue = 0};
const led_color YELLOW = {.red = 4, .green = 2, .blue = 0};
const led_color GREEN =  {.red = 1, .green = 5, .blue = 0};
const led_color BLUE =   {.red = 0, .green = 0, .blue = 8};
const led_color PURPLE = {.red = 4, .green = 0, .blue = 7};
const led_color GRAY =   {.red = 1, .green = 1, .blue = 1};
const led_color WHITE =  {.red = 6, .green = 6, .blue = 6};
const led_color GOLDEN = {.red = 2, .green = 1, .blue = 0};
const led_color SILVER = {.red = 3, .green = 3, .blue = 3};

const int8_t matrix[NUM_PIXELS] ={
    0, 3, 3, 3, 0,
    0, 0, 0, 0, 0,
    0, 2, 2, 2, 0,
    0, 0, 0, 0, 0,
    0, 1, 1, 1, 0,
};

// Converte uma estrutura de cor RGB para um valor 32 bits conforme o protocolo da matriz
uint32_t rgb_matrix(led_color color){
    return (color.green << 24) | (color.red << 16) | (color.blue << 8);
}

void draw_resistor_color(refs pio){
    for (int16_t i = 0; i < NUM_PIXELS; i++)
    {
        // Envia a cor correspondente para cada valor da matriz
        switch (matrix[i])
        {
        case 1:
            pio_sm_put_blocking(pio.ref, pio.state_machine, rgb_matrix(WHITE));
            break;
        case 2:
            pio_sm_put_blocking(pio.ref, pio.state_machine, rgb_matrix(SILVER));
            break;
        case 3:
            pio_sm_put_blocking(pio.ref, pio.state_machine, rgb_matrix(GRAY));
            break;
        default:
            pio_sm_put_blocking(pio.ref, pio.state_machine, rgb_matrix(DARK));
        }
    }
}