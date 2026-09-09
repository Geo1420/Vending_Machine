#include "rfid.h"
#include "display.h"

void initializeRFID()
{
  if (!prepareRFID())
  {
    Serial.println("RFID module failed");
  }
  else
  {
    Serial.println("RFID module ready");
  }
}

// ===== RC522 =====
void resetRFID()
{
  digitalWrite(RSTPIN, LOW);
  delay(50);

  digitalWrite(RSTPIN, HIGH);
  delay(50);

  rc.PCD_Init();
  rc.PCD_SetAntennaGain(MFRC522::RxGain_max);
  delay(50);

  lastRFIDReset = millis();
}

void maintainRFIDActive()
{
  if (millis() - lastRFIDReset >= rfidResetInterval)
  {
    resetRFID();
  }
}

bool verifyRFID()
{
  if (!rc.PICC_IsNewCardPresent())
    return false;

  if (!rc.PICC_ReadCardSerial())
  {
    resetRFID();
    return false;
  }

  for (int i = 0; i < 4; i++)
    readcard[i] = rc.uid.uidByte[i];

  rc.PICC_HaltA();
  rc.PCD_StopCrypto1();
  return true;
}

bool prepareRFID()
{
  rc.PCD_Init();
  rc.PCD_SetAntennaGain(MFRC522::RxGain_max);
  delay(50);

  byte version = rc.PCD_ReadRegister(MFRC522::VersionReg);
  Serial.print("RFID DEBUG: VersionReg before scan = 0x");
  if (version < 0x10)
    Serial.print("0");
  Serial.println(version, HEX);

  if (version == 0x00 || version == 0xFF)
  {
    Serial.println("RFID DEBUG: RC522 does not respond on SPI");
    return false;
  }

  return true;
}

bool processRFIDCard()
{
  if (!verifyRFID())
    return false;

  Serial.print("RFID UID:");
  for (int i = 0; i < 4; i++)
  {
    if (readcard[i] < 0x10)
      Serial.print("0");
    Serial.print(readcard[i], HEX);
    if (i < 3)
      Serial.print(":");
  }
  Serial.println();

  beepPositive();
  return true;
}

void diagnoseRFID()
{
  static unsigned long lastMessage = 0;
  static unsigned long lastVersionCheck = 0;

  if (millis() - lastVersionCheck >= 5000)
  {
    lastVersionCheck = millis();
    byte version = rc.PCD_ReadRegister(MFRC522::VersionReg);
    Serial.print("RFID TEST: VersionReg = 0x");
    if (version < 0x10)
      Serial.print("0");
    Serial.println(version, HEX);
  }

  if (verifyRFID())
  {
    Serial.print("RFID TEST: card read, UID = ");
    for (byte i = 0; i < rc.uid.size; i++)
    {
      if (rc.uid.uidByte[i] < 0x10)
        Serial.print("0");
      Serial.print(rc.uid.uidByte[i], HEX);
      if (i + 1 < rc.uid.size)
        Serial.print(":");
    }
    Serial.println();
    waitWithFanMonitoring(50);
    return;
  }

  if (millis() - lastMessage >= 1000)
  {
    lastMessage = millis();
    Serial.println("RFID TEST: active module, no card detected");
  }
}

int getid()
{
  if (!rc.PICC_IsNewCardPresent())
    return 0;
  if (!rc.PICC_ReadCardSerial())
    return 0;

  for (int i = 0; i < 4; i++)
    readcard[i] = rc.uid.uidByte[i];
  rc.PICC_HaltA();
  return 1;
}
