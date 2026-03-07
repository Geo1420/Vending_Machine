#include "ServoController.h"

ServoController::ServoController() {
  for (int i = 0; i < MAX_SERVOS; i++) {
    currentAngle[i] = 90;   // poziție neutru
    targetAngle[i] = 90;
  }
  lastUpdate = 0;
}

void ServoController::attach(uint8_t pin, uint8_t index) {
  if (index < MAX_SERVOS) {
    servos[index].attach(pin);
    servos[index].write(currentAngle[index]); // initializează la poziția curentă
  }
}

void ServoController::setTarget(uint8_t index, int angle) {
  if (index < MAX_SERVOS) {
    targetAngle[index] = constrain(angle, 0, 180);
  }
}

void ServoController::update() {
  unsigned long currentTime = millis();
  if (currentTime - lastUpdate >= stepDelay) {
    lastUpdate = currentTime;
    for (int i = 0; i < MAX_SERVOS; i++) {
      if (currentAngle[i] < targetAngle[i]) currentAngle[i]++;
      else if (currentAngle[i] > targetAngle[i]) currentAngle[i]--;
      servos[i].write(currentAngle[i]);
    }
  }
}