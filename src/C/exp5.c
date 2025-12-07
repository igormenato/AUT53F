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
#define DIGIT_ON_MS 4
#define DISPLAY_DIGITS 4
#define COUNT_MAX 9999

// Mapa de segmentos para display 7 segmentos (Anodo Comum)
// Valores: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
static const uint8_t segMap[] = {
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};

// Variáveis de estado
static uint16_t counterValue = 0;

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

static void init_io(void)
{
    DDRB |= (1 << DATA_PIN);
    DDRD |= (1 << LATCH_PIN) | (1 << CLOCK_PIN);
}

// Função de multiplexação do display
static void show_on_display(uint16_t value)
{
    // Separa os dígitos
    uint8_t thousands = (value / 1000) % 10;
    uint8_t hundreds = (value / 100) % 10;
    uint8_t tens = (value / 10) % 10;
    uint8_t ones = value % 10;
    uint8_t digitValues[] = {thousands, hundreds, tens, ones};

    // Varredura dos 4 dígitos
    for (uint8_t i = 0; i < DISPLAY_DIGITS; i++)
    {
        PORTD &= ~(1 << LATCH_PIN);       // Latch LOW
        shiftOut(segMap[digitValues[i]]); // Segmentos
        shiftOut(1 << i);                 // Seleção do dígito
        PORTD |= (1 << LATCH_PIN);        // Latch HIGH
        _delay_ms(DIGIT_ON_MS);           // Delay para persistência visual
    }
}

int main(void)
{
    init_io();
    while (1)
    {
        counterValue++;
        if (counterValue > COUNT_MAX)
        {
            counterValue = 0;
        }

        show_on_display(counterValue);
    }

    return 0;
}
