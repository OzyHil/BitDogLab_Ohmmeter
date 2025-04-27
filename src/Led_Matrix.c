#include "Led_Matrix.h" // Inclusão da biblioteca para controlar a matriz de LEDs

// Matriz de cores para os dígitos (0-9)
const led_color DIGIT_COLORS[10] = {
    {.red = 0, .green = 0, .blue = 0},    // 0 - Preto
    {.red = 3, .green = 1, .blue = 1},    // 1 - Marrom
    {.red = 3, .green = 0, .blue = 0},    // 2 - Vermelho
    {.red = 4, .green = 1, .blue = 0},    // 3 - Laranja
    {.red = 4, .green = 2, .blue = 0},    // 4 - Amarelo
    {.red = 1, .green = 5, .blue = 0},    // 5 - Verde
    {.red = 0, .green = 0, .blue = 8},    // 6 - Azul
    {.red = 4, .green = 0, .blue = 7},    // 7 - Roxo
    {.red = 1, .green = 1, .blue = 1},    // 8 - Cinza
    {.red = 6, .green = 6, .blue = 6}     // 9 - Branco
};
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

void draw_resistor_color(refs pio, resistor_digits digits){
    for (int16_t i = 0; i < NUM_PIXELS; i++)
    {
        // Envia a cor correspondente para cada valor da matriz
        switch (matrix[i])
        {
        case 1:
            pio_sm_put_blocking(pio.ref, pio.state_machine, rgb_matrix(DIGIT_COLORS[digits.first_digit]));
            break;
        case 2:
            pio_sm_put_blocking(pio.ref, pio.state_machine, rgb_matrix(DIGIT_COLORS[digits.second_digit]));
            break;
        case 3:
            pio_sm_put_blocking(pio.ref, pio.state_machine, rgb_matrix(DIGIT_COLORS[digits.multiplier]));
            break;
        default:
            pio_sm_put_blocking(pio.ref, pio.state_machine, rgb_matrix(DIGIT_COLORS[0]));
        }
    }
}

// Array com os nomes das cores para os dígitos
const char* COLOR_NAMES[10] = {
    "Black",   // 0
    "Brown",   // 1
    "Red",     // 2
    "Orange",  // 3
    "Yellow",  // 4
    "Green",   // 5
    "Blue",    // 6
    "Purple",  // 7
    "Gray",    // 8
    "White"    // 9
};

// Função para obter o nome da cor do primeiro dígito
const char* get_color_name(int digit) {
    if (digit >= 0 && digit <= 9) {
        return COLOR_NAMES[digit];
    }
    return "";
}