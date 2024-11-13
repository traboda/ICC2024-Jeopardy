import multiprocessing
import pebble


def is_printable_ascii(arr):
    return all(32 <= x < 127 for x in arr)


# Constants from the original code
FLAG_LENGTH = 35
DEADBEEF = 0xDEADBEEF
STATE_VECTOR_LENGTH = 624
STATE_VECTOR_M = 397

# MT19937 constants
UPPER_MASK = 0x80000000
LOWER_MASK = 0x7fffffff
TEMPERING_MASK_B = 0x9d2c5680
TEMPERING_MASK_C = 0xefc60000

# The obfuscated flag from the original code
obfuscated_xored_flag = [
    DEADBEEF ^ 26, DEADBEEF ^ 31, DEADBEEF ^ 23, DEADBEEF ^ 103, DEADBEEF ^ -120,
    DEADBEEF ^ -74, DEADBEEF ^ 108, DEADBEEF ^ 17, DEADBEEF ^ -44, DEADBEEF ^ -52,
    DEADBEEF ^ -98, DEADBEEF ^ 43, DEADBEEF ^ -89, DEADBEEF ^ -78, DEADBEEF ^ -122,
    DEADBEEF ^ 110, DEADBEEF ^ -23, DEADBEEF ^ -94, DEADBEEF ^ -2, DEADBEEF ^ 47,
    DEADBEEF ^ -97, DEADBEEF ^ 53, DEADBEEF ^ 62, DEADBEEF ^ -16, DEADBEEF ^ -123,
    DEADBEEF ^ -91, DEADBEEF ^ 20, DEADBEEF ^ 51, DEADBEEF ^ 127, DEADBEEF ^ 35,
    DEADBEEF ^ -108, DEADBEEF ^ 30, DEADBEEF ^ 92, DEADBEEF ^ 14, DEADBEEF ^ 42
]


class MTRand:
    def __init__(self):
        self.mt = [0] * STATE_VECTOR_LENGTH
        self.index = STATE_VECTOR_LENGTH + 1
    
    def seed_rand(self, seed):
        self.mt[0] = seed & 0xffffffff
        for i in range(1, STATE_VECTOR_LENGTH):
            self.mt[i] = (6069 * self.mt[i-1]) & 0xffffffff
        self.index = STATE_VECTOR_LENGTH

    def gen_rand_long(self):
        """Generate a random 32-bit number"""
        mag = [0x0, 0x9908b0df]  # mag[x] = x * 0x9908b0df for x = 0,1

        if self.index >= STATE_VECTOR_LENGTH:
            # Generate STATE_VECTOR_LENGTH words at a time
            if self.index >= STATE_VECTOR_LENGTH + 1:
                self.seed_rand(4357)
            for kk in range(0, STATE_VECTOR_LENGTH - STATE_VECTOR_M):
                y = (self.mt[kk] & UPPER_MASK) | (self.mt[kk + 1] & LOWER_MASK)
                self.mt[kk] = self.mt[kk + STATE_VECTOR_M] ^ (y >> 1) ^ mag[y & 0x1]
            for kk in range(STATE_VECTOR_LENGTH - STATE_VECTOR_M, STATE_VECTOR_LENGTH - 1):
                y = (self.mt[kk] & UPPER_MASK) | (self.mt[kk + 1] & LOWER_MASK)
                self.mt[kk] = self.mt[kk + (STATE_VECTOR_M - STATE_VECTOR_LENGTH)] ^ (y >> 1) ^ mag[y & 0x1]
            y = (self.mt[STATE_VECTOR_LENGTH - 1] & UPPER_MASK) | (self.mt[0] & LOWER_MASK)
            self.mt[STATE_VECTOR_LENGTH - 1] = self.mt[STATE_VECTOR_M - 1] ^ (y >> 1) ^ mag[y & 0x1]
            self.index = 0
        y = self.mt[self.index]
        self.index += 1

        # Tempering
        y ^= (y >> 11)
        y ^= (y << 7) & TEMPERING_MASK_B
        y ^= (y << 15) & TEMPERING_MASK_C
        y ^= (y >> 18)

        return y & 0xffffffff

    def gen_rand(self):
        """Generate a random float in [0,1]"""
        return self.gen_rand_long() / 0xffffffff


def generate_key(rand, length):
    """Generate a key of the specified length using the PRNG"""
    key = []
    for _ in range(length):
        key.append(int(rand.gen_rand() * 256))
    return key


def to_c_unsigned(num, bits=32):
    mask = (1 << bits) - 1
    if num < 0:
        return ((abs(num) ^ mask) + 1) & mask
    return num & mask


def xor_arrays(arr1, arr2):
    return [a ^ b for a, b in zip(arr1, arr2)]


def solve(chunk):
    # Try different seeds until we find one that generates a valid flag
    for seed in range(chunk[0], chunk[1]):
        if seed % 1000000 == 0:
            print(f"Trying seed: {seed}")
        rand = MTRand()
        rand.seed_rand(seed)
        key = generate_key(rand, FLAG_LENGTH)
        # XOR the key with the obfuscated flag
        potential_flag = xor_arrays(key, obfuscated_xored_flag)
        # Convert to ASCII and check if it's printable
        try:
            for i in range(len(potential_flag)):
                potential_flag[i] = potential_flag[i] ^ DEADBEEF
                if potential_flag[i] < 0:
                    potential_flag[i] = to_c_unsigned(potential_flag[i]) & 0xff
            flag_text = bytes(potential_flag).decode('ascii')
            if flag_text.startswith('icc{') and flag_text.endswith('}') and is_printable_ascii(potential_flag):
                print(f"\nFound potential flag with seed {seed}:")
                print(flag_text)
                return flag_text
        except:
            pass


if __name__ == '__main__':

    chunks = [(i * 2**24, (i + 1) * 2**24) for i in range(0, 2**32 // 2**24)]
    print(len(chunks))

    with pebble.ProcessPool(max_workers=multiprocessing.cpu_count()) as pool:
        future = pool.map(solve, chunks)
        for result in future.result():
            if result:
                break
