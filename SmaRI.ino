// Arduino
// Project name: SmaRI
// Created by: micBar0ne

#include <WiFi.h>
#include <U8g2lib.h>

#define UI_FONT u8g2_font_6x10_tf

// OLED: SSD1306 128x64 I2C
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

// ---------- USER SETTINGS ----------
const char* WIFI_SSID = "<YOUR-SSID>";
const char* WIFI_PASS = "<YOUR-PASSWORD>";
const uint32_t CONNECT_TIMEOUT_MS = 15000;
const uint32_t RETRY_INTERVAL_MS = 5000;
// ----------------------------------

// ---- Static IP settings ----
const bool USE_STATIC_IP = true;
IPAddress LOCAL_IP(192, 168, 1, 87);
IPAddress GATEWAY(192, 168, 1, 1);
IPAddress SUBNET(255, 255, 255, 0);
IPAddress DNS1(1, 1, 1, 1);
IPAddress DNS2(8, 8, 8, 8);
// ----------------------------------

// ---------- STATE MACHINE ----------
enum class WifiUiState {
  INIT,
  CONNECTING,
  CONNECTED,
  FAILED,
  DISCONNECTED,
  RECONNECTING,
  SHOW_INFO
};
// ----------------------------------

// ---------- WIFI VARIABLES ------------
WifiUiState uiState = WifiUiState::INIT;
String ipStr = "";
int rssi = 0;
unsigned long connectStart = 0;
unsigned long lastRetry = 0;
unsigned long connectedAt = 0;
// ----------------------------------

// ---------- WIFI CONTROL (polling, no events) ----------
void beginWifiTry() {
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(false);

  connectStart = millis();
  uiState = (uiState == WifiUiState::FAILED) ? WifiUiState::RECONNECTING : WifiUiState::CONNECTING;

  WiFi.disconnect(false, false);

  if (USE_STATIC_IP) {
    bool ok = WiFi.config(LOCAL_IP, GATEWAY, SUBNET, DNS1, DNS2);
  }

  WiFi.begin(WIFI_SSID, WIFI_PASS);
}
// ----------------------------------

// ------------- LINE MANAGE DATA WIFI INFO -------------
  // --- Show connection info duration ---
  const long SHOW_INFO_TIMEOUT = 5000;

  // --- layout constants (tuned for 128x64) ---
  const int left   = 2;
  const int lineH  = 13;

  // Row Y baselines (small font ~10px tall)
  const int yRow1 = 10;        // row 1 (top)
  const int yRow2   = yRow1 + 10 + lineH;  // row 2
  const int yRow3   = yRow2   + lineH;  // row 3
  const int yRow4   = yRow3   + lineH;  // row 4 (optional)
// ------------------------------------

String fitToWidth(const String &s, int maxW) {
  // returns s or a shortened version ending with ‘…’ to fit maxW
  if (u8g2.getStrWidth(s.c_str()) <= maxW) return s;
  String t = s;
  while (t.length() > 0) {
    String cand = t + "…";
    if (u8g2.getStrWidth(cand.c_str()) <= maxW) return cand;
    t.remove(t.length() - 1);
  }
  return "…";
}

void setup() {
  u8g2.begin();
  u8g2.setContrast(255);

  u8g2.clearBuffer();
  u8g2.setFont(UI_FONT);
  u8g2.drawStr(left, yRow1, "Initiliazing...");
  u8g2.sendBuffer();
}

void loop() {
  wl_status_t s = WiFi.status();
  if (millis() > connectStart + 2000) {
    u8g2.clearBuffer();
    switch (uiState) {
      case WifiUiState::CONNECTING:
      case WifiUiState::RECONNECTING:
        u8g2.drawStr(left, yRow1, "Connecting...");
        if (s == WL_CONNECTED) {
          uiState = WifiUiState::SHOW_INFO;
          ipStr = WiFi.localIP().toString();
          rssi = WiFi.RSSI();
          connectedAt = millis();
        } else if (millis() - connectStart > CONNECT_TIMEOUT_MS) {
          uiState = WifiUiState::FAILED;
          lastRetry = millis();
        }
        break;

      case WifiUiState::CONNECTED:
        if (s != WL_CONNECTED) {
          uiState = WifiUiState::RECONNECTING;
          connectStart = millis();
        } else {
          u8g2.drawStr(left, yRow1, "Connected");
          static unsigned long lastRssi = 0;
          if (millis() - lastRssi > 2000) {
            rssi = WiFi.RSSI();
            lastRssi = millis();
          }
        }
        break;

      case WifiUiState::FAILED:
        u8g2.drawStr(left, yRow1, "Failed!");
        if (millis() - lastRetry > RETRY_INTERVAL_MS) {
          beginWifiTry();
        }
        break;
      case WifiUiState::SHOW_INFO:
      {
          u8g2.drawStr(left, yRow1, "Connected");

          String SSID_INFO = "SSID:" + String(WIFI_SSID);
          u8g2.drawStr(left, yRow2, fitToWidth(SSID_INFO, 124).c_str());

          String IP_INFO = "IP: " + ipStr;
          u8g2.drawStr(left, yRow3, fitToWidth(IP_INFO, 124).c_str());

          char rssiBuf[24];
          snprintf(rssiBuf, sizeof(rssiBuf), "RSSI:%ddBm", rssi);
          u8g2.drawStr(left, yRow4, rssiBuf);

          if (millis() - connectedAt > SHOW_INFO_TIMEOUT) {
            uiState = WifiUiState::CONNECTED;
          }
        
          break;
      }
      case WifiUiState::INIT:
      case WifiUiState::DISCONNECTED:
      default:
        u8g2.drawStr(left, yRow1, "Connecting...");
        if (s != WL_CONNECTED) {
          beginWifiTry();
        } else {
          uiState = WifiUiState::CONNECTED;
        }
        break;
    }
  }

  u8g2.sendBuffer();
}