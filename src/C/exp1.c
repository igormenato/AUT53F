/*
  Experiência 01 - Blink de 1 segundo, simultaneamente (3 métodos):
  - Led1 (Pino 10/PB2): Pisca via delay no loop.
  - Led2 (Pino 11/PB3): Pisca via Timer1 COM interrupção (ISR).
  - Led3 (Pino 12/PB4): Pisca via Timer2 SEM interrupção (polling via ISR contador).
*/

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// Definição dos pinos (PORTB)
#define LED_DELAY_PIN PB2  // Pino 10
#define LED_TIMER1_PIN PB3 // Pino 11
#define LED_TIMER2_PIN PB4 // Pino 12

// Contador para Timer2 (polling)
static volatile uint8_t timer2OverflowCount = 0;

static void init_io(void)
{
    DDRB |= (1 << LED_DELAY_PIN) | (1 << LED_TIMER1_PIN) | (1 << LED_TIMER2_PIN);
}

static void init_timer1(void)
{
    // Fórmula: OCR1A = (16MHz / (prescaler * frequência)) - 1
    // Para 500ms (2Hz toggle): (16000000 / (1024 * 2)) - 1 = 7812
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;
    OCR1A = 7812;
    TCCR1B |= (1 << WGM12);              // Modo CTC
    TCCR1B |= (1 << CS12) | (1 << CS10); // Prescaler 1024
    TIMSK1 |= (1 << OCIE1A);             // Interrupção de comparação A
}

static void init_timer2(void)
{
    // Timer2 8-bit com contagem de overflows (~31 overflows para ~500ms)
    TCCR2A = 0;
    TCCR2B = 0;
    TCNT2 = 0;
    TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20); // Prescaler 1024
    TIMSK2 |= (1 << TOIE2);                            // Interrupção de overflow
}

static void blink_with_delay(void)
{
    PORTB |= (1 << LED_DELAY_PIN);
    _delay_ms(500);
    PORTB &= ~(1 << LED_DELAY_PIN);
    _delay_ms(500);
}

// --- ISR do Timer1 ---
// Executa automaticamente a cada 500ms
ISR(TIMER1_COMPA_vect)
{
    PORTB ^= (1 << LED_TIMER1_PIN); // Toggle LED
}

// --- ISR do Timer2: Conta overflows e controla LED ---
ISR(TIMER2_OVF_vect)
{
    timer2OverflowCount++;
    if (timer2OverflowCount >= 31)
    {
        timer2OverflowCount = 0;
        PORTB ^= (1 << LED_TIMER2_PIN); // Toggle LED
    }
}

int main(void)
{
    cli();
    init_io();
    init_timer1();
    init_timer2();
    sei();

    while (1)
    {
        blink_with_delay();
    }

    return 0;
}
