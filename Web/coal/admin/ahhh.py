from pwn import remote
from tqdm import tqdm

double = float(0.9295922494279303)
print(double)

state = double * (1 << 53)
state = int(state)
print(state)

first26 = state >> 27
second27 = state & ((1 << 27) - 1)
print(bin(first26)[2:].zfill(26), bin(second27)[2:].zfill(27))

multiplier = 0x5DEECE66D
addend = 0xB

mask = ((1<<27)-1) << (48 - 27)
oldseedupper26 = first26 << (48 - 26) & mask
newseedupper27 = second27 << (48 - 27) & mask
possibleSeeds = []

from tqdm import tqdm
for oldseed in tqdm(range(oldseedupper26, oldseedupper26 + ((1 << (48 - 26))))):
    newseed = oldseed * multiplier + addend
    newseed = newseed & ((1 << 48) - 1)
    if newseed & mask == newseedupper27:
        possibleSeeds.append(oldseed)

print(possibleSeeds)
if len(possibleSeeds) != 1:
    print('Error')
    exit(1)

seed = possibleSeeds[0]

def next(bits):
    global seed
    seed = (seed * multiplier + addend) & ((1 << 48) - 1)
    return seed >> (48 - bits)

def nextDouble():
    return ((next(26) << 27) + next(27)) / (1 << 53)

next(1)
nxtd = nextDouble()
print(nxtd)
