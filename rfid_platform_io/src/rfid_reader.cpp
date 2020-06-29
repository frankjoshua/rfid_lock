#include "rfid_reader.h"

WIEGAND wg = WIEGAND();

RfidReader::RfidReader() {
  wg.begin(12, 14);
}

bool RfidReader::isCardAvailable() {
  return wg.available();
}

String RfidReader::getCardId() {
  int cardType = wg.getWiegandType();
  switch(cardType){
    case 4:
    case 8:
    case 26:
    case 32:
      return String(wg.getCode(), HEX);
      break;
    default: 
      return String(cardType);
      break;
  }
}