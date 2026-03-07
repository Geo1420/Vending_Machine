#ifndef SERVO_CONTROLLER_H
#define SERVO_CONTROLLER_H

#include <ESP32Servo.h>

#define MAX_SERVOS 4

#define PIN_SERVO_1 4
#define PIN_SERVO_2 5
#define PIN_SERVO_3 16
#define PIN_SERVO_4 17

class ServoController {
  public:
    ServoController();
    void attach(uint8_t pin, uint8_t index);   // atașează un servo la un pin
    void setTarget(uint8_t index, int angle);  // setează unghiul țintă
    void update();                             // actualizează poziția fiecărui servo

  private:
    Servo servos[MAX_SERVOS];
    int currentAngle[MAX_SERVOS];  // unghiul curent
    int targetAngle[MAX_SERVOS];   // unghiul dorit
    unsigned long lastUpdate;      // timp pentru control non-blocant
    const unsigned long stepDelay = 20; // 20ms
};

#endif