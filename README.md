# AUT53F

Repositório com experimentos de microcontroladores ATmega328p, desenvolvidos para a disciplina de Microcontroladores.

## 📁 Estrutura do Projeto

```
AUT53F/
├── src/
│   ├── exp1.cpp         # Blink com 3 métodos (delay, timer c/ ISR, timer s/ ISR)
│   ├── exp2.cpp         # LEDs alternados com botões (pressionar, soltar, manter)
│   ├── exp3.cpp         # Sequencial de LEDs com inversão (polling e PCINT)
│   ├── exp4.cpp         # Contador hexadecimal (0-F) em display 7 segmentos
│   ├── exp5.cpp         # Contador 0000-9999 com multiplexação
│   ├── exp6.cpp         # Leitura de potenciômetro (A/D) no display
│   └── C/                    # Versão em C puro
│       ├── exp1.c
│       ├── exp2.c
│       ├── exp3.c
│       ├── exp4.c
│       ├── exp5.c
│       └── exp6.c
├── build/                    # Arquivos compilados (.hex)
├── platformio.ini       # Configuração do PlatformIO
└── extra_script.py      # Script para gerar arquivos .hex
```

## 🛠️ Requisitos

- [PlatformIO](https://platformio.org/) (extensão do VS Code ou CLI)
- Arduino Uno (ou compatível ATmega328P)
- [SimulIDE](https://simulide.com/) (opcional, para simulação)

## 🚀 Como Usar

### Compilar todos os experimentos

```bash
pio run
```

Os arquivos `.hex` serão gerados em:

**Versão C++:**

- `build/EXP1/firmware.hex`
- `build/EXP2/firmware.hex`
- `build/EXP3/firmware.hex`
- `build/EXP4/firmware.hex`
- `build/EXP5/firmware.hex`
- `build/EXP6/firmware.hex`

**Versão C:**

- `build/EXP1_C/firmware.hex`
- `build/EXP2_C/firmware.hex`
- `build/EXP3_C/firmware.hex`
- `build/EXP4_C/firmware.hex`
- `build/EXP5_C/firmware.hex`
- `build/EXP6_C/firmware.hex`

### Compilar um experimento específico

```bash
pio run -e EXP1    # Compila apenas o experimento 1 (C++)
pio run -e EXP3    # Compila apenas o experimento 3 (C++)
pio run -e EXP1_C  # Compila apenas o experimento 1 (C)
pio run -e EXP3_C  # Compila apenas o experimento 3 (C)
```

### Upload para Arduino

```bash
pio run -e EXP1 -t upload   # Compila e envia para o Arduino
```

### Simulação no SimulIDE

1. Abra o SimulIDE
2. Carregue o circuito correspondente ao experimento
3. Clique com botão direito no Arduino → Load Firmware
4. Selecione o arquivo `.hex` em `build/EXPX/firmware.hex`
5. Inicie a simulação

## 📝 Descrição dos Experimentos

### EXP1 - Blink com 3 Métodos

Demonstra 3 formas de piscar LEDs simultaneamente:

- **LED 1 (Pino 10)**: Usando `delay()`
- **LED 2 (Pino 11)**: Timer1 com interrupção (ISR)
- **LED 3 (Pino 12)**: Timer2 sem interrupção (polling)

### EXP2 - LEDs Alternados com Botões

Controle de 2 LEDs com 3 tipos de entrada:

- **SW1 (A1)**: Pressionar → alterna LEDs
- **SW2 (A2)**: Soltar → alterna LEDs
- **SW3 (A3)**: Manter → pisca rápido

### EXP3 - Sequencial com Inversão

LEDs em sequência (10→11→12→13) com inversão de direção:

- **SW1 (A1)**: Inverte via polling
- **SW2 (A2)**: Inverte via interrupção PCINT

### EXP4 - Contador Hexadecimal

Contador de 0 a F exibido em 4 displays 7 segmentos simultaneamente.

- Usa 74HC595 para controle
- Incrementa a cada 1 segundo

### EXP5 - Contador 0000-9999

Contador decimal com multiplexação de displays.

- Varredura dos 4 dígitos
- Incrementa a cada 10ms

### EXP6 - Leitura A/D

Lê potenciômetro (A0) e exibe valor (0-1023) no display.

- Conversão analógico-digital de 10 bits
- Atualização em tempo real

## 🔧 Configuração

O arquivo `platformio.ini` define os ambientes de compilação para C++ e C puro:

```ini
[env:EXP1]
build_src_filter = +<exp1.cpp>

[env:EXP2]
build_src_filter = +<exp2.cpp>
# ...

[env:EXP1_C]
build_src_filter = +<C/exp1.c>

[env:EXP2_C]
build_src_filter = +<C/exp2.c>
# ...
```

Cada ambiente compila apenas o arquivo correspondente. Use a convenção:

- `EXP1`, `EXP2`, ... para versão C++
- `EXP1_C`, `EXP2_C`, ... para versão C
