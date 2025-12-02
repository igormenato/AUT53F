/*
  Experiência 03 - Sequencial de LEDs com inversão de direção
  - Chave sem interrupção (A1): Inverte direção via Polling.
  - Chave com interrupção (A2): Inverte direção via PCINT (Pin Change Interrupt).

  AJUSTE: Lógica invertida para LEDs (Active LOW).
  - LOW = Acende
  - HIGH = Apaga
  Isso corrige o efeito de "rasto" deixando apenas 1 LED aceso.
*/

#include <Arduino.h>

const int leds[] = {10, 11, 12, 13};
const int btPolled = A1;
const int btInterrupt = A2; // Usaremos PCINT1

volatile bool direcao = true; // true = 10->13, false = 13->10
bool lastBtPolled = HIGH;

void setup()
{
  for (int i = 0; i < 4; i++)
  {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], HIGH); // Garante que iniciam apagados (Lógica: HIGH = Off)
  }
  pinMode(btPolled, INPUT_PULLUP);
  pinMode(btInterrupt, INPUT_PULLUP);

  // --- Configuração da Interrupção no Pino A2 (PCINT10) ---
  // A porta analógica A2 pertence ao grupo PCINT1 (Port C)
  noInterrupts();
  PCICR |= (1 << PCIE1);    // Habilita interrupção para o grupo Port C
  PCMSK1 |= (1 << PCINT10); // Habilita interrupção especificamente para o pino A2
  interrupts();
}

// Rotina de interrupção para o grupo Port C (A0-A5)
ISR(PCINT1_vect)
{
  // Verifica se o pino A2 está LOW (pressionado)
  if (digitalRead(A2) == LOW)
  {
    direcao = !direcao; // Inverte direção
  }
}

void loop()
{
  // Leitura do botão sem interrupção (Polling)
  bool leitura = digitalRead(btPolled);
  if (leitura == LOW && lastBtPolled == HIGH)
  {
    direcao = !direcao;
    delay(50); // Debounce simples
  }
  lastBtPolled = leitura;

  // Executa o sequencial

  // 1. Apaga todos (Active LOW: HIGH apaga)
  for (int i = 0; i < 4; i++)
    digitalWrite(leds[i], HIGH);

  // 2. Acende o atual (Active LOW: LOW acende)
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
