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