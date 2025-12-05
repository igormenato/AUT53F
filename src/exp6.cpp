/*
  Experiência 06 - Potenciômetro com conversão A/D:
  - Lê o potenciômetro na porta A0 (10 bits: 0 a 1023).
  - Exibe o valor no display de 7 segmentos.
*/

#include <Arduino.h>

// Definição dos pinos (74HC595)
const int DATA_PIN = 8;  // DS (dados)
const int LATCH_PIN = 4; // STCP (latch)
const int CLOCK_PIN = 7; // SHCP (clock)
const int POT_PIN = A0;  // Potenciômetro

// Mapa de segmentos para display 7 segmentos (Anodo Comum)
// Valores: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
const byte segMap[] = {
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};

// Protótipo da função
void mostrarNoDisplay(int valor);

void setup()
{
  // --- Configuração dos pinos ---
  pinMode(DATA_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
}

void loop()
{
  // --- Leitura do potenciômetro ---
  int leitura = analogRead(POT_PIN); // Valor de 0 a 1023
  mostrarNoDisplay(leitura);
}

// --- Função de multiplexação do display ---
void mostrarNoDisplay(int valor)
{
  // Separa os dígitos
  int d[4];
  d[0] = (valor / 1000) % 10; // Milhar
  d[1] = (valor / 100) % 10;  // Centena
  d[2] = (valor / 10) % 10;   // Dezena
  d[3] = valor % 10;          // Unidade

  // Varredura dos 4 dígitos
  for (int i = 0; i < 4; i++)
  {
    digitalWrite(LATCH_PIN, LOW);
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, segMap[d[i]]); // Segmentos
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, (1 << i));     // Seleção do dígito
    digitalWrite(LATCH_PIN, HIGH);
    delay(4); // Delay para persistência visual
  }
}
