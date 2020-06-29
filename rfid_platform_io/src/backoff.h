class BackoffTimer {
  public:
    BackoffTimer();
    void setDelay();
    bool isReady();
    void reset();
    int getDelay();
};