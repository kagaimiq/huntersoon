# HS6620D

[Datasheet](https://github.com/fbiego/dt78/blob/master/datasheets/HS6620D_data_sheet_V3.0.pdf)

## Specs

- ARM Cortex-M3 @ 48 MHz

- Memory
  - 128 KiB SRAM
  - 256 KiB [ROM](rom.md)
  - 1 MiB SPI Flash (internal)

- Peripherals
  - 2x GPIO (31 pins total)
  - 2x UART
  - 2x SPI
  - 1x I2C
  - 1x SFLASH

## Memory map

|  Address   | Size |     Usage      |
|------------|------|----------------|
| 0x00000000 | 512k | ROM/SRAM alias |
| 0x08000000 | 256k | [ROM](rom.md)  |
| 0x10000000 | 128k | SRAM alias     |
| 0x20000000 | 128k | SRAM           |

### Peripherals

|  Address   | Bus |       Peripheral        |       Note       |
|------------|-----|-------------------------|------------------|
| 0x40000000 |  ?  | SYS                     |                  |
| 0x40001000 |  ?  | CLK                     |                  |
| 0x40030000 | APB | UART0                   | Designware IP    |
| 0x40040000 | APB | UART1                   | Designware IP    |
| 0x40050000 | APB | SPI0                    |                  |
| 0x40060000 | APB | SPI1                    |                  |
| 0x40070000 | APB | I2C                     | Designware IP    |
| 0x400A0000 | APB | hs6600 trx              |                  |
| 0x400C0000 | APB | TIMER0                  |                  |
| 0x400C0100 | APB | TIMER1                  |                  |
| 0x400C0200 | APB | TIMER2                  |                  |
| 0x400D0000 | APB | WDT                     | Designware IP    |
| 0x400E0000 |  ?  | PMU                     |                  |
| 0x400F0000 | APB | RTC                     |                  |
| 0x40120000 | AHB? | GPIO0 (pins 0..15)     | pins 0-15        |
| 0x40124000 | AHB? | GPIO1 (pins 16..30)    | pins 16-30       |
| 0x50800000 | AHB | [SFLASH](/ip/sflash.md) |                  |

## Interrupt map

| #  |       Source        |
|----|---------------------|
| 0  |                     |
| 1  |                     |
| 2  |                     |
| 3  |                     |
| 4  |                     |
| 5  |                     |
| 6  |                     |
| 7  |                     |
| 8  | UART0               |
| 9  | UART1               |
| 10 |                     |
| 11 | GPIO0/GPIO1         |
| 12 |                     |
| 13 |                     |
| 14 |                     |
| 15 |                     |
| 16 |                     |
| 17 |                     |
| 18 |                     |
| 19 |                     |
| 20 | RTC                 |
| 21 |                     |
| 22 |                     |
| 23 |                     |
| 24 |                     |
| 25 |                     |
| 26 |                     |
| 27 |                     |
| 28 | TIMER0              |
| 29 | TIMER1              |
| 30 | TIMER2              |
| 31 |                     |
| 32 |                     |
| 33 |                     |
| 34 |                     |
| 35 |                     |
| 36 |                     |
| 37 |                     |
| 38 |                     |
| 39 |                     |
| 40 |                     |
| 41 |                     |
| 42 |                     |
| 43 |                     |
| 44 |                     |
| 45 |                     |
| 46 |                     |
| 47 |                     |

## Pinmux

- 0x00 - Not connected
- 0x04 - SPI0 MOSI
- 0x05 - SPI0 CS
- 0x06 - SPI0 SCK
- 0x0c - UART0 TX
- 0x0f - UART1 RX
- 0x10 - UART1 TX
- 0x19 - SFLASH MOSI
- 0x1a - ??
- 0x1c - GPIO
- 0x1d - SFLASH x
- 0x1e - SFLASH xqio
- 0x1f - SFLASH xqio
- 0x20 - SFLASH SCK
- 0x21 - SFLASH x
- 0x36 - ?? timer/pwm ?
- 0x38 - ??
- 0x3d - ?? SWD?

**Note**: pins 9 and 15..30 have the function no. XORed by 0x03

----

## Some links

A list of some liks that is related to the HS6620D, with key points pointed out.

- [rbaron/HS6620D-smartwatch-reveng](https://github.com/rbaron/HS6620D-smartwatch-reveng)
  - Some more resources
  - SWD bits
  - A ROM dump

- [fbiego/dt78](https://github.com/fbiego/dt78)
  - The Datasheet
  - Stock fw's BLE protocol (WearFit2 protocol) (+ ESP32 server/client implementation)
  - The HS6620D is not touched at all, for a custom firmware the ESP32 is used instead.
