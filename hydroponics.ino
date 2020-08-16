#include <U8g2lib.h>
#include <stdio.h>

const int ledPin = LED_BUILTIN;
const int relayPin = 7; // D7

const short stopwatchIcon = 0x010d;
const short rainingIcon = 0x00f1;

unsigned long fullCycleMilliseconds = 2 * 60 * 60 * 1000lu; // 2 hours
unsigned long fillTimeMilliseconds = 50 * 1000lu; // 50 seconds

unsigned long nextPumpStartTime = 0;
unsigned long nextPumpStopTime = 0;

enum State { STATE_IDLE, STATE_FILLING };
enum State currentState;

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C screen(U8G2_R0);

void setup() {
  currentState = STATE_IDLE;

  pinMode(ledPin, OUTPUT);
  pinMode(relayPin, OUTPUT);

  screen.begin();
}

void setPumpState(int pumpSignal) {
  digitalWrite(ledPin, pumpSignal);
  digitalWrite(relayPin, pumpSignal);
}

void handleIdleState() {
  unsigned long currentTime = millis();
  bool isTimeToStartPump = currentTime >= nextPumpStartTime;
  if (!isTimeToStartPump) {
    return;
  }
  
  currentState = STATE_FILLING;
  nextPumpStartTime = currentTime + fullCycleMilliseconds;
  nextPumpStopTime = currentTime + fillTimeMilliseconds;
  setPumpState(HIGH);
}

void handleFillingState() {
  unsigned long currentTime = millis();
  bool isTimeToStopPump = currentTime >= nextPumpStopTime;
  if (!isTimeToStopPump) {
    return;
  }
  
  currentState = STATE_IDLE;
  setPumpState(LOW);
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

void printRemaining(short icon, char* stateLabel, unsigned long targetTime, unsigned long fullTime) {
  screen.clearBuffer();
  screen.setFont(u8g2_font_open_iconic_all_1x_t);
  screen.drawGlyph(0, 10, icon);

  screen.setFont(u8g2_font_fur11_tf);

  unsigned long currentTime = millis();
  unsigned long msRemaining = targetTime - currentTime;
  unsigned int secondsRemaining = ceil(msRemaining / 1000.0);

  screen.drawStr(11, 11, stateLabel);

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

void loop() {
  switch (currentState) {
  case STATE_IDLE:
    printRemaining(stopwatchIcon, "Idle", nextPumpStartTime, fullCycleMilliseconds - fillTimeMilliseconds);
    handleIdleState();
    break;
  case STATE_FILLING:
    printRemaining(rainingIcon, "Filling", nextPumpStopTime, fillTimeMilliseconds);
    handleFillingState();
    break;
  }
}
