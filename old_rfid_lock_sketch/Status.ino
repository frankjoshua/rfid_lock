
Status status;
String authCardId = "xxxx";

void setMessage(String message){
  status.message = message;
  displayStatus(status);
}

bool isUnlocked(){
  return status.state == Unlocked;
}

void setLocked(){
  status.state = Locked;
  status.timeTillLock = 0;
}

long lastLoop = 0;
long getTimeElapsed(){
  long timeElapsed = millis() - lastLoop;
  lastLoop = millis();
  return timeElapsed;
}

void updateTimeTillLock(long timeElapsed){
  status.timeTillLock -= timeElapsed;
}

boolean isTimeToLock(){
  return status.timeTillLock <= 0;
}

long nextStateUpdate = 0;
boolean shouldUpdateState(){
  if(millis() > nextStateUpdate){
    nextStateUpdate = millis() + 50;
    return true;
  } else {
    return false;
  }
}

void updateLockAndDisplay(){
  displayStatus(status);

  if(isUnlocked()){
    unlock();
  } else {
    lock();
  }
}

void updateCardId(){
  setCardId(status.cardId);
}

String currentCardId(){
  return status.cardId;
}
 
void setUnlocked(){
  status.state = Unlocked;
  status.timeTillLock = 10500;
}

bool isCardAuthorized(){
  return status.cardId == authCardId;
}
