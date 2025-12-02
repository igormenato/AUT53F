/*
  Experiência 02 - Ligar e desligar 2 leds de forma alternada
  - Pressionar SW1 (A1): Liga modo alternado.
  - Soltar SW2 (A2): Desliga tudo (ou para alternância).
  - Manter SW3 (A3): Pisca alternado rápido enquanto pressionado (função 'manter').
*/

#include <Arduino.h>

const int ledA = 10;
const int ledB = 11;
const int bt1 = A1;
const int bt2 = A2;
const int bt3 = A3;

bool sistemaAtivo = false;
bool estadoBt2Anterior = HIGH; // Para detectar 'soltar' (Borda de subida)

void setup()
{
  pinMode(ledA, OUTPUT);
  pinMode(ledB, OUTPUT);
  pinMode(bt1, INPUT_PULLUP);
  pinMode(bt2, INPUT_PULLUP);
  pinMode(bt3, INPUT_PULLUP);
}

void loop()
{
  // Leitura dos botões
  bool b1State = digitalRead(bt1);
  bool b2State = digitalRead(bt2);
  bool b3State = digitalRead(bt3);

  // 1. Pressionar a chave SW1 (Detecta nível LOW) -> Ativa sistema
  if (b1State == LOW)
  {
    sistemaAtivo = true;
  }

  // 2. Soltar a chave SW2 (Detecta transição LOW -> HIGH)
  if (estadoBt2Anterior == LOW && b2State == HIGH)
  {
    sistemaAtivo = false;
    digitalWrite(ledA, LOW);
    digitalWrite(ledB, LOW);
  }
  estadoBt2Anterior = b2State;

  // 3. Manter a chave SW3 pressionada (Prioridade Alta)
  if (b3State == LOW)
  {
    // Pisca rápido enquanto segura
    digitalWrite(ledA, HIGH);
    digitalWrite(ledB, LOW);
    delay(100);
    digitalWrite(ledA, LOW);
    digitalWrite(ledB, HIGH);
    delay(100);
  }
  else if (sistemaAtivo)
  {
    // Modo alternado padrão (ativado pelo SW1)
    digitalWrite(ledA, HIGH);
    digitalWrite(ledB, LOW);
    delay(500);
    digitalWrite(ledA, LOW);
    digitalWrite(ledB, HIGH);
    delay(500);
  }
  else
  {
    // Estado de repouso
    digitalWrite(ledA, LOW);
    digitalWrite(ledB, LOW);
  }
}
