TARGET=BarusaMikosu

OBJS = \
	serprogthing.o

CFLAGS  = -mcpu=cortex-m3 -O2 -Werror -DF_CPU=48000000 -mthumb
ASFLAGS = -mcpu=cortex-m3
LDFLAGS = -mcpu=cortex-m3 -Tlink.ld -nostartfiles

#LDFLAGS += -Tmaskrom_stuff.ld

CROSS_COMPILE = arm-none-eabi-
CC      = $(CROSS_COMPILE)gcc
AS      = $(CROSS_COMPILE)as
OBJDUMP = $(CROSS_COMPILE)objdump
OBJCOPY = $(CROSS_COMPILE)objcopy
SIZE    = $(CROSS_COMPILE)size

all: $(TARGET) copy

.PHONY: all clean copy

clean:
	rm -f $(OBJS)
	rm -f $(TARGET)
	rm -f $(TARGET).bin
	rm -f $(TARGET).lst

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@
	$(OBJDUMP) -d $@ > $@.lst
	$(SIZE) -A $@

copy: $(TARGET)
	$(OBJCOPY) -Obinary $< $<.bin
