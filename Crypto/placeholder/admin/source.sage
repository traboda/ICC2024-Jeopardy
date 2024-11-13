import pickle

q = 2^8
v = 70
m = 40


n = v + m
F = GF(q)


def make_upper(M, n2):
  for i in range(0, n2):
      for j in range(i, n2):
          M[i, j] = M[i, j] + M[j, i]
          M[j, i] = 0
  return M


def map():
    mapmat = []
    
    for s in range(0, m):
        M = zero_matrix(F, n, n)
        for i in range(0, v):
            M[i, ((((i + 1) + (s + 1) - 1) % v))]   = F.random_element()
            M[i, (((i + 1) + (s + 1) - 2) % m) + v] = F.random_element()
        M = make_upper(M, n)
        mapmat.append(M)
    return mapmat


def gens(message: bytes):
    m_int = int.from_bytes(message, byteorder='big')
    m_bin = bin(m_int)[2:]
    
    if len(m_bin) % 8 != 0:
        m_bin = '0'*(8 - len(m_bin) % 8) + m_bin
    
    m_mat = [[0 for _ in range(m)] for _ in range(v)]
    
    for i in range(0, len(m_bin)):
        m_mat[i // m][i % m] = int(m_bin[i])
    
    m_mat = Matrix(F, m_mat)
    
    
    ret_mat = block_matrix([ 
                            [identity_matrix(F,v), m_mat], 
                            [zero_matrix(F,m,v), identity_matrix(F,m)] 
                            ])
    
    return ret_mat


def encrypt(R, S):
    
    S = S.submatrix(0, v, v, m)
    F1 = []
    for s in range(m):
        F1.append(R[s].submatrix(0, 0, v, v))

    F2 = []
    for s in range(m):
        F2.append(R[s].submatrix(0, v, v, m))
    
    P1 = []
    for s in range(m):
        P1.append(F1[s])
    
    P2 = []
    for s in range(m):
        P2.append((F1[s] + F1[s].transpose())*S + F2[s])
    
    P3 = []
    for s in range(m):
        P3.append(make_upper(S.transpose()*F1[s]*S+S.transpose()*F2[s], m))
        
    P = []
    for s in range(m):
        P.append(block_matrix([[P1[s], P2[s]], [zero_matrix(F, m, v), P3[s]]]))
    
    return P


flag = open("flag.txt","rb").read()
assert len(flag) == 34

R = map()
S = gens(flag)
P = encrypt(R, S)

with open("encrypted.pickle", "wb") as f:
    pickle.dump(P, f)
