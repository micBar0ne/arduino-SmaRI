#ifndef SMARI_CONFIG_H
#define SMARI_CONFIG_H

#include <Arduino.h>
#include <WiFi.h>

// ---------- WIFI SETTINGS ----------
// const char* WIFI_SSID = "<YOUR-SSID>";
// const char* WIFI_PASS = "<YOUR-PASSWORD>";
static const char* WIFI_SSID = "Vodafone-Barone";
static const char* WIFI_PASS = "Camomilla87";

static const uint32_t WIFI_CONNECT_TIMEOUT_MS = 15000;
static const uint32_t WIFI_RETRY_INTERVAL_MS  = 5000;

// ---- Static IP settings ----
static const bool WIFI_USE_STATIC_IP = true;
static const IPAddress WIFI_LOCAL_IP(192, 168, 1, 87);
static const IPAddress WIFI_GATEWAY(192, 168, 1, 1);
static const IPAddress WIFI_SUBNET (255, 255, 255, 0);
static const IPAddress WIFI_DNS1   (1, 1, 1, 1);
static const IPAddress WIFI_DNS2   (8, 8, 8, 8);

// ---------- UI SETTINGS ----------
static const uint32_t INIT_SCREEN_DURATION_MS = 2000;
static const uint32_t WIFI_SHOW_INFO_TIMEOUT  = 5000;

#endif
