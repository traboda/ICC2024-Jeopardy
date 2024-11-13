from Crypto.Util.number import long_to_bytes, getPrime
from hashlib import sha256 as Hash
from secrets import randbelow
import json

flag = 'icc{0n3_f1n4l_5p1n_b3f0r3_7h15_cycl3_3nd5}'

Initiate = 1

class Wheel_of_Samsara:
    def __init__(self):
        self.state = "DORMANT"
        self.prompt = "With this treasure, I summon..."

        self.g = 2
        self.p = getPrime(512)
        self.q = getPrime(512)

        self.N = self.p*self.q

        self.a = randbelow(self.p)
        self.b = randbelow(self.p)
        self.s = randbelow(self.p)

        self.F = lambda s: (self.a*s + self.b) % self.p

        self.reveal = f'The Wheel decided this value be known to you: {self.N}'
        

    def judgement(self, msg):
        if self.state == "ADAPT":
            self.state == "DORMANT"

            self.h = msg["h"]
            self.u = msg["u"]
            self.e = msg["e"]
            self.z = msg["z"]

            if 0 in [x%self.N for x in [self.u,self.h,self.z]]:
                return "Puny attempt."
            
            Judgement1 = (self.u == pow(self.g,self.z,self.N)*pow(self.h,int(self.e,16),self.N) % self.N)
            Judgement2 = (self.e == Hash(b''.join([long_to_bytes(x) for x in [self.g,self.N,self.u]])).hexdigest())

            if Judgement1 and Judgement2:
                return f"The Wheel stopped spinning.. {flag}"
            else:
                return f"You failed."
        else:
            return 'The Wheel is still spinning'
            
    def rotations(self):
        if self.state == "DORMANT":
            self.state = "ADAPT"
        else:
            print('The Wheel is not motivated')
            return
        spins = [5, 3, 23, 13, 24, 6, 10, 9, 7, 4, 19, 16]
        states = [self.s]
        for spin in spins:
            for _ in range(spin):
                self.s = self.F(self.s)
            states.append(self.s)

        print(spins)
        print(states)

            
while 1:
    if Initiate:
        Wheel = Wheel_of_Samsara()
        print(Wheel.prompt)
        Initiate = 0
        print(Wheel.reveal)
    
    choice = input("Choose your fate: R/J/I: ").upper()
    if choice == "R":
        Wheel.rotations()
    elif choice == "J":
        msg = json.loads(input("Your message: "))
        print(Wheel.judgement(msg))
        break
    elif choice == "I":
        print("Restarting...")
        Initiate = 1
    else:
        print("The Wheel is disappointed")
        break

    







        

