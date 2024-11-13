# calc

### Challenge Description

An experimental calculator that uses JIT to speed up calculations, or does it? what harm could some math expression really do?

**MD5 Hash**: 63dacfc0756b62209be4503c102a64dc

### Short Writeup

- rdx is used as the index register to fetch constants during the JIT evaluation
- mul instruction overwrites rdx with the upper 64 bits of the multiplication overflow
- using the same constant as the last one, adds optimization so rdx isn't updated
- a mul instruction followed by loading the same constant can casue out-of-bounds access loading arbitrary values
- the epilogue of the jit function is not included in the code size calculation
- the pop rax in the epilogue may be overwritten by the end of jit instructions
- chaining these bugs, an arbitrary address can be loaded, offsets added and executed
- this enable execution of libc functions such as system

### Flag

DYNAMIC

### Author

[**k1R4**](https://x.com/justk1R4)
