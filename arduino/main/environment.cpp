#include "environment.h"

void initializeEnvironment()
{
  dht.begin();

  pinMode(EN1, OUTPUT);
  pinMode(EN2, OUTPUT);

  digitalWrite(EN1, HIGH);
  digitalWrite(EN2, HIGH);
}

void updateFans()
{
  float temperature = dht.readTemperature();
  bool fansOn = !isnan(temperature) && temperature > FAN_TEMPERATURE_LIMIT;

  digitalWrite(FAN_PIN, fansOn ? HIGH : LOW);
  digitalWrite(FAN2_PIN, fansOn ? HIGH : LOW);
}

void waitWithFanMonitoring(unsigned long duration)
{
  unsigned long start = millis();
  while (millis() - start < duration)
  {
    updateFans();

    unsigned long elapsed = millis() - start;
    unsigned long remaining = duration - elapsed;
    unsigned long slice = remaining > 50 ? 50 : remaining;
    delay(slice);
  }
}

void enableSteppers()
{
  digitalWrite(EN1, LOW);
  digitalWrite(EN2, LOW);
}

void disableSteppers()
{
  digitalWrite(EN1, HIGH);
  digitalWrite(EN2, HIGH);
}
