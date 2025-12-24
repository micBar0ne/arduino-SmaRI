#pragma once

#ifndef SMARI_CONFIG_H
#define SMARI_CONFIG_H

#include <Arduino.h>
#include <WiFi.h>

#ifndef LED_BUILTIN
  #define LED_BUILTIN 2
#endif

static const int STATUS_LED_PIN = LED_BUILTIN;

// ---------- WIFI SETTINGS ----------
inline constexpr const char* WIFI_SSID = "<YOUR-SSID>";
inline constexpr const char* WIFI_PASS = "<YOUR-PASSWORD>";

inline constexpr const uint32_t WIFI_CONNECT_TIMEOUT_MS = 15000;
inline constexpr const uint32_t WIFI_RETRY_INTERVAL_MS  = 5000;

// ---- Static IP settings ----
inline const bool WIFI_USE_STATIC_IP = true;
inline const IPAddress WIFI_LOCAL_IP(192, 168, 1, 87);
inline const IPAddress WIFI_GATEWAY(192, 168, 1, 1);
inline const IPAddress WIFI_SUBNET (255, 255, 255, 0);
inline const IPAddress WIFI_DNS1   (1, 1, 1, 1);
inline const IPAddress WIFI_DNS2   (8, 8, 8, 8);

// ---------- UI SETTINGS ----------
inline const uint32_t INIT_SCREEN_DURATION_MS = 2000;
inline const uint32_t WIFI_SHOW_INFO_TIMEOUT  = 5000;

// ---------- RELAY SETTINGS ----------
inline constexpr int RELAY1_PIN = 26;
inline constexpr int RELAY2_PIN = 27;

inline constexpr bool RELAY_ACTIVE_LOW = true;

inline constexpr uint32_t RELAY_DEFAULT_PULSE_MS = 800;

#endif
