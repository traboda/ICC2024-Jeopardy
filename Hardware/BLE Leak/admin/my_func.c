#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// Modified TEA Encryption
void encrypt(const unsigned char *data, unsigned char *output, int data_length) {
    const uint32_t key[4] = {0xA56BABCD ^ 0x12345678, 0xDEADBEEF ^ 0x7890ABCD, 0xF2F3F542 ^ 0xFEDCBA98, 0X52333444 ^ 0x87654321};

    for (int i = 0; i < data_length; i += 8) {
        uint32_t v0 = ((uint32_t)data[i+0] << 24) | ((uint32_t)data[i+1] << 16) | ((uint32_t)data[i+2] << 8) | data[i+3];
        uint32_t v1 = ((uint32_t)data[i+4] << 24) | ((uint32_t)data[i+5] << 16) | ((uint32_t)data[i+6] << 8) | data[i+7];
        uint32_t sum = 0;

        uint32_t delta = 0x9e3779b9 ^ (v0 ^ v1);

        uint32_t fake1 = v0 ^ 0xABCDEF01;
        uint32_t fake2 = v1 ^ 0x12345678;

        for (int j = 0; j < 32; j++) {
            sum += (delta ^ fake1);
            v0 += (((v1 << 4) + (key[0] ^ fake2)) ^ (v1 + sum)) ^ (((v1 >> 5) + key[1]) ^ fake1);
            v1 += (((v0 << 4) + (key[2] ^ sum)) ^ (v0 + sum)) ^ (((v0 >> 5) + key[3]) ^ fake2);
            fake1 ^= (v0 + v1);
            fake2 ^= (sum + delta);
        }

        output[i+0] = (v0 >> 24) & 0xFF;
        output[i+1] = (v0 >> 16) & 0xFF;
        output[i+2] = (v0 >>  8) & 0xFF;
        output[i+3] = (v0 >>  0) & 0xFF;
        output[i+4] = (v1 >> 24) & 0xFF;
        output[i+5] = (v1 >> 16) & 0xFF;
        output[i+6] = (v1 >>  8) & 0xFF;
        output[i+7] = (v1 >>  0) & 0xFF;
    }
}
