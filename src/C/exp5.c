/*
  Experiência 05 - Contador de 0000 a 9999:
  - Utiliza multiplexação (varredura) para exibir 4 dígitos diferentes.
  - Incrementa a cada ciclo de display (~16ms).
*/

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>

// Definição dos pinos (74HC595)
#define DATA_PIN PB0  // Pino 8 - DS (dados)
#define LATCH_PIN PD4 // Pino 4 - STCP (latch)
#define CLOCK_PIN PD7 // Pino 7 - SHCP (clock)

// Mapa de segmentos para display 7 segmentos (Anodo Comum)
// Valores: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
static const uint8_t segMap[] = {
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};

// Variáveis de estado
static uint16_t valorContador = 0;
static uint8_t cycleCount = 0;

// Função para enviar um byte via shiftOut (MSB primeiro)
static void shiftOut(uint8_t data)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        // Envia bit mais significativo primeiro
        if (data & 0x80)
            PORTB |= (1 << DATA_PIN);
        else
            PORTB &= ~(1 << DATA_PIN);

        // Pulso de clock
        PORTD |= (1 << CLOCK_PIN);
        PORTD &= ~(1 << CLOCK_PIN);

        data <<= 1;
    }
}

// Função de multiplexação do display
static void mostrarNoDisplay(uint16_t valor)
{
    // Separa os dígitos
    uint8_t milhar = (valor / 1000) % 10;
    uint8_t centena = (valor / 100) % 10;
    uint8_t dezena = (valor / 10) % 10;
    uint8_t unidade = valor % 10;
    uint8_t digitoValores[] = {milhar, centena, dezena, unidade};

    // Varredura dos 4 dígitos
    for (uint8_t i = 0; i < 4; i++)
    {
        PORTD &= ~(1 << LATCH_PIN);         // Latch LOW
        shiftOut(segMap[digitoValores[i]]); // Segmentos
        shiftOut(1 << i);                   // Seleção do dígito
        PORTD |= (1 << LATCH_PIN);          // Latch HIGH
        _delay_ms(4);                       // Delay para persistência visual
    }
}

void setup(void)
{
    // --- Configuração dos pinos como saída ---
    DDRB |= (1 << DATA_PIN);
    DDRD |= (1 << LATCH_PIN) | (1 << CLOCK_PIN);
}

void loop(void)
{
    // --- Incrementa contador aproximadamente a cada 10ms ---
    // Como mostrarNoDisplay leva ~16ms (4 dígitos * 4ms), incrementamos a cada ciclo
    // Para simular ~10ms, contamos ciclos
    cycleCount++;
    if (cycleCount >= 1) // Aproximadamente cada ciclo de display (~16ms)
    {
        valorContador++;
        if (valorContador > 9999)
            valorContador = 0;
        cycleCount = 0;
    }

    mostrarNoDisplay(valorContador);
}

int main(void)
{
    setup();
    while (1)
    {
        loop();
    }
    return 0;
}
