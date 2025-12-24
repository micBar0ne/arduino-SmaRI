#include "SmaRiRelayController.h"
#include "Config.h"

static bool relayOnLevel() {
  return RELAY_ACTIVE_LOW ? LOW : HIGH;
}
static bool relayOffLevel() {
  return RELAY_ACTIVE_LOW ? HIGH : LOW;
}

void SmaRiRelayController::begin() {
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);

  // Ensure off at boot
  digitalWrite(RELAY1_PIN, relayOffLevel());
  digitalWrite(RELAY2_PIN, relayOffLevel());

  _busy = false;
  _lastError = nullptr;
}

void SmaRiRelayController::loop(unsigned long now) {
  if (!_busy) return;

  // stop pulse when time elapsed
  if ((long)(now - _pulseEndAtMs) >= 0) {
    setRelayPin(_activeRelay, false);
    _busy = false;
  }
}

bool SmaRiRelayController::trigger(RelayId id, uint32_t durationMs) {
  _lastError = nullptr;

  if (_busy) {
    _lastError = "relay busy";
    return false;
  }
  if (durationMs == 0 || durationMs > 10000) { // sane upper bound
    _lastError = "invalid duration";
    return false;
  }

  const int pin = pinFor(id);
  if (pin < 0) {
    _lastError = "invalid relay id";
    return false;
  }

  _activeRelay = id;
  _lastRelay = id;
  _lastActionAtMs = millis();

  setRelayPin(id, true);
  _pulseEndAtMs = _lastActionAtMs + durationMs;
  _busy = true;
  return true;
}

uint32_t SmaRiRelayController::remainingMs(unsigned long now) const {
  if (!_busy) return 0;
  if ((long)(_pulseEndAtMs - now) <= 0) return 0;
  return (uint32_t)(_pulseEndAtMs - now);
}

int SmaRiRelayController::pinFor(RelayId id) const {
  switch (id) {
    case RelayId::RELAY_1: return RELAY1_PIN;
    case RelayId::RELAY_2: return RELAY2_PIN;
    default: return -1;
  }
}

void SmaRiRelayController::setRelayPin(RelayId id, bool on) {
  const int pin = pinFor(id);
  if (pin < 0) return;
  digitalWrite(pin, on ? relayOnLevel() : relayOffLevel());
}
