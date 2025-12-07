/*
  Experiência 06 - Potenciômetro com conversão A/D:
  - Lê o potenciômetro na porta A0 (10 bits: 0 a 1023).
  - Exibe o valor no display de 7 segmentos.
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
#define POT_CHANNEL 0 // Canal ADC0 (A0)
#define DIGIT_ON_MS 4
#define DISPLAY_DIGITS 4

// Mapa de segmentos para display 7 segmentos (Anodo Comum)
// Valores: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
static const uint8_t segMap[] = {
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};

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

// Inicialização do ADC
static void adcInit(void)
{
    // AVCC como referência, canal 0 selecionado
    ADMUX = (1 << REFS0);

    // Habilita ADC, prescaler 128 (16MHz/128 = 125kHz)
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

// Leitura do ADC
static uint16_t adcRead(uint8_t channel)
{
    // Seleciona o canal (mantém REFS0)
    ADMUX = (1 << REFS0) | (channel & 0x0F);

    // Inicia conversão
    ADCSRA |= (1 << ADSC);

    // Aguarda conversão completar
    while (ADCSRA & (1 << ADSC))
        ;

    return ADC; // Retorna valor de 10 bits
}

// Função de multiplexação do display
static void show_on_display(uint16_t value)
{
    // Separa os dígitos
    uint8_t d[4];
    d[0] = (value / 1000) % 10; // Milhar
    d[1] = (value / 100) % 10;  // Centena
    d[2] = (value / 10) % 10;   // Dezena
    d[3] = value % 10;          // Unidade

    // Varredura dos 4 dígitos
    for (uint8_t i = 0; i < DISPLAY_DIGITS; i++)
    {
        PORTD &= ~(1 << LATCH_PIN); // Latch LOW
        shiftOut(segMap[d[i]]);     // Segmentos
        shiftOut(1 << i);           // Seleção do dígito
        PORTD |= (1 << LATCH_PIN);  // Latch HIGH
        _delay_ms(DIGIT_ON_MS);     // Delay para persistência visual
    }
}

int main(void)
{
    init_io();
    adcInit();
    while (1)
    {
        uint16_t reading = adcRead(POT_CHANNEL); // Valor de 0 a 1023
        show_on_display(reading);
    }
    return 0;
}
