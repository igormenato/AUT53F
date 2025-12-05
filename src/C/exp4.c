/*
  Experiência 04 - Contador hexadecimal (0-F) nos 4 displays:
  - Conta de 0 a F (hexadecimal) a cada 1 segundo.
  - Exibe o mesmo valor nos 4 displays simultaneamente.
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
// Valores: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, A, b, C, d, E, F
static const uint8_t segMap[] = {
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
    0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E};

// Variáveis de estado
static uint8_t contador = 0;
static uint16_t delayCounter = 0;

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

void setup(void)
{
    // --- Configuração dos pinos como saída ---
    DDRB |= (1 << DATA_PIN);
    DDRD |= (1 << LATCH_PIN) | (1 << CLOCK_PIN);
}

void loop(void)
{
    // --- Incrementa contador a cada ~1 segundo (20 iterações * 50ms) ---
    delayCounter++;
    if (delayCounter >= 20)
    {
        contador++;
        if (contador > 15)
            contador = 0;
        delayCounter = 0;
    }

    // --- Envia dados para os displays ---
    // 0x0F liga todos os 4 dígitos simultaneamente
    PORTD &= ~(1 << LATCH_PIN); // Latch LOW
    shiftOut(segMap[contador]); // Segmentos
    shiftOut(0x0F);             // Seleção de dígitos
    PORTD |= (1 << LATCH_PIN);  // Latch HIGH

    _delay_ms(50);
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
