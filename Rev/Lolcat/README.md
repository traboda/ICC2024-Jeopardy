# Lolcat

### Challenge Description

Chipi Chipi Chapa Chapa Roobi Roobi

**Challenge File**:

- [Primary Link](https://drive.google.com/file/d/1af3ImdPgcSMeq1R6056gLBWs-qPR8Prs/view?usp=drive_link)

**MD5 Hash**:
3444edaedd598cef6ac98c42ef82fe66

### Short Writeup

- Bypass SEH and VEH based exception handling checks
- Reflective DLL Injection
- Decrypt AES using a runtime generated key and IV which is generated based on a value from TLS BLock
- Red-herrings have been implemented
- Reverse custom encryption scheme involving matrix and complex number math
- Decryption involves a bruteforce approach to construct a non corrupted PNG

### Flag

icc{n0_c47n1p_n33d3d_70_50lv3_7h15_d3cryp710n}

### Author

[**retr0ds**](https://x.com/_retr0ds_) and [**k1n0r4**](https://x.com/k1n0r4)
