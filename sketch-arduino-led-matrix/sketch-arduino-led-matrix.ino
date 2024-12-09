#include <stdint.h>
#define HIGH 0x1
#define LOW 0x0

#define OUTPUT 0x1

#define PIN_adder 10
#define PIN_voidAdd 12
#define PIN_OUTER 11
#define PIN_1 2
#define PIN_2 3
#define PIN_3 4
#define PIN_4 5
#define PIN_5 6
#define PIN_6 7
#define PIN_7 8
#define PIN_8 9

// prototypes from
// https://github.com/arduino/ArduinoCore-avr/tree/master/cores/arduino
void pinMode(uint8_t pin, uint8_t mode);
void delayMicroseconds(unsigned int us);
void delay(unsigned long ms);
void digitalWrite(uint8_t pin, uint8_t val);

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(5);
  pinMode(PIN_adder, OUTPUT);
  pinMode(PIN_voidAdd, OUTPUT);
  pinMode(PIN_OUTER, OUTPUT);
  pinMode(PIN_1, OUTPUT);
  pinMode(PIN_2, OUTPUT);
  pinMode(PIN_3, OUTPUT);
  pinMode(PIN_4, OUTPUT);
  pinMode(PIN_5, OUTPUT);
  pinMode(PIN_6, OUTPUT);
  pinMode(PIN_7, OUTPUT);
  pinMode(PIN_8, OUTPUT);
}

unsigned char alphabet[27][8] = {{255, 195, 189, 189, 129, 189, 189, 189},
                                 {255, 193, 189, 189, 193, 189, 189, 193},
                                 {255, 195, 189, 189, 253, 189, 189, 195},
                                 {255, 225, 221, 189, 189, 189, 221, 225},
                                 {255, 193, 253, 253, 193, 253, 253, 193},
                                 {255, 193, 253, 253, 225, 253, 253, 253},
                                 {255, 195, 189, 189, 253, 141, 189, 195},
                                 {255, 189, 189, 189, 129, 189, 189, 189},
                                 {255, 193, 247, 247, 247, 247, 247, 193},
                                 {255, 129, 239, 239, 239, 237, 237, 243},
                                 {255, 221, 237, 245, 249, 245, 237, 221},
                                 {255, 253, 253, 253, 253, 253, 253, 193},
                                 {255, 189, 153, 165, 189, 189, 189, 189},
                                 {255, 189, 185, 181, 173, 157, 189, 189},
                                 {255, 195, 189, 189, 189, 189, 189, 195},
                                 {255, 193, 189, 189, 193, 253, 253, 253},
                                 {255, 195, 189, 189, 189, 173, 157, 195},
                                 {255, 193, 189, 189, 193, 245, 237, 221},
                                 {255, 131, 253, 253, 195, 191, 191, 193},
                                 {255, 129, 247, 247, 247, 247, 247, 247},
                                 {255, 189, 189, 189, 189, 189, 189, 195},
                                 {255, 189, 189, 219, 219, 219, 231, 231},
                                 {255, 189, 189, 165, 165, 165, 219, 219},
                                 {255, 189, 219, 231, 231, 219, 189, 189},
                                 {255, 189, 219, 231, 247, 247, 247, 247},
                                 {255, 129, 191, 223, 239, 247, 251, 129}};

inline void switchLetter(unsigned char *matrix, unsigned char *matrixTwo) {
  for (int i = 0; i < 8; i++) {
    matrix[i] = matrixTwo[i];
  }
}

inline int bitPrint(unsigned char *byte, int number) {
  return (*byte & (1 << number)) ? HIGH : LOW;
}

#define TABLE_SIZE 8

unsigned char matrix[TABLE_SIZE] = {255, 255, 255, 255, 255, 255, 255, 255};

byte table[TABLE_SIZE] = {255, 255, 255, 255, 255, 255, 255, 255};
byte buff[TABLE_SIZE] = {255, 255, 255, 255, 255, 255, 255, 255};

int counter = 0;
// 60 is the duration of the figure
#define DURATION 60

void loop() {
  if (counter == 27) {
    counter = 0;
  }
  Serial.println("ASKING_VALUE");
  for (int z = 0; z < DURATION; z++) {
    digitalWrite(PIN_adder, HIGH);
    digitalWrite(PIN_voidAdd, HIGH);
    digitalWrite(PIN_voidAdd, LOW);
    delayMicroseconds(100);
    digitalWrite(PIN_adder, LOW);
    digitalWrite(PIN_OUTER, HIGH);
    digitalWrite(PIN_OUTER, LOW);
    for (int a = 0; a < 8; a++) {
      digitalWrite(PIN_1, bitPrint(&matrix[a], 7));
      digitalWrite(PIN_2, bitPrint(&matrix[a], 6));
      digitalWrite(PIN_3, bitPrint(&matrix[a], 5));
      digitalWrite(PIN_4, bitPrint(&matrix[a], 4));
      digitalWrite(PIN_5, bitPrint(&matrix[a], 3));
      digitalWrite(PIN_6, bitPrint(&matrix[a], 2));
      digitalWrite(PIN_7, bitPrint(&matrix[a], 1));
      digitalWrite(PIN_8, bitPrint(&matrix[a], 0));
      delay(2);
      digitalWrite(PIN_1, HIGH);
      digitalWrite(PIN_2, HIGH);
      digitalWrite(PIN_3, HIGH);
      digitalWrite(PIN_4, HIGH);
      digitalWrite(PIN_5, HIGH);
      digitalWrite(PIN_6, HIGH);
      digitalWrite(PIN_7, HIGH);
      digitalWrite(PIN_8, HIGH);
      // add next line
      digitalWrite(PIN_voidAdd, HIGH);
      delayMicroseconds(100);
      digitalWrite(PIN_voidAdd, LOW);
      digitalWrite(PIN_OUTER, HIGH);
      digitalWrite(PIN_OUTER, LOW);
    }
  }
  size_t nb_in_buffer = Serial.readBytes(buff, TABLE_SIZE);
  if (nb_in_buffer == 0) {
    Serial.println("READ_NOTHING");
  } else if (nb_in_buffer != TABLE_SIZE) {
    Serial.println("ERROR_READING");
  } else {
    Serial.println("");
    switchLetter(table, buff);
  }
  // switchLetter(matrix, alphabet[counter]);
  switchLetter(matrix, table);

  counter++;
}
