/**
 * https://github.com/miguelbalboa/rfid
 */

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN     0
#define SS_PIN      2

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

MFRC522::MIFARE_Key key;

void initRFIDReader(){
  SPI.begin();
  mfrc522.PCD_Init();
  delay(4);
  mfrc522.PCD_DumpVersionToSerial();
}

bool isCardPresent() {
  if ( ! mfrc522.PICC_IsNewCardPresent() || 
       ! mfrc522.PICC_ReadCardSerial()) {
    return false;
  }
  return true;
}

String getCardId(){  
  byte cardId[mfrc522.uid.size];
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    cardId[i] = mfrc522.uid.uidByte[i];
  } 
  char id[mfrc522.uid.size * 8];
  array_to_string(cardId, mfrc522.uid.size, id);
  return id;
}

void array_to_string(byte array[], unsigned int len, char buffer[])
{
    for (unsigned int i = 0; i < len; i++)
    {
        byte nib1 = (array[i] >> 4) & 0x0F;
        byte nib2 = (array[i] >> 0) & 0x0F;
        buffer[i*2+0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
        buffer[i*2+1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
    }
    buffer[len*2] = '\0';
}
