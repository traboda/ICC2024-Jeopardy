#ifndef NEURO_LOCK_H
#define NEURO_LOCK_H

#define numChars 32
#define UART_NUM UART_NUM_0
#define BUF_SIZE (1024)
#define FLAG_LEN 31

extern double weight_digit_1[3];
extern double weight_digit_2[3];
extern double weight_digit_3[3];
extern double weight_digit_4[3];
extern double inputs[8][3];

const int flag[FLAG_LEN] = {0x25, 0x2f, 0x2f, 0x37, 0x29, 0x3f, 0x3c, 0x7f,
                            0x7e, 0x61, 0x3f, 0x7f, 0x13, 0x38, 0x7d, 0x22,
                            0x35, 0x21, 0x20, 0x13, 0x2e, 0x78, 0x2e, 0x35,
                            0x22, 0x7f, 0x39, 0x3e, 0x7c, 0x22, 0x31};

// Function declarations
void avail_commands(void);
void test_neural_network(void);
int parseSetData(void);
void showParsedData(void);
void setData(void);
int parseLoginData(void);
int loginFunc(void);
void read_from_uart(void);

#endif // NEURO_LOCK_H
