#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/reg.h>

#define N 256  // 2^8

void swap(unsigned char *a, unsigned char *b) {
    unsigned char tmp = *a;
    *a = *b;
    *b = tmp;
}

int KSA(char *key, unsigned char *S) {

    int len = 16;
    int j = 0;

    for(int i = 0; i < N; i++)
        S[i] = i;

    for(int i = 0; i < N; i++) {
        j = (j + S[i] + key[i % len]) % N;
        if (j < 0) {
            j += N;
        }

        swap(&S[i], &S[j]);
    }

    return 0;
}


int PRGA(unsigned char *S, char *plaintext, unsigned char *ciphertext) {

    int i = 0;
    int j = 0;

    for(size_t n = 0, len = strlen(plaintext); n < len; n++) {
        i = (i + 1) % N;
        j = (j + S[i]) % N;

        swap(&S[i], &S[j]);
        int rnd = S[(S[i] + S[j]) % N];

        ciphertext[n] = rnd ^ plaintext[n];

    }

    return 0;
}

int RC4(char *key, char *plaintext, unsigned char *ciphertext) {

    unsigned char S[N];
    KSA(key, S);

    PRGA(S, plaintext, ciphertext);

    return 0;
}


// Global var to act as trigger for grandchild to child 
int *trigger = 0;
int fake_rc4_key[] = {7, 135, 134, 184, 92, 43, 80, 152, 59, 133, 218, 38, 181, 148, 17, 58};
int encrypted_flag[] = {144, 73, 163, 235, 125, 28, 238, 196, 37, 71, 42, 117, 14, 128, 0, 151, 157, 205, 189, 181, 31, 150, 207, 205, 26, 39, 152, 59, 185, 49, 194, 97, 154, 58, 179, 195, 63, 191, 75, 206, 184, 42, 227};


bool flag_checker(char *flag)
{
    char key[16];
    for (int i = 0; i < 16; i++)
    {
        key[i] = fake_rc4_key[i] ^ *trigger;
    }

    unsigned char *ciphertext = malloc(sizeof(char) * N);

    // At this point, key *should* contain the real key and not the fake one
    RC4(key, flag, ciphertext);
    int bool_flag = 1;

    if (strlen(flag) < 43) bool_flag = 0;

    for (int i = 0; i < strlen(flag); i++)
    {
        if (encrypted_flag[i] != ciphertext[i])
        {
            bool_flag = 0;
        }
    }
    
    return (bool)bool_flag;
}

int main(int argc, char *argv[])
{
    /*
    This is the grandparent process, this process will fork into a child 
    (which will be the main child), and the grandparent will have no work 
    apart from that. The remaining part of the program will be handled by 
    the main child and the grandchild. The only work this process has to do 
    now is to stay alive for occasional checks by the grandchild - if the 
    grandparent is not found, the program will exit entirely.
    */

    if (argc != 2)
    {
        puts("Not even close");
        exit(1);
    }

    // // Create the main child 
    pid_t main_child_pid = fork();

    if (main_child_pid == 0)
    {
        /* 
        The main child to execute 
        */
        int grandchild_status;
        struct user_regs_struct regs;

        pid_t grandchild_pid = fork();

        if (grandchild_pid == 0)
        {
            /*
            The grandchild to execute, which will trigger the main child 
            */
            ptrace(PTRACE_TRACEME, 0, 0, 0);
                    
            if (flag_checker(argv[1]))
            {
                puts("Correct");
            }
            else
            {
                puts("Wrong");
            }

            exit(0);
        }

        ptrace(PTRACE_ATTACH, grandchild_pid, 0, 0);
        srand(69); // PLACE IN INIT 
        while (true)
        {
            waitpid(grandchild_pid, &grandchild_status, 0);

            if (WIFEXITED(grandchild_status) || WIFSIGNALED(grandchild_status)) 
            {
                break;
            }

            if (grandchild_status != 0xffff)
            {

                if (WIFSTOPPED(grandchild_status) && WSTOPSIG(grandchild_status) == SIGSEGV)
                {
                    /*
                    If a SEGFAULT signal was triggered
                    */
                    
                    ptrace(PTRACE_GETREGS, grandchild_pid, 0, &regs);
                    regs.rip += 2;
                    regs.rax = rand();
                    ptrace(PTRACE_SETREGS, grandchild_pid, 0, &regs);
                }
                ptrace(PTRACE_CONT, grandchild_pid, 0, 0);
            }
        }

        return 0;
    }

    return 0;
}