#include "status.h"

class Storage {
  public:
    void save(Status* status);
    void restore(Status* status);
  private:
    void writeString(char address, String data);
    String readString(char address);
};