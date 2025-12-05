/*
  Experiência 01 - Blink de 1 segundo, simultaneamente (3 métodos):
  - Led1 (Pino 10): Pisca via delay() no loop.
  - Led2 (Pino 11): Pisca via Timer1 COM interrupção (ISR).
  - Led3 (Pino 12): Pisca via Timer2 SEM interrupção (polling).
*/

#include <Arduino.h>

// Definição dos pinos
const int ledPinDelay = 10; // Led1 - delay()
const int ledPinTimer = 11; // Led2 - Timer1 com interrupção
const int ledPin3 = 12;     // Led3 - Timer2 sem interrupção (polling)

// Contador para Timer2 (polling)
volatile uint8_t timer2OverflowCount = 0;

void setup()
{
  pinMode(ledPinDelay, OUTPUT);
  pinMode(ledPinTimer, OUTPUT);
  pinMode(ledPin3, OUTPUT);

  // --- Configuração do Timer1: COM interrupção (16-bit) ---
  // Fórmula: OCR1A = (16MHz / (prescaler * frequência)) - 1
  // Para 500ms (2Hz toggle): (16000000 / (1024 * 2)) - 1 = 7812
  noInterrupts();
  TCCR1A = 0;                          // Limpa registrador de controle A
  TCCR1B = 0;                          // Limpa registrador de controle B
  TCNT1 = 0;                           // Zera contador
  OCR1A = 7812;                        // Valor de comparação para 500ms
  TCCR1B |= (1 << WGM12);              // Modo CTC (Clear Timer on Compare)
  TCCR1B |= (1 << CS12) | (1 << CS10); // Prescaler 1024
  TIMSK1 |= (1 << OCIE1A);             // Habilita interrupção de comparação A
  interrupts();

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
}

// --- ISR do Timer1 ---
// Executa automaticamente a cada 500ms
ISR(TIMER1_COMPA_vect)
{
  digitalWrite(ledPinTimer, !digitalRead(ledPinTimer));
}

// --- ISR do Timer2: Apenas conta overflows (LED controlado no loop) ---
ISR(TIMER2_OVF_vect)
{
  timer2OverflowCount++;
  if (timer2OverflowCount >= 31)
  {
    timer2OverflowCount = 0;
    digitalWrite(ledPin3, !digitalRead(ledPin3));
  }
}

void loop()
{
  // --- Led1: Método delay() ---
  digitalWrite(ledPinDelay, HIGH);
  delay(500);
  digitalWrite(ledPinDelay, LOW);
  delay(500);
}