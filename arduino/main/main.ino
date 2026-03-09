#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <AccelStepper.h>

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

// 200 pasi = 360° la majoritatea NEMA17
// 180° = 100 pasi
#define HALF_TURN 100

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

Keypad keypad = Keypad(makeKeymap(keys),rowPins,colPins,ROWS,COLS);

char inputCode[3];
int inputIndex=0;

// ===== BUZZER =====
#define BUZZER_PIN 8

// ===== CODURI =====
String validCodes[]={"11","12","13","14"};

void setup(){

Serial.begin(9600);

SPI.begin();
rc.PCD_Init();

pinMode(EN1,OUTPUT);
pinMode(EN2,OUTPUT);

digitalWrite(EN1,LOW);
digitalWrite(EN2,LOW);

stepper1.setMaxSpeed(800);
stepper1.setAcceleration(400);

stepper2.setMaxSpeed(800);
stepper2.setAcceleration(400);

pinMode(BUZZER_PIN,OUTPUT);

lcd.begin();
lcd.backlight();

lcd.setCursor(0,0);
lcd.print("Introduceti cod");

resetInput();
}

void loop(){

char key = keypad.getKey();

if(key && inputIndex<2){

inputCode[inputIndex]=key;
inputIndex++;

lcd.setCursor(0,1);
lcd.print("  ");
lcd.setCursor(0,1);
lcd.print(inputCode);
}

if(inputIndex==2){

String combo = String(inputCode[0]) + String(inputCode[1]);

if(!isValidCode(combo)){

lcd.clear();
lcd.print("Cod invalid");

beepNegative();
delay(1500);

resetLCD();
resetInput();
return;
}

lcd.clear();
lcd.print("Scanati card");

if(getid()){

int match=0;

for(int i=0;i<N;i++){
if(!memcmp(readcard,defcard[i],4)) match++;
}

if(match){

lcd.clear();
lcd.print("Acces permis");

beepPositive();

activateSteppers();

}else{

lcd.clear();
lcd.print("Acces respins");

beepNegative();
}

delay(2000);

resetLCD();
resetInput();
}
}
}

// ===== MISCARE STEPPER =====
void activateSteppers(){

stepper1.move(HALF_TURN);

while(stepper1.distanceToGo()!=0){
stepper1.run();
}

delay(500);

stepper2.move(HALF_TURN);

while(stepper2.distanceToGo()!=0){
stepper2.run();
}

}

// ===== LCD reset =====
void resetLCD(){
lcd.clear();
lcd.print("Introduceti cod");
}

// ===== verificare cod =====
bool isValidCode(String code){

for(int i=0;i<4;i++){
if(code==validCodes[i]) return true;
}

return false;
}

// ===== reset input =====
void resetInput(){

inputIndex=0;
inputCode[0]='\0';
inputCode[1]='\0';

lcd.setCursor(0,1);
lcd.print("  ");
}

// ===== citire RFID =====
int getid(){

if(!rc.PICC_IsNewCardPresent()) return 0;
if(!rc.PICC_ReadCardSerial()) return 0;

for(int i=0;i<4;i++){
readcard[i]=rc.uid.uidByte[i];
}

rc.PICC_HaltA();
return 1;
}

// ===== beep pozitiv =====
void beepPositive(){

for(int i=0;i<3;i++){
digitalWrite(BUZZER_PIN,HIGH);
delay(100);
digitalWrite(BUZZER_PIN,LOW);
delay(100);
}
}

// ===== beep negativ =====
void beepNegative(){

digitalWrite(BUZZER_PIN,HIGH);
delay(500);
digitalWrite(BUZZER_PIN,LOW);

}