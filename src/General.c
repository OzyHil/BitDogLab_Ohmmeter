#include "General.h"
#include "Led_Matrix.h"

// Função para configurar o GPIO como entrada ou saída, com a opção de ativar o resistor de pull-up
void configure_gpio(uint gpio, bool is_input, bool use_pull_up)
{
    gpio_init(gpio); // Inicializa o GPIO especificado

    gpio_set_dir(gpio, is_input ? GPIO_IN : GPIO_OUT); // Define a direção do GPIO (entrada ou saída)

    // Se for uma entrada e o pull-up for necessário, ativa o pull-up no GPIO
    if (is_input && use_pull_up)
    {
        gpio_pull_up(gpio);
    }
}

// Função para configurar o display
void configure_display(ssd1306_t *ssd)
{
    i2c_init(I2C_PORT, 400 * 1000); // Inicializa o I2C à 400khz

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
    gpio_pull_up(I2C_SDA);                     // Pull up the data line
    gpio_pull_up(I2C_SCL);                     // Pull up the clock line

    ssd1306_init(ssd, WIDTH, HEIGHT, false, ADDRESS, I2C_PORT); // Inicializa o display
    ssd1306_config(ssd);                                        // Configura o display
    ssd1306_send_data(ssd);                                     // Envia os dados para o display
}

// Função para inicializar o PIO e configurar a máquina de estado com o programa da matriz de LEDs
refs init_pio()
{
    refs pio;
    pio.ref = pio0;

    pio.state_machine = pio_claim_unused_sm(pio.ref, true);     // Obtém uma máquina de estado livre
    pio.offset = pio_add_program(pio.ref, &pio_matrix_program); // Adiciona o programa da matriz

    pio_matrix_program_init(pio.ref, pio.state_machine, pio.offset, LED_MATRIX); // Inicializa o programa
    return pio;
}

// Valores padrão dos resistores E24 (valores de 1 a 10 multiplicados por potências de 10)
float e24_values[] = {
    1.0, 1.1, 1.2, 1.3, 1.5,
    1.6, 1.8, 2.0, 2.2, 2.4,
    2.7, 3.0, 3.3, 3.6, 3.9,
    4.3, 4.7, 5.1, 5.6, 6.2,
    6.8, 7.5, 8.2, 9.1, 10.0};

// Função para encontrar o valor de resistor mais próximo da série E24
float find_nearest_e24(float resistance)
{
    if (resistance <= 0)
        return 0;

    // Calcula a potência de 10 mais próxima da resistência
    float exponent = floor(log10(resistance));
    float power_of_ten = pow(10, exponent);

    float normalized = resistance / power_of_ten; // Normaliza a resistência para um valor entre 1 e 10

    // Inicializa variáveis para armazenar o valor mínimo da diferença e o valor mais próximo
    float min_diff = INFINITY;
    float closest_value = 0;

    // Compara a resistência normalizada com os valores da série E24
    for (int i = 0; i < 25; i++)
    {
        float diff = fabsf(normalized - e24_values[i]); // Calcula a diferença absoluta

        if (diff < min_diff) // Se a diferença for menor que a anterior, atualiza o valor mais próximo
        {
            min_diff = diff;
            closest_value = e24_values[i];
        }
    }

    // Se o valor mais próximo for 10.0, é necessário multiplicar por 10
    if (closest_value == 10.0)
    {
        return 1.0 * power_of_ten * 10;
    }
    else
    {
        return closest_value * power_of_ten;
    }
}

// Função para calcular a porcentagem de erro entre o valor medido e o valor E24
float calculate_error_percentage(float measured_value, float e24_value)
{
    return (fabsf(measured_value - e24_value) / e24_value) * 100.0f;
}

// Função para extrair os dígitos do valor da resistência para exibição
resistor_digits extract_digits(float resistance)
{
    resistor_digits result;

    // Determinar quantos dígitos temos antes do ponto decimal
    int num_digits = 0;
    float temp = resistance;
    while (temp >= 1.0)
    {
        temp /= 10.0;
        num_digits++;
    }

    // Normalizar para um valor entre 1.0 e 99.9
    float normalized = resistance / pow(10, num_digits - 2);

    // Converte o valor normalizado para inteiro arredondado
    int normalized_int = (int)round(normalized);

    // Extrai os três dígitos: primeiro, segundo e o multiplicador
    result.first_digit = normalized_int / 10; // Segunda casa decimal
    result.second_digit = normalized_int % 10; // Segunda casa decimal
    result.multiplier = num_digits - 2; // Multiplicador de potência de 10 (e.g., 10^2, 10^3)

    return result;
}

// Cáucula a média ponderada de de n "samples" levando (sample * 0,0005) segundos
float calculate_average_adc(uint adc_input, int samples)
{
    float sum = 0.0f;
    adc_select_input(adc_input);
    for (int i = 0; i < samples; i++)
    {
        sum += adc_read();
        sleep_us(500);
    }
    return sum / (float)samples;
}

// Prencher o display com as informações
void update_resistor_display(ssd1306_t *ssd, bool color, resistor_digits digits, const char *adc, const char *resistance, const char *error)
{
    ssd1306_fill(ssd, !color); // Limpa o display

    ssd1306_rect(ssd, 3, 3, 122, 60, color, !color); // Desenha a borda retângular

    //============= (Seção 1) =============//
    // Desenha os números 1,2 e 3 seguindos de um ponto
    ssd1306_draw_string(ssd, "1", 8, 8);
    ssd1306_rect(ssd, 13, 17, 2, 2, color, !color);
    ssd1306_draw_string(ssd, "2", 8, 17);
    ssd1306_rect(ssd, 22, 17, 2, 2, color, !color);
    ssd1306_draw_string(ssd, "3", 8, 26);
    ssd1306_rect(ssd, 31, 17, 2, 2, color, !color);

    // Escreve as cores do resistor
    ssd1306_draw_string(ssd, get_color_name(digits.first_digit), 25, 8);
    ssd1306_draw_string(ssd, get_color_name(digits.second_digit), 25, 17);
    ssd1306_draw_string(ssd, get_color_name(digits.multiplier), 25, 26);

    ssd1306_line(ssd, 85, 4, 85, 36, color); // Linha vertical que divide as seções 1 e 2

    //============= (Seção 2) =============//
    ssd1306_draw_string(ssd, "E(%)", 90, 8); // Desenha uma string
    ssd1306_draw_string(ssd, error, 90, 26); // Desenha o valor da porcentagem de erro

    ssd1306_line(ssd, 3, 37, 123, 37, color); // Linha que divide as seções 1,2 e 3

    //============= (Seção 3) =============//
    ssd1306_draw_string(ssd, "ADC", 22, 41); // Desenha uma string
    ssd1306_draw_string(ssd, adc, 10, 52);   // Desenha o valor do ADC

    ssd1306_line(ssd, 64, 37, 64, 60, color); // Desenha uma linha vertical que divide as seções 3 e 4

    //============= (Seção 4) =============//
    ssd1306_draw_string(ssd, "Ohm", 83, 41);      // Desenha uma string
    ssd1306_draw_string(ssd, resistance, 68, 52); // Desenha o valor da resistência

    ssd1306_send_data(ssd); // Atualiza o display
}