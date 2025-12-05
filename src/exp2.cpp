/*
  Experiência 02 - Ligar e desligar 2 LEDs de forma alternada:
  - SW1 (Pino A1): Pressionar alterna entre ledA e ledB.
  - SW2 (Pino A2): Soltar alterna entre ledA e ledB.
  - SW3 (Pino A3): Manter pressionado pisca alternado rápido.
*/

#include <Arduino.h>

// Definição dos pinos
const int ledA = 10;
const int ledB = 11;
const int bt1 = A1;
const int bt2 = A2;
const int bt3 = A3;

// Variáveis de estado
bool ledState = false;         // false = ledA aceso, true = ledB aceso
bool estadoBt1Anterior = HIGH; // Estado anterior do botão 1
bool estadoBt2Anterior = HIGH; // Estado anterior do botão 2

void setup()
{
  // --- Configuração dos pinos ---
  pinMode(ledA, OUTPUT);
  pinMode(ledB, OUTPUT);
  pinMode(bt1, INPUT_PULLUP);
  pinMode(bt2, INPUT_PULLUP);
  pinMode(bt3, INPUT_PULLUP);

  // --- Estado inicial dos LEDs ---
  digitalWrite(ledA, !ledState);
  digitalWrite(ledB, ledState);
}

void loop()
{
  // --- Leitura dos botões ---
  bool b1State = digitalRead(bt1);
  bool b2State = digitalRead(bt2);
  bool b3State = digitalRead(bt3);

  // --- SW1: Pressionar (Transição HIGH -> LOW) ---
  if (estadoBt1Anterior == HIGH && b1State == LOW)
  {
    ledState = !ledState;
    digitalWrite(ledA, !ledState);
    digitalWrite(ledB, ledState);
    delay(50); // Debounce
  }
  estadoBt1Anterior = b1State;

  // --- SW2: Soltar (Transição LOW -> HIGH) ---
  if (estadoBt2Anterior == LOW && b2State == HIGH)
  {
    ledState = !ledState;
    digitalWrite(ledA, !ledState);
    digitalWrite(ledB, ledState);
    delay(50); // Debounce
  }
  estadoBt2Anterior = b2State;

  // --- SW3: Manter pressionado (Nível LOW) ---
  if (b3State == LOW)
  {
    digitalWrite(ledA, HIGH);
    digitalWrite(ledB, LOW);
    delay(100);
    digitalWrite(ledA, LOW);
    digitalWrite(ledB, HIGH);
    delay(100);
    ledState = true; // Atualiza estado ao soltar
  }
}