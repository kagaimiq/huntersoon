import serial, time
from hsproto import HunterSunDL

baudrate = 115200
port = '/dev/ttyUSB0'

with HunterSunDL(serial.Serial(port, baudrate, timeout=.1)) as hsdl:
    baudrate = hsdl.cmd_setbaudrate(921600)

with HunterSunDL(serial.Serial(port, baudrate, timeout=.1)) as hsdl:

    def read32(addr):
        return int.from_bytes(hsdl.cmd_exec(0x20D, b'\x20' + addr.to_bytes(4, 'big')), 'big')

    def write32(addr, val):
        hsdl.cmd_exec(0x20E, b'\x20' + addr.to_bytes(4, 'big') + val.to_bytes(4, 'big'))

    #for i in range(0, 0x100, 4):
    #    print('>>>>%02x: %08x' % (i, read32(0x40000000+i)))

    #hsdl.cmd_exec(0x290, b'\x00\x06') # enable write
    #hsdl.cmd_exec(0x290, b'\x00\x60') # erase whole chip

    for i in range(100, -1, -1):
        print("...%d" % i)
        stat = int.from_bytes(hsdl.cmd_exec(0x291, b''), 'big')
        if not (stat & 1): break

    '''
    with open('flash2.bin', 'wb') as f:
        for addr in range(0x0000, 0x100000, 0x100):
            print('%08x' % addr)

            write32(0x50800010, (0x3b<<24)|addr)
            write32(0x50800020, 0x20000000)
            write32(0x5080000C, (0x100<<16)|((32+8)<<8)|(0<<4)|1)

            while not (read32(0x50800004) & 1): pass
            write32(0x50800004, 1)

            print('%08x' % read32(0x20000000))

            for i in range(0, 0x100, 4):
                f.write(read32(0x20000000+i).to_bytes(4, 'little'))
    '''

    with open('flash.bin', 'rb') as f:
        addr = 0x0

        while True:
            print("%06x" % addr)

            data = f.read(0x1000)
            if not data: break

            hsdl.cmd_exec(0x290, b'\x00\x06') # enable write

            hsdl.cmd_exec(0x293, addr.to_bytes(4, 'big')) # erase

            for i in range(100, -1, -1):
                stat = int.from_bytes(hsdl.cmd_exec(0x291, b''), 'big')
                if not (stat & 1): break

            for off in range(0, len(data), 0x100):
                datapage = data[off:off+0x100]

                hsdl.cmd_exec(0x290, b'\x00\x06') # enable write

                hsdl.cmd_exec(0x294, int.to_bytes(addr+off, 4, 'big') + len(datapage).to_bytes(4, 'big') + datapage) # program

                for i in range(100, -1, -1):
                    stat = int.from_bytes(hsdl.cmd_exec(0x291, b''), 'big')
                    if not (stat & 1): break

            addr += len(data)
