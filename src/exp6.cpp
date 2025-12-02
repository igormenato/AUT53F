/*
  Experiência 06 - Potenciômetro para A/D e saída no display
  - Lê porta A0 (10 bits: 0 a 1023)
  - Exibe valor constantemente.
*/

#include <Arduino.h>

const int DATA_PIN = 8;
const int LATCH_PIN = 4;
const int CLOCK_PIN = 7;
const int POT_PIN = A0;

const byte segMap[] = {
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};

void mostrarNoDisplay(int valor);

void setup()
{
  pinMode(DATA_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  // A0 é entrada por padrão
}

void loop()
{
  int leitura = analogRead(POT_PIN); // Lê valor entre 0 e 1023
  mostrarNoDisplay(leitura);
}

void mostrarNoDisplay(int valor)
{
  // Decompõe o valor em 4 dígitos
  // Se valor < 1000, o dígito do milhar será 0.
  int d[4];
  d[0] = (valor / 1000) % 10; // Milhar
  d[1] = (valor / 100) % 10;  // Centena
  d[2] = (valor / 10) % 10;   // Dezena
  d[3] = valor % 10;          // Unidade

  for (int i = 0; i < 4; i++)
  {
    digitalWrite(LATCH_PIN, LOW);

    // Envia segmentos
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, segMap[d[i]]);

    // Envia seleção do dígito (1, 2, 4, 8)
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, (1 << i));

    digitalWrite(LATCH_PIN, HIGH);

    delay(4); // Delay para varredura estável
  }
}
