#include "SmaRiDisplay.h"
#include <math.h>

SmaRiDisplay::SmaRiDisplay()
: _u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE)
{}

void SmaRiDisplay::begin() {
  _u8g2.begin();
  _u8g2.setContrast(255);
  _u8g2.setFont(UI_FONT);
}

String SmaRiDisplay::fitToWidth(const String &s, int maxW) {
  if (_u8g2.getStrWidth(s.c_str()) <= maxW) return s;
  String t = s;
  while (t.length() > 0) {
    String cand = t + "…";
    if (_u8g2.getStrWidth(cand.c_str()) <= maxW) return cand;
    t.remove(t.length() - 1);
  }
  return "…";
}

int SmaRiDisplay::rssiToBars(int rssi) {
  if (rssi > -60) return 3;
  if (rssi > -75) return 2;
  if (rssi > -90) return 1;
  return 0;
}

void SmaRiDisplay::drawWifiBars16(int x, int y, int bars) {
  int bx = x + 2, by = y + 14, w = 3, gap = 2;
  for (int i = 0; i < 3; i++) {
    int h = 3 + i * 4;
    if (i < bars) _u8g2.drawBox(bx + i * (w + gap), by - h, w, h);
    else          _u8g2.drawFrame(bx + i * (w + gap), by - h, w, h);
  }
}

void SmaRiDisplay::drawCheck16(int x, int y, int thickness) {
  for (int o = 0; o < thickness; ++o) {
    _u8g2.drawLine(x+3,  y+8+o, x+6,  y+12+o);
    _u8g2.drawLine(x+7,  y+12+o, x+13, y+2+o);
  }
}

void SmaRiDisplay::drawSpinner16(int x, int y) {
  int cx = x + 8, cy = y + 8, r = 6;
  _u8g2.drawCircle(cx, cy, r);
  float a = (millis() % 800) * (2 * PI / 800.0f);
  _u8g2.drawLine(cx, cy,
                 cx + (int)(cos(a)*r),
                 cy + (int)(sin(a)*r));
}

void SmaRiDisplay::drawLoading(int x, int y, unsigned long initStart, uint32_t durationMs) {
  const int loaderHeight = 6;
  const int loaderWidth  = 16;
  _u8g2.drawFrame(x, y, loaderWidth, loaderHeight);

  uint32_t elapsed = millis() - initStart;
  float progress = (float)elapsed / (float)durationMs;
  if (progress > 1.0f) progress = 1.0f;
  uint8_t filledWidth = (uint8_t)(progress * loaderWidth + 0.5f);

  if (filledWidth > 0) {
    _u8g2.drawBox(x, y, filledWidth, loaderHeight);
  }
}

// ---------- PUBLIC RENDER METHODS ----------

void SmaRiDisplay::renderInitScreen(unsigned long initStart, uint32_t initDurationMs) {
  _u8g2.clearBuffer();
  drawLoading(0, 5, initStart, initDurationMs);
  _u8g2.drawStr(_leftIconSpace, _yRow1, "Initializing...");
  _u8g2.sendBuffer();
}

void SmaRiDisplay::renderConnecting() {
  _u8g2.clearBuffer();
  drawSpinner16(0, 0);
  _u8g2.drawStr(_leftIconSpace, _yRow1, "Connecting...");
  _u8g2.sendBuffer();
}

void SmaRiDisplay::renderFailed() {
  _u8g2.clearBuffer();
  _u8g2.drawStr(_leftIconSpace, _yRow1, "WiFi Failed!");
  _u8g2.sendBuffer();
}

void SmaRiDisplay::renderWifiInfo(const String& ssid,
                                  const String& ip,
                                  int rssi) {
  _u8g2.clearBuffer();

  drawCheck16(0, 0);
  drawWifiBars16(128-18, 0, rssiToBars(rssi));
  _u8g2.drawStr(_leftIconSpace, _yRow1, "Connected");

  String ssidInfo = "SSID:" + ssid;
  _u8g2.drawStr(_left, _yRow2, fitToWidth(ssidInfo, 124).c_str());

  String ipInfo = "IP: " + ip;
  _u8g2.drawStr(_left, _yRow3, fitToWidth(ipInfo, 124).c_str());

  char rssiBuf[24];
  snprintf(rssiBuf, sizeof(rssiBuf), "RSSI:%ddBm", rssi);
  _u8g2.drawStr(_left, _yRow4, rssiBuf);

  _u8g2.sendBuffer();
}

void SmaRiDisplay::renderWifiCompact(int rssi) {
  _u8g2.clearBuffer();
  drawCheck16(0, 0);
  drawWifiBars16(128-18, 0, rssiToBars(rssi));
  _u8g2.drawStr(_leftIconSpace, _yRow1, "Online");
  _u8g2.sendBuffer();
}
