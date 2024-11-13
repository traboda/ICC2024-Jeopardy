# side-channel analysis

### Challenge description

You are tasked with testing a **\*hardware implementation of AES-128 on an FPGA** to assess its security. A pkl file is provided, which contains the plaintext (hex), ciphertext (hex), and the corresponding power trace data captured via oscilloscope during encryption. Your goal is to use side-channel analysis to recover the encryption key and demonstrate a potential vulnerability.

`sha256sum  power_trace.pkl 089f1361038f540a28156763da10720b00c878326547da739d2426d9ef69608d`

Flag Format icc{...}

### Flag

`icc{^[Y;s[3itG;}`

### Author

**hari_1203**
