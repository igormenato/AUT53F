/*
  Experiência 03 - Sequencial de LEDs com inversão de direção:
  - SW1 (Pino A1): Inverte direção via Polling (sem interrupção).
  - SW2 (Pino A2): Inverte direção via PCINT (com interrupção).
*/

#include <Arduino.h>

// Definição dos pinos
const int leds[] = {10, 11, 12, 13};
const int btPolled = A1;    // Botão com polling
const int btInterrupt = A2; // Botão com interrupção (PCINT10)

// Variáveis de estado
volatile bool direcao = true; // true = 10->13, false = 13->10
bool lastBtPolled = HIGH;     // Estado anterior do botão polling

void setup()
{
  // --- Configuração dos pinos ---
  for (int i = 0; i < 4; i++)
  {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], HIGH); // LEDs apagados (lógica invertida: HIGH = Off)
  }
  pinMode(btPolled, INPUT_PULLUP);
  pinMode(btInterrupt, INPUT_PULLUP);

  // --- Configuração do PCINT para o pino A2 (PCINT10) ---
  // A2 pertence ao grupo PCINT1 (Port C: A0-A5)
  noInterrupts();
  PCICR |= (1 << PCIE1);    // Habilita interrupção para o grupo Port C
  PCMSK1 |= (1 << PCINT10); // Habilita interrupção para o pino A2
  interrupts();
}

// --- ISR do PCINT1 (Port C: A0-A5) ---
ISR(PCINT1_vect)
{
  if (digitalRead(A2) == LOW)
  {
    direcao = !direcao;
  }
}

void loop()
{
  // --- Botão com Polling (SW1) ---
  bool leitura = digitalRead(btPolled);
  if (leitura == LOW && lastBtPolled == HIGH)
  {
    direcao = !direcao;
    delay(50); // Debounce
  }
  lastBtPolled = leitura;

  // --- Sequencial de LEDs ---
  // Apaga todos
  for (int i = 0; i < 4; i++)
    digitalWrite(leds[i], HIGH);

  // Acende o atual
  static int atual = 0;
  digitalWrite(leds[atual], LOW);
  delay(200);

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
