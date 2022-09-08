# SFLASH

SFLASH (Serial FLASH) is an dedicated controller for an internal/external SPI flashes,
which primarily built upon DMA transfers, and might even map the flash into memory!

It supports the DSPI and QSPI modes, which are selected automatically depending
on the command you send! That's not what your average SPI controller does...

## Registers

```
reg00: intr status
reg04: raw intr status
reg08: intr mask
    b0 = Transfer completed

reg0C: command
    b0 = Start transfer
    b2 = ?
    b4~b5 = Chip select number
    b8~b15 = Command bit count
    b16~b31 = DMA transfer length

reg10: command data0 reg
reg14: command data1 reg
    Command bits
    the MSB (bit31) of the whole reg is sent first.
    e.g. 0xACEBECA0,0xFECABECA 
       with bit count = 48 will be sent as ACEBECA0FECA,
            bit count = 24 will be sent as ACEBEC.

reg18: read0 reg
reg1C: read1 reg
    Received data bits (which were received upon sending command bits)

reg20: address reg
    b2~b31 = DMA address

reg24: read opcode reg
    

reg28: configuration 0
reg2C: cs configuration 0
reg30: configuration 1
reg34: cs configuration 1
reg38: configuration 2
reg3C: cs configuration 2
reg40: configuration 3
reg44: cs configuration 3
```
