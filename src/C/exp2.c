/*
  Experiência 02 - Ligar e desligar 2 LEDs de forma alternada:
  - SW1 (Pino A1/PC1): Pressionar alterna entre ledA e ledB.
  - SW2 (Pino A2/PC2): Soltar alterna entre ledA e ledB.
  - SW3 (Pino A3/PC3): Manter pressionado pisca alternado rápido.
*/

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <stdbool.h>
#include <avr/io.h>
#include <util/delay.h>

// Definição dos pinos
#define LED_A_PIN PB2 // Pino 10
#define LED_B_PIN PB3 // Pino 11
#define BT1_PIN PC1   // Pino A1
#define BT2_PIN PC2   // Pino A2
#define BT3_PIN PC3   // Pino A3
#define DEBOUNCE_MS 50
#define BLINK_MS 100

// Variáveis de estado
static bool ledASelected = true;
static bool lastBt1Released = true;
static bool lastBt2Pressed = false;

// Funções auxiliares para leitura de botões (com pull-up: 0 = pressionado)
static inline bool button_is_pressed(uint8_t pin)
{
    return (PINC & (1 << pin)) == 0;
}

static void update_leds(void)
{
    if (ledASelected)
    {
        PORTB |= (1 << LED_A_PIN);  // LED A ON
        PORTB &= ~(1 << LED_B_PIN); // LED B OFF
    }
    else
    {
        PORTB &= ~(1 << LED_A_PIN); // LED A OFF
        PORTB |= (1 << LED_B_PIN);  // LED B ON
    }
}

static void setup(void)
{
    // --- Configuração dos pinos de LED como saída ---
    DDRB |= (1 << LED_A_PIN) | (1 << LED_B_PIN);

    // --- Configuração dos pinos de botão como entrada com pull-up ---
    DDRC &= ~((1 << BT1_PIN) | (1 << BT2_PIN) | (1 << BT3_PIN));
    PORTC |= (1 << BT1_PIN) | (1 << BT2_PIN) | (1 << BT3_PIN);

    // --- Estado inicial dos LEDs ---
    update_leds();
}

static void loop_once(void)
{
    // --- Leitura dos botões ---
    bool b1Pressed = button_is_pressed(BT1_PIN);
    bool b2Pressed = button_is_pressed(BT2_PIN);
    bool b3Pressed = button_is_pressed(BT3_PIN);

    // --- SW1: Pressionar (Transição HIGH -> LOW) ---
    if (lastBt1Released && b1Pressed)
    {
        ledASelected = !ledASelected;
        update_leds();
        _delay_ms(DEBOUNCE_MS);
    }
    lastBt1Released = !b1Pressed;

    // --- SW2: Soltar (Transição LOW -> HIGH) ---
    if (lastBt2Pressed && !b2Pressed)
    {
        ledASelected = !ledASelected;
        update_leds();
        _delay_ms(DEBOUNCE_MS);
    }
    lastBt2Pressed = b2Pressed;

    // --- SW3: Manter pressionado (Nível LOW) ---
    if (b3Pressed)
    {
        PORTB |= (1 << LED_A_PIN);
        PORTB &= ~(1 << LED_B_PIN);
        _delay_ms(BLINK_MS);
        PORTB &= ~(1 << LED_A_PIN);
        PORTB |= (1 << LED_B_PIN);
        _delay_ms(BLINK_MS);
        ledASelected = false; // Atualiza estado ao soltar
    }
}

int main(void)
{
    setup();
    while (1)
    {
        loop_once();
    }
    return 0;
}
