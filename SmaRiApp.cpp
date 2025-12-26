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

static const char* wifiStateToStr(WifiConnState s) {
  switch (s) {
    case WifiConnState::CONNECTED:  return "connected";
    case WifiConnState::CONNECTING: return "connecting";
    case WifiConnState::FAILED:     return "failed";
    case WifiConnState::IDLE:
    default:                        return "idle";
  }
}

static const char* relayIdToStr(RelayId id) {
  switch (id) {
    case RelayId::RELAY_1: return "relay_1";
    case RelayId::RELAY_2: return "relay_2";
    default: return "unknown";
  }
}

String SmaRiApp::buildStatusJson() const {
  const unsigned long now = millis();
  const WifiConnState s = _wifi.state();
  const bool connected = (s == WifiConnState::CONNECTED);

  const int rssi = connected ? _wifi.rssi() : 0;
  const String ipStr = connected ? _wifi.ip() : "";

  const bool relayBusy = _relay.isBusy();
  const String relayActive = relayBusy ? relayIdToStr(_relay.activeRelay()) : "";
  const uint32_t remaining = relayBusy ? _relay.remainingMs(now) : 0;

  String json = "{";
  json += "\"uptime_ms\":" + String(now) + ",";
  json += "\"wifi_state\":\"" + String(wifiStateToStr(s)) + "\",";
  json += "\"ip\":\"" + ipStr + "\",";
  json += "\"rssi\":" + String(rssi) + ",";
  json += "\"relay_busy\":" + String(relayBusy ? "true" : "false") + ",";
  json += "\"relay_active\":\"" + relayActive + "\",";
  json += "\"relay_remaining_ms\":" + String(remaining);
  
  if (_audit.size() > 0) {
    AuditEntry e = _audit.getNewest(0);
    json += ",\"last_action_ms\":" + String(e.atMs);
    json += ",\"last_action_relay\":" + String(e.relayId);
    json += ",\"last_action_ok\":" + String(e.ok ? "true" : "false");
  }

  json += "}";

  return json;
}


void SmaRiApp::setup() {
  _statusLed.begin(STATUS_LED_PIN);
  _statusLed.setMode(LedMode::Connecting);

  _initStart = millis();

  _display.begin();

  _wifi.setTimeouts(WIFI_CONNECT_TIMEOUT_MS, WIFI_RETRY_INTERVAL_MS);
  _wifi.begin();

  _relay.begin();

  _web.setStatusProvider([this]() {
    return this->buildStatusJson();
  });

  _web.setRelayCommandHandler([this](uint8_t id, uint32_t ms, String& error) {
    if (ms == 0) ms = RELAY_DEFAULT_PULSE_MS;  // keep WebServer generic

    RelayId relay;
    switch (id) {
      case 1: relay = RelayId::RELAY_1; break;
      case 2: relay = RelayId::RELAY_2; break;
      default:
        error = "invalid relay id";
        _audit.add(AuditEventType::RELAY_TRIGGER_FAIL, id, ms, false);
        return false;
    }

    const bool ok = _relay.trigger(relay, ms);

    // ✅ audit log lives here (best place)
    _audit.add(ok ? AuditEventType::RELAY_TRIGGER_OK : AuditEventType::RELAY_TRIGGER_FAIL,
               id, ms, ok);

    if (!ok) error = _relay.lastError();
    return ok;
  });

  _web.setLogProvider([this]() {
    return _audit.toJson();  // returns JSON array
  });
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
  
  _relay.loop(now);

  _web.loop();
}
