
#include "pch.h"
#include <windows.h>
#include <iostream>  
#include <fstream>
#include <vector>
#include <bitset>
#include <complex>
#include <algorithm>
#include <cstring>
#include <cstdint>
#include <iomanip>
#include "xxhash.h"
#include <wincrypt.h>
#pragma comment(lib, "crypt32.lib")
#include <winternl.h>
#include <Windows.h>
#include <libloaderapi.h>
#include <stdlib.h>


#define BUFSIZE 1024
#define RVA2VA(type, base, rva)(type)((ULONG_PTR) base + rva)


typedef void* LPVOID;
typedef unsigned __int64 QWORD;

typedef struct BASE_RELOCATION_ENTRY {
    USHORT Offset : 12;
    USHORT Type : 4;
} BASE_RELOCATION_ENTRY, * PBASE_RELOCATION_ENTRY;

typedef struct BASE_RELOCATION_BLOCK {
    DWORD PageAddress;
    DWORD BlockSize;
} BASE_RELOCATION_BLOCK, * PBASE_RELOCATION_BLOCK;

typedef enum _SECTION_INHERIT {
    ViewShare = 1,
    ViewUnmap = 2
} SECTION_INHERIT, * PSECTION_INHERIT;

DWORD tlsIndex;

using namespace std;
typedef union uwb {
    unsigned w;
    unsigned char b[4];
} MD5union;

typedef unsigned DigestArray[4];

static unsigned func0(unsigned abcd[]) {
    return (abcd[1] & abcd[2]) | (~abcd[1] & abcd[3]);
}

static unsigned func1(unsigned abcd[]) {
    return (abcd[3] & abcd[1]) | (~abcd[3] & abcd[2]);
}

static unsigned func2(unsigned abcd[]) {
    return  abcd[1] ^ abcd[2] ^ abcd[3];
}

static unsigned func3(unsigned abcd[]) {
    return abcd[2] ^ (abcd[1] | ~abcd[3]);
}

typedef unsigned(*DgstFctn)(unsigned a[]);

static unsigned* calctable(unsigned* k)
{
    double s, pwr;
    int i;

    pwr = pow(2.0, 32);
    for (i = 0; i < 64; i++) {
        s = fabs(sin(1.0 + i));
        k[i] = (unsigned)(s * pwr);
    }
    return k;
}

static unsigned rol(unsigned r, short N)
{
    unsigned  mask1 = (1 << N) - 1;
    return ((r >> (32 - N)) & mask1) | ((r << N) & ~mask1);
}

static unsigned* MD5Hash(string msg)
{
    int mlen = msg.length();
    static DigestArray h0 = { 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476 };
    static DgstFctn ff[] = { &func0, &func1, &func2, &func3 };
    static short M[] = { 1, 5, 3, 7 };
    static short O[] = { 0, 1, 5, 0 };
    static short rot0[] = { 7, 12, 17, 22 };
    static short rot1[] = { 5, 9, 14, 20 };
    static short rot2[] = { 4, 11, 16, 23 };
    static short rot3[] = { 6, 10, 15, 21 };
    static short* rots[] = { rot0, rot1, rot2, rot3 };
    static unsigned kspace[64];
    static unsigned* k;

    static DigestArray h;
    DigestArray abcd;
    DgstFctn fctn;
    short m, o, g;
    unsigned f;
    short* rotn;
    union {
        unsigned w[16];
        char     b[64];
    }mm;
    int os = 0;
    int grp, grps, q, p;
    unsigned char* msg2;

    if (k == NULL) k = calctable(kspace);

    for (q = 0; q < 4; q++) h[q] = h0[q];

    {
        grps = 1 + (mlen + 8) / 64;
        msg2 = (unsigned char*)malloc(64 * grps);
        memcpy(msg2, msg.c_str(), mlen);
        msg2[mlen] = (unsigned char)0x80;
        q = mlen + 1;
        while (q < 64 * grps) { msg2[q] = 0; q++; }
        {
            MD5union u;
            u.w = 8 * mlen;
            q -= 8;
            memcpy(msg2 + q, &u.w, 4);
        }
    }

    for (grp = 0; grp < grps; grp++)
    {
        memcpy(mm.b, msg2 + os, 64);
        for (q = 0; q < 4; q++) abcd[q] = h[q];
        for (p = 0; p < 4; p++) {
            fctn = ff[p];
            rotn = rots[p];
            m = M[p]; o = O[p];
            for (q = 0; q < 16; q++) {
                g = (m * q + o) % 16;
                f = abcd[1] + rol(abcd[0] + fctn(abcd) + k[q + 16 * p] + mm.w[g], rotn[q % 4]);

                abcd[0] = abcd[3];
                abcd[3] = abcd[2];
                abcd[2] = abcd[1];
                abcd[1] = f;
            }
        }
        for (p = 0; p < 4; p++)
            h[p] += abcd[p];
        os += 64;
    }

    return h;
}

