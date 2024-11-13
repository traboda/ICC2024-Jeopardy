# ppppp

### Challenge Description

PassPassPassPassPass

**MD5 Hash**: 9554d86ccf6d52a1b82eca95fff7dbfa

### Short Writeup

+ crypto.randomBytes function not called so static string, get guest password+hash
+ using guest hash add-motd message
+ ASI bug -> pass message as array and get html injection
+ Use base tag to steal admin password when bot submits forms
+ ujson bug to pass other filename -> overwrite python file
+ Read /flag.txt

### Flag

icc{Pefect_Plaintext_Password_Protection_Package_313370nG0nG}

### Tested by

**Web**

### Author

**-**  