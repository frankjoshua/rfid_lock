#include <Arduino.h>

class Display {
  public:
    Display();
    void initDisplay();
    void println(String msg);
};