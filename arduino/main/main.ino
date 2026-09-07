#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <AccelStepper.h>
#include <Servo.h>
#include <DHT.h> // Biblioteca DHT

// ===== LCD =====
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ===== RC522 =====
#define RSTPIN 48
#define SSPIN 53
MFRC522 rc(SSPIN, RSTPIN);

byte defcard[][4] = {
    {0xCB, 0xB3, 0x10, 0x06},
    {0x32, 0xD7, 0x0F, 0x0B}};

int N = 2;
byte readcard[4];

unsigned long ultimaResetareRFID = 0;
const unsigned long intervalResetRFID = 3000;

// ===== STEPPER DRIVERS =====
#define STEP1 22
#define DIR1 23
#define EN1 24

#define STEP2 25
#define DIR2 26
#define EN2 27

AccelStepper stepper1(AccelStepper::DRIVER, STEP1, DIR1);
AccelStepper stepper2(AccelStepper::DRIVER, STEP2, DIR2);

#define HALF_TURN 300 // 1200

// ===== SERVO =====
#define PIN_SERVO_1 40
#define PIN_SERVO_2 41
#define PIN_SERVO_3 42
#define PIN_SERVO_4 43

#define SERVO_1_STOP 23 // 90
#define SERVO_2_STOP 90
#define SERVO_3_STOP 90 // 23
#define SERVO_4_STOP 90

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

// ===== KEYPAD =====
const byte ROWS = 4;
const byte COLS = 3;

char keys[ROWS][COLS] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}};

byte rowPins[ROWS] = {30, 31, 32, 33};
byte colPins[COLS] = {34, 35, 36};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

char inputCode[3];
int inputIndex = 0;

// ===== BUZZER =====
#define BUZZER_PIN 8

// ===== CODURI =====
String validCodes[] = {"11", "12", "13", "14"};

// ===== DHT11 =====
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// ==== LEDs ====
#define GREEN_LED 6
#define RED_LED 7

// ==== PUSH BUTTON ====
#define BUTTON_DOWN_PIN 38
#define BUTTON_UP_PIN 39
#define BUTTON_RIGHT_PIN 44
#define BUTTON_LEFT_PIN 45
const unsigned long BUTTON_PRINT_INTERVAL = 500;
unsigned long lastButtonPrint = 0;

// ===== CONTROL MANUAL STEPPERI =====
float stepper1ManualSpeed = 0.0;
float stepper2ManualSpeed = 0.0;

// FAN
#define FAN_PIN 5
#define FAN2_PIN 4
#define FAN_TEMPERATURE_LIMIT 28.0

// Test temporar: RC522 este verificat continuu, fara cod de acces.
const bool RFID_DIAGNOSTIC_MODE = false;

void resetRFID()
{
  digitalWrite(RSTPIN, LOW);
  delay(50);

  digitalWrite(RSTPIN, HIGH);
  delay(50);

  rc.PCD_Init();
  rc.PCD_SetAntennaGain(MFRC522::RxGain_max);
  delay(50);

  ultimaResetareRFID = millis();
}

void maintainRFIDActiv()
{
  if (millis() - ultimaResetareRFID >= intervalResetRFID)
  {
    resetRFID();
  }
}

bool verifyRFID()
{
  if (!rc.PICC_IsNewCardPresent())
    return false;

  if (!rc.PICC_ReadCardSerial())
  {
    resetRFID();
    return false;
  }

  for (int i = 0; i < 4; i++)
    readcard[i] = rc.uid.uidByte[i];

  rc.PICC_HaltA();
  rc.PCD_StopCrypto1();
  return true;
}

bool prepareRFID()
{
  rc.PCD_Init();
  rc.PCD_SetAntennaGain(MFRC522::RxGain_max);
  delay(50);

  byte version = rc.PCD_ReadRegister(MFRC522::VersionReg);
  Serial.print("RFID DEBUG: VersionReg inainte de scanare = 0x");
  if (version < 0x10)
    Serial.print("0");
  Serial.println(version, HEX);

  if (version == 0x00 || version == 0xFF)
  {
    Serial.println("RFID DEBUG: RC522 nu raspunde pe SPI");
    return false;
  }

  return true;
}

