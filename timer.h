class Timer {
  unsigned long duration;
  unsigned long previousTime;
  unsigned long elapsed;
  bool stopped;

  public:

  Timer(unsigned long duration) {
    this->duration = duration;
    stop();
  }

  void restart() {
    elapsed = 0;
    previousTime = millis();
    stopped = false;
  }

  void stop() {
    stopped = true;
  }

  bool isRunning() {
    return !stopped;
  }

  double getPercentComplete() {
    if (isComplete()) {
      return 100.0;
    }

    return elapsed * 1.0 / duration;
  }

  unsigned int getSecondsRemaining() {
    if (isComplete()) {
      return 0;
    }
    
    return (unsigned int) ceil((duration - elapsed) / 1000.0);
  }

  bool isComplete() {
    unsigned long currentTime = millis();
    elapsed += (unsigned long)(currentTime - previousTime);
    previousTime = currentTime;
    
    return elapsed >= duration;
  }
};
