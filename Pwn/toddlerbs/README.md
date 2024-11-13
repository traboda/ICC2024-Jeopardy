# toddlerbs

### Challenge Description

just another useless bs program which does _nothing_ (or does it?).

Note: Depending on the approach the exploit might take a while to run, so be patient.

**Challenge File**:

- [Primary Link](https://drive.google.com/file/d/1jmk45PnbIx3dBBCuqviB-xvWhAB1fgt0/view?usp=share_link)

**MD5 Hash**: 767b2e297673df1801e3825bfe041366

### Short Writeup

- The program doesnt check for the index, which allows users to do out of bounds writes with the increment and decrement functions.
- This allows users to modify the instructions, and the flag is written in the binary,
- the flag is placed in the 2nd sector, so write shellcode to read the flag from the 2nd sector and print it. and overwrite one of the jump instructions to jump to the shellcode.

### Flag

icc{b00ts3ct0rs_are_n0ic3_3rtk3p5}

### Author

**-**
