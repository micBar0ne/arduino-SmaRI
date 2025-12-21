#pragma once
#include <Arduino.h>
#include <WebServer.h>

class SmaRiWebServer {
public:
  explicit SmaRiWebServer(uint16_t port = 1987);

  void begin();        // start server + register routes
  void end();          // stop server
  void loop();         // handleClient()
  bool isRunning() const;

private:
  void registerRoutes();

  WebServer _server;
  bool _running = false;
};
