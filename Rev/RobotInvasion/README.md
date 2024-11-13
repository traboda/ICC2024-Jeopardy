# RobotInvasion

### Challenge Description

Here's a Demo, can you make a keygen?

The flag is strictly in the format:

```
icc\{[a-zA-Z0-9_]+\}
```

### Short Writeup

The challenge is a game made in the Godot Engine. It has a license check that is triggered once the player kills 7 enemies. The player is expected to input the flag as the license. The functionality will only work properly if there is file called `user_id.config` in the same directory as the binary with the ID `1337424242`. The player is expected to gather this information by reversing the binary. The binary implements several techniques to obfuscate and mislead the player. The user_id.config file check exists as a method to prevent simple dynamic techniques to solve the challenge. **The player will need to properly understand the license check functionality to brute force the flag. The script to do to can be found [here](https://github.com/akulpillai/RobotInvasion/blob/main/Admin/solver_parallel.py).**

The game itself is a simple wrapper and the license check is implemented in a different C++ binary which is dumped in `/tmp` . The license check binary uses a tamperproof, encryption and obfuscation technique detailed in “Surreptitious Software” [1]

I have also included the paper referenced in the book [here](https://github.com/akulpillai/RobotInvasion/blob/main/Admin/license_check/article-811.pdf). [2] Specifically the challenge implements a modified version of Scheme 2.

The challenge also uses the [Mersenne twister](https://github.com/ESultanik/mtwister); a personal obsession at this point I guess. :P

1. Christian Collberg and Jasvir Nagra. 2009. Surreptitious Software: Obfuscation, Watermarking, and Tamperproofing for Software Protection (1st. ed.). Addison-Wesley Professional.
2. Cappaert, J., Kisserli, N., Schellekens, D., & Preneel, B. (2006). Self-encrypting Code to Protect Against Analysis and Tampering.

### Flag

```jsx
icc{73l3ph0n3_numb3r5_w3ll_y0ukn0w}
```

### Author

[**k4iz3n**](https://x.com/akulpillai)
