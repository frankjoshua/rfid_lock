#ifndef BACKOFFTIMER_H
#define BACKOFFTIMER_H

class BackoffTimer
{
public:
  BackoffTimer();
  void setDelay(unsigned long uptimeInMillis);
  bool isReady(unsigned long uptimeInMillis);
  void reset();
  int getDelay();
};

#endif