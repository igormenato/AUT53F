/*
  Experiência 02 - Ligar e desligar 2 LEDs de forma alternada:
  - SW1 (Pino A1/PC1): Pressionar alterna entre ledA e ledB.
  - SW2 (Pino A2/PC2): Soltar alterna entre ledA e ledB.
  - SW3 (Pino A3/PC3): Manter pressionado pisca alternado rápido.
*/

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>

// Definição dos pinos
#define LED_A_PIN PB2 // Pino 10
#define LED_B_PIN PB3 // Pino 11
#define BT1_PIN PC1   // Pino A1
#define BT2_PIN PC2   // Pino A2
#define BT3_PIN PC3   // Pino A3

// Variáveis de estado
static uint8_t ledState = 0;          // 0 = ledA aceso, 1 = ledB aceso
static uint8_t estadoBt1Anterior = 1; // Estado anterior do botão 1 (HIGH)
static uint8_t estadoBt2Anterior = 1; // Estado anterior do botão 2 (HIGH)

// Funções auxiliares para leitura de botões (com pull-up: 0 = pressionado)
static inline uint8_t readButton(uint8_t pin)
{
    return (PINC & (1 << pin)) ? 1 : 0;
}

static void updateLeds(void)
{
    if (ledState == 0)
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

void setup(void)
{
    // --- Configuração dos pinos de LED como saída ---
    DDRB |= (1 << LED_A_PIN) | (1 << LED_B_PIN);

    // --- Configuração dos pinos de botão como entrada com pull-up ---
    DDRC &= ~((1 << BT1_PIN) | (1 << BT2_PIN) | (1 << BT3_PIN));
    PORTC |= (1 << BT1_PIN) | (1 << BT2_PIN) | (1 << BT3_PIN);

    // --- Estado inicial dos LEDs ---
    updateLeds();
}

void loop(void)
{
    // --- Leitura dos botões ---
    uint8_t b1State = readButton(BT1_PIN);
    uint8_t b2State = readButton(BT2_PIN);
    uint8_t b3State = readButton(BT3_PIN);

    // --- SW1: Pressionar (Transição HIGH -> LOW) ---
    if (estadoBt1Anterior == 1 && b1State == 0)
    {
        ledState = !ledState;
        updateLeds();
        _delay_ms(50); // Debounce
    }
    estadoBt1Anterior = b1State;

    // --- SW2: Soltar (Transição LOW -> HIGH) ---
    if (estadoBt2Anterior == 0 && b2State == 1)
    {
        ledState = !ledState;
        updateLeds();
        _delay_ms(50); // Debounce
    }
    estadoBt2Anterior = b2State;

    // --- SW3: Manter pressionado (Nível LOW) ---
    if (b3State == 0)
    {
        PORTB |= (1 << LED_A_PIN);
        PORTB &= ~(1 << LED_B_PIN);
        _delay_ms(100);
        PORTB &= ~(1 << LED_A_PIN);
        PORTB |= (1 << LED_B_PIN);
        _delay_ms(100);
        ledState = 1; // Atualiza estado ao soltar
    }
}

int main(void)
{
    setup();
    while (1)
    {
        loop();
    }
    return 0;
}
