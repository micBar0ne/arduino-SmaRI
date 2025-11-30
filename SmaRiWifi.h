#ifndef SMARI_WIFI_H
#define SMARI_WIFI_H

#include <Arduino.h>
#include <WiFi.h>

enum class WifiConnState {
  IDLE,
  CONNECTING,
  CONNECTED,
  FAILED
};

class SmaRiWifi {
public:
  SmaRiWifi(const char* ssid,
            const char* pass,
            bool useStaticIp = false,
            IPAddress local   = IPAddress(),
            IPAddress gateway = IPAddress(),
            IPAddress subnet  = IPAddress(),
            IPAddress dns1    = IPAddress(),
            IPAddress dns2    = IPAddress());

  void begin();
  void loop();

  WifiConnState state() const { return _state; }
  bool isConnected() const;
  String ip() const;
  int rssi() const;
  unsigned long connectedSince() const { return _connectedAt; }

  void setTimeouts(uint32_t connectTimeoutMs, uint32_t retryIntervalMs);

private:
  const char* _ssid;
  const char* _pass;
  bool _useStaticIp;
  IPAddress _local;
  IPAddress _gateway;
  IPAddress _subnet;
  IPAddress _dns1;
  IPAddress _dns2;

  WifiConnState _state = WifiConnState::IDLE;

  uint32_t _connectTimeoutMs = 15000;
  uint32_t _retryIntervalMs  = 5000;

  unsigned long _connectAttemptStart = 0;
  unsigned long _lastRetry           = 0;
  unsigned long _connectedAt         = 0;

  void startAttempt();
};

#endif