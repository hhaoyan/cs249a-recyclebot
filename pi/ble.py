import bluepy.btle as ble
import struct

addr = 'c0:98:e5:49:20:48'
service = '4607eda0-f65e-4d59-a9ff-84420d87a4ca'
char = '4607108a-f65e-4d59-a9ff-84420d87a4ca'

class BleController(object):
    def __init__(self, size):
        print('connecting to {}'.format(addr))
        self.angle = 360 // size
        self.bot = ble.Peripheral(addr)
        self.svc = self.bot.getServiceByUUID(service)
        self.char = self.svc.getCharacteristics(char)[0]
        print('connected')

    def setValue(self, value):
        self.char.write(struct.pack('<i', value * self.angle))

if __name__ == '__main__':
    ble = BleController(3)
    while True:
        value = int(input())
        ble.setValue(value)