static string GetMD5String(string msg) {
    string str;
    int j, k;
    unsigned* d = MD5Hash(msg);
    MD5union u;
    for (j = 0; j < 4; j++) {
        u.w = d[j];
        char s[9];
        sprintf_s(s, "%02x%02x%02x%02x", u.b[0], u.b[1], u.b[2], u.b[3]);
        str += s;
    }

    return str;
}



class TinyMT {
    
private:
    struct tinymt32_t {
        uint32_t status[4];
        uint32_t mat1;
        uint32_t mat2;
        uint32_t tmat;
    };

    tinymt32_t state;

    void tinymt32_init(uint32_t seed) {
        state.status[0] = seed;
        state.status[1] = state.mat1;
        state.status[2] = state.mat2;
        state.status[3] = state.tmat;
        for (int i = 1; i < 8; i++) {
            state.status[i & 3] ^= i + UINT32_C(1812433253) * (state.status[(i - 1) & 3] ^ (state.status[(i - 1) & 3] >> 30));
        }
    }

public:
    TinyMT(uint32_t mat1 = 0x8f7011ee, uint32_t mat2 = 0xfc78ff1f, uint32_t tmat = 0x3793fdff) {
        state.mat1 = mat1;
        state.mat2 = mat2;
        state.tmat = tmat;
    }

    void seed(uint32_t seed) {
        tinymt32_init(seed);
    }

    uint32_t generate_uint32() {
        uint32_t y = state.status[3];
        uint32_t x = (state.status[0] & UINT32_C(0x7fffffff)) ^ state.status[1] ^ state.status[2];
        x ^= (x << 1);
        y ^= (y >> 1) ^ x;
        state.status[0] = state.status[1];
        state.status[1] = state.status[2];
        state.status[2] = x ^ (y << 10);
        state.status[3] = y;
        state.status[1] ^= -((int32_t)(y & 1)) & state.mat1;
        state.status[2] ^= -((int32_t)(y & 1)) & state.mat2;
        return y;
    }

    uint8_t generate_byte() {
        static uint32_t last_value = 0;
        static int byte_index = 0;

        if (byte_index == 0) {
            last_value = generate_uint32();
        }

        uint8_t result = static_cast<uint8_t>((last_value >> (byte_index * 8)) & 0xFF);
        byte_index = (byte_index + 1) % 4;

        return result;
    }
};

class AES256 {
private:
    static const uint8_t SBOX[256];
    static const uint8_t INV_SBOX[256];
    static const uint8_t RCON[11];

    std::vector<uint32_t> round_keys;

    uint32_t sub_word(uint32_t word) {
        uint32_t result = 0;
        for (int i = 0; i < 4; i++) {
            result |= static_cast<uint32_t>(SBOX[(word >> (i * 8)) & 0xFF]) << (i * 8);
        }
        return result;
    }

    uint32_t rot_word(uint32_t word) {
        return ((word << 8) | (word >> 24)) & 0xFFFFFFFF;
    }

    void key_expansion(const std::vector<uint8_t>& key) {
        round_keys.resize(60);

        
        for (int i = 0; i < 8; i++) {
            round_keys[i] = (static_cast<uint32_t>(key[4 * i]) << 24) |
                (static_cast<uint32_t>(key[4 * i + 1]) << 16) |
                (static_cast<uint32_t>(key[4 * i + 2]) << 8) |
                static_cast<uint32_t>(key[4 * i + 3]);
        }

        
        for (int i = 8; i < 60; i++) {
            uint32_t temp = round_keys[i - 1];
            if (i % 8 == 0) {
                temp = sub_word(rot_word(temp)) ^ (static_cast<uint32_t>(RCON[i / 8]) << 24);
            }
            else if (i % 8 == 4) {
                temp = sub_word(temp);
            }
            round_keys[i] = round_keys[i - 8] ^ temp;
        }
    }

