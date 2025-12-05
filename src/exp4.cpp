/*
  Experiência 04 - Contador hexadecimal (0-F) nos 4 displays:
  - Conta de 0 a F (hexadecimal) a cada 1 segundo.
  - Exibe o mesmo valor nos 4 displays simultaneamente.
*/

#include <Arduino.h>

// Definição dos pinos (74HC595)
const int DATA_PIN = 8;  // DS (dados)
const int LATCH_PIN = 4; // STCP (latch)
const int CLOCK_PIN = 7; // SHCP (clock)

// Mapa de segmentos para display 7 segmentos (Anodo Comum)
// Valores: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, A, b, C, d, E, F
const byte segMap[] = {
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
    0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E};

// Variáveis de estado
int contador = 0;
unsigned long lastIncrement = 0;

void setup()
{
  // --- Configuração dos pinos ---
  pinMode(DATA_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
}

void loop()
{
  // --- Incrementa contador a cada 1 segundo ---
  if (millis() - lastIncrement > 1000)
  {
    contador++;
    if (contador > 15)
      contador = 0;
    lastIncrement = millis();
  }

  // --- Envia dados para os displays ---
  // 0x0F liga todos os 4 dígitos simultaneamente
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, segMap[contador]); // Segmentos
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, 0x0F);             // Seleção de dígitos
  digitalWrite(LATCH_PIN, HIGH);

  delay(50);
}