void diagnoseRFID()
{
  static unsigned long lastMessage = 0;
  static unsigned long lastVersionCheck = 0;

  if (millis() - lastVersionCheck >= 5000)
  {
    lastVersionCheck = millis();
    byte version = rc.PCD_ReadRegister(MFRC522::VersionReg);
    Serial.print("RFID TEST: VersionReg = 0x");
    if (version < 0x10)
      Serial.print("0");
    Serial.println(version, HEX);
  }

  if (verifyRFID())
  {
    Serial.print("RFID TEST: card citit, UID = ");
    for (byte i = 0; i < rc.uid.size; i++)
    {
      if (rc.uid.uidByte[i] < 0x10)
        Serial.print("0");
      Serial.print(rc.uid.uidByte[i], HEX);
      if (i + 1 < rc.uid.size)
        Serial.print(":");
    }
    Serial.println();
    delay(500);
    return;
  }

  if (millis() - lastMessage >= 1000)
  {
    lastMessage = millis();
    Serial.println("RFID TEST: modul activ, niciun card detectat");
  }
}

void updateFans()
{
  float temperature = dht.readTemperature();
  bool fansOn = !isnan(temperature) && temperature > FAN_TEMPERATURE_LIMIT;

  digitalWrite(FAN_PIN, fansOn ? HIGH : LOW);
  digitalWrite(FAN2_PIN, fansOn ? HIGH : LOW);
}

void waitWithFanMonitoring(unsigned long duration)
{
  unsigned long start = millis();
  while (millis() - start < duration)
  {
    updateFans();
    delay(50);
  }
}

void enableSteppers()
{
  digitalWrite(EN1, LOW);
  digitalWrite(EN2, LOW);
}

void disableSteppers()
{
  digitalWrite(EN1, HIGH);
  digitalWrite(EN2, HIGH);
}

