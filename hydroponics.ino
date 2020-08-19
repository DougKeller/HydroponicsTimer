
#include <U8g2lib.h>
#include <stdio.h>
#include "timer.h"

const int LED_PIN = LED_BUILTIN;
const int RELAY_PIN = 7; // D7

const short ICON_STOPWATCH = 0x010d;
const short ICON_DROPLET = 0x0098;

const unsigned long FULL_CYCLE_MS = 2 * 60 * 60 * 1000lu; // 2 hours
const unsigned long FILL_TIME_MS = 45 * 1000lu; // 45 seconds

Timer fillTimer = Timer(FILL_TIME_MS);
Timer idleTimer = Timer(FULL_CYCLE_MS - FILL_TIME_MS);

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C screen(U8G2_R0);
const unsigned char SCREEN_WIDTH = 128;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  screen.begin();

  startPump();
}

void loop() {
  checkPump();
  updateScreen();
}

void checkPump() {
  if (shouldStartPump()) {
    startPump();
  }

  if (shouldStopPump()) {
    stopPump();
  }
}

bool shouldStartPump() {
  return idleTimer.isRunning() && idleTimer.isComplete();
}

void startPump() {
  setPumpState(HIGH);
  idleTimer.stop();
  fillTimer.restart();
};

bool shouldStopPump() {
  return fillTimer.isRunning() && fillTimer.isComplete();
}

void stopPump() {
  setPumpState(LOW);
  fillTimer.stop();
  idleTimer.restart();
}

void updateScreen() {
  if (fillTimer.isRunning()) {
    printRemaining(ICON_DROPLET, "Filling", fillTimer);
    return;
  }

  printRemaining(ICON_STOPWATCH, "Idle", idleTimer);
}

void printRemaining(short icon, char* stateLabel, Timer& timer) {
  screen.clearBuffer();

  screen.setFont(u8g2_font_open_iconic_all_1x_t);
  screen.drawGlyph(0, 10, icon);

  screen.setFont(u8g2_font_fur11_tf);
  screen.drawStr(10, 11, stateLabel);

  char formattedRemaining[10];
  formatRemaining(formattedRemaining, timer.getSecondsRemaining());
  int offset = SCREEN_WIDTH - screen.getStrWidth(formattedRemaining);
  screen.drawStr(offset, 11, formattedRemaining);

  unsigned short barLength = timer.getPercentComplete() * (SCREEN_WIDTH - 4);
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

void setPumpState(int v) {
  digitalWrite(LED_PIN, v);
  digitalWrite(RELAY_PIN, v);
}
