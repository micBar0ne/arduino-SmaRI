#ifndef SMARI_APP_H
#define SMARI_APP_H

#include <Arduino.h>
#include "Config.h"
#include "SmaRIWifi.h"
#include "SmaRIDisplay.h"
#include "SmaRILed.h"
#include "SmaRIWebServer.h"

class SmaRiApp {
public:
  SmaRiApp();

  void setup();
  void loop();

private:
  StatusLed    _statusLed;
  SmaRiWifi    _wifi;
  SmaRiDisplay _display;
  SmaRiWebServer _web;

  unsigned long _initStart = 0;
};

#endif
