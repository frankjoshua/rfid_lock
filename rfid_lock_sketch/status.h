#ifndef status_h
#define status_h

enum State {
  Locked,
  Unlocked
};

struct Status {
  String message;
  State state;
  String cardId;
  long timeTillLock;
};

#endif