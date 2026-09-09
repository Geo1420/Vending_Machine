#ifndef PLATFORM_H
#define PLATFORM_H

#include "common.h"

void initializePlatform();
void moveSteppers(long stepper1Steps, long stepper2Steps);
void moveUp(long steps);
void moveDown(long steps);
void moveLeft(long steps);
void moveRight(long steps);
void movePlatformToCode(String combo);
void returnPlatformHome(String combo);
void activateServo(String combo);
void rotateServo(Servo &s, int stopPos);
void rotateServo1();

#endif
