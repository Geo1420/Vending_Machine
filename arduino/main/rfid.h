#ifndef RFID_H
#define RFID_H

#include "common.h"

void resetRFID();
void maintainRFIDActiv();
bool verifyRFID();
bool prepareRFID();
void diagnoseRFID();
int getid();

#endif
