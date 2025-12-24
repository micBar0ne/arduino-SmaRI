#pragma once
#include <Arduino.h>
#include <WebServer.h>
#include <functional>

class SmaRiWebServer {
public:
  explicit SmaRiWebServer(uint16_t port = 1987);

  void begin();        // start server + register routes
  void end();          // stop server
  void loop();         // handleClient()
  bool isRunning() const;

void setStatusProvider(std::function<String()> provider);\

void setRelayCommandHandler(
  std::function<bool(uint8_t relayId, uint32_t durationMs, String& error)>
    handler
);

private:
  void registerRoutes();

  WebServer _server;
  bool _running = false;
  bool requireAuth();

  unsigned int _authFailures = 0;
  unsigned long _lockoutUntilMs = 0;

  std::function<String()> _statusProvider;
  std::function<bool(uint8_t, uint32_t, String&)> _relayHandler;
};
