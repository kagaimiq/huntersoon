# A Mi band clone or something like that

Some fitness bracelet (or how do you call it, a smartwatch, fitness watch?) that came branded as some Mi Band but in fact it's not.

I thought it was based on an **nRF52832** MCU like the watch i had previously (can't remember what it was),
but it turned out to be based on a **HS6620D** instead.

Board label: **B-MI3P_V10**

- HS6620D MCU
- 80x160 IPS LCD (whatever it is)
- Some heart rate sensor
- Some accelerometer (reg0F "who am i" == 0x11, i2c addr 0x19)
- There could be a SPI flash i guess

## Photos

No photos yet

## GPIO usage

|  #  |              Usage                 |
|-----|------------------------------------|
|   0 | SWCLK **exposed as CLK**           |
|   1 | SWDIO **exposed as DIO**           |
|   2 |                                    |
|   3 | Vibration motor enable             |
|   4 | Download mode trigger **exposed as DL** |
|   5 | UART1 TX **exposed as TX**         |
|   6 | UART1 RX **exposed as RX**         |
|   7 | Heart rate something (pin 2)       |
|   8 | ext SPI flash MISO                 |
|   9 | ext SPI flash WP                   |
|  10 | ext SPI flash MOSI                 |
|  11 | LCD RS (command/data)              |
|  12 | LCD RES (reset)                    |
|  13 | LCD CS (select)                    |
|  14 | ext SPI flash SCK                  |
|  15 | ext SPI flash HOLD+VDD -- power is supplied directly from a pin?? |
|  16 | Accelerometer SCL                  |
|  17 |                                    |
|  18 | LCD SDA (data)                     |
|  19 | LCD backlight enable               |
|  20 |                                    |
|  21 | Touch button                       |
|  22 | LCD SCL (clock)                    |
|  23 | Heart rate something (pin 3)       |
|  24 | Heart rate LED (pin 4)             |
|  25 |                                    |
|  26 | ext SPI flash CS                   |
|  27 |                                    |
|  28 | Accelerometer SDA                  |
|  29 | probably accelerometer             |
|  30 |                                    |
