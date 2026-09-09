#ifndef COMMON_H
#define COMMON_H

#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <AccelStepper.h>
#include <Servo.h>
#include <DHT.h>

// ===== LCD =====
#define LCD_I2C_ADDRESS 0x27
#define LCD_COLUMNS 16
#define LCD_ROWS 2
extern LiquidCrystal_I2C lcd;

// ===== RC522 =====
#define RFID_RST_PIN 48
#define RFID_SS_PIN 53
extern MFRC522 rc;

extern byte defcard[][4];
extern int N;
extern byte readcard[4];

extern unsigned long lastRFIDReset;
extern const unsigned long rfidResetInterval;

// ===== STEPPER DRIVERS =====
#define STEP_ONE 22
#define DIR_ONE 23
#define ENABLE_ONE 24
#define STEP_TWO 25
#define DIR_TWO 26
#define ENABLE_TWO 27

#define STEP1 STEP_ONE
#define DIR1 DIR_ONE
#define EN1 ENABLE_ONE
#define STEP2 STEP_TWO
#define DIR2 DIR_TWO
#define EN2 ENABLE_TWO

extern AccelStepper stepper1;
extern AccelStepper stepper2;

#define HALF_TURN 300

// ===== SERVO =====
#define SERVO_ONE_PIN 40
#define SERVO_TWO_PIN 41
#define SERVO_THREE_PIN 42
#define SERVO_FOUR_PIN 43

#define PIN_SERVO_1 SERVO_ONE_PIN
#define PIN_SERVO_2 SERVO_TWO_PIN
#define PIN_SERVO_3 SERVO_THREE_PIN
#define PIN_SERVO_4 SERVO_FOUR_PIN

#define SERVO_1_STOP 23
#define SERVO_2_STOP 90
#define SERVO_3_STOP 90
#define SERVO_4_STOP 90

extern Servo servo1;
extern Servo servo2;
extern Servo servo3;
extern Servo servo4;

// ===== KEYPAD =====
extern const byte ROWS;
extern const byte COLS;
extern char keys[4][3];
extern byte rowPins[4];
extern byte colPins[3];
extern Keypad keypad;

extern char inputCode[3];
extern int inputIndex;

// ===== BUZZER =====
#define BUZZER_PIN 8

// ===== DHT11 =====
#define DHTPIN 2
#define DHTTYPE DHT11
extern DHT dht;

// ==== LEDs ====
#define GREEN_LED 6
#define RED_LED 7

// ==== PUSH BUTTON ====
#define BUTTON_DOWN_PIN 38
#define BUTTON_UP_PIN 39
#define BUTTON_RIGHT_PIN 44
#define BUTTON_LEFT_PIN 45
extern const unsigned long BUTTON_PRINT_INTERVAL;
extern unsigned long lastButtonPrint;

// ===== CONTROL MANUAL STEPPERS =====
extern float stepper1ManualSpeed;
extern float stepper2ManualSpeed;

// FAN
#define FAN_PIN 5
#define FAN_SECOND_PIN 4
#define FAN_TEMPERATURE_LIMIT 28.0

#define FAN2_PIN FAN_SECOND_PIN

extern const bool RFID_DIAGNOSTIC_MODE;

#endif
