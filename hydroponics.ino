#include <U8g2lib.h>
#include <stdio.h>

const int LED_PIN = LED_BUILTIN;
const int RELAY_PIN = 7; // D7

const short ICON_STOPWATCH = 0x010d;
const short ICON_DROPLET = 0x0098;

const unsigned long FULL_CYCLE_MS = 3 * 60 * 60 * 1000lu; // 3 hours
const unsigned long FILL_TIME_MS = 45 * 1000lu; // 45 seconds

// store time at the start of each loop so that it's the same for the full iteration
unsigned long elapsedTime;
unsigned long previousTime;

int pumpState;

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C screen(U8G2_R0);
const unsigned char SCREEN_WIDTH = 128;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  screen.begin();

  elapsedTime = 0;
  previousTime = 0;
}

void loop() {
  updateTime();
  checkPump();
  updateScreen();
}

void updateTime() {
  unsigned long currentTime = millis();
  elapsedTime += (unsigned long)(currentTime - previousTime);
  previousTime = currentTime;

  if (elapsedTime >= FULL_CYCLE_MS) {
    elapsedTime -= FULL_CYCLE_MS;
  }
}

void checkPump() {
  if (shouldStartPump()) {
    setPumpState(HIGH);
    return;
  }

  if (shouldStopPump()) {
    setPumpState(LOW);
    return;
  }
}

void updateScreen() {
  if (isPumpOn()) {
    printRemaining(ICON_DROPLET, "Filling", elapsedTime, FILL_TIME_MS);
    return;
  }

  printRemaining(ICON_STOPWATCH, "Idle", elapsedTime - FILL_TIME_MS, FULL_CYCLE_MS - FILL_TIME_MS);
}

void printRemaining(short icon, char* stateLabel, unsigned long msCompleted, unsigned long msTotal) {
  screen.clearBuffer();

  screen.setFont(u8g2_font_open_iconic_all_1x_t);
  screen.drawGlyph(0, 10, icon);

  screen.setFont(u8g2_font_fur11_tf);

  screen.drawStr(10, 11, stateLabel);

  unsigned long msRemaining = msTotal - msCompleted;
  unsigned int secondsRemaining = ceil(msRemaining / 1000.0);

  char formattedRemaining[10];
  formatRemaining(formattedRemaining, secondsRemaining);
  int offset = SCREEN_WIDTH - screen.getStrWidth(formattedRemaining);
  screen.drawStr(offset, 11, formattedRemaining);

  double percentComplete = msCompleted * 1.0 / msTotal;
  unsigned short barLength = percentComplete * (SCREEN_WIDTH - 4);
  screen.drawFrame(0, 16, SCREEN_WIDTH, 16);
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

bool shouldStartPump() {
  return !isPumpOn() && elapsedTime < FILL_TIME_MS;
}

bool shouldStopPump() {
  return isPumpOn() && elapsedTime >= FILL_TIME_MS;
}

void setPumpState(int v) {
  pumpState = v;
  digitalWrite(LED_PIN, pumpState);
  digitalWrite(RELAY_PIN, pumpState);
}

bool isPumpOn() {
  return pumpState == HIGH;
}
