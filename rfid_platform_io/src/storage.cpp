#include "storage.h"
#include <EEPROM.h>

#define MEMORY_ADDRESS 0

void Storage::save(Status *status)
{
  writeString(MEMORY_ADDRESS, status->assetTag);
  writeString(MEMORY_ADDRESS + 255, status->secretKey);
  writeString(MEMORY_ADDRESS + 255 + 255, String(status->unlockDuration));
}

void Storage::restore(Status *status)
{
  String assetTag = readString(MEMORY_ADDRESS);
  // Ver long variables mean that the value was never saved
  if (assetTag.length() < 64)
  {
    status->assetTag = assetTag;
    String secretKey = readString(MEMORY_ADDRESS + 255);
    if (secretKey.length() < 64)
    {
      status->secretKey = secretKey;
      String unlockDuration = readString(MEMORY_ADDRESS + 255 + 255);
      if (unlockDuration.length() < 64)
      {
        status->unlockDuration = unlockDuration.toInt();
      }
    }
  }
}

void Storage::writeString(char address, String data)
{
  EEPROM.begin(512);
  int _size = data.length();
  int i;
  for (i = 0; i < _size; i++)
  {
    EEPROM.write(address + i, data[i]);
  }
  EEPROM.write(address + _size, '\0'); //Add termination null character for String Data
  EEPROM.commit();
  EEPROM.end();
}

String Storage::readString(char address)
{
  EEPROM.begin(512);
  int maxSize = 100;
  char data[maxSize]; //Max 100 Bytes
  int len = 0;
  unsigned char k;
  k = EEPROM.read(address);
  //Read until null character
  while (k != '\0' && len < maxSize)
  {
    k = EEPROM.read(address + len);
    data[len] = k;
    len++;
  }
  data[len] = '\0';
  EEPROM.commit();
  EEPROM.end();
  return String(data);
}