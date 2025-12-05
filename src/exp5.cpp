/*
  Experiência 05 - Contador de 0000 a 9999:
  - Utiliza multiplexação (varredura) para exibir 4 dígitos diferentes.
  - Incrementa a cada 10ms.
*/

#include <Arduino.h>

// Definição dos pinos (74HC595)
const int DATA_PIN = 8;  // DS (dados)
const int LATCH_PIN = 4; // STCP (latch)
const int CLOCK_PIN = 7; // SHCP (clock)

// Mapa de segmentos para display 7 segmentos (Anodo Comum)
// Valores: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
const byte segMap[] = {
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};

// Variáveis de estado
int valorContador = 0;
unsigned long tempoAnterior = 0;

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
  // --- Incrementa contador a cada 10ms ---
  if (millis() - tempoAnterior > 10)
  {
    valorContador++;
    if (valorContador > 9999)
      valorContador = 0;
    tempoAnterior = millis();
  }

  mostrarNoDisplay(valorContador);
}

// --- Função de multiplexação do display ---
void mostrarNoDisplay(int valor)
{
  // Separa os dígitos
  int milhar = (valor / 1000) % 10;
  int centena = (valor / 100) % 10;
  int dezena = (valor / 10) % 10;
  int unidade = valor % 10;
  int digitoValores[] = {milhar, centena, dezena, unidade};

  // Varredura dos 4 dígitos
  for (int i = 0; i < 4; i++)
  {
    digitalWrite(LATCH_PIN, LOW);
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, segMap[digitoValores[i]]); // Segmentos
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, (1 << i));                 // Seleção do dígito
    digitalWrite(LATCH_PIN, HIGH);
    delay(4); // Delay para persistência visual
  }
}
