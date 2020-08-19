class Timer {
  unsigned long duration;
  unsigned long previousTime;
  unsigned long elapsed;
  bool stopped;

  public:

  Timer(unsigned long duration) {
    this->duration = duration;
    stop();
    reset();
  }

  void restart() {
    reset();
    start();
  }

  void start() {
    stopped = false;
  }

  void reset() {
    elapsed = 0;
    previousTime = millis();
  }

  void stop() {
    stopped = true;
  }

  bool isRunning() {
    return !stopped;
  }

  bool isComplete() {
    if (isRunning()) {
      elapsed += (unsigned long)(millis() - previousTime);  
    }
    
    return elapsed >= duration;
  }

  double getPercentComplete() {
    if (isComplete()) {
      return 100.0;
    }

    return elapsed * 1.0 / duration;
  }

  unsigned int getSecondsRemaining() {
    return ceil((duration - elapsed) / 1000.0);
  }
};
