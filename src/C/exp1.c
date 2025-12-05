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
volatile uint8_t timer2OverflowCount = 0;

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

void setup(void)
{
    // --- Configuração dos pinos como saída ---
    DDRB |= (1 << LED_DELAY_PIN) | (1 << LED_TIMER1_PIN) | (1 << LED_TIMER2_PIN);

    // --- Configuração do Timer1: COM interrupção (16-bit) ---
    // Fórmula: OCR1A = (16MHz / (prescaler * frequência)) - 1
    // Para 500ms (2Hz toggle): (16000000 / (1024 * 2)) - 1 = 7812
    cli();                               // Desabilita interrupções
    TCCR1A = 0;                          // Limpa registrador de controle A
    TCCR1B = 0;                          // Limpa registrador de controle B
    TCNT1 = 0;                           // Zera contador
    OCR1A = 7812;                        // Valor de comparação para 500ms
    TCCR1B |= (1 << WGM12);              // Modo CTC (Clear Timer on Compare)
    TCCR1B |= (1 << CS12) | (1 << CS10); // Prescaler 1024
    TIMSK1 |= (1 << OCIE1A);             // Habilita interrupção de comparação A

    // --- Configuração do Timer2: SEM interrupção (8-bit) ---
    // Timer2 é 8-bit (0-255), precisamos contar múltiplos overflows
    // Com prescaler 1024: 16MHz / 1024 = 15625 Hz
    // Overflow a cada 256 ticks: 256 / 15625 = 16.384ms
    // Para 500ms: 500 / 16.384 ≈ 31 overflows
    TCCR2A = 0;                                        // Limpa registrador de controle A
    TCCR2B = 0;                                        // Limpa registrador de controle B
    TCNT2 = 0;                                         // Zera contador
    TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20); // Prescaler 1024
    TIMSK2 |= (1 << TOIE2);                            // Habilita interrupção de overflow

    sei(); // Habilita interrupções
}

void loop(void)
{
    // --- Led1: Método delay ---
    PORTB |= (1 << LED_DELAY_PIN); // LED ON
    _delay_ms(500);
    PORTB &= ~(1 << LED_DELAY_PIN); // LED OFF
    _delay_ms(500);
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
