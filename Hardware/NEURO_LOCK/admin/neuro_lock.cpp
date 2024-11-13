#include "neuro_lock.h"
#include <Arduino.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

char receivedChars[numChars];
char tempChars[numChars];
char messageonbus[numChars] = {0};

float weight_1 = 0.0;
float weight_2 = 0.0;
float weight_3 = 0.0;
int pin = 0;
int key[8] = {0, 0, 0, 0, 0, 0, 0, 0};

boolean newData = false;

double weight_digit_4[3] = {9.67299303, -0.2078435, -4.62963669};

double inputs[8][3] = {
    {0, 0, 0}, {0, 0, 1}, {0, 1, 0}, {0, 1, 1},
    {1, 0, 0}, {1, 0, 1}, {1, 1, 0}, {1, 1, 1},
};

double sigmoid(double x) { return 1.0 / (1.0 + exp(-x)); }

double dot(double *a, double *b, int size) {
  double sum = 0;
  for (int i = 0; i < size; i++) {
    sum += a[i] * b[i];
  }
  return sum;
}

void avail_commands() {
  Serial.println("Available commands.....");
  Serial.println(">PIN <PIN>");
  Serial.println(">RESET");
  Serial.println(">HELP");
  Serial.print(">");
}

void test_neural_network() {
  for (int i = 0; i < 8; i++) {
    double result = sigmoid(dot(inputs[i], weight_digit_4, 3));
    Serial.print("3 Inputs: ");
    Serial.print(inputs[i][0]);
    Serial.print(' ');
    Serial.print(inputs[i][1]);
    Serial.print(' ');
    Serial.print(inputs[i][2]);
    Serial.print("\tFinal Result: ");
    Serial.println(result);
  }
  Serial.println();
}

void recvWithStartEndMarkers() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '>';
  char endMarker = '\n';
  char rc;

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();
    if (recvInProgress == true) {
      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      } else {
        receivedChars[ndx] = '\0';
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    } else if (rc == startMarker) {
      recvInProgress = true;
    }
  }
}

void getline() {
  memset(receivedChars, 0, sizeof(receivedChars));
  int count = 0;

  while (true) {
    delay(5);
    if (count > 30) {
      Serial.println("Buffer exceeded");
      return;
    }
    if (Serial.available()) {
      receivedChars[count] = Serial.read();
      Serial.print(receivedChars[count]);
      if (receivedChars[count] == 015 || receivedChars[count] == '\r') {
        Serial.print('\n');
        receivedChars[count] = '\0';
        newData = true;
        return;
      }
      count++;
    }
  }
}

int parseSetData() {
  char *strtokIndx = strtok(tempChars, " ");
  if (strtokIndx == NULL || strcmp(strtokIndx, "FACTORY_CFG") != 0) {
    return -1;
  }
  strcpy(messageonbus, strtokIndx);
  // Parse weight 1
  strtokIndx = strtok(NULL, " ");
  if (strtokIndx == NULL)
    return -1;
  weight_1 = atof(strtokIndx);
  // Parse weight 2
  strtokIndx = strtok(NULL, " ");
  if (strtokIndx == NULL)
    return -1;
  weight_2 = atof(strtokIndx);
  // Parse weight 3
  strtokIndx = strtok(NULL, " ");
  if (strtokIndx == NULL)
    return -1;
  weight_3 = atof(strtokIndx);
  // Validate no extra tokens
  if (strtok(NULL, " ") != NULL)
    return -1;
  return 0;
}

void showParsedData() {
  Serial.print("Command ");
  Serial.println(messageonbus);
  Serial.print("Weight 1\t");
  Serial.println(weight_1);
  Serial.print("Weight 2\t");
  Serial.println(weight_2);
  Serial.print("Weight 3\t");
  Serial.println(weight_3);
}

void setData() {
  Serial.println("Recalibrating weights......");
  weight_digit_4[0] = weight_1;
  weight_digit_4[1] = weight_2;
  weight_digit_4[2] = weight_3;
  Serial.println("Done......");
}

int parseLoginData() {
  char *strtokIndx = strtok(tempChars, " ");
  if (strtokIndx == NULL || strcmp(strtokIndx, "PIN") != 0) {
    return -1;
  }
  strcpy(messageonbus, strtokIndx);
  // Parse PIN
  strtokIndx = strtok(NULL, " ");
  if (strtokIndx == NULL)
    return -1;
  // Validate PIN is numeric
  for (int i = 0; strtokIndx[i] != '\0'; i++) {
    if (!isdigit(strtokIndx[i]))
      return -1;
  }
  pin = atoi(strtokIndx);
  // Validate no extra tokens
  if (strtok(NULL, " ") != NULL)
    return -1;
  return 0;
}

int loginFunc() {
  if (pin == 9313) {
    Serial.println("PIN check successful");
    int key_val = 0;
    for (int i = 0; i < 8; i++) {
      double result = sigmoid(dot(inputs[i], weight_digit_4, 3));
      key[i] = (result > 0.65) ? 1 : 0;
      key_val = (key_val << 1) | key[i];
    }
    Serial.println();

    if (((flag[0] ^ key_val) == 105) && ((flag[1] ^ key_val) == 99) &&
        ((flag[2] ^ key_val) == 99)) {
      Serial.print("Congrats: \t");
      for (int i = 0; i < FLAG_LEN; i++) {
        Serial.print((char)(flag[i] ^ key_val));
      }
      Serial.println();
    } else {
      Serial.println("AI signature doesn't match");
    }
    Serial.println();
    return 0;
  } else {
    Serial.println("(x_x) Failed PIN check (x_x)");
    return -1;
  }
}

void lock_setup() {
  Serial.println("\033[2J"); //clear
  Serial.println("Booting lock firmware.......");
  avail_commands();
}

void lock_main() {
  recvWithStartEndMarkers();
  int ret_val = 0;
  getline();
  // Serial.println(receivedChars);
  if (newData == true) {
    strcpy(tempChars, receivedChars);
    if (strcmp(receivedChars, "GUESS") == 0) {
      Serial.println("Prediction command detected....");
      Serial.println();
      test_neural_network();
    } else if (strstr(receivedChars, "FACTORY_CFG") != 0) {
      Serial.println(
          "Factory configuration mode, enter required weights W1 W2 W3\n");
      ret_val = parseSetData();
      if (ret_val == -1) {
        Serial.println("Invalid format, try again");
      } else {
        showParsedData();
        setData();
      }
    } else if (strstr(receivedChars, "PIN") != 0) {
      // Serial.println(receivedChars);
      ret_val = parseLoginData();
      if (ret_val == -1) {
        Serial.println("Invalid format, try again");
      } else {
        loginFunc();
      }
    } else if (strcmp(receivedChars, "RESET") == 0) {
      ESP.restart();
    } else if (strcmp(receivedChars, "HELP") == 0) {
      avail_commands();
    } else {
      Serial.println("Invalid command, type HELP for available commands");
      avail_commands();
    }
  }
  newData = false;
  avail_commands();
}

extern "C" {
uint8_t firmware2() {
  lock_setup();
  while (true)
    lock_main();
  return 1;
}
}
