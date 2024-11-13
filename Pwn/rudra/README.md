# rudra

### Challenge Description

The ultimate website tester is here to replace curl. can you pwn it?

**MD5 Hash**: 76edb2540d468184adf52bdf3c8e6cc2

### Short Writeup

- To get a libc leak, use any functionality and get residual pointer at the last 8 bytes and leak it with `content`.
- Create a web page without the starting <html> tag to get OOB in the heap.
- Overwrite the free@got in stdc++ library to get shell.

### Flag

DYNAMIC

### Author

[**tourpran**](https://x.com/tourpran)
