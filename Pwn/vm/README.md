# vm

### Challenge Description

A simple and unsuspicious vm. Nothing to see here!

**MD5 Hash**: adb6d0a6384a64bdd9c83d202ceed437

### Short Writeup

- Stack based VM with stack overflow
- Overflow onto VM struct and corrupt memory size
- R/W out-of-bounds using memory access
- Create a fake dtor_obj in the heap
- Null out pointer guard and populate dtor_list with the fake obj
- Call exit for shell

### Flag

DYNAMIC

### Author

**4rbit3r**
