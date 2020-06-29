#include <Wiegand.h>

class RfidReader {

public:
  RfidReader();
  bool isCardAvailable();
  String getCardId();

private:
  WIEGAND wg;
};