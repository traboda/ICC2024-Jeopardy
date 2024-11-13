#!/bin/python3

from util import *
from flag import flag
from ecdsa.curves import SECP128r1
from ecdsa import SigningKey, VerifyingKey
import os
import sys

key = os.urandom(16)

def negentropy(*args, **kwargs):
    return HMAC(key, inp_data).digest()

sk = SigningKey.generate(curve=SECP128r1, hashfunc=MD5)
vk = sk.verifying_key

seen = set()
try: 
    print("Hello. I sign stuff. Can you sign stuff?")
    for _ in range(2):
        inp_data = bytes.fromhex(input())
        if inp_data in seen:
            sys.exit()
        seen.add(inp_data)
        if len(inp_data) != 8:
            sys.exit()
        rand_data = os.urandom(16)
        sig = sk.sign(rand_data, entropy=negentropy)
        assert vk.verify(sig, rand_data)
        print("Data:", rand_data.hex())
        print("Signature:", sig.hex())

    chall = os.urandom(16)
    print("Sign this: ")
    print(chall.hex())
    sig = bytes.fromhex(input("> "))
    if vk.verify(sig, chall):
        print(flag)

except:
    print("I am not okay")
