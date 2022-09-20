import serial, time
from hsproto import HunterSunDL

baudrate = 115200
port = '/dev/ttyUSB0'

with HunterSunDL(serial.Serial(port, baudrate, timeout=.1)) as hsdl:
    def read8(addr):
        return int.from_bytes(hsdl.cmd_exec(0x20D, b'\x08' + addr.to_bytes(4, 'big')), 'big')

    def write8(addr, val):
        hsdl.cmd_exec(0x20E, b'\x08' + addr.to_bytes(4, 'big') + val.to_bytes(4, 'big'))

    def read32(addr):
        return int.from_bytes(hsdl.cmd_exec(0x20D, b'\x20' + addr.to_bytes(4, 'big')), 'big')

    def write32(addr, val):
        hsdl.cmd_exec(0x20E, b'\x20' + addr.to_bytes(4, 'big') + val.to_bytes(4, 'big'))

    def wmask32(addr, mask, val):
        write32(addr, (read32(addr) & ~mask) | (val & mask))

    def wsmask32(addr, shift, mask, val):
        write32(addr, (read32(addr) & ~(mask << shift)) | ((val & mask) << shift))

    def rsmask32(addr, shift, mask):
        return (read32(addr) >> shift) & mask

    #------------

    # 0x04 -> CPU
    write32(0x40001000+0x08, 0xa)
    write32(0x40001000+0x0c, 1) 
    # 0x10 -> SFLASH
    write32(0x40001000+0x14, 0x106|0) #
    write32(0x40001000+0x18, 0x106|0) #
    write32(0x40001000+0x1c, 0x106|0) #
    # 0x20 -> UART0
    # 0x24 -> UART1
    write32(0x40001000+0x28, 0x106|0) # I2C
    write32(0x40001000+0x30, 0) # SPI0, gate only
    write32(0x40001000+0x34, 0) # WDT, gate only
    write32(0x40001000+0x3c, 0x106|0) #
    write32(0x40001000+0x44, 0x106|0) #
    write32(0x40001000+0x4c, 0x106|0) #
    write32(0x40001000+0x54, 0) # gate only
    write32(0x40001000+0x58, 0) # gate only
    write32(0x40001000+0x5c, 0x106|0) #
    write32(0x40001000+0x60, 0) # SPI1, gate only
    write32(0x40001000+0x64, 0) # GPIO, gate only
    write32(0x40001000+0x68, 0) # gate only
    write32(0x40001000+0x74, 0) # gate only
    write32(0x40001000+0x78, 0) # gate only
    write32(0x40001000+0x7c, 0) # gate only

    def GPIO_SetPinFunc(pin, func):
        if (pin == 9) or (pin >= 15):
            func ^= 3

        wsmask32(0x40000080 + ((pin // 4) * 4), (pin % 4) * 8, 0xff, func)

    def GPIO_SetDirection(pin, output):
        if pin >= 16:
            pin -= 16
            addr = 0x41240000
        else:
            addr = 0x41200000

        write32(addr + (0x10 if output else 0x14), 1 << pin)

    def GPIO_SetValue(pin, value):
        if pin >= 16:
            pin -= 16
            addr = 0x41240000
        else:
            addr = 0x41200000

        wsmask32(addr + 0x04, pin, 1, int(value))

    def GPIO_GetValue(pin):
        if pin >= 16:
            pin -= 16
            addr = 0x41240000
        else:
            addr = 0x41200000

        return bool(rsmask32(addr + 0x00, pin, 1))


    print("---gpio---")
    for i in range(0, 0x40, 4):
        print("%02x: %08x %08x" % (i, read32(0x41200000+i), read32(0x41240000+i)))


    GPIO_SetPinFunc(24, 0x1c)

    GPIO_SetDirection(24, True)

    GPIO_SetValue(24, False)
    time.sleep(.25)
    GPIO_SetValue(24, True)
    time.sleep(.25)
    GPIO_SetValue(24, False)
    time.sleep(.25)
    GPIO_SetValue(24, True)


    # load the code into SRAM
    with open('hai/BarusaMikosu.bin', 'rb') as f:
        a = 0x20000000
        while True:
            if (a & 0xff) == 0: print("%08x" % a)

            rd = f.read(4)
            if len(rd) < 1: break
            write32(a, int.from_bytes(rd, 'little'))
            a += len(rd)

    try:
        # reset into SRAM mapping
        wsmask32(0x400e003c, 5, 1, 1)
        wmask32(0x400e003c, 0x1f, 0x06)
    except:
        pass

    exit()



    #--------------

    '''
    dRES = 12
    dCS  = 13
    dRS  = 11
    dSDA = 18
    dSCL = 22
    dBL  = 19

    GPIO_SetValue(dRES, True)
    GPIO_SetValue(dCS,  True)
    GPIO_SetValue(dRS,  False)
    GPIO_SetValue(dSDA, False)
    GPIO_SetValue(dSCL, False)
    GPIO_SetValue(dBL,  False)

    GPIO_SetDirection(dRES, True)
    GPIO_SetDirection(dCS,  True)
    GPIO_SetDirection(dRS,  True)
    GPIO_SetDirection(dSDA, True)
    GPIO_SetDirection(dSCL, True)
    GPIO_SetDirection(dBL,  True)

    GPIO_SetPinFunc(dRES, 0x1c)
    GPIO_SetPinFunc(dCS,  0x1c)
    GPIO_SetPinFunc(dRS,  0x1c)
    GPIO_SetPinFunc(dSDA, 0x1c)
    GPIO_SetPinFunc(dSCL, 0x1c)
    GPIO_SetPinFunc(dBL,  0x1c)

    def dDBI_SPISend(val):
        #GPIO_SetDirection(dSDA, True)

        print("%02x" % val)

        for i in range(8):
            GPIO_SetValue(dSDA, (val & (0x80 >> i)) != 0)
            GPIO_SetValue(dSCL, True)
            GPIO_SetValue(dSCL, False)

    def dDBI_SendCMD(val):
        GPIO_SetValue(dRS, False)
        dDBI_SPISend(val)

    def dDBI_SendDAT(data):
        if isinstance(data, int): data = [data]

        GPIO_SetValue(dRS, True)
        for b in data: dDBI_SPISend(b)

    inits = [
        [0x01],     # soft reset
        [0x11],     # get out of sleep mode
        [0x28],     # turn on display

        [0xfe],
        [0xef],
        [0xb3, 0x2b],
        [0xb5, 0x01],
        [0xb6, 0x11],
        [0xac, 0x0b],
        [0xb4, 0x21],
        [0xb1, 0xc8],
        [0xc0, 0xc3],
        [0xc6, 0x1f, 0x00],
        [0xf8, 0x80, 0x06],
        [0xf3, 0x01, 0x03],
        [0xf5, 0x48, 0x90],
        [0xb2, 0x0d],
        [0xea, 0x65],
        [0xeb, 0x75, 0x66],
        [0xb0, 0x3d],
        [0xc2, 0x0d],
        [0xc3, 0x4c],
        [0xc4, 0x10],
        [0xc5, 0x10],
        [0xe6, 0x40, 0x27],
        [0xe7, 0x60, 0x07],
        [0xa3, 0x12],
        [0xf0, 0x17, 0x39, 0x1b, 0x4e, 0x95, 0x32, 0x33, 0x00, 0x12, 0x0a, 0x09, 0x12, 0x12, 0x0f],
        [0xf1, 0x17, 0x39, 0x1b, 0x4e, 0x65, 0x32, 0x33, 0x00, 0x12, 0x0a, 0x09, 0x0e, 0x0c, 0x0f],
        [0xfe],
        [0xef],

        [0x13],         # set normal display
        [0x29],         # turn on display
        [0x36, 0x68],   # memory data acc ctrl
        [0x3a, 0x05],   # pixel format: DBI=RGB565
        #[0x3a, 0x06],  # pixel format: DBI=RGB666

        [0x2a, 0x00,0x00, 0x00,0x9f], # h window - 0~160
        [0x2b, 0x00,0x14, 0x00,0x67], # v window - 24~103

        [0x2c, 0x55,0x55, 0xaa,0xaa, 0x30,0x30, 0x55,0x55], # data
    ]

    GPIO_SetValue(dCS, False)

    for cseq in inits:
        dDBI_SendCMD(cseq[0])
        if len(cseq) > 1:
            dDBI_SendDAT(cseq[1:])

    GPIO_SetValue(dCS, True)

    GPIO_SetValue(dBL, True)
    '''

    #--------------

    '''
    aSCL = 16
    aSDA = 29
    aIRQ = 28

    GPIO_SetValue(aSCL, False)
    GPIO_SetValue(aSDA, False)

    GPIO_SetDirection(aSCL, False)
    GPIO_SetDirection(aSDA, False)
    GPIO_SetDirection(aIRQ, False)

    GPIO_SetPinFunc(aSCL, 0x1c)
    GPIO_SetPinFunc(aSDA, 0x1c)
    GPIO_SetPinFunc(aIRQ, 0x1c)

    def aI2C_Start():
        GPIO_SetDirection(aSDA, True)
        GPIO_SetDirection(aSCL, True)

    def aI2C_Stop():
        GPIO_SetDirection(aSCL, False)
        GPIO_SetDirection(aSDA, False)

    def aI2C_Send(val):
        for b in range(8):
            GPIO_SetDirection(aSDA, (val & (0x80 >> b)) == 0)
            GPIO_SetDirection(aSCL, False)
            GPIO_SetDirection(aSCL, True)

        GPIO_SetDirection(aSDA, False)
        GPIO_SetDirection(aSCL, False)
        ack = not GPIO_GetValue(aSDA)
        GPIO_SetDirection(aSCL, True)

        return ack


    for i in range(0x80):
        # start
        aI2C_Start()

        # sla+w
        print("%02x %d" % (i, aI2C_Send(i << 1)))

        # stop
        aI2C_Stop()
    '''


    '''
    GPIO_SetPinFunc(24, 0x0c)

    clkdiv = int((48000000 / 16 / 19200) * 0x100) & 0xffff

    write32(0x40001020, 6 | (clkdiv & 0xff00) | ((clkdiv & 0xff) << 24))

    print("%08x %08x" % (read32(0x40001020), read32(0x40001024)))

    print("---uart---")
    for i in range(0, 0x100, 4):
        print("%02x: %08x %08x" % (i, read32(0x40030000+i), read32(0x40040000+i)))

    write32(0x40030000+0x0c, 0x00) # lcr
    write32(0x40030000+0x04, 0x00) # icr
    write32(0x40030000+0x08, 0x00) # fsr ---?
    write32(0x40030000+0x0c, 0x80) # lcr: en dll/dlh access
    write32(0x40030000+0x00, 0x01) # dll
    write32(0x40030000+0x04, 0x00) # dlh
    write32(0x40030000+0x0c, 0x03) # lcr: 8n1

    for c in "Kagami Hiiragi":
        write32(0x40030000+0x00, ord(c))
        print('%08x' % read32(0x40030000+0x14))
    '''

    exit()

    ###################################################################################

    #print("---clk---")
    #for i in range(0, 0x80, 4):
    #    print("%02x: %08x" % (i, read32(0x40001000+i)))

    #print("---spi---")
    #for i in range(0, 0x40, 4):
    #    print("%02x: %08x %08x" % (i, read32(0x40050000+i), read32(0x40060000+i)))

    #print("---i2c---")
    #for i in range(0, 0x100, 4):
    #    print("%02x: %08x" % (i, read32(0x40070000+i)))

    #print("---timer---")
    #for i in range(0, 0x100, 4):
    #    print("%02x: %08x %08x %08x" % (i, read32(0x400C0000+i), read32(0x400C0100+i), read32(0x400C0200+i)))

    #print("---wdt---")
    #for i in range(0, 0x100, 4):
    #    print("%02x: %08x" % (i, read32(0x400D0000+i)))

    #print("---pmu---")
    #for i in range(0, 0x100, 4):
    #    print("%02x: %08x" % (i, read32(0x400e0000+i)))

    #print("---rtc---")
    #for i in range(0, 0x100, 4):
    #    print("%02x: %08x" % (i, read32(0x400f0000+i)))

    #print("---gpio---")
    #for i in range(0, 0x40, 4):
    #    print("%02x: %08x %08x" % (i, read32(0x41200000+i), read32(0x41240000+i)))

    #print("-----rangescan4dwc------")
    #for i in range(0x00, 0x20):
    #    addr = 0x40000000 + (i << 16)
    #    print("%08x: %08x %08x" % (addr, read32(addr + 0xf8), read32(addr + 0xfc)))