    void add_round_key(uint8_t state[4][4], int round) {
        for (int i = 0; i < 4; i++) {
            uint32_t rk = round_keys[round * 4 + i];
            for (int j = 0; j < 4; j++) {
                state[j][i] ^= static_cast<uint8_t>((rk >> (24 - j * 8)) & 0xFF);
            }
        }
    }

    void sub_bytes(uint8_t state[4][4]) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                state[i][j] = SBOX[state[i][j]];
            }
        }
    }

    void shift_rows(uint8_t state[4][4]) {
        uint8_t temp;

        
        temp = state[1][0];
        state[1][0] = state[1][1];
        state[1][1] = state[1][2];
        state[1][2] = state[1][3];
        state[1][3] = temp;

        
        temp = state[2][0];
        state[2][0] = state[2][2];
        state[2][2] = temp;
        temp = state[2][1];
        state[2][1] = state[2][3];
        state[2][3] = temp;

        
        temp = state[3][3];
        state[3][3] = state[3][2];
        state[3][2] = state[3][1];
        state[3][1] = state[3][0];
        state[3][0] = temp;
    }

    uint8_t gmul(uint8_t a, uint8_t b) {
        uint8_t p = 0;
        uint8_t hi_bit_set;
        for (int i = 0; i < 8; i++) {
            if (b & 1)
                p ^= a;
            hi_bit_set = (a & 0x80);
            a <<= 1;
            if (hi_bit_set)
                a ^= 0x1b;
            b >>= 1;
        }
        return p;
    }

    void mix_columns(uint8_t state[4][4]) {
        uint8_t temp[4];
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                temp[j] = state[j][i];
            }
            state[0][i] = gmul(0x02, temp[0]) ^ gmul(0x03, temp[1]) ^ temp[2] ^ temp[3];
            state[1][i] = temp[0] ^ gmul(0x02, temp[1]) ^ gmul(0x03, temp[2]) ^ temp[3];
            state[2][i] = temp[0] ^ temp[1] ^ gmul(0x02, temp[2]) ^ gmul(0x03, temp[3]);
            state[3][i] = gmul(0x03, temp[0]) ^ temp[1] ^ temp[2] ^ gmul(0x02, temp[3]);
        }
    }

    void encrypt_block(uint8_t state[4][4]) {
        add_round_key(state, 0);

        for (int round = 1; round < 14; round++) {
            sub_bytes(state);
            shift_rows(state);
            mix_columns(state);
            add_round_key(state, round);
        }

        sub_bytes(state);
        shift_rows(state);
        add_round_key(state, 14);
    }

public:
    AES256(const std::vector<uint8_t>& key) {
        if (key.size() != 32) {
            
        }
        key_expansion(key);
    }

    std::vector<uint8_t> encrypt_cbc(const std::vector<uint8_t>& data, const std::vector<uint8_t>& iv) {
        if (iv.size() != 16) {
            
        }

        
        std::vector<uint8_t> padded_data = data;
        size_t padding_size = 16 - (data.size() % 16);
        padded_data.insert(padded_data.end(), padding_size, static_cast<uint8_t>(padding_size));

        std::vector<uint8_t> encrypted_data(padded_data.size());
        uint8_t previous_block[16];
        std::copy(iv.begin(), iv.end(), previous_block);

        
        for (size_t i = 0; i < padded_data.size(); i += 16) {
            uint8_t state[4][4];

            
            for (int j = 0; j < 16; j++) {
                uint8_t current_byte = padded_data[i + j];
                current_byte ^= previous_block[j];
                state[j % 4][j / 4] = current_byte;
            }

            encrypt_block(state);

            
            for (int j = 0; j < 16; j++) {
                uint8_t encrypted_byte = state[j % 4][j / 4];
                encrypted_data[i + j] = encrypted_byte;
                previous_block[j] = encrypted_byte;
            }
        }

        return encrypted_data;
    }
};


const uint8_t AES256::SBOX[256] = {
   0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
                        0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
                        0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
                        0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
                        0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
                        0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
                        0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
                        0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
                        0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
                        0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
                        0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
                        0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
                        0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
                        0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
                        0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
                        0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};


