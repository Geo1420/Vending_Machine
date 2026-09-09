#include "common.h"
#include "display.h"
#include "environment.h"
#include "platform.h"
#include "product.h"
#include "rfid.h"

String inputCode = "";

void setup()
{
  Serial.begin(115200);

  SPI.begin();
  Wire.begin();

  initializeDisplay();
  initializeEnvironment();
  initializePlatform();
  initializeRFID();
  initializeProductLogic();

  resetLCD();
  Serial.println("Vending Arduino sketch started");
}

void loop()
{
  updateFans();
  maintainRFIDActive();

  if (Serial1.available())
  {
    updateProductStockFromESP32();
  }

  processRFIDCard();
  processKeypadInput(inputCode);
}
