#pragma once

#include <Arduino.h>

enum class LedMode {
  Off,
  Connected,    // 1 short blink every 5 seconds
  Connecting,   // fast blink (pairing)
  NoService     // DDNS not responding
};

class StatusLed {
public:
  StatusLed() = default;

  void begin(uint8_t pin) {
    _pin = pin;
    pinMode(_pin, OUTPUT);
    _state = LOW;
    digitalWrite(_pin, _state);
  }

  void setMode(LedMode mode) {
    if (mode != _mode) {
      _mode = mode;
      _lastToggle = millis();
      _state = LOW;
      digitalWrite(_pin, _state);
    }
  }

  void update() {
    if (_pin == 255) return;

    unsigned long now = millis();

    switch (_mode) {
      case LedMode::Off:
        _state = LOW;
        digitalWrite(_pin, _state);
        break;

      case LedMode::Connected:
        updateConnected(now);
        break;

      case LedMode::Connecting:
        updateConnecting(now);
        break;
        
      case LedMode::NoService:
        updateNoService(now);
        break;
    }
  }

private:
  void updateConnected(unsigned long now) {
    const unsigned long PERIOD = 5000;
    const unsigned long ON_TIME = 100;

    unsigned long t = now % PERIOD;
    bool shouldBeOn = (t < ON_TIME);

    if (shouldBeOn != _state) {
      _state = shouldBeOn ? HIGH : LOW;
      digitalWrite(_pin, _state);
    }
  }

  void updateConnecting(unsigned long now) {
    const unsigned long FAST_INTERVAL = 150;

    if (now - _lastToggle >= FAST_INTERVAL) {
      _lastToggle = now;
      _state = !_state;
      digitalWrite(_pin, _state);
    }
  }

  void updateNoService(unsigned long now) {
    const unsigned long FAST_INTERVAL = 1000;

    if (now - _lastToggle >= FAST_INTERVAL) {
      _lastToggle = now;
      _state = !_state;
      digitalWrite(_pin, _state);
    }
  }

  uint8_t _pin = 255;
  LedMode _mode = LedMode::Off;
  bool _state = LOW;
  unsigned long _lastToggle = 0;
};