const uint8_t AES256::INV_SBOX[256] = {
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
};


const uint8_t AES256::RCON[11] = {
    0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
};


class ByteExtractor {
private:
    std::string exe_path;
    std::vector<uint8_t> random_offsets;
    std::vector<uint8_t> extracted_bytes;

    std::string get_executable_path() {
        char path[MAX_PATH];
        GetModuleFileNameA(NULL, path, MAX_PATH);
        return std::string(path);
    }

    void generate_random_offsets(const std::string& seed_string, size_t count) {
        TinyMT tinymt;
        uint32_t seed = 0;
        for (size_t i = 0; i < (((seed_string.length()) < (size_t(4))) ? (seed_string.length()) : (size_t(4))); ++i) {
            seed = (seed << 8) | static_cast<unsigned char>(seed_string[i]);
        }
        tinymt.seed(seed);

        random_offsets.clear();
        for (size_t i = 0; i < count; ++i) {
            random_offsets.push_back(tinymt.generate_byte());
        }
    }

    void extract_bytes_from_file() {
        std::ifstream file(exe_path, std::ios::binary);
        if (!file) {
            
        }

        file.seekg(0, std::ios::end);
        std::streampos file_size = file.tellg();
        file.seekg(0, std::ios::beg);

        extracted_bytes.clear();
        for (uint8_t offset : random_offsets) {
            std::streampos pos = static_cast<std::streampos>(offset) % file_size;
            file.seekg(pos);
            char byte;
            if (file.get(byte)) {
                extracted_bytes.push_back(static_cast<uint8_t>(byte));
            }
        }
    }

public:
    ByteExtractor(const std::string& seed_string) {
        exe_path = get_executable_path();
        generate_random_offsets(seed_string, 48); 
        extract_bytes_from_file();
    }

    std::vector<uint8_t> get_extracted_bytes() const {
        return extracted_bytes;
    }

    void print_results() {
        std::cout << "Executable path: " << exe_path << std::endl;

        std::cout << "\nIV (first 16 bytes):" << std::endl;
        for (size_t i = 0; i < 16; ++i) {
            std::cout << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(extracted_bytes[i]) << " ";
            if ((i + 1) % 8 == 0) std::cout << std::endl;
        }

        std::cout << "\nKey (next 32 bytes):" << std::endl;
        for (size_t i = 16; i < 48; ++i) {
            std::cout << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(extracted_bytes[i]) << " ";
            if ((i + 1) % 8 == 0) std::cout << std::endl;
        }
        std::cout << std::endl;
    }
};



class FileProcessor {
public:
    std::vector<unsigned char> readFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        std::vector<unsigned char> fileBytes;

        if (file) {
            unsigned char byte;
            while (file.read(reinterpret_cast<char*>(&byte), 1)) {
                fileBytes.push_back(byte);
            }
        }
        else {
            
            exit(1);
        }
        file.close();
        return fileBytes;
    }

    std::vector<unsigned char> xorBytes(const std::vector<unsigned char>& bytes) {
        std::vector<unsigned char> result;

        for (size_t i = 0; i < bytes.size() - 1; ++i) {
            result.push_back(bytes[i] ^ bytes[i + 1]);
        }

        
        result.push_back(bytes.back() ^ 0x00);
        return result;
    }

    
    std::string changeFileExtension(const std::string& filename, const std::string& newExtension) {
        size_t dotPos = filename.find_last_of('.');
        return filename.substr(0, dotPos) + newExtension;
    }

    
    void writeToFile(const std::string& filename, std::vector<uint8_t>& aes_encrypted_data) {
        std::ofstream outFile(filename, std::ios::binary);

        if (!outFile) {
            
            exit(1);
        }

        for (const auto& result : aes_encrypted_data) {
            outFile.write(reinterpret_cast<const char*>(&result), sizeof(result));
        }

        outFile.close();
        
    }
};


class MatrixManipulator {
public:
    
