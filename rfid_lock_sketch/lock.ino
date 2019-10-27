#define LOCK_PIN D8
#define LOCK_INPUT 10

void initLock(){
  pinMode(LOCK_PIN, OUTPUT);
  pinMode(LOCK_INPUT, INPUT);
}

void unlock(){
  digitalWrite(LOCK_PIN, HIGH);
}

void lock(){
  digitalWrite(LOCK_PIN, LOW);
}

boolean isLockEngaged(){
  return digitalRead(LOCK_INPUT);
}
