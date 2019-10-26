#define LOCK_PIN D8

void initLock(){
  pinMode(LOCK_PIN, OUTPUT);
}

void unlock(){
  digitalWrite(LOCK_PIN, HIGH);
}

void lock(){
  digitalWrite(LOCK_PIN, LOW);
}
