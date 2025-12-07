/*
  Experiência 03 - Sequencial de LEDs com inversão de direção:
  - SW1 (Pino A1/PC1): Inverte direção via Polling (sem interrupção).
  - SW2 (Pino A2/PC2): Inverte direção via PCINT (com interrupção).
*/

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <stdbool.h>
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
#define DEBOUNCE_MS 50
#define STEP_DELAY_MS 200

// Máscaras dos LEDs
#define LED_MASK ((1 << LED0_PIN) | (1 << LED1_PIN) | (1 << LED2_PIN) | (1 << LED3_PIN))

// Array de pinos de LED para indexação
static const uint8_t ledPins[] = {LED0_PIN, LED1_PIN, LED2_PIN, LED3_PIN};

// Variáveis de estado
static volatile bool direction_forward = true; // 1 = 10->13, 0 = 13->10
static bool lastBtPolledReleased = true;
static int8_t atual = 0; // LED atual

static inline bool button_is_pressed(uint8_t pin)
{
    return (PINC & (1 << pin)) == 0;
}

static void init_io(void)
{
    DDRB |= LED_MASK;
    PORTB |= LED_MASK; // LEDs apagados (lógica invertida: HIGH = Off)

    DDRC &= ~((1 << BT_POLLED_PIN) | (1 << BT_INTERRUPT_PIN));
    PORTC |= (1 << BT_POLLED_PIN) | (1 << BT_INTERRUPT_PIN);
}

static void init_pcint(void)
{
    PCICR |= (1 << PCIE1);    // Habilita interrupção para Port C
    PCMSK1 |= (1 << PCINT10); // Habilita interrupção para A2
}

static void advance_led(void)
{
    PORTB |= LED_MASK; // Apaga todos (HIGH = Off)
    PORTB &= ~(1 << ledPins[atual]);
    _delay_ms(STEP_DELAY_MS);

    if (direction_forward)
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

// --- ISR do PCINT1 (Port C: A0-A5) ---
ISR(PCINT1_vect)
{
    // Verifica se o pino A2 está LOW (pressionado)
    if (!(PINC & (1 << BT_INTERRUPT_PIN)))
    {
        direction_forward = !direction_forward;
    }
}

int main(void)
{
    cli();
    init_io();
    init_pcint();
    sei();

    while (1)
    {
        // Botão com Polling (SW1)
        bool pressed = button_is_pressed(BT_POLLED_PIN);
        if (pressed && lastBtPolledReleased)
        {
            direction_forward = !direction_forward;
            _delay_ms(DEBOUNCE_MS);
        }
        lastBtPolledReleased = !pressed;

        advance_led();
    }

    return 0;
}
