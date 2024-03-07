/*------------------------

 * Pin layout should be as follows:
 * Signal     Pin              Pin               Pin
 *            Arduino Uno      Arduino Mega      MFRC522 board
 * ------------------------------------------------------------
 * Reset      9                5                 RST
 * SPI SS     10               53                SDA
 * SPI MOSI   11               51                MOSI
 * SPI MISO   12               50                MISO
 * SPI SCK    13               52                SCK
 * voltage 3.3v

 */

#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

int elock = 4;

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.

void setup()
{
  Serial.begin(9600); // Initialize serial communications with the PC
  pinMode(elock, OUTPUT);
  digitalWrite(elock, LOW);
  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card
  Serial.println("Scan a MIFARE Classic PICC to demonstrate Value Blocks.");
}

void loop()
{

  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++)
  {
    key.keyByte[i] = 0xFF;
  }
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  // Now a card is selected. The UID and SAK is in mfrc522.uid.

  // Dump UID
  Serial.print("Card UID:");
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], DEC);
  }
  Serial.println();

  // Dump PICC type
  byte piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  //    Serial.print("PICC type: ");
  // Serial.println(mfrc522.PICC_GetTypeName(piccType));
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI && piccType != MFRC522::PICC_TYPE_MIFARE_1K && piccType != MFRC522::PICC_TYPE_MIFARE_4K)
  {
    // Serial.println("This sample only works with MIFARE Classic cards.");
    return;
  }
  // defining Cards here

  if ((mfrc522.uid.uidByte[0] == 243) && (mfrc522.uid.uidByte[1] == 36) && (mfrc522.uid.uidByte[2] == 177) && (mfrc522.uid.uidByte[3] == 207))
  {
    Serial.println("Tag A detected");
  }
  else if ((mfrc522.uid.uidByte[0] == 218) && (mfrc522.uid.uidByte[1] == 136) && (mfrc522.uid.uidByte[2] == 125) && (mfrc522.uid.uidByte[3] == 122))
  {
    Serial.println("Tag B detected");
  }
  else if ((mfrc522.uid.uidByte[0] == 218) && (mfrc522.uid.uidByte[1] == 94) && (mfrc522.uid.uidByte[2] == 219) && (mfrc522.uid.uidByte[3] == 122))
  {
    Serial.println("Tag C detected");
  }
  else
    Serial.println("unregistered user");

  delay(1000);
}