from Crypto.Cipher import DES

class MD5:
    shifts = [7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
              5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
              4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
              6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21]
    
    K = [3614090360, 3905402710, 606105819, 3250441966, 4118548399, 1200080426, 2821735955, 4249261313, 1770035416, 2336552879, 4294925233, 2304563134, 1804603682, 4254626195, 2792965006, 1236535329, 4129170786, 3225465664, 643717713, 3921069994, 3593408605, 38016083, 3634488961, 3889429448, 568446438, 3275163606, 4107603335, 1163531501, 2850285829, 4243563512, 1735328473, 2368359562, 4294588738, 2272392833, 1839030562, 4259657740, 2763975236, 1272893353, 4139469664, 3200236656, 681279174, 3936430074, 3572445317, 76029189, 3654602809, 3873151461, 530742520, 3299628645, 4096336452, 1126891415, 2878612391, 4237533241, 1700485571, 2399980690, 4293915773, 2240044497, 1873313359, 4264355552, 2734768916, 1309151649, 4149444226, 3174756917, 718787259, 3951481745]
    
    def __init__(self, data=None):
        self.name = 'MD5'
        self.a0 = 0x67452301
        self.b0 = 0xefcdab89
        self.c0 = 0x98badcfe
        self.d0 = 0x10325476
        if data:
            self.update(data)
        
    def update(self, data):
        self.data = self.pad(bytes(data))
        for i in range(0, len(self.data), 64):
            self.process_block(self.data[i:i+64])
        
    def digest(self):
        return b''.join(int.to_bytes(x, 4, 'little') for x in (self.a0, self.b0, self.c0, self.d0))

    def pad(self, data):
        result = data + b'\x80'
        while len(result) % 64 != 56:
            result += b'\x00'
        return result + int.to_bytes(len(data) * 8, 8, 'little')
    
    @staticmethod
    def F(b,c,d):
        return (b & c) | (~b & d)
    @staticmethod
    def G(b,c,d):
        return (b & d) | (c & ~d)
    @staticmethod
    def H(b,c,d):
        return b ^ c ^ d
    @staticmethod
    def I(b,c,d):
        return c ^ (b | ~d)
    @staticmethod
    def leftrotate(x, c):
        return (x << c) | (x >> (32 - c))
    
    def process_block(self, block):
        try:
            assert len(block) == 64
        except AssertionError:
            print(block)
        M = [int.from_bytes(block[i:i+4], 'little') for i in range(0, 64, 4)]
        A = self.a0
        B = self.b0
        C = self.c0
        D = self.d0
        for i in range(64):
            if 0 <= i <= 15:
                f = MD5.F(B, C, D)
                g = i
            elif 16 <= i <= 31:
                f = MD5.G(B, C, D)
                g = (5*i + 1) % 16
            elif 32 <= i <= 47:
                f = MD5.H(B, C, D)
                g = (3*i + 5) % 16
            elif 48 <= i <= 63:
                f = MD5.I(B, C, D)
                g = (7*i) % 16
            f = (f + A + self.K[i] + M[g]) & 0xffffffff
            A = D
            D = C
            C = B
            B = (B + MD5.leftrotate(f, self.shifts[i])) & 0xffffffff
        self.a0 = (self.a0 + A) & 0xffffffff
        self.b0 = (self.b0 + B) & 0xffffffff
        self.c0 = (self.c0 + C) & 0xffffffff
        self.d0 = (self.d0 + D) & 0xffffffff

class HMAC:
    def __init__(self, key, data):
        self.key = key
        self.data = data
        self.name = 'HMAC'
    
    def digest(self):
        key = MD5(self.key).digest()
        return DES.new(self.data[:8], DES.MODE_ECB).encrypt(key)
    
    def hexdigest(self):
        return self.digest().hex()
        