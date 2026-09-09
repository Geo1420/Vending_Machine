#include "display.h"
#include "environment.h"
#include "product.h"

#define BEEP_POSITIVE_REPEAT_COUNT 3
#define BEEP_POSITIVE_ON_MS 50
#define BEEP_NEGATIVE_ON_MS 500
#define BEEP_LONG_NEGATIVE_ON_MS 50

void initializeDisplay()
{
  lcd.init();
  lcd.backlight();

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  resetLCD();
}

void resetLCD()
{
  lcd.clear();
  lcd.print("Enter code");
}

void showProductCodeOnLCD(const String& code)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Code:");
  lcd.setCursor(0, 1);
  lcd.print(code);
}

void processKeypadInput(String& inputCode)
{
  char key = keypad.getKey();
  if (key == NO_KEY)
    return;

  if (key == '*')
  {
    inputCode = "";
    resetLCD();
    return;
  }

  if (key == '#')
  {
    if (inputCode.length() >= PRODUCT_CODE_LENGTH)
    {
      processProductSelection(inputCode);
    }
    else
    {
      beepNegative();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Short code");
      waitWithFanMonitoring(50);
      resetLCD();
    }

    inputCode = "";
    return;
  }

  if (inputCode.length() < PRODUCT_CODE_LENGTH)
  {
    inputCode += key;
    showProductCodeOnLCD(inputCode);
  }
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
  unsigned long elapsed = 0;
  unsigned long start = millis();

  digitalWrite(BUZZER_PIN, HIGH);
  digitalWrite(RED_LED, HIGH);

  while (elapsed < BEEP_NEGATIVE_ON_MS)
  {
    updateFans();
    delay(50);
    elapsed = millis() - start;
  }

  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(RED_LED, LOW);
}

void beepLongNegative()
{
  unsigned long elapsed = 0;
  unsigned long start = millis();

  digitalWrite(BUZZER_PIN, HIGH);
  digitalWrite(RED_LED, HIGH);

  while (elapsed < BEEP_LONG_NEGATIVE_ON_MS)
  {
    updateFans();
    delay(50);
    elapsed = millis() - start;
  }

  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(RED_LED, LOW);
}
