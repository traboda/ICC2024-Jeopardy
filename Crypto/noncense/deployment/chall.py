#!/usr/bin/env sage

import hashlib
import ecdsa
from Crypto.Cipher import AES
from Crypto.Util.Padding import pad
from Crypto.Util.number import *
import random
from secret import flag

def gen(curve):
    order = curve.order
    d = random.randint(1, order-1)
    while d.bit_length() != 256:
        d = random.randint(1, order-1)
    pubkey = ecdsa.ecdsa.Public_key(curve.generator, curve.generator * d)
    privkey = ecdsa.ecdsa.Private_key(pubkey, d)
    return pubkey, privkey, d


def noncence(N,curve):
    a = [random.randint(1, curve.order - 1) for _ in range(N-2)]

    k = None
    while True:
        if k is not None:
            k_ = 0
            for j in range(N-2):
                k_ += a[j]*(k**j) % curve.order
            k = int(k_)
            yield k
        else:
            k = random.randint(1, curve.order - 1)
            yield k


def sign(msg, privkey, nonce, curve):
    msg_hash = bytes_to_long(hashlib.sha256(msg).digest()) % curve.order
    sig = privkey.sign(msg_hash, nonce)
    s, r = sig.s, sig.r
    return s, r

if __name__ == '__main__':
        
    try:
        N = 24
        curve = ecdsa.curves.SECP256k1
        pubkey, privkey, d = gen(curve)
        k = noncence(N,curve)
        m = pad(flag, 16)
        aes = AES.new(long_to_bytes(d), mode=AES.MODE_ECB)
        cipher = aes.encrypt(m)
        print(f'cipher = {cipher.hex()}')

        while True:
            msg = input('msg = ').encode()
            nonce = next(k)
            s, r = sign(msg, privkey, nonce, curve)
            print(f's = {s}, r = {r}')
            
    except Exception as e:
        exit(0)


