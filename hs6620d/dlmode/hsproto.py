import time

class HunterSunDL:
    MaxRawPacketLen = 1500

    def __init__(self, port):
        self.port = port
        self.txcounter = 0
        self.handshake()

    def __enter__(self):
        return self

    def __exit__(self, etype, evalue, trace):
        pass

    #==============================================#

    def handshake(self):
        self.port.flush()
        self.port.write(b'\xdb\xde')
        time.sleep(.5) # neccessary!

    def rawpacket_send(self, data):
        if len(data) > self.MaxRawPacketLen:
            raise Exception('The packet data is too long! %d > %d' % (len(data), self.MaxRawPacketLen))

        odata = b''

        check = 0
        for b in data: check ^= b

        for b in [0x102] + list(data) + [0x103, check]:
            if (b == 0x10) or (b >= 0x100):
                odata += b'\x10'

            odata += bytes([b & 0xff])

        #print("TX:", ':'.join(['%02x' % b for b in data]))
        self.port.write(odata)

    def rawpacket_recv(self):
        data = b''
        check = 0
        stage = 'idle'

        def rxbyte():
            b = self.port.read(1)
            if not b: return None

            if b == b'\x10':
                b = self.port.read(1)
                if not b: return None

                if b != b'\x10':
                    return b[0] | 0x100

            return b[0]

        while True:
            b = rxbyte()
            if b is None:
                raise Exception('Failed to receive byte')

            if b == 0x102:
                if stage != 'idle':
                    raise Exception('Double start?')

                stage = 'data'

            elif b == 0x103:
                if stage == 'end':
                    raise Exception('Double stop?')

                elif stage != 'data':
                    raise Exception("We haven't started yet!")

                stage = 'end'

            elif b >= 0x100:
                raise Exception('Unknown escaped byte %02x!' % (b & 0xff))

            else:
                if stage == 'data':
                    if len(data) >= self.MaxRawPacketLen:
                        raise Exception('The message seems to be rather too long...')

                    data += bytes([b])
                    check ^= b

                elif stage == 'end':
                    if check != b:
                        raise Exception("Checksum mismatch c:%02x != r:%02x!" % (b, check))

                    break

        self.port.flush()

        #print("RX:", ':'.join(['%02x' % b for b in data]))
        return data

    #==============================================#

    #----------------------------------------#
    # awwawawa that's all mess in my opinion #
    #----------------------------------------#

    def packet_send(self, data, ptype):
        hdr = len(data) & 0x7ff
        hdr |= (ptype & 7) << 11
        hdr |= (self.txcounter & 3) << 14
        self.txcounter += 1

        data = hdr.to_bytes(2, 'big') + data
        self.rawpacket_send(data)

    def packet_recv(self):
        data = self.rawpacket_recv()
        if len(data) < 2:
            raise Exception("Received packet doesn't contain enough space for a header")

        hdr = int.from_bytes(data[:2], 'big')
        plen = hdr & 0x7ff
        ptype = (hdr >> 11) & 7
        counter = (hdr >> 14) & 3

        data = data[2:]

        if len(data) < plen:
            raise Exception('Claimed length (%d) is bigger than the actual packet length (%d)' % (plen, len(data)))

        return data[:plen], ptype, counter

    def cmd_exec(self, cmd, data):
        self.packet_send(bytes([cmd & 0xff]) + data, cmd >> 8)
        resp = self.packet_recv()

        if resp[1] != cmd >> 8:
            raise Exception("Command type or whatever doesn't match, c:%x != r:%x!" % (cmd >> 8, resp[1]))

        resp = resp[0]
        cmd &= 0xff

        if len(resp) < 1:
            raise Exception("Response doesn't even have a command number!")

        if resp[0] != cmd:
            raise Exception("Command number doesn't match, c:%02x != r:%02x!" % (cmd, resp[0]))

        return resp[1:]

    #==============================================#

    def cmd_echo(self, data):
        return self.cmd_exec(0x001, data)

    def cmd_setbaudrate(self, baud):
        return int.from_bytes(self.cmd_exec(0x003, baud.to_bytes(4, 'big')), 'big')


