# Lockboxx

### Challenge Description

My boss made a password manager, he told me that everything is encrypted, and there's no way for there to be a flaw! But I don't believe him, can you help me prove him wrong?

**MD5 Hash**: a4fa308c25360cbe96749ba90e0e34ae

### Short Writeup

- Canary brute in update_master_password, also has a one address overwrite, use it to stack pivot to the bss.
- Can write payload in bss, byte-by-byte xor input with rand value, with seed as current epoch time//5 (given 5 seconds of buffer accounting for server delay).
- ret2plt for libc leak, call read and pivot to a later part of the bss.
- execve("/bin/sh", 0, 0), since the bss isnt big enough to call system.

### Flag

DYNAMIC

### Author

**-**
