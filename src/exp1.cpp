/*
  Experiência 01 - Blink de 1 segundo, simultaneamente
  - Led1: Pisca controlado por delay() no loop principal.
  - Led2: Pisca controlado por Interrupção do Timer1 (Hardware).
*/

#include <Arduino.h>

const int ledPinDelay = 10; // Led1
const int ledPinTimer = 11; // Led2

void setup()
{
  pinMode(ledPinDelay, OUTPUT);
  pinMode(ledPinTimer, OUTPUT);

  // --- Configuração do Timer1 para interrupção a cada 1 segundo (1Hz) ---
  noInterrupts(); // Desabilita interrupções globalmente
  TCCR1A = 0;     // Limpa registradores de controle do Timer1
  TCCR1B = 0;
  TCNT1 = 0; // Zera contador

  // Define valor de comparação para 0.5Hz (500ms) para igualar ao delay
  // 7812 = (16000000 / (1024 * 2)) - 1  -> Frequência de toggle de 2Hz (para piscar 1Hz completo)
  OCR1A = 7812;

  // Modo CTC (Clear Timer on Compare Match)
  TCCR1B |= (1 << WGM12);
  // Prescaler de 1024
  TCCR1B |= (1 << CS12) | (1 << CS10);
  // Habilita interrupção de comparação (Timer1 Compare Match A)
  TIMSK1 |= (1 << OCIE1A);
  interrupts(); // Habilita interrupções novamente
}

// Rotina de Serviço de Interrupção (ISR) do Timer1
// Executa automaticamente a cada 1 segundo, independentemente do loop()
ISR(TIMER1_COMPA_vect)
{
  digitalWrite(ledPinTimer, !digitalRead(ledPinTimer)); // Inverte estado do Led2
}

void loop()
{
  // Blink convencional com Delay para o Led1
  digitalWrite(ledPinDelay, HIGH);
  delay(500); // 500ms ligado
  digitalWrite(ledPinDelay, LOW);
  delay(500); // 500ms desligado
  // Total = 1 segundo de ciclo
}
