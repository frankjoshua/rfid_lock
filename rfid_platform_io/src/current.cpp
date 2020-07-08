#include "current.h"
#include "EmonLib.h"

EnergyMonitor emon1;


CurrentSensor::CurrentSensor() {
  emon1.current(A0, 111.1);
}

float CurrentSensor::getCurrentInAmps() {
  double Irms = emon1.calcIrms(1480);
  return Irms * 230.0;
}