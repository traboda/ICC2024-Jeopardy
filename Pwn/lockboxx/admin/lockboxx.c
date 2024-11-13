#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <openssl/evp.h>
#include <openssl/md5.h>
#include <string.h>

uint8_t PassListSize = 0;
char newbuf[0x50];
int seed;
char blacklist[] = { '\x02', '\x0b', '\x0a', '\x0c', '\x3b' };

struct PasswordBlock{
    uint64_t id;
    char url[32];
    char username[16];
    char email[24];
    char passwd[24];
};

struct PasswordBlock PassList[5] = {0};

void stack_smash() {
    printf("*** stack smashing detected ***: terminated\n");
    pthread_exit(NULL);
}

__attribute__((weak, alias("stack_smash"))) void __stack_chk_fail(void);

void compute_md5(const char *str, unsigned char *digest) {
    EVP_MD_CTX *mdctx;

    mdctx = EVP_MD_CTX_new();
    if (mdctx == NULL) {
        fprintf(stderr, "Failed to create MD context\n");
        return;
    }

    if (EVP_DigestInit_ex(mdctx, EVP_md5(), NULL) != 1) {
        fprintf(stderr, "Failed to initialize MD5\n");
        EVP_MD_CTX_free(mdctx);
        return;
    }

    if (EVP_DigestUpdate(mdctx, str, strlen(str)) != 1) {
        fprintf(stderr, "Failed to update MD5\n");
        EVP_MD_CTX_free(mdctx);
        return;
    }

    if (EVP_DigestFinal_ex(mdctx, digest, NULL) != 1) {
        fprintf(stderr, "Failed to finalize MD5\n");
        EVP_MD_CTX_free(mdctx);
        return;
    }

    EVP_MD_CTX_free(mdctx);
}

void init(){
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
}

void add_random_noise(char *str, int size) {
    for (int i = 0; i < size; i++) {
        srand(seed);
        int temp = rand() % 256;
        str[i] ^= temp;
    }
}

void crypt_password(int index){
    add_random_noise(PassList[index].url, 31);
    add_random_noise(PassList[index].username, 15);
    add_random_noise(PassList[index].email, 23);
    add_random_noise(PassList[index].passwd, 23);
}

void add_password(){

    if(PassListSize == 5){
        printf("\n");
        printf("Only 5 Passwords Allowed In The Free Version.\n");
        printf("Unlock Premium To Store More.\n");
        printf("\n");
        return;
    }

    struct PasswordBlock NewPass, pass_ind;

    for(int i = 0; i < 5; i++){
        if(PassList[i].id == 0) {
            printf("\n");
            for(;;){
                printf("ID: ");
                scanf("%d", &PassList[i].id);
                if(!PassList[i].id){
                    printf("Invalid ID.\n");
                }
                else{
                    break;
                }
            }
            printf("URL: ");
            scanf("%32s", PassList[i].url);
            printf("Username: ");
            scanf("%16s", PassList[i].username);
            printf("E-m@il: ");
            scanf("%24s", PassList[i].email);
            printf("Password: ");
            scanf("%24s", PassList[i].passwd);
            crypt_password(i);
            
            PassListSize++;

            printf("\nNew Password Added!\n");
            printf("\n");
            return;
        }
    }
}

void remove_password(){
   int remove_id;

    printf("\nGive Password ID: ");
    scanf("%d", &remove_id);

    if(remove_id <= 0){
        printf("\nInvalid ID!\n\n");
        return;
    }

    for(int i = 0; i < 5; i++){
        if(PassList[i].id == remove_id){
            PassList[i].id = 0;
            PassListSize--;
            printf("\nPassword Removed.\n\n");
            return;
        }
    }
    printf("\nInvalid ID!\n\n");
}

void list_password(){
    if(PassListSize == 0){
        printf("\nPassword List Empty.\n\n");
        return;
    }

    for(int i = 0; i < 5; i++){
        if(PassList[i].id != 0){
            crypt_password(i);
            printf("\n");
            printf("-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n");
            printf("ID: %d\n", PassList[i].id);
            printf("Username: %s\n", PassList[i].username);
            printf("URL: %s\n", PassList[i].url);
            printf("Email: %s\n", PassList[i].email);
            printf("Password: %s\n", PassList[i].passwd);
            printf("-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n");
            printf("\n");
            crypt_password(i);
        }
    }
}