void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600); // Comunicarea cu ESP32
  pinMode(RSTPIN, OUTPUT);
  digitalWrite(RSTPIN, HIGH);
  pinMode(SSPIN, OUTPUT);
  digitalWrite(SSPIN, HIGH);
  SPI.begin();
  rc.PCD_Init();
  rc.PCD_SetAntennaGain(MFRC522::RxGain_max);
  ultimaResetareRFID = millis();
  Serial.println("RC522 initializat. Versiune citita:");
  rc.PCD_DumpVersionToSerial();

  pinMode(EN1, OUTPUT);
  pinMode(EN2, OUTPUT);
  digitalWrite(EN1, HIGH);
  digitalWrite(EN2, HIGH);

  pinMode(STEP1, OUTPUT);
  pinMode(DIR1, OUTPUT);
  pinMode(STEP2, OUTPUT);
  pinMode(DIR2, OUTPUT);

  digitalWrite(STEP1, LOW);
  digitalWrite(DIR1, LOW);
  digitalWrite(STEP2, LOW);
  digitalWrite(DIR2, LOW);

  stepper1.setMaxSpeed(800);
  stepper1.setAcceleration(400);
  stepper2.setMaxSpeed(800);
  stepper2.setAcceleration(400);

  servo1.attach(PIN_SERVO_1);
  servo1.write(SERVO_1_STOP);
  servo2.attach(PIN_SERVO_2);
  servo2.write(SERVO_2_STOP);
  servo3.attach(PIN_SERVO_3);
  servo3.write(SERVO_3_STOP);
  servo4.attach(PIN_SERVO_4);
  servo4.write(SERVO_4_STOP);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);
  pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT_PIN, INPUT_PULLUP);
  pinMode(BUTTON_LEFT_PIN, INPUT_PULLUP);

  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Introduceti cod");
  resetInput();

  // ===== initializare DHT =====
  dht.begin();
  pinMode(FAN_PIN, OUTPUT);
  pinMode(FAN2_PIN, OUTPUT);
  digitalWrite(FAN_PIN, LOW);  // ventilator oprit
  digitalWrite(FAN2_PIN, LOW); // ventilator oprit
}
int fanState = 0;
void loop()
{
  updateFans();

  if (RFID_DIAGNOSTIC_MODE)
  {
    diagnoseRFID();
    return;
  }

  // Monitorizarea temperaturii ramane activa in orice stare a ciclului.
  updateFans();

  bool downButton = (digitalRead(BUTTON_DOWN_PIN) == LOW);
  bool upButton = (digitalRead(BUTTON_UP_PIN) == LOW);
  bool rightButton = (digitalRead(BUTTON_RIGHT_PIN) == LOW);
  bool leftButton = (digitalRead(BUTTON_LEFT_PIN) == LOW);

  stepper1ManualSpeed = 0.0;
  stepper2ManualSpeed = 0.0;

  // DownButton: both steppers move together with -800
  if (downButton)
  {
    stepper1ManualSpeed -= 800.0;
    stepper2ManualSpeed -= 800.0;
  }

  // UpButton: both steppers move together with +800
  if (upButton)
  {
    stepper1ManualSpeed += 800.0;
    stepper2ManualSpeed += 800.0;
  }

  // RightButton: stepper1 +800, stepper2 -800
  if (rightButton)
  {
    stepper1ManualSpeed += 800.0;
    stepper2ManualSpeed -= 800.0;
  }

  // LeftButton: stepper1 -800, stepper2 +800
  if (leftButton)
  {
    stepper1ManualSpeed -= 800.0;
    stepper2ManualSpeed += 800.0;
  }

  if (stepper1ManualSpeed != 0.0)
  {
    enableSteppers();
    stepper1.setSpeed(stepper1ManualSpeed);
    stepper1.runSpeed();
  }
  else
  {
    stepper1.setSpeed(0);
    stepper1.stop();
  }

  if (stepper2ManualSpeed != 0.0)
  {
    enableSteppers();
    stepper2.setSpeed(stepper2ManualSpeed);
    stepper2.runSpeed();
  }
  else
  {
    stepper2.setSpeed(0);
    stepper2.stop();
  }

  if (stepper1ManualSpeed == 0.0 && stepper2ManualSpeed == 0.0)
  {
    disableSteppers();
  }

  unsigned long now = millis();
  if (now - lastButtonPrint >= BUTTON_PRINT_INTERVAL)
  {
    lastButtonPrint = now;

    Serial.print("Down:");
    Serial.print(downButton ? "1" : "0");
    Serial.print(" Up:");
    Serial.print(upButton ? "1" : "0");
    Serial.print(" Right:");
    Serial.print(rightButton ? "1" : "0");
    Serial.print(" Left:");
    Serial.println(leftButton ? "1" : "0");
  }

  // Resetarea periodica poate intrerupe o citire RFID in curs.
  // maintainRFIDActiv();
  char key = keypad.getKey();

  /*
  if (dht.readTemperature() > 26)
  {
    digitalWrite(FAN_PIN, HIGH);
    Serial.println(dht.readTemperature());
  }
  else
  {
    digitalWrite(FAN_PIN, LOW);
  }
  // Trimitem catre ESP32
  // Serial1.print("TEMP:");
  // Serial1.println(dht.readTemperature());

  // Optional, trimitem si umiditatea
  // Serial1.print("HUM:");
  // Serial1.println(dht.readHumidity());
  // fanState = !fanState;
  // Serial1.print("FAN:");
  // Serial1.println(fanState);
  // delay(2000);
  */

  if (key && inputIndex < 2)
  {
    inputCode[inputIndex] = key;
    inputIndex++;
    inputCode[inputIndex] = '\0';
    Serial.print("Tasta apasata: ");
    Serial.println(key);
    lcd.setCursor(0, 1);
    lcd.print("  ");
    lcd.setCursor(0, 1);
    lcd.print(inputCode);
  }

  if (inputIndex == 2)
  {
    String combo = String(inputCode[0]) + String(inputCode[1]);
    Serial.print("Cod introdus: ");
    Serial.println(combo);

    if (!isValidCode(combo))
    {
      Serial.println("Cod invalid. Incercati din nou.");
      lcd.clear();
      lcd.print("Cod invalid");
      beepNegative();
      waitWithFanMonitoring(1500);
      resetLCD();
      resetInput();
      return;
    }

    lcd.clear();
    lcd.print("Scanati card");
    Serial.println("Cod valid. Astept scanarea cardului...");

    const unsigned long RFID_WAIT_TIMEOUT = 5000;
    bool cardRead = false;
    bool rfidReady = false;
    unsigned long startRFID = millis();
    unsigned long lastRFIDRetry = 0;

    while (millis() - startRFID < RFID_WAIT_TIMEOUT)
    {
      updateFans();

      if (!rfidReady && millis() - lastRFIDRetry >= 1000)
      {
        lastRFIDRetry = millis();
        rfidReady = prepareRFID();
      }

      if (rfidReady && verifyRFID())
      {
        cardRead = true;
        break;
      }

      delay(20);
    }

    if (cardRead)
    {
      bool cardValid = false;
      for (int i = 0; i < N; i++)
      {
        if (!memcmp(readcard, defcard[i], 4))
        {
          cardValid = true;
          break;
        }
      }

      Serial.print("Card scanat. UID: ");
      for (int i = 0; i < 4; i++)
      {
        if (readcard[i] < 0x10)
          Serial.print("0");
        Serial.print(readcard[i], HEX);
        if (i < 3)
          Serial.print(":");
      }
      Serial.println();

      if (cardValid)
      {
        Serial.println("Card valid. Bip pozitiv si LED verde.");
        lcd.clear();
        lcd.print("Acces permis");
        beepPositive();

        movePlatformToCode(combo);

        activateServo(combo);
        returnPlatformHome(combo);

        // ===== STEPPERE: revenire la pozitia HOME =====
        // Aici se va adauga revenirea platformei la pozitia de home.
      }
      else
      {
        Serial.println("Card necunoscut. Bip lung si LED rosu.");
        lcd.clear();
        lcd.print("Card invalid");
        beepNegative();
      }

      /*
      // ===== STEPPELERE =====
      activateSteppers();

      // ===== SERVO corespunzator codului =====
      activateServo(combo);

      // ===== CITIRE DHT11 =====
      float temp = dht.readTemperature();
      float hum = dht.readHumidity();
      */

      waitWithFanMonitoring(1500);
      resetLCD();
      resetInput();
    }
    else
    {
      Serial.println("RFID DEBUG: timeout dupa 5 secunde. Acces pozitiv pentru test.");
      lcd.clear();
      lcd.print("Acces test OK");
      beepPositive();

      movePlatformToCode(combo);

      activateServo(combo);
      returnPlatformHome(combo);

      waitWithFanMonitoring(1500);
      resetLCD();
      resetInput();
    }
  }
}

