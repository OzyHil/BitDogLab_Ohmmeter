#include "General.h"

// Função para inicializar a configuração do sistema
void init_system_config(){
    // Inicializa a biblioteca de entrada/saída padrão (stdio), permitindo uso de printf/scanf
    stdio_init_all();

    // Inicializa o conversor analógico-digital (ADC), usado para ler valores analógicos
    adc_init();

    // Configura o relógio do sistema para 128 kHz (sistema de 32 bits com precisão de tempo)
    set_sys_clock_khz(128000, false);
}

// Função para configurar o GPIO como entrada ou saída, com a opção de ativar o resistor de pull-up
void configure_gpio(uint gpio, bool is_input, bool use_pull_up){
    gpio_init(gpio); // Inicializa o GPIO especificado

    gpio_set_dir(gpio, is_input ? GPIO_IN : GPIO_OUT); // Define a direção do GPIO (entrada ou saída)

    // Se for uma entrada e o pull-up for necessário, ativa o pull-up no GPIO
    if (is_input && use_pull_up)
    {
        gpio_pull_up(gpio);
    }
}

// Função para configurar o display ssd 1306
void configure_display(ssd1306_t *ssd){
    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400 * 1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);                   // Set the GPIO pin function to I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);                   // Set the GPIO pin function to I2C
    gpio_pull_up(I2C_SDA);                                       // Pull up the data line
    gpio_pull_up(I2C_SCL);                                       // Pull up the clock line                                              
    ssd1306_init(ssd, WIDTH, HEIGHT, false, ADDRESS, I2C_PORT); // Inicializa o display
    ssd1306_config(ssd);                                        // Configura o display
    ssd1306_send_data(ssd);                                     // Envia os dados para o display
}

// Função para inicializar o PIO e configurar a máquina de estado com o programa da matriz de LEDs
refs init_pio()
{
    refs pio;
    pio.ref = pio0; // Usa o PIO 0

    pio.state_machine = pio_claim_unused_sm(pio.ref, true);     // Obtém uma máquina de estado livre
    pio.offset = pio_add_program(pio.ref, &pio_matrix_program); // Adiciona o programa da matriz

    pio_matrix_program_init(pio.ref, pio.state_machine, pio.offset, LED_MATRIX); // Inicializa o programa
    return pio;
}
float e24_values[] = {
    1.0, 1.1, 1.2, 1.3, 1.5, 
    1.6, 1.8, 2.0, 2.2, 2.4, 
    2.7, 3.0, 3.3, 3.6, 3.9, 
    4.3, 4.7, 5.1, 5.6, 6.2, 
    6.8, 7.5, 8.2, 9.1, 10.0
};

float find_nearest_e24(float resistance) {
    if (resistance <= 0) return 0; 
    
    float exponent = floor(log10(resistance));
    float power_of_ten = pow(10, exponent);
    
    float normalized = resistance / power_of_ten;
    
    float min_diff = INFINITY;
    float closest_value = 0;
    
    for (int i = 0; i < 25; i++) {
        float diff = fabsf(normalized - e24_values[i]);
        if (diff < min_diff) {
            min_diff = diff;
            closest_value = e24_values[i];
        }
    }
    
    if (closest_value == 10.0) {
        return 1.0 * power_of_ten * 10;
    } else {
        return closest_value * power_of_ten;
    }
}
float calculate_error_percentage(float measured_value, float e24_value) {
    return (fabsf(measured_value - e24_value) / e24_value) * 100.0f;
}

resistor_digits extract_digits(float resistance) {
    resistor_digits result;
    
    // Determinar quantos dígitos temos antes do ponto decimal
    int num_digits = 0;
    float temp = resistance;
    while (temp >= 1.0) {
        temp /= 10.0;
        num_digits++;
    }
    
    // Normalizar para um valor entre 1.0 e 99.9
    float normalized = resistance / pow(10, num_digits - 2);
    
    // Extrair os dígitos
    int normalized_int = (int)round(normalized);
    
    result.first_digit = normalized_int / 10;
    result.second_digit = normalized_int % 10;
    result.multiplier = num_digits - 2;
    
    return result;
}