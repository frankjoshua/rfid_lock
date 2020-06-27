/**
 * https://github.com/monkeyboard/Wiegand-Protocol-Library-for-Arduino
 */

#include <Wiegand.h>

WIEGAND wg;

void initRFIDReader(){
  wg.begin(10, 9);
}

bool isCardPresent() {
//  if(wg.available()){
//    return true;
//  } else {
//    return false;
//  }
  return false;
}

void setCardId(String &cardId){
//  int cardType = wg.getWiegandType();
//  switch(cardType){
//    case 4:
//    case 8:
//    case 26:
//    case 32:
//      cardId = String(wg.getCode(), HEX);
//      break;
//    default: 
//      cardId = String(cardType);
//      break;
//  }
  cardId = "HI";
}

void arrayToString(byte array[], unsigned int len, char buffer[]){
    for (unsigned int i = 0; i < len; i++){
        byte nib1 = (array[i] >> 4) & 0x0F;
        byte nib2 = (array[i] >> 0) & 0x0F;
        buffer[i*2+0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
        buffer[i*2+1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
    }
    buffer[len*2] = '\0';
}
