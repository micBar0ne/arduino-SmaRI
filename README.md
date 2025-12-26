
# SmaRI – Smart Remote Intercom

SmaRI is an ESP32-based smart remote intercom / relay controller designed to be **securely accessible over the internet** via DDNS and HTTP, with a clean modular architecture and a simple web UI.

The project is built with **maintainability, safety, and long‑term reliability** in mind.

---

## ✨ Features

- ESP32-based controller
- Non-blocking relay control
- Web UI (mobile-friendly, dark mode)
- HTTP JSON API
- HTTP Basic Authentication
- Optional API Token authentication
- Audit log (ring buffer, fixed memory usage)
- Static IP support
- DDNS-ready (router-based)
- Secrets isolated from repository
- OOP-style modular architecture

---

## 🧱 Project Architecture

The project is structured to keep responsibilities clearly separated:

```
SmaRI/
├── SmaRI.ino                # Arduino entry point
├── SmaRiApp.{h,cpp}         # Application orchestration
├── SmaRiWifi.{h,cpp}        # Wi-Fi connection & state
├── SmaRiWebServer.{h,cpp}   # HTTP server & routing
├── SmaRiRelayController.{h,cpp} # Relay timing & GPIO control
├── SmaRiAuditLog.{h,cpp}    # Fixed-size audit log (ring buffer)
├── SmaRiDisplay.{h,cpp}     # OLED UI
├── SmaRiLed.{h,cpp}         # Status LED
├── Config.h                 # Non-secret configuration
├── Secrets.h.example        # Example secrets (committed)
└── Secrets.h                # Real secrets (NOT committed)
```

---

## 🔐 Security Model

### Authentication
- **Web UI**: HTTP Basic Auth
- **API endpoints**: API token OR Basic Auth

### Protection
- No credentials committed to GitHub
- Fixed relay duration enforced by firmware
- Audit logging for relay actions
- Non-blocking logic (no `delay()`)

⚠️ **Important**: Basic Auth over HTTP is not encrypted.  
For internet exposure, use **VPN or HTTPS reverse proxy**.

---

## 🔑 Secrets Handling

Secrets are **never committed**.

### Setup
1. Copy:
   ```
   Secrets.h.example → Secrets.h
   ```
2. Fill in your credentials.

### `.gitignore`
```
Secrets.h
```

### Example `Secrets.h.example`
```cpp
#define SMARI_WIFI_SSID "YOUR_SSID"
#define SMARI_WIFI_PASS "YOUR_PASSWORD"

#define SMARI_WEB_USER "admin"
#define SMARI_WEB_PASS "change-me"

#define SMARI_API_TOKEN "change-me-long-random"
```

---

## 🌐 Remote Access (DDNS)

SmaRI does **not implement DDNS in firmware**.

DDNS must be handled by:
- Your router
- A NAS / home server
- A PC running a DDNS client

### Typical setup
- Public DNS: `http://yourname.ddns.net`
- Router forwards WAN port → ESP32 local IP (port 80)

Example:
```
WAN TCP 1987 → 192.168.1.87:80
```

Then access:
```
http://yourname.ddns.net:1987/
```

---

## 📡 HTTP API

### Health
```
GET /health
```

### Status
```
GET /api/status
```

Returns JSON:
```json
{
  "uptime_ms": 123456,
  "wifi_state": "connected",
  "ip": "192.168.1.87",
  "rssi": -55,
  "relay_busy": false,
  "relay_remaining_ms": 0
}
```

### Relay Trigger
```
GET /api/relay?id=1
```

- Relay duration is **enforced by firmware**
- Client-provided duration is ignored or clamped

### Audit Log
```
GET /api/log
```

Returns a JSON array (most recent first).

---

## 🧠 Relay Logic

- Non-blocking timing using `millis()`
- Fixed maximum duration (safety)
- Busy state prevents overlapping activations
- Relay controller never blocks the main loop

---

## 🖥️ Web UI

- Embedded HTML (no external assets)
- Dark mode
- Auto-refresh status
- Buttons disabled only while relay is busy
- Works on mobile and desktop

---

## 🧪 Stability & Reliability

- No dynamic memory growth
- Fixed-size audit log
- No blocking delays
- Designed for 24/7 uptime

---

## 🚀 Future Enhancements (optional)

- OTA firmware updates
- Persistent configuration (NVS)
- mDNS (`smari.local`)
- HTTPS via reverse proxy
- VPN-only access (recommended)

---

## 📜 License

MIT License

---

## 👤 Author

Created by **micBar0ne**  
Project: **SmaRI – Smart Remote Intercom**
