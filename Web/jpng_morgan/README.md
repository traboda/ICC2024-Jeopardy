# Jpng Morgan

## Challenge Description

Jpng Morgan just started hiring recently, but it seems like you are too poor to buy anything

**MD5 Hash**: b5d47dc81427e0e21d2c549317703f3ecfcc4b61

### Short Writeup

- The challenge consists of two services: one web and the other internal, with only the web service being exposed.
- Normal users can only upload a single png file to convert it to jpeg. To upload a `.tar.gz` file, users need to be premium members.
- The `app.secret_key` is set with `app.secret_key = hex(random.getrandbits(20))`, making it susceptible to brute force attacks and allowing token forgery.
- There is a zip slip vulnerability that allows an upload to overwrite `cron.sh` and connect to the internal server via the `/dev/tcp` file descriptor.
- There is also Server-Side Template Injection (SSTI) present in [`main.py`](http://main.py) internal service.
- SSTI payloads can be sent via the `/dev/tcp` file descriptor.

### Flag

icc{jpng_m0rg4n_w4s_n3v3r_r34l_26h4ff6}

## Tested by

**Web**

## Author

**-**
