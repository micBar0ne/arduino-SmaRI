#ifndef SMARI_DISPLAY_H
#define SMARI_DISPLAY_H

#include <Arduino.h>
#include <U8g2lib.h>

#define UI_FONT u8g2_font_6x10_tf

class SmaRiDisplay {
public:
  SmaRiDisplay();

  void begin();
  void renderInitScreen(unsigned long initStart, uint32_t initDurationMs);

  void renderConnecting();
  void renderFailed();

  void renderWifiInfo(const String& ssid,
                      const String& ip,
                      int rssi);

  void renderWifiCompact(int rssi);

private:
  U8G2_SSD1306_128X64_NONAME_F_HW_I2C _u8g2;

  const int _left          = 2;
  const int _leftIconSpace = 18;
  const int _lineH         = 13;
  const int _yRow1         = 12;
  const int _yRow2         = _yRow1 + 10 + _lineH;
  const int _yRow3         = _yRow2 + _lineH;
  const int _yRow4         = _yRow3 + _lineH;

  String fitToWidth(const String &s, int maxW);
  void drawWifiBars16(int x, int y, int bars);
  void drawCheck16(int x, int y, int thickness = 3);
  void drawSpinner16(int x, int y);
  void drawLoading(int x, int y, unsigned long initStart, uint32_t durationMs);
  int rssiToBars(int rssi);
};

#endif
