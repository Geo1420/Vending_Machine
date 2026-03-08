#include <SPI.h>
#include <MFRC522.h>

// creating mfrc522 instance
#define RSTPIN 9
#define SSPIN 53

MFRC522 rc(SSPIN, RSTPIN);

int readsuccess;

// UID-urile cardurilor autorizate
byte defcard[][4] = {
  {0xCB,0xB3,0x10,0x06},
  {0x32,0xD7,0x0F,0x0B}
};

int N = 2;
byte readcard[4];

void setup() {

  Serial.begin(9600);      // monitor serial
  Serial1.begin(9600);     // comunicare ESP32

  SPI.begin();
  rc.PCD_Init();
  rc.PCD_DumpVersionToSerial();

  pinMode(6, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  Serial.println("Scan Access Card");
}

void loop() {

  readsuccess = getid();

  if (readsuccess) {

    //Serial1.println("READ_OK");  // anunta ESP32 ca s-a citit un card

    int match = 0;

    for (int i = 0; i < N; i++) {

      if (!memcmp(readcard, defcard[i], 4)) {
        match++;
      }
    }

    if (match) {

      Serial.println("CARD AUTHORISED");
      Serial1.println("AUTH");   // trimite verdict ESP32

      digitalWrite(6, HIGH);
      delay(2000);
      digitalWrite(6, LOW);

    } else {

      Serial.println("CARD NOT AUTHORISED");
      Serial1.println("DENIED"); // trimite verdict ESP32

      digitalWrite(5, HIGH);
      delay(2000);
      digitalWrite(5, LOW);
    }

    delay(500);
  }
}

// functie pentru citirea UID
int getid() {

  if (!rc.PICC_IsNewCardPresent()) {
    return 0;
  }

  if (!rc.PICC_ReadCardSerial()) {
    return 0;
  }

  Serial.println("THE UID OF THE SCANNED CARD IS:");

  for (int i = 0; i < 4; i++) {

    readcard[i] = rc.uid.uidByte[i];
    Serial.print(readcard[i], HEX);
    Serial.print(" ");
  }

  Serial.println("");

  rc.PICC_HaltA();

  return 1;
}