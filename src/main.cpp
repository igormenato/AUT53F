#include <Arduino.h>

#if defined(BUILD_EXP1)

/*
  Experiência 01 - Blink de 1 segundo, simultaneamente
  - Led1: Pisca controlado por delay() no loop principal.
  - Led2: Pisca controlado por Interrupção do Timer1 (Hardware).
*/

const int ledPinDelay = 10; // Led1
const int ledPinTimer = 11; // Led2

void setup()
{
  pinMode(ledPinDelay, OUTPUT);
  pinMode(ledPinTimer, OUTPUT);

  // --- Configuração do Timer1 para interrupção a cada 1 segundo (1Hz) ---
  noInterrupts(); // Desabilita interrupções globalmente
  TCCR1A = 0;     // Limpa registradores de controle do Timer1
  TCCR1B = 0;
  TCNT1 = 0; // Zera contador

  // Define valor de comparação para 0.5Hz (500ms) para igualar ao delay
  // 7812 = (16000000 / (1024 * 2)) - 1  -> Frequência de toggle de 2Hz (para piscar 1Hz completo)
  OCR1A = 7812;

  // Modo CTC (Clear Timer on Compare Match)
  TCCR1B |= (1 << WGM12);
  // Prescaler de 1024
  TCCR1B |= (1 << CS12) | (1 << CS10);
  // Habilita interrupção de comparação (Timer1 Compare Match A)
  TIMSK1 |= (1 << OCIE1A);
  interrupts(); // Habilita interrupções novamente
}

// Rotina de Serviço de Interrupção (ISR) do Timer1
// Executa automaticamente a cada 1 segundo, independentemente do loop()
ISR(TIMER1_COMPA_vect)
{
  digitalWrite(ledPinTimer, !digitalRead(ledPinTimer)); // Inverte estado do Led2
}

void loop()
{
  // Blink convencional com Delay para o Led1
  digitalWrite(ledPinDelay, HIGH);
  delay(500); // 500ms ligado
  digitalWrite(ledPinDelay, LOW);
  delay(500); // 500ms desligado
  // Total = 1 segundo de ciclo
}

#elif defined(BUILD_EXP2)

/*
  Experiência 02 - Ligar e desligar 2 leds de forma alternada
  - Pressionar SW1 (A1): Liga modo alternado.
  - Soltar SW2 (A2): Desliga tudo (ou para alternância).
  - Manter SW3 (A3): Pisca alternado rápido enquanto pressionado (função 'manter').
*/

const int ledA = 10;
const int ledB = 11;
const int bt1 = A1;
const int bt2 = A2;
const int bt3 = A3;

bool sistemaAtivo = false;
bool estadoBt2Anterior = HIGH; // Para detectar 'soltar' (Borda de subida)

void setup()
{
  pinMode(ledA, OUTPUT);
  pinMode(ledB, OUTPUT);
  // Botões no esquema Simulide costumam ter pull-up e ativam em LOW
  pinMode(bt1, INPUT_PULLUP);
  pinMode(bt2, INPUT_PULLUP);
  pinMode(bt3, INPUT_PULLUP);
}

void loop()
{
  // Leitura dos botões
  bool b1State = digitalRead(bt1);
  bool b2State = digitalRead(bt2);
  bool b3State = digitalRead(bt3);

  // 1. Pressionar a chave SW1 (Detecta nível LOW) -> Ativa sistema
  if (b1State == LOW)
  {
    sistemaAtivo = true;
  }

  // 2. Soltar a chave SW2 (Detecta transição LOW -> HIGH)
  if (estadoBt2Anterior == LOW && b2State == HIGH)
  {
    sistemaAtivo = false;
    digitalWrite(ledA, LOW);
    digitalWrite(ledB, LOW);
  }
  estadoBt2Anterior = b2State;

  // 3. Manter a chave SW3 pressionada (Prioridade Alta)
  if (b3State == LOW)
  {
    // Pisca rápido enquanto segura
    digitalWrite(ledA, HIGH);
    digitalWrite(ledB, LOW);
    delay(100);
    digitalWrite(ledA, LOW);
    digitalWrite(ledB, HIGH);
    delay(100);
  }
  else if (sistemaAtivo)
  {
    // Modo alternado padrão (ativado pelo SW1)
    digitalWrite(ledA, HIGH);
    digitalWrite(ledB, LOW);
    delay(500);
    digitalWrite(ledA, LOW);
    digitalWrite(ledB, HIGH);
    delay(500);
  }
  else
  {
    // Estado de repouso
    digitalWrite(ledA, LOW);
    digitalWrite(ledB, LOW);
  }
}

#elif defined(BUILD_EXP3)

