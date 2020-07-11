#include <Arduino.h>
#include "status.h"

class Display {
  public:
    Display();
    void initDisplay();
    void println(String msg);
    void print(Status* status);
};