#include "platform.h"
#include "environment.h"

#define STEPPER_PLATFORM_LEFT_OFFSET_STEPS 450
#define STEPPER_PLATFORM_UP_OFFSET_STEPS 675
#define STEPPER_PLATFORM_EXTRA_UP_STEPS 1300
#define PLATFORM_HOME_RETURN_DELAY_MS 5000
#define SERVO_ROTATION_MS 5000
#define SERVO_ONE_ROTATION_MS 3000

void initializePlatform()
{
  servo1.attach(SERVO_ONE_PIN);
  servo2.attach(SERVO_TWO_PIN);
  servo3.attach(SERVO_THREE_PIN);
  servo4.attach(SERVO_FOUR_PIN);

  servo1.write(SERVO_1_STOP);
  servo2.write(SERVO_2_STOP);
  servo3.write(SERVO_3_STOP);
  servo4.write(SERVO_4_STOP);
}

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
    moveLeft(STEPPER_PLATFORM_LEFT_OFFSET_STEPS);
    moveUp(STEPPER_PLATFORM_UP_OFFSET_STEPS);
  }
  else if (combo == "14")
  {
    moveUp(STEPPER_PLATFORM_UP_OFFSET_STEPS);
  }
  else if (combo == "13")
  {
    moveUp(STEPPER_PLATFORM_EXTRA_UP_STEPS);
  }
  else if (combo == "11")
  {
    moveLeft(STEPPER_PLATFORM_LEFT_OFFSET_STEPS);
    moveUp(STEPPER_PLATFORM_EXTRA_UP_STEPS);
  }
}

void returnPlatformHome(String combo)
{
  if (combo == "12")
  {
    moveDown(STEPPER_PLATFORM_UP_OFFSET_STEPS);
    moveRight(STEPPER_PLATFORM_LEFT_OFFSET_STEPS);
  }
  else if (combo == "14")
  {
    moveDown(STEPPER_PLATFORM_UP_OFFSET_STEPS);
  }
  else if (combo == "13")
  {
    moveDown(STEPPER_PLATFORM_EXTRA_UP_STEPS);
  }
  else if (combo == "11")
  {
    moveDown(STEPPER_PLATFORM_EXTRA_UP_STEPS);
    moveRight(STEPPER_PLATFORM_LEFT_OFFSET_STEPS);
  }
}

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
  while (millis() - startServo < SERVO_ROTATION_MS)
  {
    updateFans();
    delay(50);
  }

  s.write(stopPos);
}

void rotateServo1()
{
  servo1.write(0);
  delay(SERVO_ONE_ROTATION_MS);
  servo1.write(SERVO_1_STOP);
}
