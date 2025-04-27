#include "General.h"
#include "Led_Matrix.h"

int R_conhecido = 10000;   // Resistor de 10k ohm
float R_x = 0.0;           // Resistor desconhecido
int ADC_RESOLUTION = 4095; // Resolução do ADC (12 bits)
float nearest_resistor = 0.0f;
float error_percentage = 0.0f;

// Trecho para modo BOOTSEL com botão B
void gpio_irq_handler(uint gpio, uint32_t events)
{
  reset_usb_boot(0, 0);
}

int main()
{
  // Para ser utilizado o modo BOOTSEL com botão B
  gpio_init(BUTTON_B);
  gpio_set_dir(BUTTON_B, GPIO_IN);
  gpio_pull_up(BUTTON_B);
  gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

  ssd1306_t ssd; // Estrutura que representa o display OLED SSD1306
  refs pio = init_pio();

  init_system_config();
  configure_display(&ssd);

  // Limpa o display. O display inicia com todos os pixels apagados.
  ssd1306_fill(&ssd, false);
  ssd1306_send_data(&ssd);

  adc_gpio_init(ADC_PIN); // GPIO 28 como entrada analógica

  char adc[8];        // Buffer para armazenar a string
  char resistance[8]; // Buffer para armazenar a string
  char nearest_str[8];
  char error[8];

  bool color = true;

  while (true)
  {
    adc_select_input(2); // Seleciona o ADC para eixo X. O pino 28 como entrada analógica

    float sum = 0.0f;
    for (int i = 0; i < 500; i++)
    {
      sum += adc_read();
      sleep_ms(1);
    }
    float average = sum / 500.0f;

    // Fórmula simplificada: R_x = R_conhecido * ADC_encontrado /(ADC_RESOLUTION - ADC_encontrado)
    R_x = (R_conhecido * average) / (ADC_RESOLUTION - average);

    nearest_resistor = find_nearest_e24(R_x);
    error_percentage = calculate_error_percentage(R_x, nearest_resistor);

    resistor_digits digits = extract_digits(nearest_resistor);




    sprintf(adc, "%.1f", average);    // Converte o inteiro em string
    sprintf(resistance, "%.1f", R_x); // Converte o float em string
    sprintf(error, "%.1f", error_percentage);




    //  Atualiza o conteúdo do display com animações
    ssd1306_fill(&ssd, !color); // Limpa o display

    ssd1306_rect(&ssd, 3, 3, 122, 60, color, !color); // Desenha um retângulo

    ssd1306_draw_string(&ssd, "1", 8, 8);            // Desenha uma string
    ssd1306_rect(&ssd, 13, 17, 2, 2, color, !color); // Desenha um retângulo
    ssd1306_draw_string(&ssd, "2", 8, 17);           // Desenha uma string
    ssd1306_rect(&ssd, 22, 17, 2, 2, color, !color); // Desenha um retângulo
    ssd1306_draw_string(&ssd, "3", 8, 26);           // Desenha uma string
    ssd1306_rect(&ssd, 31, 17, 2, 2, color, !color); // Desenha um retângulo

    ssd1306_draw_string(&ssd, get_color_name(digits.first_digit), 25, 8);  // Desenha uma string
    ssd1306_draw_string(&ssd, get_color_name(digits.second_digit), 25, 17); // Desenha uma string
    ssd1306_draw_string(&ssd, get_color_name(digits.multiplier), 25, 26); // Desenha uma string

    ssd1306_line(&ssd, 85, 4, 85, 36, color); // Desenha uma linha vertical

    ssd1306_draw_string(&ssd, "E(%)", 90, 8);  // Desenha uma string
    ssd1306_draw_string(&ssd, error, 90, 26); // Desenha uma string

    ssd1306_line(&ssd, 3, 37, 123, 37, color); // Desenha uma linha

    ssd1306_draw_string(&ssd, "ADC", 22, 41); // Desenha uma string
    ssd1306_draw_string(&ssd, "Ohm", 83, 41); // Desenha uma string

    ssd1306_line(&ssd, 64, 37, 64, 60, color); // Desenha uma linha vertical

    ssd1306_draw_string(&ssd, adc, 10, 52);        // Desenha uma string
    ssd1306_draw_string(&ssd, resistance, 68, 52); // Desenha uma string

    ssd1306_send_data(&ssd); // Atualiza o display
    sleep_ms(700);

    draw_resistor_color(pio, digits);
  }
}