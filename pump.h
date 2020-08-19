class Pump {
  const int LED_PIN = LED_BUILTIN;
  const int RELAY_PIN = 7; // D7

  public:

  Pump() {
    pinMode(LED_PIN, OUTPUT);
    pinMode(RELAY_PIN, OUTPUT);
    stop();
  }

  void start() {
    setState(HIGH);
  }

  void stop() {
    setState(LOW);
  }

  private:

  void setState(int v) {
    digitalWrite(LED_PIN, v);
    digitalWrite(RELAY_PIN, v);
  }
};
