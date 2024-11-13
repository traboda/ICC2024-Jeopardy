# not-spooning

### Challenge Description

if not spooning, what is it then?
(wrap the flag you get in `icc{}`)

**Challenge File**:

- [Primary Link]()

**MD5 Hash**:
`4a5798453cdd3a706c21211f872a8808`

### Short Writeup

- Control flow graph flattened binary
- Contains a ptrace based modification after forking into child process, which modifies RC4 key
- Decrypt with said key to retrieve back flag

### Flag

icc{wh4t_1s_th3_fun_1f_th3re_is_n0_0bfusc4tion?}

### Author

**-**
