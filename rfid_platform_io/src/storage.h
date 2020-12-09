#include "status.h"

class Storage
{
public:
  void save(Status *status);
  void restore(Status *status);

private:
  void writeString(int address, String data);
  String readString(int address);
};