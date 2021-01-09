#include "backoff.h"

unsigned long delayUntil = 0;
const int defaultDelay = 250;
unsigned int lastDelay = defaultDelay;

BackoffTimer::BackoffTimer()
{
}

bool BackoffTimer::isReady(unsigned long uptimeInMillis)
{
  return uptimeInMillis > delayUntil;
}

void BackoffTimer::setDelay(unsigned long uptimeInMillis)
{
  lastDelay = lastDelay * 2;
  delayUntil = uptimeInMillis + lastDelay;
}

void BackoffTimer::reset()
{
  delayUntil = 0;
  lastDelay = defaultDelay;
}

int BackoffTimer::getDelay()
{
  return lastDelay;
}