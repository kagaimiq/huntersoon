# SFLASH

SFLASH (Serial FLASH)

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
    - Each reg contains the bits that is sent MSB-first
      i.e. value of 0xacebecaf, with bit count = 32 will be sent out as [ac eb ec af],
      and the same thing with bit count = 20 will be sent out as [ac eb eX]

reg18: read0 reg
reg1C: read1 reg
    Data received while sending command data
    - Same deal as in command data regs

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
