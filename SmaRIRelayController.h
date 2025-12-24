#pragma once
#include <Arduino.h>

enum class RelayId : uint8_t {
  RELAY_1 = 1,
  RELAY_2 = 2
};

class SmaRiRelayController {
public:
  void begin();
  void loop(unsigned long now);

  // returns true if trigger accepted
  bool trigger(RelayId id, uint32_t durationMs);

  bool isBusy() const { return _busy; }
  RelayId activeRelay() const { return _activeRelay; }
  uint32_t remainingMs(unsigned long now) const;

  const char* lastError() const { return _lastError; }
  RelayId lastRelay() const { return _lastRelay; }
  unsigned long lastActionAtMs() const { return _lastActionAtMs; }

private:
  void setRelayPin(RelayId id, bool on);
  int pinFor(RelayId id) const;

  bool _busy = false;
  RelayId _activeRelay = RelayId::RELAY_1;
  unsigned long _pulseEndAtMs = 0;

  RelayId _lastRelay = RelayId::RELAY_1;
  unsigned long _lastActionAtMs = 0;
  const char* _lastError = nullptr;
};
