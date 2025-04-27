#include "General.h"
#include "Led_Matrix.h"

uint R_known = 1000;           // Resistor de 1k Ohm
float R_x = 0.0;               // Resistor desconhecido
int adc_resolution = ADC_USB;  // Resolução do ADC (12 bits)
float nearest_resistor = 0.0f; // Valor da resistor na série E24
float error_percentage = 0.0f; // Valor da porcentagem do erro de tolerância
absolute_time_t  last_time_irq_B = 0;  // Controle de tempo para debounce (evitar múltiplos acionamentos)

// Alterar o ADC referencial com base na alimentação da placa
void gpio_irq_handler(uint gpio, uint32_t events);

int main()
{
  adc_init();             // Inicializa o conversor analógico-digital (ADC)
  adc_gpio_init(ADC_PIN); // Inicializa o GPIO 28 como entrada analógica

  configure_gpio(BUTTON_B, true, true);                                                      // Inicializa o botão B e o configura com pull-up
  gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler); // Interrupção com callback para o botão B

  ssd1306_t ssd; // Estrutura que representa o display OLED SSD1306
  refs pio = init_pio();

  configure_display(&ssd); // Configuração inicial do display

  // Limpa o display. O display inicia com todos os pixels apagados.
  ssd1306_fill(&ssd, false);
  ssd1306_send_data(&ssd);

  char adc[8];        // Buffer para armazenar o adc
  char resistance[8]; // Buffer para armazenar a resistencia
  char error[8];      // Buffer para armazenar a porcentagem de erro

  bool color = true; // Variável para determinar a cor/preenchimento do display

  while (true)
  {
    float average = calculate_average_adc(2, 1000); // Valor da média de filtragem do ADC

    // Fórmula simplificada: R_x = R_known * ADC_encontrado /(adc_resolution - ADC_encontrado)
    R_x = (R_known * average) / (adc_resolution - average);

    nearest_resistor = find_nearest_e24(R_x); // Resistor da série E24 mais próximo

    error_percentage = calculate_error_percentage(R_x, nearest_resistor); // Resistor da série E24 mais próximo

    resistor_digits digits = extract_digits(nearest_resistor); // Digitos correspondentes às faixas de cores

    // Conversões númericas para string
    sprintf(adc, "%.1f", average);
    sprintf(resistance, "%.1f", R_x);
    sprintf(error, "%.1f", error_percentage);

    // Mostrar no display as informações
    update_resistor_display(&ssd, color, digits, adc, resistance, error);

    // Mostrar na matriz as cores correspondente as faixas do resistor
    draw_resistor_color(pio, digits);
  }
}

void gpio_irq_handler(uint gpio, uint32_t events)
{
  absolute_time_t  current_time = get_absolute_time(); // Pega o tempo atual (para debounce)

  // Verifica debounce
  if (absolute_time_diff_us(current_time, last_time_irq_B) < DEBOUNCE_DELAY){
    adc_resolution = (adc_resolution == ADC_BATTERY) ? ADC_USB : ADC_BATTERY;
    last_time_irq_B = current_time;
  }
}