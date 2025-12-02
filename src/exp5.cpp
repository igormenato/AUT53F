/*
  Experiência 05 - Contador de 0000 a 9999
  - Utiliza multiplexação (varredura) para exibir 4 dígitos diferentes.
*/

#include <Arduino.h>

const int DATA_PIN = 8;
const int LATCH_PIN = 4;
const int CLOCK_PIN = 7;

// Mapa de segmentos (Anodo Comum)
const byte segMap[] = {
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};

int valorContador = 0;
unsigned long tempoAnterior = 0;

void mostrarNoDisplay(int valor);

void setup()
{
  pinMode(DATA_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
}

void loop()
{
  // Incrementa contador a cada 100ms (velocidade rápida para teste)
  if (millis() - tempoAnterior > 100)
  {
    valorContador++;
    if (valorContador > 9999)
      valorContador = 0;
    tempoAnterior = millis();
  }

  mostrarNoDisplay(valorContador);
}

void mostrarNoDisplay(int valor)
{
  // Separa os dígitos
  int milhar = (valor / 1000) % 10;
  int centena = (valor / 100) % 10;
  int dezena = (valor / 10) % 10;
  int unidade = valor % 10;

  // Bitmask para ativar um dígito por vez: 0x01, 0x02, 0x04, 0x08
  // Ordem reversa para coincidir gt1(milhar) a gt4(unidade) se Q0=gt1
  int digitoValores[] = {milhar, centena, dezena, unidade};

  for (int i = 0; i < 4; i++)
  {
    digitalWrite(LATCH_PIN, LOW);

    // 1. Envia segmentos do número atual
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, segMap[digitoValores[i]]);

    // 2. Envia seleção do dígito (Ativa apenas 1 transistor por vez)
    // 1 << i gera: 1, 2, 4, 8
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, (1 << i));

    digitalWrite(LATCH_PIN, HIGH);

    // Pequeno delay para brilho, mas curto o suficiente para não piscar (Persistence of Vision)
    delay(4);
  }
}
