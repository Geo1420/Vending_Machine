#include "product.h"

ProductStock products[PRODUCT_LOOKUP_COUNT] = {
  {"11", 5},
  {"12", 3},
  {"13", 4},
  {"14", 2}
};

int findProductIndex(String code)
{
  for (int i = 0; i < PRODUCT_LOOKUP_COUNT; i++)
    if (code == products[i].code)
      return i;

  return -1;
}

int getProductQuantity(String code)
{
  int idx = findProductIndex(code);
  if (idx == -1)
    return 0;

  return products[idx].quantity;
}

bool isProductAvailable(String code)
{
  return getProductQuantity(code) > 0;
}

bool isValidCode(String code)
{
  return findProductIndex(code) != -1;
}

void registerSuccessfulProductSale(String code)
{
  int idx = findProductIndex(code);
  if (idx == -1)
    return;

  if (products[idx].quantity > 0)
  {
    products[idx].quantity--;
  }

  Serial1.print(PRODUCT_SERIAL_PREFIX);
  Serial1.println(code);

  Serial1.print(PRODUCT_STOCK_PREFIX);
  Serial1.print(products[idx].code);
  Serial1.print(":");
  Serial1.println(products[idx].quantity);

  Serial.print("Produs selectat: ");
  Serial.println(code);
  Serial.print("Stoc ramas pentru cod ");
  Serial.print(code);
  Serial.print(": ");
  Serial.println(products[idx].quantity);
}

void updateProductStockFromESP32()
{
  if (!Serial1.available())
    return;

  String data = Serial1.readStringUntil(SERIAL_READ_TERMINATOR);
  data.trim();

  if (data.startsWith("FAN_THRESHOLD:"))
  {
    String payload = data.substring(strlen("FAN_THRESHOLD:"));
    fanTemperatureLimit = payload.toFloat();

    Serial.print("Prag ventilator sincronizat Arduino: ");
    Serial.println(fanTemperatureLimit);
    return;
  }

  if (!data.startsWith(STOCK_SYNC_TOKEN))
    return;

  String payload = data.substring(strlen(STOCK_SYNC_TOKEN));
  int colonIndex = payload.indexOf(':');

  if (colonIndex < 0)
    return;

  String code = payload.substring(0, colonIndex);
  int quantity = payload.substring(colonIndex + 1).toInt();

  int idx = findProductIndex(code);
  if (idx >= 0)
  {
    products[idx].quantity = quantity;
    Serial.print("Stock sincronizat Arduino pentru ");
    Serial.print(code);
    Serial.print(": ");
    Serial.println(quantity);
  }
}
