import pickle

q = 2^8
v = 70
m = 40


n = v + m
K = GF(q)

def StoM(S):
    m_bin = ''
    for i in range(34 * 8):
        m_bin += str(S[i//m, i%m])
    m_int = int(m_bin, 2)
    return bytes.fromhex(hex(m_int)[2:])

P = pickle.load(open('encrypted.pickle', 'rb'))

def V2(i, s):
    return ((((i+1)+(s+1)-1) % v))

def O2(i, s):
    return ((((i+1)+(s+1)-2) % m) + v)

def Upper(M, nn):
  for i in range(0, nn):
      for j in range(i, nn):
          M[i, j] += M[j, i]
          M[j, i] = 0
  return M

def MQSignHeatMap(s):
    H = zero_matrix(K, n, n)
    for i in range(0, v):
        H[i, V2(i, s)] = 1
        H[i, O2(i, s)] = 1
    H = Upper(H, n)
    return H

def GetSolution(P):
    P1 = [P[s].submatrix(0, 0, v, v) for s in range(0, m)]
    P2 = [P[s].submatrix(0, v, v, m) for s in range(0, m)]

    P1_tilde = [P1[s] + P1[s].transpose() for s in range(0, m)]
    BigP = block_matrix([ P1_tilde[s] for s in range(0, m) ] , ncols=1)

    H = [MQSignHeatMap(s) for s in range(0, m)]
    H2 = [H[s].submatrix(0, v, v, m) for s in range(0, m)] #Heat map of F2

    cols = []
    for j in range(0, m):
        b = block_matrix([ P2[s].submatrix(0, j, v, 1) for s in range(0, m) ] , ncols=1)
        A = BigP
        #remove non valid rows
        cpt_removed = 0
        for s in range(0, m):
            for i in range(0, v):
                if H2[s][i][j] != 0:
                    A = A.delete_rows([s*v+i-cpt_removed])
                    b = b.delete_rows([s*v+i-cpt_removed])
                    cpt_removed += 1
        cols = cols + [A.solve_right(b)]

    S_sol = block_matrix([ cols[j] for j in range(0, m) ] , nrows=1)

    return S_sol

S_sol = GetSolution(P)

print(StoM(S_sol).decode())