// ===== PLATFORM MOVEMENT =====
void moveSteppers(long stepper1Steps, long stepper2Steps)
{
  enableSteppers();

  stepper1.move(stepper1Steps);
  stepper2.move(stepper2Steps);

  while (stepper1.distanceToGo() != 0 || stepper2.distanceToGo() != 0)
  {
    stepper2.run();
    stepper1.run();
  }

  disableSteppers();
}

void moveUp(long steps)
{
  moveSteppers(steps, steps);
}

void moveDown(long steps)
{
  moveSteppers(-steps, -steps);
}

void moveLeft(long steps)
{
  moveSteppers(-steps, steps);
}

void moveRight(long steps)
{
  moveSteppers(steps, -steps);
}

void movePlatformToCode(String combo)
{
  if (combo == "12")
  {
    moveLeft(450);
    moveUp(675);
  }
  else if (combo == "14")
  {
    moveUp(675);
  }
  else if (combo == "13")
  {
    moveUp(1300);
  }
  else if (combo == "11")
  {
    moveLeft(450);
    moveUp(1300);
  }
}

void returnPlatformHome(String combo)
{
  if (combo == "12")
  {
    moveDown(675);
    moveRight(450);
  }
  else if (combo == "14")
  {
    moveDown(675);
  }
  else if (combo == "13")
  {
    moveDown(1300);
  }
  else if (combo == "11")
  {
    moveDown(1300);
    moveRight(450);
  }
}

// ===== SERVO =====
void activateServo(String combo)
{
  if (combo == "11")
  {
    rotateServo1();
  }
  else if (combo == "12")
  {
    rotateServo(servo2, SERVO_2_STOP);
  }
  else if (combo == "13")
  {
    rotateServo(servo3, SERVO_3_STOP);
  }
  else if (combo == "14")
  {
    rotateServo(servo4, SERVO_4_STOP);
  }
}

void rotateServo(Servo &s, int stopPos)
{
  s.write(180);

  unsigned long startServo = millis();
  while (millis() - startServo < 5000)
  {
    updateFans();
    delay(50);
  }

  s.write(stopPos);
}

void rotateServo1()
{
  servo1.write(0);
  delay(3000);
  servo1.write(SERVO_1_STOP);
}

// ===== LCD =====
void resetLCD()
{
  lcd.clear();
  lcd.print("Introduceti cod");
}

// ===== COD VALID =====
bool isValidCode(String code)
{
  for (int i = 0; i < 4; i++)
    if (code == validCodes[i])
      return true;
  return false;
}

// ===== RESET INPUT =====
void resetInput()
{
  inputIndex = 0;
  inputCode[0] = '\0';
  inputCode[1] = '\0';
  lcd.setCursor(0, 1);
  lcd.print("  ");
}

// ===== CITIRE RFID =====
int getid()
{
  if (!rc.PICC_IsNewCardPresent())
    return 0;
  if (!rc.PICC_ReadCardSerial())
    return 0;

  for (int i = 0; i < 4; i++)
    readcard[i] = rc.uid.uidByte[i];
  rc.PICC_HaltA();
  return 1;
}

// ===== BEEP =====
void beepPositive()
{
  for (int i = 0; i < 3; i++)
  {
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(GREEN_LED, HIGH);
    waitWithFanMonitoring(100);
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(GREEN_LED, LOW);
    waitWithFanMonitoring(100);
  }
}
void beepNegative()
{
  digitalWrite(BUZZER_PIN, HIGH);
  digitalWrite(RED_LED, HIGH);
  waitWithFanMonitoring(500);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(RED_LED, LOW);
}