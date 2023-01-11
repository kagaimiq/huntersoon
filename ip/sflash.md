# SFLASH

SFLASH (Serial FLASH interface) is a special hardware block that talks to the SPI flashes.

An interesting feature it has is the automatic setting of the bus type (SPI/DSPI/QSPI) based on what opcode we send.

And seems like it could map the flash contents into memory, as any other SPI flash controller usually does.

## Registers

| Name               | Offset | Description          |
|--------------------|--------|----------------------|
| INTR_STATUS        | 0x00   | Interrupt status     |
| RAW_INTR_STATUS    | 0x04   | Raw interrupt status |
| INTR_MASK          | 0x08   | Interrupt mask       |
| COMMAND            | 0x0C   | Command              |
| COMMAND_DATA0_REG  | 0x10   | Command data 0       |
| COMMAND_DATA1_REG  | 0x14   | Command data 1       |
| READ0_REG          | 0x18   | Command read 0       |
| READ1_REG          | 0x1C   | Command read 1       |
| ADDRESS_REG        | 0x20   | DMA address          |
| READ_OPCODE_REG    | 0x24   | Read opcode          |
| CONFIGURATION_0    | 0x28   | Chip 0 config        |
| CS_CONFIGURATION_0 | 0x2C   | Chip 0 CS config     |
| CONFIGURATION_1    | 0x30   | Chip 1 config        |
| CS_CONFIGURATION_1 | 0x34   | Chip 1 CS config     |
| CONFIGURATION_2    | 0x38   | Chip 2 config        |
| CS_CONFIGURATION_2 | 0x3C   | Chip 2 CS config     |
| CONFIGURATION_3    | 0x40   | Chip 3 config        |
| CS_CONFIGURATION_3 | 0x44   | Chip 3 CS config     |

### INTR_STATUS

| Bits | R/W | Default | Description        |
|------|-----|---------|--------------------|
| 31:1 | /   | /       | /                  |
| 0    | R   | 0       | Transfer completed |

### RAW_INTR_STATUS

| Bits | R/W | Default | Description        |
|------|-----|---------|--------------------|
| 31:1 | /   | /       | /                  |
| 0    | R/W | 0       | Transfer completed |

Note: interrupts are cleared by writing '1' to the desired bit.

### INTR_MASK

| Bits | R/W | Default | Description        |
|------|-----|---------|--------------------|
| 31:1 | /   | /       | /                  |
| 0    | R/W | 0       | Transfer completed |

### COMMAND

| Bits  | R/W | Default | Description                         |
|-------|-----|---------|-------------------------------------|
| 31:16 | W   | /       | DMA transfer length (0 = no dma)    |
| 15:8  | W   | /       | Command bit count (up to 64 bits)   |
| 7     | /   | /       | /                                   |
| 6     | W   | /       | Keep the chip select low            |
| 5:4   | W   | /       | Chip select number                  |
| 3     | /   | /       |                                     |
| 2     | W   | /       | ?                                   |
| 1:0   | W   | /       | Transfer type (1 = read, 2 = write) |

### COMMAND_DATA[0-1]_REG

| Bits  | R/W | Default | Description       |
|-------|-----|---------|-------------------|
| 31:0  | R/W | 0       | Command data bits |

Note: the bits are read *from the MSB*, so this means that setting the reg to e.g. 0x4d495a55
with the bit count set to 8 will send out the 0x4d, while with bit count = 16 it will send out 0x4d49.

### READ[0-1]_REG

| Bits  | R/W | Default | Description                                  |
|-------|-----|---------|----------------------------------------------|
| 31:0  | R/W | 0       | Bits received upon sending command data bits |

Note: the bits are stored *from the LSB*, so this means that with the bit count set to 8, if 0x4d was received on the bus,
then the reg will be 0x0000004d. If the bit count was set to 16, and 0x4d49 was received, then the reg will be 0x00004d49.

### ADDRESS_REG

| Bits  | R/W | Default | Description                                |
|-------|-----|---------|--------------------------------------------|
| 31:2  | R/W | 0       | DMA address (aligned to a 32-bit boundary) |
| 1:0   | /   | /       | /                                          |

Note: this advances on each DMA transfer, so if this was set to 0x20000000, and there was a 512-byte DMA transfer,
then this will advance to 0x20000200 (i.e. by 0x200 -> 512 bytes)

### READ_OPCODE_REG

| Bits  | R/W | Default | Description                                |
|-------|-----|---------|--------------------------------------------|

Seems like this is the setting of the read opcode parameters for the memory mapped SPI flash...

### CONFIGURATION_[0-3]

| Bits  | R/W | Default | Description                                |
|-------|-----|---------|--------------------------------------------|

### CS_CONFIGURATION_[0-3]

| Bits  | R/W | Default | Description                                |
|-------|-----|---------|--------------------------------------------|
