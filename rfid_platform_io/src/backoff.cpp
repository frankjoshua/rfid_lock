#include "backoff.h"
#include <Arduino.h>

unsigned long delayUntil = 0;
const int defaultDelay = 250;
int lastDelay = defaultDelay;

BackoffTimer::BackoffTimer(){

}

bool BackoffTimer::isReady() {
  return millis() > delayUntil;
}

void BackoffTimer::setDelay() {
  lastDelay *= 2;
  delayUntil = millis() + lastDelay;
}

void BackoffTimer::reset() {
  delayUntil = 0;
  lastDelay = defaultDelay;
}

int BackoffTimer::getDelay() {
  return lastDelay;
}