    void diagonalManipulation(std::vector<std::bitset<8>>& matrix) {
        int n = 8;

        
        for (int i = 0; i < n / 2; ++i) {
            for (int j = 0; j < n; ++j) {
                bool temp = matrix[i][j];
                matrix[i][j] = matrix[n - 1 - i][n - 1 - j];
                matrix[n - 1 - i][n - 1 - j] = temp;
            }
        }
    }

    
    void fillMatrixWithBits(const std::vector<std::bitset<8>>& originalMatrix, std::vector<std::bitset<8>>& newMatrix) {
        size_t c = 0; 

        
        for (int i = 3; i >= 0; i--) {
            int g = (4 - i) * 2;

            for (int a = 0; a < g; a++) {
                newMatrix[i][i + a] = originalMatrix[c / 8][c % 8];
                
                c++;
            }

            for (int a = 1; a < g - 1; a++) {
                newMatrix[i + a][i + g - 1] = originalMatrix[c / 8][c % 8];
                
                c++;
            }

            for (int a = 0; a < g; a++) {
                newMatrix[i + g - 1][i + g - 1 - a] = originalMatrix[c / 8][c % 8];
                
                c++;
            }

            for (int a = 1; a < g - 1; a++) {
                newMatrix[i + g - 1 - a][i] = originalMatrix[c / 8][c % 8];
                
                c++;
            }
        }
    }

    
    std::vector<unsigned char> columnsToBytes(const std::vector<std::bitset<8>>& matrix) {
        std::vector<unsigned char> byteArray(8);

        for (int col = 0; col < 8; ++col) {
            std::bitset<8> columnBits;

            for (int row = 0; row < 8; ++row) {
                columnBits[row] = matrix[row][col];
            }

            byteArray[col] = static_cast<unsigned char>(columnBits.to_ulong());
        }

        return byteArray;
    }
};


#include <iostream>
#include <windows.h>



HMODULE GetModuleHandleH(uint64_t dwModuleNameHash) {

    if (dwModuleNameHash == NULL)
        return NULL;

#ifdef _WIN64
    PPEB					pPeb = (PEB*)(__readgsqword(0x60));
#elif _WIN32
    PPEB					pPeb = (PEB*)(__readfsdword(0x30));
#endif

    PPEB_LDR_DATA			pLdr = (PPEB_LDR_DATA)(pPeb->Ldr);
    PLDR_DATA_TABLE_ENTRY	pDte = (PLDR_DATA_TABLE_ENTRY)(pLdr->InMemoryOrderModuleList.Flink);

    while (pDte) {

        if (pDte->FullDllName.Length != NULL && pDte->FullDllName.Length < MAX_PATH) {

            
            CHAR UpperCaseDllName[MAX_PATH];

            DWORD i = 0;
            while (pDte->FullDllName.Buffer[i]) {
                UpperCaseDllName[i] = (CHAR)toupper(pDte->FullDllName.Buffer[i]);
                i++;
            }
            UpperCaseDllName[i] = '\0';
            std::string Uppercasedllname_s = UpperCaseDllName;
            
            
            if (XXH64(Uppercasedllname_s.c_str(), Uppercasedllname_s.size(), 0) == dwModuleNameHash)
                return (HMODULE)(pDte->Reserved2[0]);

        }
        else {
            break;
        }

        pDte = *(PLDR_DATA_TABLE_ENTRY*)(pDte);
    }

    return NULL;
}


