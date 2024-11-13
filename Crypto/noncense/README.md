# noncense

### Challenge Description

Man is blindest where he thinks he sees most clearly; his deceptions are his truest reflections.

### Short Writeup

the polynomial we construct will have degree 1 + Sum_(i=1)^(i=N-3)i in dd <br>
our task here is to compute this polynomial in a constructive way starting from the N signatures in the given list order<br>
the generic formula will be given in terms of differences of nonces, i.e. k_ij = k_i - k_j where i and j are the signature indexes<br>
each k_ij is a first-degree polynomial in dd<br>
this function has the goal of returning it given i and j
```py
def k_ij_poly(i, j):
	hi = Z(h[i])
	hj = Z(h[j])
	s_invi = Z(s_inv[i])
	s_invj = Z(s_inv[j])
	ri = Z(r[i])
	rj = Z(r[j])
	poly = dd*(ri*s_invi - rj*s_invj) + hi*s_invi - hj*s_invj
	return poly
```
the idea is to compute the polynomial recursively from the given degree down to 0<br>
the algorithm is as follows:<br>
for 4 signatures the second degree polynomial is:
k_12*k_12 - k_23*k_01<br>
so we can compute its coefficients.<br>
the polynomial for N signatures has degree 1 + Sum_(i=1)^(i=N-3)i and can be derived from the one for N-1 signatures<br>

let's define dpoly(i, j) recursively as the dpoly of degree i starting with index j
```py
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
```
### Flag

icc{1f_G0d_1s_d34d_3v3ryth1ng_1s_p3rm1tt3d}

### Author

**-**  