from pwn import *
from Crypto.Util.number import *
from hashlib import sha256 as Hash
import json

HOST = 'localhost'
PORT = 5000

io = remote(HOST,PORT)

g = 2
spins = [5, 3, 23, 13, 24, 6, 10, 9, 7, 4, 19, 16]
p = 1
counter = 0

while not isPrime(p):
    
    if counter:
         io.sendlineafter(b'R/J/I: ', b'I')
    counter+=1
    io.recvuntil(b'you: ')
    N = eval(io.recvuntil(b'\n')[:-1])
    io.sendlineafter(b'R/J/I: ', b'R')
    io.recvuntil(b'\n')

    output = eval(io.recvuntil(b'\n')[:-1])

    # for n in range(40):
    #     for i in range(n - 1):
    #         for j in range(i, len(spins)):
    #             if sum(spins[i: j+1]) == n:
    #                 print(n, i, j)


    S1 = (output[5] - output[11]) * (output[9] - output[12]) - (output[7] - output[12]) * (output[7] - output[11])
    S2 = (output[4] - output[8]) * (output[10] - output[11]) - (output[6] - output[11]) * (output[6] - output[8])
    p = GCD(S1, S2)
    print(isPrime(p))
    if isPrime(p):
        q = N//p
        u = pow(g,256,N)
        z = pow(g,65537,N)
        e = Hash(b''.join([long_to_bytes(x) for x in [g,N,u]]))
        in_e = int(e.hexdigest(),16)
        try: 
            einv = pow(in_e,-1,(p - 1)*(q - 1))
        except:
            p = 1
            print("failed")
            pass

io.sendlineafter(b'R/J/I: ', b'J')
print('loop broken')
h = pow(u * pow(g,-z,N),einv,N)
msg = {"h": h, "u": u, "z": z, "e": e.hexdigest()}
io.sendlineafter(b'message: ',json.dumps(msg).encode())
print(io.recvline().decode())




