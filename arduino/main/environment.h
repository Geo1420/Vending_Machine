#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "common.h"

float readTemperatureFourSamples();
void updateFans();
void waitWithFanMonitoring(unsigned long duration);
void enableSteppers();
void disableSteppers();

#endif
