from math import gcd
from ecdsa.curves import SECP128r1
from ecdsa import SigningKey, VerifyingKey
from util import MD5

def solve_congruence(a, b, m):
    """
    Solves a congruence of the form ax = b mod m.
    :param a: the parameter a
    :param b: the parameter b
    :param m: the modulus m
    :return: a generator generating solutions for x
    """
    g = gcd(a, m)
    a //= g
    b //= g
    n = m // g
    for i in range(g):
        yield (pow(a, -1, n) * b + i * n) % m

def attack(n, r, m1, m2, s1, s2):
    """
    Recovers the nonce and private key from two messages signed using the same nonce.
    :param n: the order of the elliptic curve
    :param m1: the first message
    :param r1: the signature of the first message
    :param s1: the signature of the first message
    :param m2: the second message
    :param r2: the signature of the second message
    :param s2: the signature of the second message
    :return: generates tuples containing the possible nonce and private key
    """
    for k in solve_congruence(int(s1 - s2), int(m1 - m2), int(n)):
        for x in solve_congruence(int(r), int(k * s1 - m1), int(n)):
            yield int(k), int(x)
            

m1 = MD5(bytes.fromhex(input())).digest()
sig1 = bytes.fromhex(input())
m2 = MD5(bytes.fromhex(input())).digest()
sig2 = bytes.fromhex(input())
r1, s1 = int.from_bytes(sig1[:16], 'big'), int.from_bytes(sig1[16:], 'big')
r2, s2 = int.from_bytes(sig2[:16], 'big'), int.from_bytes(sig2[16:], 'big')
m1 = int.from_bytes(m1, 'big')
m2 = int.from_bytes(m2, 'big')
m3 = bytes.fromhex(input())
for k, x in attack(SECP128r1.order, r1, m1, m2, s1, s2):
    sk = SigningKey.from_secret_exponent(x, curve=SECP128r1, hashfunc=MD5)
    vk = sk.verifying_key
    print(vk)
    sig = sk.sign(m3, k=k)
    print(sig.hex())
    