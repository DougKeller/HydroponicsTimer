#include <U8g2lib.h>
#include <stdio.h>
#include "timer.h"
#include "pump.h"

const short ICON_STOPWATCH = 0x010d;
const short ICON_DROPLET = 0x0098;
const short ICON_ERROR = 0x011b;

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C screen(U8G2_R2);
const unsigned char SCREEN_WIDTH = 128;

Timer* fillTimer = nullptr;
Timer* idleTimer = nullptr;
Pump* pump = nullptr;

void setup() {
  unsigned long fullCycleMs = 3 * 60 * 60 * 1000lu; // 4 hours
  unsigned long fillTimeMs = 60 * 1000lu; // 20 seconds

  fillTimer = new Timer(fillTimeMs);
  idleTimer = new Timer(fullCycleMs - fillTimeMs);
  pump = new Pump();

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
    return;
  }

  if (shouldStopPump()) {
    stopPump();
    return;
  }
}

bool shouldStartPump() {
  return idleTimer->isRunning() && idleTimer->isComplete();
}

void startPump() {
  pump->start();
  idleTimer->stop();
  fillTimer->restart();
};

bool shouldStopPump() {
  return fillTimer->isRunning() && fillTimer->isComplete();
}

void stopPump() {
  pump->stop();
  fillTimer->stop();
  idleTimer->restart();
}

void updateScreen() {
  if (fillTimer->isRunning()) {
    drawState(ICON_DROPLET, "Filling", fillTimer);
    return;
  }

  if (idleTimer->isRunning()) {
    drawState(ICON_STOPWATCH, "Idle", idleTimer);  
    return;
  }
}

void drawState(short icon, String stateLabel, Timer* timer) {
  screen.clearBuffer();

  drawStatus(icon, stateLabel);
  drawProgress(timer);
  
  screen.sendBuffer();
}

void drawStatus(short icon, String label) {
  screen.setFont(u8g2_font_open_iconic_all_1x_t);
  screen.drawGlyph(0, 10, icon);

  screen.setFont(u8g2_font_fur11_tf);
  screen.drawStr(10, 11, label.c_str());
}

void drawProgress(Timer* timer) {
  char formattedRemaining[10];
  formatRemaining(formattedRemaining, timer->getSecondsRemaining());
  int offset = SCREEN_WIDTH - screen.getStrWidth(formattedRemaining);
  screen.drawStr(offset, 11, formattedRemaining);

  unsigned short barLength = timer->getPercentComplete() * (SCREEN_WIDTH - 4);
  screen.drawFrame(0, 16, SCREEN_WIDTH, 16);
  screen.drawBox(2, 18, barLength, 12);  
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
