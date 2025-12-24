#include "SmaRIApp.h"

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

bool _relayTestDone = false;

void SmaRiApp::setup() {
  _statusLed.begin(STATUS_LED_PIN);
  _statusLed.setMode(LedMode::Connecting);

  _initStart = millis();

  _display.begin();

  _wifi.setTimeouts(WIFI_CONNECT_TIMEOUT_MS, WIFI_RETRY_INTERVAL_MS);
  _wifi.begin();

  _relay.begin();
}

void SmaRiApp::loop() {
  _wifi.loop();
  unsigned long now = millis();
  _statusLed.update(); 
  if (now - _initStart < INIT_SCREEN_DURATION_MS) {
    _display.renderInitScreen(_initStart, INIT_SCREEN_DURATION_MS);
    
    return;
  }

  WifiConnState s = _wifi.state();

  switch (s) {
    case WifiConnState::CONNECTING:
      _display.renderConnecting();
      _statusLed.setMode(LedMode::Connecting);
      break;

    case WifiConnState::CONNECTED: {
      unsigned long connectedFor = now - _wifi.connectedSince();
      int rssi = _wifi.rssi();

      if (!_relayTestDone) {
        _relayTestDone = true;
        _relay.trigger(RelayId::RELAY_1, 800);
      }
      
      _statusLed.setMode(LedMode::Connected);

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
      _statusLed.setMode(LedMode::Connecting);
      break;
  }

  
  if (s == WifiConnState::CONNECTED && !_web.isRunning()) {
    _web.begin();
  } else if (s !=  WifiConnState::CONNECTED && _web.isRunning()) {
    _web.end();
  }

  _web.loop();

  _relay.loop(now);
}
