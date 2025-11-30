#include "SmaRiApp.h"

SmaRiApp::SmaRiApp()
: _wifi(WIFI_SSID,
        WIFI_PASS,
        WIFI_USE_STATIC_IP,
        WIFI_LOCAL_IP,
        WIFI_GATEWAY,
        WIFI_SUBNET,
        WIFI_DNS1,
        WIFI_DNS2)
{}

void SmaRiApp::setup() {
  _initStart = millis();

  _display.begin();

  _wifi.setTimeouts(WIFI_CONNECT_TIMEOUT_MS, WIFI_RETRY_INTERVAL_MS);
  _wifi.begin();
}

void SmaRiApp::loop() {
  _wifi.loop();
  unsigned long now = millis();

  if (now - _initStart < INIT_SCREEN_DURATION_MS) {
    _display.renderInitScreen(_initStart, INIT_SCREEN_DURATION_MS);
    return;
  }

  WifiConnState s = _wifi.state();

  switch (s) {
    case WifiConnState::CONNECTING:
      _display.renderConnecting();
      break;

    case WifiConnState::CONNECTED: {
      unsigned long connectedFor = now - _wifi.connectedSince();
      int rssi = _wifi.rssi();

      if (connectedFor < WIFI_SHOW_INFO_TIMEOUT) {
        _display.renderWifiInfo(String(WIFI_SSID), _wifi.ip(), rssi);
      } else {
        _display.renderWifiCompact(rssi);
      }
      break;
    }

    case WifiConnState::FAILED:
      _display.renderFailed();
      break;

    case WifiConnState::IDLE:
    default:
      _display.renderConnecting();
      break;
  }
}
