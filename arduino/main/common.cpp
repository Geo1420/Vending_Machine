#include "common.h"

// ===== LCD =====
LiquidCrystal_I2C lcd(LCD_I2C_ADDRESS, LCD_COLUMNS, LCD_ROWS);

// ===== RC522 =====
MFRC522 rc(RFID_SS_PIN, RFID_RST_PIN);

byte defcard[][4] = {
    {0xCB, 0xB3, 0x10, 0x06},
    {0x32, 0xD7, 0x0F, 0x0B}};

int N = 2;
byte readcard[4];

unsigned long lastRFIDReset = 0;
const unsigned long rfidResetInterval = 3000;

// ===== STEPPER DRIVERS =====
AccelStepper stepper1(AccelStepper::DRIVER, STEP1, DIR1);
AccelStepper stepper2(AccelStepper::DRIVER, STEP2, DIR2);

// ===== SERVO =====
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

// ===== DHT11 =====
DHT dht(DHTPIN, DHTTYPE);

// ===== CONTROL MANUAL STEPPERI =====
float stepper1ManualSpeed = 0.0;
float stepper2ManualSpeed = 0.0;

// ===== BUTTONS =====
const unsigned long BUTTON_PRINT_INTERVAL = 500;
unsigned long lastButtonPrint = 0;

// ===== DIAGNOSTIC =====
const bool RFID_DIAGNOSTIC_MODE = false;
