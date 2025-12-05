/*
  Experiência 03 - Sequencial de LEDs com inversão de direção:
  - SW1 (Pino A1/PC1): Inverte direção via Polling (sem interrupção).
  - SW2 (Pino A2/PC2): Inverte direção via PCINT (com interrupção).
*/

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// Definição dos pinos de LED (PORTB)
#define LED0_PIN PB2 // Pino 10
#define LED1_PIN PB3 // Pino 11
#define LED2_PIN PB4 // Pino 12
#define LED3_PIN PB5 // Pino 13

// Definição dos pinos de botão (PORTC)
#define BT_POLLED_PIN PC1    // Pino A1
#define BT_INTERRUPT_PIN PC2 // Pino A2

// Máscaras dos LEDs
#define LED_MASK ((1 << LED0_PIN) | (1 << LED1_PIN) | (1 << LED2_PIN) | (1 << LED3_PIN))

// Array de pinos de LED para indexação
static const uint8_t ledPins[] = {LED0_PIN, LED1_PIN, LED2_PIN, LED3_PIN};

// Variáveis de estado
volatile uint8_t direcao = 1;    // 1 = 10->13, 0 = 13->10
static uint8_t lastBtPolled = 1; // Estado anterior do botão polling (HIGH)
static int8_t atual = 0;         // LED atual

// --- ISR do PCINT1 (Port C: A0-A5) ---
ISR(PCINT1_vect)
{
    // Verifica se o pino A2 está LOW (pressionado)
    if (!(PINC & (1 << BT_INTERRUPT_PIN)))
    {
        direcao = !direcao;
    }
}

void setup(void)
{
    // --- Configuração dos pinos de LED como saída ---
    DDRB |= LED_MASK;

    // --- LEDs apagados (lógica invertida: HIGH = Off) ---
    PORTB |= LED_MASK;

    // --- Configuração dos pinos de botão como entrada com pull-up ---
    DDRC &= ~((1 << BT_POLLED_PIN) | (1 << BT_INTERRUPT_PIN));
    PORTC |= (1 << BT_POLLED_PIN) | (1 << BT_INTERRUPT_PIN);

    // --- Configuração do PCINT para o pino A2 (PCINT10) ---
    // A2 pertence ao grupo PCINT1 (Port C: A0-A5)
    cli();
    PCICR |= (1 << PCIE1);    // Habilita interrupção para o grupo Port C
    PCMSK1 |= (1 << PCINT10); // Habilita interrupção para o pino A2
    sei();
}

void loop(void)
{
    // --- Botão com Polling (SW1) ---
    uint8_t leitura = (PINC & (1 << BT_POLLED_PIN)) ? 1 : 0;
    if (leitura == 0 && lastBtPolled == 1)
    {
        direcao = !direcao;
        _delay_ms(50); // Debounce
    }
    lastBtPolled = leitura;

    // --- Sequencial de LEDs ---
    // Apaga todos (lógica invertida: HIGH = Off)
    PORTB |= LED_MASK;

    // Acende o atual (lógica invertida: LOW = On)
    PORTB &= ~(1 << ledPins[atual]);
    _delay_ms(200);

    // Calcula o próximo
    if (direcao)
    {
        atual++;
        if (atual > 3)
            atual = 0;
    }
    else
    {
        atual--;
        if (atual < 0)
            atual = 3;
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
