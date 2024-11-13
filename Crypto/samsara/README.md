# Samsara

### Challenge Description:

With this treasure I summon...

**Challenge File**:

- [Primary Link](./Handout/Samsara.zip)

**MD5 Hash: c6006e54c704d0f88c338c0ae39df53a**:

### Short Writeup:

- This is a challenge based on Non-Consecutive LCG and Frozen Heart vulnerability on Girault's proof of identification.
- Since the outputs are non-consecutive, equations will be of the form: s′ =a^n\*s+(a^n−1 +a^n− +⋯+1)\*b mod p.
- Find similar pairs of (s,s') and then rearrange the terms and take GCD of them to find p
- Once p is found, we can recover q and construct phi(N)
- This is useful for forging proofs as e doesn't consider h in the Hash.
- h can be constructed as: h = (u\*(g)^-z)^e_inv mod N where e_inv = e^-1 mod phi(N)
- This value can be sent to the server to get the flag.

### Flag:

`icc{0n3_f1n4l_5p1n_b3f0r3_7h15_cycl3_3nd5}`

### Author:

**-**
