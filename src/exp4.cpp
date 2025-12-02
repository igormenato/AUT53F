/*
  Experiência 04 - Contador 0 a F (Hex) simultâneo nos 4 displays.
  - Com Flicker: Adicionado delay na varredura para tornar a troca visível.
*/

#include <Arduino.h>

// Pinos do 74HC595
const int DATA_PIN = 8;  // DS
const int LATCH_PIN = 4; // STC
const int CLOCK_PIN = 7; // SHC

// Mapa de segmentos (0 = Ligado, 1 = Desligado)
// Hex: 0,1,2,3,4,5,6,7,8,9,A,b,C,d,E,F
const byte segMap[] = {
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
    0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E};

int contador = 0;
unsigned long lastIncrement = 0;

void setup()
{
  pinMode(DATA_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
}

void loop()
{
  // Incrementa o contador a cada 1 segundo
  if (millis() - lastIncrement > 1000)
  {
    contador++;
    if (contador > 15)
      contador = 0; // Reinicia após F
    lastIncrement = millis();
  }

  // Mostra o valor nos 4 displays SIMULTANEAMENTE
  // Para mostrar simultaneamente em displays multiplexados, ligamos todos os transistores
  // Transistores ligados aos pinos Q0-Q3 do segundo 595.
  // 0x0F (00001111) liga os 4 dígitos.

  digitalWrite(LATCH_PIN, LOW);
  // 1. Envia Byte dos Segmentos (vai para o 595 de CIMA)
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, segMap[contador]);
  // 2. Envia Byte de Seleção de Dígito (fica no 595 de BAIXO)
  // Ligamos todos os 4 dígitos para mostrar o mesmo valor
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, 0x0F);
  digitalWrite(LATCH_PIN, HIGH);

  delay(50); // Introduz flicker visível se houvesse varredura
}
