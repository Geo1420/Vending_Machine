#include <Keypad.h>
#include <ESP32Servo.h>

#define PIN_SERVO_1 4
#define PIN_SERVO_2 5
#define PIN_SERVO_3 16
#define PIN_SERVO_4 17

// stop values for 5v reference
#define SERVO_1_STOP_VALUE 90
#define SERVO_2_STOP_VALUE 90
#define SERVO_3_STOP_VALUE 22
#define SERVO_4_STOP_VALUE 27

// Obiecte servo
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

// Dimensiune keypad
const byte ROWS = 4; 
const byte COLS = 3;

// Layout taste
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

// Pini keypad
byte rowPins[ROWS] = {18, 19, 21, 22};
byte colPins[COLS] = {23, 25, 26};

// Inițializare keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {

  Serial.begin(115200);
  Serial.println("Sistem Servo + Keypad ESP32 pornit");

  // Setare frecvență PWM standard pentru servo
  servo1.setPeriodHertz(50);
  servo2.setPeriodHertz(50);
  servo3.setPeriodHertz(50);
  servo4.setPeriodHertz(50);

  // Atașare pini
  servo1.attach(PIN_SERVO_1, 500, 2400);
  servo2.attach(PIN_SERVO_2, 500, 2400);
  servo3.attach(PIN_SERVO_3, 500, 2400);
  servo4.attach(PIN_SERVO_4, 500, 2400);


}

void loop() {

  char key = keypad.getKey();
  
  if (key) {
    Serial.print("Tasta apasata: ");
    Serial.println(key);

    switch (key) {

      case '1':
        servo1.write(180);
        delay(2000);
        servo1.write(SERVO_1_STOP_VALUE);
        break;

      case '2':
        servo2.write(180);
        delay(2000);
        servo2.write(SERVO_2_STOP_VALUE);
        break;

      case '3':
        servo3.write(180);
        delay(2000);
        servo3.write(SERVO_3_STOP_VALUE);
        break;

      case '4':
        servo4.write(50);
        delay(2000);
        servo4.write(SERVO_4_STOP_VALUE);
        break;
    }
  }
}