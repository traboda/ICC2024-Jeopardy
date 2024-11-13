# Kn1ghtF4ll

### Challenge Description 

Monitor Bob is making a plan to take down Jason Todd, The RedHood, but unfortunately his cryptowallet is lost when the batfamily got hands on it. Is it bob that sent a weapon to take that down? Can the Dark Knight save Jason Todd from the monitor? well we got this!

Flag format: `icc{md5sum(crypto-wallet-password)_md5sum(seed.seco)_md5sum(storage.seco)}`

**MD5 Hash**: f0066c7d3e2af51a32e4ad7c94fa8589  Kn1ghtF4ll.ad1

### Short Writeup

+ get the exodus wallet password from the pagefile.sys by pattern matching
+ dump the ransomware from the memdump
+ reverse the ransomware and understand what it does
+ get the key from registry and iv from environment variable for AES decryption
+ get the encrypted files from ransomware heap
+ decrypt the encrypted files using AES-CBC by using the key and iv obtained


### Handout 
[Kn1ghtF4ll.zip](https://drive.google.com/file/d/1MFprnEclNpkDrnz6Dc7jNcPzOzv1aqtT/view?usp=sharing)

### Password

```
URpJTxNg3PDjbv8zI2jC
```

### Flag 

`icc{8881b7c230ca7835c03e52ee157031f0_4c5b2adbd36341474aa8a37528bd0aad_f665c8b1287408dd78256fef59e83d1d}`

### Author

**Azr43lKn1ght** 
**sp3p3x**
**jl_24**
**gh0stkn1ght**
