#ifndef SMARI_APP_H
#define SMARI_APP_H

#include <Arduino.h>
#include "Config.h"
#include "SmaRiWifi.h"
#include "SmaRiDisplay.h"

class SmaRiApp {
public:
  SmaRiApp();

  void setup();
  void loop();

private:
  SmaRiWifi    _wifi;
  SmaRiDisplay _display;

  unsigned long _initStart = 0;
};

#endif
