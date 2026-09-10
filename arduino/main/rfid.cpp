#include "rfid.h"

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

  ultimaResetareRFID = millis();
}

void maintainRFIDActiv()
{
  if (millis() - ultimaResetareRFID >= intervalResetRFID)
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
  Serial.print("RFID DEBUG: VersionReg inainte de scanare = 0x");
  if (version < 0x10)
    Serial.print("0");
  Serial.println(version, HEX);

  if (version == 0x00 || version == 0xFF)
  {
    Serial.println("RFID DEBUG: RC522 nu raspunde pe SPI");
    return false;
  }

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
    Serial.print("RFID TEST: card citit, UID = ");
    for (byte i = 0; i < rc.uid.size; i++)
    {
      if (rc.uid.uidByte[i] < 0x10)
        Serial.print("0");
      Serial.print(rc.uid.uidByte[i], HEX);
      if (i + 1 < rc.uid.size)
        Serial.print(":");
    }
    Serial.println();
    delay(500);
    return;
  }

  if (millis() - lastMessage >= 1000)
  {
    lastMessage = millis();
    Serial.println("RFID TEST: modul activ, niciun card detectat");
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
