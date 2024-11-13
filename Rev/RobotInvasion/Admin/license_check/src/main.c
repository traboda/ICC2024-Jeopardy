#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <elf.h>
#include <link.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "mtwister.h"

#define FLAG_LENGTH 35
#define OBSCURE(x) ((x) ^ 0xDEADBEEF)

const int obfuscated_xored_flag[FLAG_LENGTH] = {
    OBSCURE(26), OBSCURE(31), OBSCURE(23), OBSCURE(103), OBSCURE(-120),
    OBSCURE(-74), OBSCURE(108), OBSCURE(17), OBSCURE(-44), OBSCURE(-52),
    OBSCURE(-98), OBSCURE(43), OBSCURE(-89), OBSCURE(-78), OBSCURE(-122),
    OBSCURE(110), OBSCURE(-23), OBSCURE(-94), OBSCURE(-2), OBSCURE(47),
    OBSCURE(-97), OBSCURE(53), OBSCURE(62), OBSCURE(-16), OBSCURE(-123),
    OBSCURE(-91), OBSCURE(20), OBSCURE(51), OBSCURE(127), OBSCURE(35),
    OBSCURE(-108), OBSCURE(30), OBSCURE(92), OBSCURE(14), OBSCURE(42)
};

void *(*volatile memcpy_ptr)(void *, const void *, size_t) = memcpy;
int (*volatile memcmp_ptr)(const void *, const void *, size_t) = memcmp;

int main(int, char**);
void guard(unsigned char * func_addr, uint32_t size);
void make_text_rwx();
static void cipher(unsigned char* data, size_t len, const unsigned char* key, size_t keylen);

__attribute__((section(".text")))
uint32_t custom_hash(const char *str) {
    uint32_t hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash;
}

__attribute__((section(".text")))
void init_rand(MTRand *r, uint32_t seed) {
    guard((unsigned char *)seedRand, 43);
    *r = seedRand(seed);
    guard((unsigned char *)seedRand, 43);
}

__attribute__((section(".text")))
uint32_t read_seed_from_file(const char *filename) {
    FILE *file = fopen(filename, "rb");
    uint32_t seed = 0;
    if (file != NULL) {
        fread(&seed, sizeof(uint32_t), 1, file);
        fclose(file);
    }
    guard((unsigned char *)custom_hash, 79);
    uint32_t hash2 = custom_hash((char *)&seed);
    guard((unsigned char *)custom_hash, 79);
    return seed;
}

__attribute__((section(".text")))
void generate_key(unsigned char *key, MTRand *r, int length) {
    for (int i = 0; i < length; i++) {
        key[i] = (unsigned char)(genRand(r) * 256);
    }
}

__attribute__((section(".text")))
void xor_arrays(unsigned char *result, const unsigned char *arr1, const int *arr2, int length) {
    for (int i = 0; i < length; i++) {
        result[i] = arr1[i] ^ OBSCURE(arr2[i]);
    }
}

__attribute__((section(".text")))
int compare_flags(const char *input_flag, const unsigned char *correct_flag, int length) {
    return memcmp_ptr(input_flag, correct_flag, length) == 0;
}

__attribute__((section(".text")))
int process_flag(const char *input_flag) {
    guard((unsigned char *)read_seed_from_file, 181);
    uint32_t seed = read_seed_from_file("user_id.config");
    guard((unsigned char *)read_seed_from_file, 181);
    MTRand r;
    guard((unsigned char *)init_rand, 213);
    init_rand(&r, seed);
    guard((unsigned char *)init_rand, 213);
    unsigned char generated_key[FLAG_LENGTH];
    guard((unsigned char *)generate_key, 92);
    generate_key(generated_key, &r, FLAG_LENGTH);
    guard((unsigned char *)generate_key, 92);
    unsigned char correct_flag[FLAG_LENGTH];
    guard((unsigned char *)xor_arrays, 106);
    xor_arrays(correct_flag, generated_key, obfuscated_xored_flag, FLAG_LENGTH);
    guard((unsigned char *)xor_arrays, 106);
    guard((unsigned char *)compare_flags, 63);
    int res = compare_flags(input_flag, correct_flag, FLAG_LENGTH);
    guard((unsigned char *)compare_flags, 63);
    return res;
}
typedef void (*func_ptr)();

static void cipher(unsigned char* data, size_t len, const unsigned char* key, size_t keylen) {
    for(size_t i = 0; i < len; i++) {
        data[i] ^= key[i % keylen];
    }
}

void make_text_rwx() {
    long pagesize = sysconf(_SC_PAGESIZE);
    
    void *addr = (void *)make_text_rwx;
    
    void *page_start = (void *)((uintptr_t)addr & ~(pagesize - 1));
    
    if (mprotect(page_start, pagesize, PROT_READ | PROT_WRITE | PROT_EXEC) != 0) {
        return;
    }
    
}


void guard(unsigned char * func_addr, uint32_t size) {
    static unsigned char* code_buffer = NULL;
    static size_t buffer_size = 0;
    unsigned char key[] = {0x69, 0x79, 0x34, 0x23, 0x56, 0x23, 0x56, 0x35,
                            0x64, 0x45, 0x56, 0x34, 0x57, 0x73, 0x23, 0x23};
    size_t keylen = 16;

    cipher((unsigned char *)func_addr, size, key, keylen);
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    make_text_rwx();
    
    if (rand() % 2 == 0) {
        memcpy_ptr = memcpy;
        memcmp_ptr = memcmp;
    } else {
        memcpy_ptr = memmove;
        memcmp_ptr = strncmp;
    }
   
    guard((unsigned char *)process_flag, 341);
    int ret = process_flag(argv[1]);
    guard((unsigned char *)process_flag, 341);
    return ret;
}
