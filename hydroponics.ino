#include <U8g2lib.h>
#include <stdio.h>

const int LED_PIN = LED_BUILTIN;
const int RELAY_PIN = 7; // D7

const short ICON_STOPWATCH = 0x010d;
const short ICON_DROPLET = 0x0098;

const unsigned long FULL_CYCLE_MS = 3 * 60 * 60 * 1000lu; // 3 hours
const unsigned long FILL_TIME_MS = 45 * 1000lu; // 45 seconds

enum State { STATE_IDLE, STATE_FILLING };

// store time at the start of each loop so that it's the same for the full iteration
unsigned long currentTime;

unsigned long nextPumpStartTime;
unsigned long nextPumpStopTime;
int pumpState;

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C screen(U8G2_R0);

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  screen.begin();

  updateCurrentTime();
  startPump();
}

void loop() {
  updateCurrentTime();
  handleState();
  handleDisplay();
}

void updateCurrentTime() {
  currentTime = millis();
}

void handleState() {
  switch (getState()) {
  case STATE_IDLE:
    handleIdleState();
    break;
  case STATE_FILLING:
    handleFillingState();
    break;
  }
}

void handleDisplay() {
  switch (getState()) {
  case STATE_IDLE:
    printRemaining(ICON_STOPWATCH, "Idle", nextPumpStartTime, FULL_CYCLE_MS - FILL_TIME_MS);
    break;
  case STATE_FILLING:
    printRemaining(ICON_DROPLET, "Filling", nextPumpStopTime, FILL_TIME_MS);
    break;
  }
}

State getState() {
  return getPumpState() == HIGH ? STATE_FILLING : STATE_IDLE;
}

void printRemaining(short icon, char* stateLabel, unsigned long targetTime, unsigned long fullTime) {
  screen.clearBuffer();
  screen.setFont(u8g2_font_open_iconic_all_1x_t);
  screen.drawGlyph(0, 10, icon);

  screen.setFont(u8g2_font_fur11_tf);

  unsigned long msRemaining = targetTime - currentTime;
  unsigned int secondsRemaining = ceil(msRemaining / 1000.0);

  screen.drawStr(10, 11, stateLabel);

  char remaining[10];
  formatRemaining(remaining, secondsRemaining);
  int offset = 128 - screen.getStrWidth(remaining);
  screen.drawStr(offset, 11, remaining);

  double percentComplete = (fullTime - msRemaining) * 1.0 / fullTime;
  unsigned short barLength = percentComplete * 124;
  screen.drawFrame(0, 16, 128, 16);
  screen.drawBox(2, 18, barLength, 12);
  
  screen.sendBuffer();
}

void formatRemaining(char* s, unsigned int seconds) {
  unsigned int hours = seconds / (60 * 60);
  seconds -= hours * 60 * 60;
  unsigned int minutes = seconds / 60;
  seconds -= minutes * 60;

  if (hours > 0) {
    sprintf(s, "%u:%02u:%02u", hours, minutes, seconds);
  } else {
    sprintf(s, "%u:%02u", minutes, seconds);
  }
}

void handleIdleState() {
  bool isTimeToStartPump = currentTime >= nextPumpStartTime;
  if (!isTimeToStartPump) {
    return;
  }

  startPump();
}

void startPump() {
  setPumpState(HIGH);

  nextPumpStopTime = currentTime + FILL_TIME_MS;
}

void handleFillingState() {
  bool isTimeToStopPump = currentTime >= nextPumpStopTime;
  if (!isTimeToStopPump) {
    return;
  }

  stopPump();
}

void stopPump() {
  setPumpState(LOW);

  nextPumpStartTime = currentTime + FULL_CYCLE_MS - FILL_TIME_MS;
}

void setPumpState(int v) {
  pumpState = v;
  digitalWrite(LED_PIN, pumpState);
  digitalWrite(RELAY_PIN, pumpState);
}

int getPumpState() {
  return pumpState;
}
