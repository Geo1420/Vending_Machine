#ifndef DISPLAY_H
#define DISPLAY_H

#include "common.h"

void initializeDisplay();
void resetLCD();
void showProductCodeOnLCD(const String& code);
void processKeypadInput(String& inputCode);
void beepPositive();
void beepNegative();
void beepLongNegative();

#endif
