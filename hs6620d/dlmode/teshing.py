import serial, time
from hsproto import HunterSunDL

baudrate = 115200
port = '/dev/ttyUSB0'

with HunterSunDL(serial.Serial(port, baudrate, timeout=.1)) as hsdl:
    def read32(addr):
        return int.from_bytes(hsdl.cmd_exec(0x20D, b'\x20' + addr.to_bytes(4, 'big')), 'big')

    def write32(addr, val):
        hsdl.cmd_exec(0x20E, b'\x20' + addr.to_bytes(4, 'big') + val.to_bytes(4, 'big'))

    def wmask32(addr, mask, val):
        write32(addr, (read32(addr) & ~mask) | (val & mask))

    write32(0x41200000+4, 0xffff)
    write32(0x40001010, 0x4ffff)

    print("---clk---")
    for i in range(0, 0x100, 4):
        print("%02x: %08x" % (i, read32(0x40001000+i)))

    print("---pmu---")
    for i in range(0, 0x100, 4):
        print("%02x: %08x" % (i, read32(0x400e0000+i)))

    print("---gpio---")
    for i in range(0, 0x40, 4):
        print("%02x: %08x %08x" % (i, read32(0x41200000+i), read32(0x41240000+i)))

    print("---spi---")
    for i in range(0, 0x40, 4):
        print("%02x: %08x %08x" % (i, read32(0x40050000+i), read32(0x40060000+i)))

