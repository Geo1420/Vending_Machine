#include "display.h"
#include "environment.h"

#define BEEP_POSITIVE_REPEAT_COUNT 3
#define BEEP_POSITIVE_ON_MS 100
#define BEEP_NEGATIVE_ON_MS 500
#define BEEP_LONG_NEGATIVE_ON_MS 1500

void resetLCD()
{
  lcd.clear();
  lcd.print("Introduceti cod");
}

void beepPositive()
{
  for (int i = 0; i < BEEP_POSITIVE_REPEAT_COUNT; i++)
  {
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(GREEN_LED, HIGH);
    waitWithFanMonitoring(BEEP_POSITIVE_ON_MS);
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(GREEN_LED, LOW);
    waitWithFanMonitoring(BEEP_POSITIVE_ON_MS);
  }
}

void beepNegative()
{
  digitalWrite(BUZZER_PIN, HIGH);
  digitalWrite(RED_LED, HIGH);
  waitWithFanMonitoring(BEEP_NEGATIVE_ON_MS);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(RED_LED, LOW);
}

void beepLongNegative()
{
  digitalWrite(BUZZER_PIN, HIGH);
  digitalWrite(RED_LED, HIGH);
  waitWithFanMonitoring(BEEP_LONG_NEGATIVE_ON_MS);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(RED_LED, LOW);
}
