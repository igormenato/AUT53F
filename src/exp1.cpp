/*
  Experiência 01 - Blink de 3 LEDs (Shield v10)
  - Led1 (Pino 10): Pisca via delay() no loop.
  - Led2 (Pino 11): Pisca via Interrupção do Timer1.
  - Led3 (Pino 12): Pisca sincronizado com Led1 (lógica manual).
*/

#include <Arduino.h>

// Mapeamento conforme imagem do Shield v10
const int ledPinDelay = 10; // Led1
const int ledPinTimer = 11; // Led2
const int ledPin3 = 12;     // Led3

void setup()
{
  pinMode(ledPinDelay, OUTPUT);
  pinMode(ledPinTimer, OUTPUT);
  pinMode(ledPin3, OUTPUT);

  // --- Configuração do Timer1 para interrupção a cada 1 segundo (1Hz) ---
  noInterrupts(); // Desabilita interrupções para configuração segura

  TCCR1A = 0; // Limpa registradores
  TCCR1B = 0;
  TCNT1 = 0; // Zera contador

  // Valor de comparação para 500ms (toggle).
  // Fórmula: (16MHz / (1024 * 2Hz)) - 1 = 7811.5 -> 7812
  OCR1A = 7812;

  // Modo CTC (Clear Timer on Compare Match)
  TCCR1B |= (1 << WGM12);
  // Prescaler de 1024
  TCCR1B |= (1 << CS12) | (1 << CS10);
  // Habilita interrupção de comparação A
  TIMSK1 |= (1 << OCIE1A);

  interrupts(); // Reabilita interrupções
}

// ISR: Executa automaticamente a cada 500ms (inverte o estado)
ISR(TIMER1_COMPA_vect)
{
  digitalWrite(ledPinTimer, !digitalRead(ledPinTimer)); // Toggle Led2
}

void loop()
{
  // Led1 e Led3 controlados manualmente por delay
  // Como o Timer1 cuida do Led2 sozinho, o processador fica "preso" aqui
  // apenas cuidando do Led1 e Led3.

  digitalWrite(ledPinDelay, HIGH);
  digitalWrite(ledPin3, HIGH);
  delay(500);

  digitalWrite(ledPinDelay, LOW);
  digitalWrite(ledPin3, LOW);
  delay(500);
}