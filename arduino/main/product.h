#ifndef PRODUCT_H
#define PRODUCT_H

#include "common.h"

#define PRODUCT_LOOKUP_COUNT 4
#define PRODUCT_CODE_LENGTH 2
#define PRODUCT_SERIAL_PREFIX "PROD:"
#define PRODUCT_STOCK_PREFIX "STOCK:"
#define STOCK_SYNC_TOKEN "STOCK:"
#define SERIAL_READ_TERMINATOR '\n'

struct ProductStock
{
  String code;
  int quantity;
};

extern ProductStock products[PRODUCT_LOOKUP_COUNT];

void initializeProductSerial();
void initializeProductLogic();
int findProductIndex(String code);
int getProductQuantity(String code);
bool isProductAvailable(String code);
bool isValidCode(String code);
bool processProductSelection(const String& code);
void registerSuccessfulProductSale(String code);
void updateProductStockFromESP32();

#endif