/*
  Experiência 03 - Sequencial de LEDs com inversão de direção
  - Chave sem interrupção (A1): Inverte direção via Polling.
  - Chave com interrupção (A2): Inverte direção via PCINT (Pin Change Interrupt).

  AJUSTE: Lógica invertida para LEDs (Active LOW).
  - LOW = Acende
  - HIGH = Apaga
  Isso corrige o efeito de "rasto" deixando apenas 1 LED aceso.
*/

const int leds[] = {10, 11, 12, 13};
const int btPolled = A1;
const int btInterrupt = A2; // Usaremos PCINT1

volatile bool direcao = true; // true = 10->13, false = 13->10
bool lastBtPolled = HIGH;

void setup()
{
  for (int i = 0; i < 4; i++)
  {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], HIGH); // Garante que iniciam apagados (Lógica: HIGH = Off)
  }
  pinMode(btPolled, INPUT_PULLUP);
  pinMode(btInterrupt, INPUT_PULLUP);

  // --- Configuração da Interrupção no Pino A2 (PCINT10) ---
  // A porta analógica A2 pertence ao grupo PCINT1 (Port C)
  noInterrupts();
  PCICR |= (1 << PCIE1);    // Habilita interrupção para o grupo Port C
  PCMSK1 |= (1 << PCINT10); // Habilita interrupção especificamente para o pino A2
  interrupts();
}

// Rotina de interrupção para o grupo Port C (A0-A5)
ISR(PCINT1_vect)
{
  // Verifica se o pino A2 está LOW (pressionado)
  if (digitalRead(A2) == LOW)
  {
    direcao = !direcao; // Inverte direção
  }
}

void loop()
{
  // Leitura do botão sem interrupção (Polling)
  bool leitura = digitalRead(btPolled);
  if (leitura == LOW && lastBtPolled == HIGH)
  {
    direcao = !direcao;
    delay(50); // Debounce simples
  }
  lastBtPolled = leitura;

  // Executa o sequencial

  // 1. Apaga todos (Active LOW: HIGH apaga)
  for (int i = 0; i < 4; i++)
    digitalWrite(leds[i], HIGH);

  // 2. Acende o atual (Active LOW: LOW acende)
  static int atual = 0;
  digitalWrite(leds[atual], LOW);
  delay(200);

  // Calcula o próximo
  if (direcao)
  {
    atual++;
    if (atual > 3)
      atual = 0;
  }
  else
  {
    atual--;
    if (atual < 0)
      atual = 3;
  }
}

#elif defined(BUILD_EXP4)

/*
  Experiência 04 - Contador 0 a F (Hex) simultâneo nos 4 displays.
  - Com Flicker: Adicionado delay na varredura para tornar a troca visível.
*/

// Pinos do 74HC595
const int DATA_PIN = 8;  // DS
const int LATCH_PIN = 4; // STC
const int CLOCK_PIN = 7; // SHC

// Mapa de segmentos para Anodo Comum (0 = Ligado, 1 = Desligado)
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

  // "Com Flicker": Pequeno delay que faz o display piscar se estivéssemos varrendo,
  // ou simplesmente mostra atualização lenta. Se o objetivo é ver a varredura falhar:
  delay(50); // Introduz flicker visível se houvesse varredura
}

#elif defined(BUILD_EXP5)

/*
  Experiência 05 - Contador de 0000 a 9999
  - Utiliza multiplexação (varredura) para exibir 4 dígitos diferentes.
*/

void mostrarNoDisplay(int valor); // Forward declaration

const int DATA_PIN = 8;
const int LATCH_PIN = 4;
const int CLOCK_PIN = 7;

// Mapa de segmentos (Anodo Comum)
const byte segMap[] = {
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};

int valorContador = 0;
unsigned long tempoAnterior = 0;

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

    // Apaga displays para evitar "ghosting" antes de trocar
    // (Opcional, mas recomendado se houver fantasmas)
    // digitalWrite(LATCH_PIN, LOW);
    // shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, 0xFF); // Segmentos apagados
    // shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, 0x00); // Digitos desligados
    // digitalWrite(LATCH_PIN, HIGH);
  }
}

#elif defined(BUILD_EXP6)

/*
  Experiência 06 - Potenciômetro para A/D e saída no display
  - Lê porta A0 (10 bits: 0 a 1023)
  - Exibe valor constantemente.
*/

void mostrarNoDisplay(int valor); // Forward declaration

const int DATA_PIN = 8;
const int LATCH_PIN = 4;
const int CLOCK_PIN = 7;
const int POT_PIN = A0;

const byte segMap[] = {
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};

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

#else
#error "No build variant defined. Please define one of BUILD_EXP1 to BUILD_EXP6."
#endif