void share_password(){
    char buf[80];
    printf("This operation is under beta testing and might be unstable\n\n");
    read(0, buf, 80);
}

void * update_master_password(void * arg) {
    int ln;    
    char * mp = (char *)arg;
    char cmp_buf[30];
    char new_buf[30];

    puts("\nEnter Old Master Password: ");
    read(0, cmp_buf, sizeof(cmp_buf));
    ln = strcspn(cmp_buf, "\n");
    cmp_buf[ln] = '\0';

    if (strcmp(cmp_buf, mp) == 0) {
        puts("\nSet New Master Password: ");
        ln = read(0, new_buf, sizeof(newbuf));
        if ( ln > 48 ) {
            for (int i = 48; i < ln; i++) {
                for ( int b = 0; b < sizeof(blacklist); b++) {
                    if (blacklist[b] == (*(new_buf+i))) {
                        printf("Invalid Byte %c!",blacklist[b]);
                        exit(0);
                    }
                }
            }
        }
        *mp = *new_buf;
        printf("\nUpdated Master Password.\n");
        return NULL;
    }   
    printf("Invalid Master Password.\n");
    return NULL;
}

void password_gen(){
    int length;
    printf("\nEnter Password Length: ");
    scanf("%d", &length);

    const char characters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()";
    int characters_len = sizeof(characters) - 1;
    char password[length + 1];

    srand(time(NULL));

    for (int i = 0; i < length; i++) {
        password[i] = characters[rand() % characters_len];
    }
    password[length] = '\0';

    printf("\nGenerated Password: %s\n\n", password);
}

void ascii_art(){
    printf("\n\n\n");
    printf("\t\t██╗      ██████╗  ██████╗██╗  ██╗██████╗  ██████╗ ██╗  ██╗██╗  ██╗\n");
    printf("\t\t██║     ██╔═══██╗██╔════╝██║ ██╔╝██╔══██╗██╔═══██╗╚██╗██╔╝╚██╗██╔╝\n");
    printf("\t\t██║     ██║   ██║██║     █████╔╝ ██████╔╝██║   ██║ ╚███╔╝  ╚███╔╝ \n");
    printf("\t\t██║     ██║   ██║██║     ██╔═██╗ ██╔══██╗██║   ██║ ██╔██╗  ██╔██╗ \n");
    printf("\t\t███████╗╚██████╔╝╚██████╗██║  ██╗██████╔╝╚██████╔╝██╔╝ ██╗██╔╝ ██╗\n");
    printf("\t\t╚══════╝ ╚═════╝  ╚═════╝╚═╝  ╚═╝╚═════╝  ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝\n");
    printf("\t\t\t\t    Lock it down, keep it safe.\n");
    printf("\n\n\n");
}

void print_menu(){
    printf("\n[ MENU ]\n1. Add password\n2. Remove password\n3. List passwords\n4. Update Master password\n5. Share your password\n6. Random password generator\n7. Exit\n");
}

int main() {
    init();
    ascii_art();

    char master_pass[30];
    seed = time(NULL) / 5;
    int sel; 
    pthread_t t_id;

    unsigned char master_pass_digest[MD5_DIGEST_LENGTH];

    printf("\nSet Master Password: ");

    if(fgets(master_pass, sizeof(master_pass), stdin) == NULL){
        printf("*** INTRUDER ALERT ***\n");
        exit(0);
    }
    else{
        size_t len = strlen(master_pass);
        if (len > 0 && master_pass[len - 1] == '\n') {
            master_pass[len - 1] = '\0';
        }
    }
    
    compute_md5(master_pass, master_pass_digest);

    printf("\nMD5('%s'): ", master_pass);
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++){
        printf("%02x", master_pass_digest[i]);
    }
    printf("\n\n");

    for(;;){
        print_menu();
        printf(">> ");
        scanf("%d", &sel);

        switch(sel){
            case 1:
                add_password();
                break;
            case 2:
                remove_password();
                break;
            case 3:
                list_password();
                break;
            case 4:
                pthread_create(&t_id, NULL, update_master_password, (void *)master_pass);
                pthread_join(t_id, NULL);
                break;
            case 5:
                share_password();
                break;
            case 6:
                password_gen();
                break;
            case 7:
                printf("[ EXIT ]\n");
                exit(0);
        }
    }
    return 0;
}