#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <AccelStepper.h>
#include <Servo.h>
#include <DHT.h>  // Biblioteca DHT

// ===== LCD =====
LiquidCrystal_I2C lcd(0x27,16,2);

// ===== RC522 =====
#define RSTPIN 9
#define SSPIN 53
MFRC522 rc(SSPIN,RSTPIN);

byte defcard[][4] = {
  {0xCB,0xB3,0x10,0x06},
  {0x32,0xD7,0x0F,0x0B}
};

int N = 2;
byte readcard[4];

// ===== STEPPER DRIVERS =====
#define STEP1 22
#define DIR1 23
#define EN1 24

#define STEP2 25
#define DIR2 26
#define EN2 27

AccelStepper stepper1(AccelStepper::DRIVER, STEP1, DIR1);
AccelStepper stepper2(AccelStepper::DRIVER, STEP2, DIR2);

#define HALF_TURN 1200

// ===== SERVO =====
#define PIN_SERVO_1 40
#define PIN_SERVO_2 41
#define PIN_SERVO_3 42
#define PIN_SERVO_4 43

#define SERVO_1_STOP 90
#define SERVO_2_STOP 90
#define SERVO_3_STOP 23
#define SERVO_4_STOP 90

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

// ===== KEYPAD =====
const byte ROWS = 4;
const byte COLS = 3;

char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte rowPins[ROWS] = {30,31,32,33};
byte colPins[COLS] = {34,35,36};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

char inputCode[3];
int inputIndex = 0;

// ===== BUZZER =====
#define BUZZER_PIN 8

// ===== CODURI =====
String validCodes[] = {"11","12","13","14"};

// ===== DHT11 =====
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rc.PCD_Init();

  pinMode(EN1, OUTPUT);
  pinMode(EN2, OUTPUT);
  digitalWrite(EN1, LOW);
  digitalWrite(EN2, LOW);

  stepper1.setMaxSpeed(800);
  stepper1.setAcceleration(400);
  stepper2.setMaxSpeed(800);
  stepper2.setAcceleration(400);

  servo1.attach(PIN_SERVO_1); servo1.write(SERVO_1_STOP);
  servo2.attach(PIN_SERVO_2); servo2.write(SERVO_2_STOP);
  servo3.attach(PIN_SERVO_3); servo3.write(SERVO_3_STOP);
  servo4.attach(PIN_SERVO_4); servo4.write(SERVO_4_STOP);

  pinMode(BUZZER_PIN, OUTPUT);

  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Introduceti cod");
  resetInput();

  // ===== initializare DHT =====
  dht.begin();
}

void loop() {
  char key = keypad.getKey();

  if(key && inputIndex < 2){
    inputCode[inputIndex] = key;
    inputIndex++;
    lcd.setCursor(0,1); lcd.print("  "); lcd.setCursor(0,1); lcd.print(inputCode);
  }

  if(inputIndex == 2){
    String combo = String(inputCode[0]) + String(inputCode[1]);

    if(!isValidCode(combo)){
      lcd.clear(); lcd.print("Cod invalid");
      beepNegative(); delay(1500);
      resetLCD(); resetInput();
      return;
    }

    lcd.clear(); lcd.print("Scanati card");

    if(getid()){
      int match = 0;
      for(int i=0;i<N;i++) if(!memcmp(readcard,defcard[i],4)) match++;

      if(match){
        lcd.clear(); lcd.print("Acces permis");
        beepPositive();

        // ===== STEPPELERE =====
        activateSteppers();

        // ===== SERVO corespunzator codului =====
        activateServo(combo);

        // ===== CITIRE DHT11 =====
        float temp = dht.readTemperature();
        float hum = dht.readHumidity();
        lcd.setCursor(0,1);
        if(isnan(temp) || isnan(hum)){
          lcd.print("DHT11 err");
        } else {
          lcd.print("T:");
          lcd.print(temp);
          lcd.print("C H:");
          lcd.print(hum);
          lcd.print("%");
        }

      } else {
        lcd.clear(); lcd.print("Acces respins");
        beepNegative();
      }

      delay(2000);
      resetLCD();
      resetInput();
    }
  }
}

// ===== STEPPERele =====
void activateSteppers(){
  stepper1.move(HALF_TURN);
  while(stepper1.distanceToGo()!=0) stepper1.run();

  delay(500);

  stepper2.move(HALF_TURN);
  while(stepper2.distanceToGo()!=0) stepper2.run();
}

// ===== SERVO =====
void activateServo(String combo){
  if(combo == "11"){ rotateServo(servo1, SERVO_1_STOP); }
  else if(combo == "12"){ rotateServo(servo2, SERVO_2_STOP); }
  else if(combo == "13"){ rotateServo(servo3, SERVO_3_STOP); }
  else if(combo == "14"){ rotateServo(servo4, SERVO_4_STOP); }
}

void rotateServo(Servo &s, int stopPos){
  for(int i=0;i<2;i++){
    for(int pos=0; pos<=180; pos++){ s.write(pos); delay(10); }
    for(int pos=180; pos>=0; pos--){ s.write(pos); delay(10); }
  }
  s.write(stopPos); // revine la pozitia initiala
}

// ===== LCD =====
void resetLCD(){ lcd.clear(); lcd.print("Introduceti cod"); }

// ===== COD VALID =====
bool isValidCode(String code){
  for(int i=0;i<4;i++) if(code==validCodes[i]) return true;
  return false;
}

// ===== RESET INPUT =====
void resetInput(){ inputIndex=0; inputCode[0]='\0'; inputCode[1]='\0'; lcd.setCursor(0,1); lcd.print("  "); }

// ===== CITIRE RFID =====
int getid(){
  if(!rc.PICC_IsNewCardPresent()) return 0;
  if(!rc.PICC_ReadCardSerial()) return 0;

  for(int i=0;i<4;i++) readcard[i]=rc.uid.uidByte[i];
  rc.PICC_HaltA();
  return 1;
}

// ===== BEEP =====
void beepPositive(){ for(int i=0;i<3;i++){ digitalWrite(BUZZER_PIN,HIGH); delay(100); digitalWrite(BUZZER_PIN,LOW); delay(100); } }
void beepNegative(){ digitalWrite(BUZZER_PIN,HIGH); delay(500); digitalWrite(BUZZER_PIN,LOW); }