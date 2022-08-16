# HS6620D

[Datasheet](https://github.com/fbiego/dt78/blob/master/datasheets/HS6620D_data_sheet_V3.0.pdf)

## Specs

- ARM Cortex-M3 @ 48 MHz at most

- Memory
  - 128 KiB SRAM
  - 256 KiB [ROM](rom.md)
  - 1 MiB SPI Flash (internal)

## Memory map

|   Address  |      Usage           |
|------------|----------------------|
| 0x00000000 | SRAM/ROM alias       |
| 0x08000000 | [ROM](rom.md)        |
| 0x10000000 | SRAM alias           |
| 0x20000000 | SRAM                 |

### Peripherals

|   Address  |         Usage           |
|------------|-------------------------|
| 0x40000000 | SYS                     |
| 0x40030000 | UART0                   |
| 0x40040000 | UART1                   |
| 0x40050000 | [SPI0](/ip/spi.md)      |
| 0x40060000 | [SPI1](/ip/spi.md)      |
| 0x400E0000 | PMU                     |
| 0x40120000 | [GPIO0](/ip/gpio.md) (pins 0..15)  |
| 0x40124000 | [GPIO1](/ip/gpio.md) (pins 16..30) |
| 0x50800000 | [SFLASH](/ip/sflash.md) |

## Pinmux

- 0x00 - ??N/C??
- 0x04 - SPI0 MOSI
- 0x05 - SPI0 CS
- 0x06 - SPI0 SCK
- 0x0f - UART1 RX
- 0x10 - UART1 TX
- 0x19 - SFLASH MOSI
- 0x1c - GPIO?
- 0x1d - SFLASH x
- 0x1e - SFLASH xqio
- 0x1f - SFLASH xqio
- 0x20 - SFLASH SCK
- 0x21 - SFLASH x
- 0x36 - ? timer/pwm x ?

## Other stuff

Other stuff that is related to this is listed below, with key points pointed out.

- [rbaron/HS6620D-smartwatch-reveng](https://github.com/rbaron/HS6620D-smartwatch-reveng)
  - Some more resources
  - SWD bits
  - A ROM dump

- [fbiego/dt78](https://github.com/fbiego/dt78)
  - The Datasheet
  - Stock fw's BLE protocol (WearFit2 protocol) (+ ESP32 server/client implementation)
  - The HS6620D is not touched at all, for a custom firmware the ESP32 is used instead.