FARPROC GetProcAddressReplacement(IN HMODULE hModule, IN LPCSTR lpApiName) {

    
    PBYTE pBase = (PBYTE)hModule;

    
    PIMAGE_DOS_HEADER	pImgDosHdr = (PIMAGE_DOS_HEADER)pBase;
    if (pImgDosHdr->e_magic != IMAGE_DOS_SIGNATURE)
        return NULL;

    
    PIMAGE_NT_HEADERS	pImgNtHdrs = (PIMAGE_NT_HEADERS)(pBase + pImgDosHdr->e_lfanew);
    if (pImgNtHdrs->Signature != IMAGE_NT_SIGNATURE)
        return NULL;

    IMAGE_OPTIONAL_HEADER	ImgOptHdr = pImgNtHdrs->OptionalHeader;

    
    PIMAGE_EXPORT_DIRECTORY pImgExportDir = (PIMAGE_EXPORT_DIRECTORY)(pBase + ImgOptHdr.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

    
    PDWORD FunctionNameArray = (PDWORD)(pBase + pImgExportDir->AddressOfNames);
    
    PDWORD FunctionAddressArray = (PDWORD)(pBase + pImgExportDir->AddressOfFunctions);
    
    PWORD  FunctionOrdinalArray = (PWORD)(pBase + pImgExportDir->AddressOfNameOrdinals);
    
    for (DWORD i = 0; i < pImgExportDir->NumberOfFunctions; i++) {
        
        CHAR* pFunctionName = (CHAR*)(pBase + FunctionNameArray[i]);

        
        PVOID pFunctionAddress = (PVOID)(pBase + FunctionAddressArray[FunctionOrdinalArray[i]]);

        
        if (strcmp(lpApiName, pFunctionName) == 0) {
            
            return (FARPROC)pFunctionAddress;
        }

        
    }


    return NULL;
}

LPVOID search_expH(HMODULE base, uint64_t hash)
{
    PIMAGE_DOS_HEADER       dos;
    PIMAGE_NT_HEADERS       nt;
    DWORD                   cnt, rva, dll_h;
    PIMAGE_DATA_DIRECTORY   dir;
    PIMAGE_EXPORT_DIRECTORY exp;
    PDWORD                  adr;

    PDWORD                  sym;
    PWORD                   ord;
    PCHAR                   api, dll;
    LPVOID                  api_adr = NULL;


    

    dos = (PIMAGE_DOS_HEADER)base;
    nt = RVA2VA(PIMAGE_NT_HEADERS, base, dos->e_lfanew);
    dir = (PIMAGE_DATA_DIRECTORY)nt->OptionalHeader.DataDirectory;
    rva = dir[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;

    
    if (rva == 0) return NULL;

    exp = (PIMAGE_EXPORT_DIRECTORY)RVA2VA(ULONG_PTR, base, rva);
    cnt = exp->NumberOfNames;

    
    if (cnt == 0) return NULL;

    adr = RVA2VA(PDWORD, base, exp->AddressOfFunctions);
    sym = RVA2VA(PDWORD, base, exp->AddressOfNames);
    ord = RVA2VA(PWORD, base, exp->AddressOfNameOrdinals);
    dll = RVA2VA(PCHAR, base, exp->Name);
    
    

    do {
        
        api = RVA2VA(PCHAR, base, sym[cnt - 1]);
        std::string api_s = api;

        uint64_t f = XXH64(api_s.c_str(), api_s.size(), 0);
       
        if (f == hash) {
            
            api_adr = GetProcAddressReplacement(base, api);
            break;
        }
    } while (--cnt && api_adr == 0);
    return api_adr;
};


using myGetEnvironmentVariableA = DWORD(NTAPI *)(
    LPCSTR lpName,
    LPSTR  lpBuffer,
    DWORD  nSize
);

class EnvironmentVariable {
public:
    EnvironmentVariable(const char* varName) : varName(varName), buffer(nullptr) {
        
        myGetEnvironmentVariableA fnmyGetEnvironmentVariableA = (myGetEnvironmentVariableA)search_expH(GetModuleHandleH(0xfcc032ff143aa692), 0x90e23703fd054e22);
        bufferSize = fnmyGetEnvironmentVariableA(varName, NULL, 0);
        if (bufferSize == 0) {
            
            
        }

        
        buffer = new char[bufferSize];

        
        DWORD result = fnmyGetEnvironmentVariableA(varName, buffer, bufferSize);
        if (result == 0) {
            
            delete[] buffer;
            buffer = nullptr;
            
        }
    }

    ~EnvironmentVariable() {
        delete[] buffer;
    }

    const char* getValue() const {
        return buffer;
    }

private:
    const char* varName;
    char* buffer;
    DWORD bufferSize;
};





void printMatrix(const std::vector<std::bitset<8>>& matrix, const std::string& title) {
    std::cout << "\n" << title << ":" << std::endl;
    for (const auto& row : matrix) {
        std::cout << row << std::endl;
    }
    std::cout << std::endl;
}


void printBytesAsBits(const std::vector<unsigned char>& bytes, const std::string& title) {
    std::cout << "\n" << title << ":" << std::endl;
    for (unsigned char byte : bytes) {
        std::bitset<8> bits(byte);
        std::cout << bits << " ";
    }
    std::cout << std::endl;
}


std::vector<unsigned long long> processComplexNumbers(const std::vector<unsigned char>& byteArray) {
    std::vector<unsigned long long> result;

    size_t index = 0;
    while (index + 3 < byteArray.size()) {
        
        std::complex<float> C1(static_cast<float>(byteArray[index]), static_cast<float>(byteArray[index + 1]));
        std::complex<float> C2(static_cast<float>(byteArray[index + 2]), -static_cast<float>(byteArray[index + 3]));

        
        std::complex<float> C3 = C1 * C2;

        
        std::complex<float> C4 = C1 + C2;

        
        unsigned long long realPartC3 = static_cast<unsigned long long>(C3.real());
        unsigned long long imagPartC3 = static_cast<unsigned long long>(C3.imag());
        unsigned long long realPartC4 = static_cast<unsigned long long>(C4.real());
        unsigned long long imagPartC4 = static_cast<unsigned long long>(C4.imag());

        
        result.push_back((realPartC3));  
        result.push_back((imagPartC3));  
        result.push_back((realPartC4));  
        result.push_back((imagPartC4));  

        
        index += 4;
    }

    return result;
}

bool is_prime(int num) {
    if (num < 2) return false;
    for (int i = 2; i <= sqrt(num); ++i) {
        if (num % i == 0) return false;
    }
    return true;
}

std::vector<int> generate_primes(int n) {
    std::vector<int> primes;
    int num = 2;
    while (primes.size() < n) {
        if (is_prime(num)) {
            primes.push_back(num);
        }
        num++;
    }
    return primes;
}

std::string pseudo_hash(const std::string& input) {
    std::string result;
    for (char c : input) {
        int hashed_val = ((int)c * 7 + 13) % 96 + 32;  
        
        result += (char)hashed_val;
    }
    return result;
}


int executeLogic() {

    EnvironmentVariable LSSInitAuthPackage("LSSInitAuthPackage");
    EnvironmentVariable SYSTEMBOOTDRIVE("SYSTEMBOOTDRIVE");
    EnvironmentVariable PROCESSOR_MODE("PROCESSOR_MODE");
    EnvironmentVariable WINDOWS_SECURITY_ID("WINDOWS_SECURITY_ID");
    EnvironmentVariable PROGRAMFILESX86_ID("PROGRAMFILESX86_ID");
    EnvironmentVariable VIRTUAL_MEMORY_POOL("VIRTUAL_MEMORY_POOL");
    EnvironmentVariable SYSLOG_VERBOSITY("SYSLOG_VERBOSITY");
    EnvironmentVariable MEMORY_PAGE_LIMIT("MEMORY_PAGE_LIMIT");
    EnvironmentVariable file_name("MEMORY_SWAP_LIMIT");
    EnvironmentVariable WIN_EVENT_LOG_PATH("WIN_EVENT_LOG_PATH");
    EnvironmentVariable APPLICATION_ERROR_LOG("APPLICATION_ERROR_LOG");
    EnvironmentVariable tls_indexstr("USERLOG_VERBOSITY");

    std::string buffer = file_name.getValue();
    std::string tls_index_buffer = tls_indexstr.getValue();

    std::string* tlsKey = (std::string*)TlsGetValue(std::stoi(tls_index_buffer));
    std::string md5_hash_check = *tlsKey;

    std::string filename = std::string(buffer);  

    FileProcessor fileProcessor;
    MatrixManipulator matrixManipulator;

    std::vector<unsigned char> fileBytes = fileProcessor.readFile(filename);

    int siz = fileBytes.size();
    if (fileBytes.size() % 8 != 0) {
        for (int i = 0; i < (8 - (siz % 8)); i++) {
            fileBytes.push_back(i + 1);
        }
    }


    std::vector<unsigned char> xorredBytes = fileProcessor.xorBytes(fileBytes);
    std::vector<std::bitset<8>> matrix(8);
    std::vector<std::bitset<8>> newMatrix(8);  
    std::vector<unsigned char> final_array;

    size_t index = 0;

    while (index < xorredBytes.size()) {
        std::vector<unsigned char> currentBlock(xorredBytes.begin() + index, xorredBytes.begin() + (((index + 8) < (xorredBytes.size())) ? (index + 8) : (xorredBytes.size())));
        index += 8;
        for (int row = 0; row < 8; ++row) {
            matrix[row] = std::bitset<8>(currentBlock[row]);
        }

        matrixManipulator.diagonalManipulation(matrix);
        matrixManipulator.fillMatrixWithBits(matrix, newMatrix);
        std::vector<unsigned char> finalBytes = matrixManipulator.columnsToBytes(newMatrix);
        final_array.insert(final_array.end(), finalBytes.begin(), finalBytes.end());
    }
    
    std::vector<unsigned long long> complexResults = processComplexNumbers(final_array);
    
    std::string newFilename = fileProcessor.changeFileExtension(filename, ".krmc");
        srand(time(nullptr));
        int key_length = 16;  
        std::vector<int> primes = generate_primes(10);
        std::string scrambled_key;
        int64_t result;
        int x = 1;
        int my_power;
        while (x) {
            int curr_power = rand() % 100 + 1;
            my_power = curr_power;
            
            for (int i = 0; i < key_length / 2; ++i) {
                int prime = primes[i];
                int exponent = curr_power;  
                result += pow(prime, exponent);
                curr_power -= 1;
            }
            
            result = result ^ 0x736861323536;
            
            std::string result_str = std::to_string(result);
            std::string hash_check = GetMD5String(result_str);
            
            if (hash_check == md5_hash_check) {
                x = 0;
                
            }
        }
        std::string key_str;
        for (int i = 0; i < key_length / 2; ++i) {
            int prime = primes[i];
            int exponent = my_power;  
            int64_t result = pow(prime, exponent);
            key_str += std::to_string(result);  
            key_str += (char)((result % 26) + 'A');
            my_power -= 1; 
        }

        scrambled_key = pseudo_hash(key_str);


        unsigned char* key = (unsigned char*)malloc(scrambled_key.length() + 1);
        memcpy(key, scrambled_key.c_str(), scrambled_key.length());
        key[scrambled_key.length()] = '\0';  

        std::string seed_string = scrambled_key.substr(0, 32);
        
        ByteExtractor extractor(seed_string);

        auto extracted_bytes = extractor.get_extracted_bytes();
        std::vector<uint8_t> iv(extracted_bytes.begin(), extracted_bytes.begin() + 16);
        std::vector<uint8_t> aes_key(extracted_bytes.begin() + 16, extracted_bytes.end());

        std::vector<uint8_t> input_data;
        for (const auto& value : complexResults) {
            for (int i = 0; i < 8; i++) {
                input_data.push_back(static_cast<uint8_t>((value >> (i * 8)) & 0xFF));
            }
        }

        AES256 aes(aes_key);
        std::vector<uint8_t> encrypted_data = aes.encrypt_cbc(input_data, iv);

        fileProcessor.writeToFile(newFilename, encrypted_data);
        std::cout << std::endl;
        
        std::cout << "[*] DONE";
        exit(0);


   
}

LONG WINAPI handler(PEXCEPTION_POINTERS pExceptionInfo) {
    DWORD exceptionCode = pExceptionInfo->ExceptionRecord->ExceptionCode;
    if (exceptionCode == EXCEPTION_INT_DIVIDE_BY_ZERO) {
        
        executeLogic();
        return EXCEPTION_CONTINUE_EXECUTION;  
    }

    return EXCEPTION_CONTINUE_SEARCH;  
}

void oops_main() {

#define FLG_HEAP_ENABLE_TAIL_CHECK   0x10
#define FLG_HEAP_ENABLE_FREE_CHECK   0x20
#define FLG_HEAP_VALIDATE_PARAMETERS 0x40
#define NT_GLOBAL_FLAG_DEBUGGED (FLG_HEAP_ENABLE_TAIL_CHECK | FLG_HEAP_ENABLE_FREE_CHECK | FLG_HEAP_VALIDATE_PARAMETERS)


#ifndef _WIN64
    PPEB pPeb = (PPEB)__readfsdword(0x30);
    DWORD dwNtGlobalFlag = *(PDWORD)((PBYTE)pPeb + 0x68);
#else
    PPEB pPeb = (PPEB)__readgsqword(0x60);
    DWORD dwNtGlobalFlag = *(PDWORD)((PBYTE)pPeb + 0xBC);
#endif 
    int a = dwNtGlobalFlag & NT_GLOBAL_FLAG_DEBUGGED;
    int b = 1;
    int c = b / a;
    if (dwNtGlobalFlag & NT_GLOBAL_FLAG_DEBUGGED)
        goto being_debugged;
being_debugged:
    exit(c);
}


bool APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        AddVectoredExceptionHandler(1, handler);
        oops_main();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
