#include "environment.h"

float readTemperatureFourSamples()
{
  float samples[4];
  int validSamples = 0;
  float total = 0.0f;

  for (int i = 0; i < 4; i++)
  {
    float sample = dht.readTemperature();
    if (!isnan(sample))
    {
      samples[validSamples] = sample;
      total += sample;
      validSamples++;
    }

    if (i < 3)
    {
      delay(50);
    }
  }

  if (validSamples == 0)
  {
    return NAN;
  }

  return total / validSamples;
}

void updateFans()
{
  float temperature = readTemperatureFourSamples();
  bool fansOn = !isnan(temperature) && temperature > fanTemperatureLimit;

  digitalWrite(FAN_PIN, fansOn ? HIGH : LOW);
  digitalWrite(FAN2_PIN, fansOn ? HIGH : LOW);
}

void waitWithFanMonitoring(unsigned long duration)
{
  unsigned long start = millis();
  while (millis() - start < duration)
  {
    updateFans();
    delay(50);
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
