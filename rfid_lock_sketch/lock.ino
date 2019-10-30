#define LOCK_PIN D8
#define LOCK_INPUT 10

#define LOCK_STATE LOW

void initLock(){
  pinMode(LOCK_PIN, OUTPUT);
  pinMode(LOCK_INPUT, INPUT);
  lock();
}

void unlock(){
  digitalWrite(LOCK_PIN, LOCK_STATE);
}

void lock(){
  digitalWrite(LOCK_PIN, !LOCK_STATE);
}

boolean isLockEngaged(){
  return digitalRead(LOCK_INPUT) == LOCK_STATE;
}
