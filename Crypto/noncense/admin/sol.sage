#!/usr/bin/env sage
import os

os.environ['TERM'] = 'xterm-256color'


import hashlib
import ecdsa
import random
from Crypto.Cipher import AES
from pwn import *
from Crypto.Util.number import long_to_bytes, bytes_to_long

def separator():
	print("-" * 150)

N = 24
assert N >= 4
usedcurve = ecdsa.curves.SECP256k1

io = remote("localhost", 5000)

print("Connected to the server")
io.recvuntil(b"cipher = ")
cipher = io.recvline().strip().decode()
print("CIPHER =", cipher)

s = []
r = []
h = []

print("Receiving the signatures")
for i in range(N):
	io.sendlineafter(b"msg = ", str(i).encode())
	sr = io.recvline().decode().split(',')
	h.append(bytes_to_long(hashlib.sha256(str(i).encode()).digest())% int(usedcurve.order))
	s.append(int(sr[0].split('=')[1]))
	r.append(int(sr[1].split('=')[1]))

assert len(s) == N and len(r) == N and len(h) == N

print("Selected curve :")
print(usedcurve.name)
separator()


s_inv = []
for i in range(N):
	s_inv.append(ecdsa.numbertheory.inverse_mod(s[i], usedcurve.order))

Z = GF(usedcurve.order)
R = PolynomialRing(Z, names=('dd',))
(dd,) = R._first_ngens(1)

# the polynomial we construct will have degree 1 + Sum_(i=1)^(i=N-3)i in dd
# our task here is to compute this polynomial in a constructive way starting from the N signatures in the given list order
# the generic formula will be given in terms of differences of nonces, i.e. k_ij = k_i - k_j where i and j are the signature indexes
# each k_ij is a first-degree polynomial in dd
# this function has the goal of returning it given i and j

def k_ij_poly(i, j):
	hi = Z(h[i])
	hj = Z(h[j])
	s_invi = Z(s_inv[i])
	s_invj = Z(s_inv[j])
	ri = Z(r[i])
	rj = Z(r[j])
	poly = dd*(ri*s_invi - rj*s_invj) + hi*s_invi - hj*s_invj
	return poly

# the idea is to compute the polynomial recursively from the given degree down to 0
# the algorithm is as follows:
# for 4 signatures the second degree polynomial is:
# k_12*k_12 - k_23*k_01
# so we can compute its coefficients.
# the polynomial for N signatures has degree 1 + Sum_(i=1)^(i=N-3)i and can be derived from the one for N-1 signatures

# let's define dpoly(i, j) recursively as the dpoly of degree i starting with index j

def dpoly(n, i, j):
	if i == 0:
		return (k_ij_poly(j+1, j+2))*(k_ij_poly(j+1, j+2)) - (k_ij_poly(j+2, j+3))*(k_ij_poly(j+0, j+1))
	else:
		left = dpoly(n, i-1, j)
		for m in range(1,i+2):
			left = left*(k_ij_poly(j+m, j+i+2))
		right = dpoly(n, i-1, j+1)
		for m in range(1,i+2):
			right = right*(k_ij_poly(j, j+m))
		return (left - right)

def print_dpoly(n, i, j):
	if i == 0:
		print('(k', j+1, j+2, '*k', j+1, j+2, '-k', j+2, j+3, '*k', j+0, j+1, ')', sep='', end='')
	else:
		print('(', sep='', end='')
		print_dpoly(n, i-1, j)
		for m in range(1,i+2):
			print('*k', j+m, j+i+2, sep='', end='')
		print('-', sep='', end='')
		print_dpoly(n, i-1, j+1)
		for m in range(1,i+2):
			print('*k', j, j+m, sep='', end='')
		print(')', sep='', end='')


print("Nonces difference equation :")
print_dpoly(N-4, N-4, 0)
print(' = 0', sep='', end='')
print()
separator()

poly_target = dpoly(N-4, N-4, 0)
print("Polynomial in d :")
print(poly_target)
separator()

d_guesses = poly_target.roots()
print("Roots of the polynomial :")
print(d_guesses)
separator()

for guess in d_guesses:
	aes = AES.new(long_to_bytes(int(guess[0])), mode=AES.MODE_ECB)
	m = aes.decrypt(bytes.fromhex(cipher))
	if b'icc{' in m:
		print("Flag found :")
		print(m)
		break
