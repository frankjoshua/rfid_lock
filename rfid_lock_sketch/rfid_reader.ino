/**
 * https://github.com/miguelbalboa/rfid
 */

#include <SPI.h>
#include <MFRC522.h>

#define RFID_LEN  7
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

//String getCardId(){  
//  byte cardId[mfrc522.uid.size];
//  for (byte i = 0; i < mfrc522.uid.size; i++) {
//    cardId[i] = mfrc522.uid.uidByte[i];
//  } 
//  char id[mfrc522.uid.size * 8];
//  array_to_string(cardId, mfrc522.uid.size, id);
//  return id;
//}

String getCardId(){
    // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
    MFRC522::MIFARE_Key key;
    for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  
    //some variables we need
    byte block;
    MFRC522::StatusCode status;

    unsigned char len = 100;
    byte buffer1[101] = {};

    block = 4;

  
    //------------------------------------------- GET FIRST NAME
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 4, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("Authentication failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      mfrc522.PICC_HaltA();
      mfrc522.PCD_StopCrypto1();
      return "AUTH_FAIL";
    }

    
    status = mfrc522.MIFARE_Read(block, buffer1, &len);
    //set terminating character and correct len
    len = RFID_LEN;
    buffer1[len] = 0;
    
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("Reading failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      mfrc522.PICC_HaltA();
      mfrc522.PCD_StopCrypto1();
      return "READ_FAIL";
    }

    //create string from buffer
    String strbuf((char*)buffer1);
  
    //print id
//    Serial.print((char*)buffer1);
//    Serial.print("-*-");
//    Serial.println(strbuf);
//    
//    Serial.print("String size:");
//    Serial.println(strbuf.length());
    
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    return strbuf;
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
