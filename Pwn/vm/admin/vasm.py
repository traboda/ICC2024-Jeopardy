from pwn import p8, p16, p32, p64

PUSH = 0x1
POP = 0x2
ADD = 0x3
SUB = 0x4
MUL = 0x5
DIV = 0x6
LDI = 0x8
LDR = 0x9
JEQ = 0xA
JLT = 0xB
JGT = 0xC
JMP = 0xD
HLT = 0xE

opcodes = {
    "PUSH": PUSH,
    "POP": POP,
    "ADD": ADD,
    "SUB": SUB,
    "MUL": MUL,
    "DIV": DIV,
    "LDI": LDI,
    "LDR": LDR,
    "JEQ": JEQ,
    "JLT": JLT,
    "JGT": JGT,
    "JMP": JMP,
    "HLT": HLT
}


def parse_num(num):
    try:
        if num.startswith("0X"):
            return int(num,16)
        else:
            return int(num)
    except:
        return None



def assemble(code):
    labels = {}
    markers = []
    bytecode = b""
    curr_ins = b""

    for line in code.split("\n"):
        curr_ins = b""
        
        line = line.lstrip(" ").rstrip(" ")
        if line.startswith("#"):
            continue

        if line.endswith(":"):
            labels[line[:-1].upper()] = len(bytecode)
            continue

        for token in line.split():
            token = token.upper()
            if token in opcodes.keys():
                curr_ins += p8(opcodes[token])
            else:
                if token.startswith("*"):
                    count = parse_num(token[1:])
                    bytecode += curr_ins*count
                    curr_ins = b""
                    break

                elif token.startswith("$"):
                    curr_ins += b"mark%04d" % len(markers)
                    markers.append(token[1:])

                else:
                    num = parse_num(token)
                    if num != None:
                        curr_ins += p64(num)
                    else:
                        print(f"[-] Invalid token: {token}")

        bytecode += curr_ins

    for m in range(len(markers)):

        if markers[m] not in labels.keys():
            print(f"Label {markers[m]} not found")
            exit()

        bytecode = bytecode.replace(b"mark%04d" % m, p64(labels[markers[m]]))

    return bytecode