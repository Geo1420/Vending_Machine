#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>
#include <Servo.h>

// ===== RC522 setup =====
#define RSTPIN 9
#define SSPIN 53

MFRC522 rc(SSPIN, RSTPIN);

byte defcard[][4] = {
  {0xCB,0xB3,0x10,0x06},
  {0x32,0xD7,0x0F,0x0B}
};

int N = 2;
byte readcard[4];

// ===== Servo setup =====
#define PIN_SERVO_1 4
#define PIN_SERVO_2 5
#define PIN_SERVO_3 6
#define PIN_SERVO_4 7

#define SERVO_1_STOP_VALUE 90
#define SERVO_2_STOP_VALUE 90
#define SERVO_3_STOP_VALUE 22
#define SERVO_4_STOP_VALUE 27

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

// ===== Keypad setup =====
const byte ROWS = 4;
const byte COLS = 3;

char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte rowPins[ROWS] = {30, 31, 32, 33};
byte colPins[COLS] = {34, 35, 36};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

char inputCode[3];
int inputIndex = 0;

// ===== Buzzer =====
#define BUZZER_PIN 8

// ===== Tabel coduri valide =====
String validCodes[] = {"11","12","13","14"};

void setup() {

  Serial.begin(9600);

  SPI.begin();
  rc.PCD_Init();
  rc.PCD_DumpVersionToSerial();

  Serial.println("Sistem Keypad + RC522 + Servo pornit");

  servo1.attach(PIN_SERVO_1);
  servo2.attach(PIN_SERVO_2);
  servo3.attach(PIN_SERVO_3);
  servo4.attach(PIN_SERVO_4);

  servo1.write(SERVO_1_STOP_VALUE);
  servo2.write(SERVO_2_STOP_VALUE);
  servo3.write(SERVO_3_STOP_VALUE);
  servo4.write(SERVO_4_STOP_VALUE);

  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {

  char key = keypad.getKey();

  if (key && inputIndex < 2) {

    inputCode[inputIndex] = key;
    inputIndex++;

    Serial.print("Tasta apasata: ");
    Serial.println(key);
  }

  if (inputIndex == 2) {

    String combo = String(inputCode[0]) + String(inputCode[1]);

    Serial.print("Cod introdus: ");
    Serial.println(combo);

    if (!isValidCode(combo)) {

      Serial.println("Cod INVALID - reintroduceti!");

      beepNegative();

      resetInput();

      return;
    }

    Serial.println("Cod valid. Scanati cardul...");

    if (getid()) {

      int match = 0;

      for (int i = 0; i < N; i++) {
        if (!memcmp(readcard, defcard[i], 4)) match++;
      }

      if (match) {

        Serial.println("CARD AUTORIZAT!");
        beepPositive();

        activateServo(combo);

      } else {

        Serial.println("CARD NEAUTORIZAT!");
        beepNegative();
      }

      resetInput();
    }
  }
}

// ===== Verificare cod in tabel =====
bool isValidCode(String code) {

  for (int i = 0; i < 4; i++) {
    if (code == validCodes[i]) return true;
  }

  return false;
}

// ===== Reset introducere keypad =====
void resetInput() {

  inputIndex = 0;
  inputCode[0] = '\0';
  inputCode[1] = '\0';
}

// ===== Citire UID =====
int getid() {

  if (!rc.PICC_IsNewCardPresent()) return 0;
  if (!rc.PICC_ReadCardSerial()) return 0;

  Serial.print("UID: ");

  for (int i = 0; i < 4; i++) {

    readcard[i] = rc.uid.uidByte[i];
    Serial.print(readcard[i], HEX);
    Serial.print(" ");
  }

  Serial.println("");

  rc.PICC_HaltA();

  return 1;
}

// ===== Activare servo =====
void activateServo(String combo) {

  Serial.print("Activare pentru cod: ");
  Serial.println(combo);

  if (combo == "11") {

    servo1.write(180);
    delay(2000);
    servo1.write(SERVO_1_STOP_VALUE);

  } else if (combo == "12") {

    servo2.write(180);
    delay(2000);
    servo2.write(SERVO_2_STOP_VALUE);

  } else if (combo == "13") {

    servo3.write(180);
    delay(2000);
    servo3.write(SERVO_3_STOP_VALUE);

  } else if (combo == "14") {

    servo4.write(50);
    delay(2000);
    servo4.write(SERVO_4_STOP_VALUE);
  }
}

// ===== Beep pozitiv =====
void beepPositive() {

  for (int i = 0; i < 3; i++) {

    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);
    delay(100);
  }
}

// ===== Beep negativ =====
void beepNegative() {

  digitalWrite(BUZZER_PIN, HIGH);
  delay(500);
  digitalWrite(BUZZER_PIN, LOW);
}