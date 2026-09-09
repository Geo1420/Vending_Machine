#ifndef RFID_H
#define RFID_H

#include "common.h"

void initializeRFID();
void resetRFID();
void maintainRFIDActive();
bool verifyRFID();
bool prepareRFID();
void diagnoseRFID();
bool processRFIDCard();
int getid();

#endif
