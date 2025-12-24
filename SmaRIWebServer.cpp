#include "SmaRiWebServer.h"
#include "Config.h"

SmaRiWebServer::SmaRiWebServer(uint16_t port)
: _server(port) {}

void SmaRiWebServer::registerRoutes() {
  _server.on("/health", HTTP_GET, [this]() {
    if (!requireAuth()) return;

    _server.send(200, "text/plain", "OK");
  });

  _server.on("/api/status", HTTP_GET, [this]() {
    if (!requireAuth()) return;

    if (_statusProvider) {
      const String json = _statusProvider();
      _server.send(200, "application/json", json);
    } else {
      _server.send(503, "application/json", "{\"error\":\"status provider not set\"}");
    }
  });

  _server.on("/api/relay", HTTP_GET, [this]() {
    if (!requireAuth()) return;

    if (!_relayHandler) {
      _server.send(503, "application/json", "{\"error\":\"handler not set\"}");
      return;
    }

    if (!_server.hasArg("id")) {
      _server.send(400, "application/json", "{\"error\":\"missing id\"}");
      return;
    }

    uint8_t id = _server.arg("id").toInt();
    uint32_t ms = _server.hasArg("ms") ? _server.arg("ms").toInt() : 0;

    String error;
    bool ok = _relayHandler(id, ms, error);

    if (ok) {
      _server.send(200, "application/json", "{\"result\":\"ok\"}");
    } else {
      _server.send(409, "application/json",
                  "{\"error\":\"" + error + "\"}");
    }
  });

  _server.onNotFound([this]() {
    if (!requireAuth()) return;

    _server.send(404, "text/plain", "Not Found");
  });

  _server.on("/", HTTP_GET, [this]() {
    if (!requireAuth()) return;
    _server.send(200, "text/html", R"HTML(
      <!doctype html>
      <html lang="en">
      <head>
        <meta charset="utf-8"/>
        <meta name="viewport" content="width=device-width, initial-scale=1"/>
        <title>SmaRI</title>
        <style>
          :root{
            --bg: #0b0f14;
            --card: #0f1622;
            --border: #223045;
            --text: #e6edf3;
            --muted: #9aa7b6;
            --good: #3ddc84;
            --bad: #ff5d5d;
            --btn: #1f2a3a;
            --btnHover: #2a3a52;
            --btnActive: #334a6b;
            --shadow: rgba(0,0,0,.35);
            --ring: rgba(125, 211, 252, .25);
          }

          * { box-sizing: border-box; }
          body {
            margin: 0;
            padding: 18px;
            font-family: system-ui, -apple-system, Segoe UI, Roboto, Arial, sans-serif;
            background: radial-gradient(1200px 600px at 20% -10%, #172033 0%, transparent 60%),
                        radial-gradient(900px 500px at 90% 0%, #1a2a3f 0%, transparent 55%),
                        var(--bg);
            color: var(--text);
            height: 100vh;
          }

          .wrap { max-width: 540px; margin: 0 auto; }

          .card {
            background: linear-gradient(180deg, rgba(255,255,255,.02), transparent 60%), var(--card);
            border: 1px solid var(--border);
            border-radius: 16px;
            padding: 16px;
            box-shadow: 0 10px 30px var(--shadow);
          }

          h2 { margin: 0 0 12px 0; font-size: 20px; letter-spacing: .2px; }
          .sub { color: var(--muted); font-size: 12px; margin-top: -6px; margin-bottom: 12px; }

          .row {
            display: flex;
            justify-content: space-between;
            gap: 12px;
            padding: 10px 12px;
            border-radius: 12px;
            border: 1px solid rgba(255,255,255,.04);
            background: rgba(255,255,255,.02);
            margin: 8px 0;
          }

          .label { color: var(--muted); font-size: 13px; }
          .value { font-weight: 650; font-size: 13px; }

          .pill {
            display: inline-flex;
            align-items: center;
            gap: 8px;
            padding: 6px 10px;
            border-radius: 999px;
            border: 1px solid rgba(255,255,255,.08);
            background: rgba(255,255,255,.03);
          }

          .ok { color: var(--good); }
          .bad { color: var(--bad); }

          .grid {
            display: grid;
            grid-template-columns: 1fr;
            gap: 10px;
            margin-top: 12px;
          }

          button {
            width: 100%;
            padding: 14px 14px;
            font-size: 16px;
            font-weight: 650;
            border-radius: 14px;
            border: 1px solid rgba(255,255,255,.10);
            background: linear-gradient(180deg, rgba(255,255,255,.05), rgba(255,255,255,.02));
            color: var(--text);
            cursor: pointer;
            transition: transform .06s ease, background .15s ease, border-color .15s ease;
            outline: none;
          }

          button:hover { background: rgba(255,255,255,.06); border-color: rgba(255,255,255,.18); }
          button:active { transform: translateY(1px); background: rgba(255,255,255,.08); }
          button:focus { box-shadow: 0 0 0 4px var(--ring); }

          button:disabled {
            opacity: .45;
            cursor: not-allowed;
            transform: none;
          }

          .msg {
            margin-top: 12px;
            color: var(--muted);
            font-size: 12px;
            min-height: 18px;
          }

          .spinner {
            display: inline-block;
            width: 11px;
            height: 11px;
            border: 2px solid rgba(255,255,255,.25);
            border-top-color: rgba(255,255,255,.85);
            border-radius: 50%;
            animation: spin 0.8s linear infinite;
            vertical-align: middle;
          }

          @keyframes spin { to { transform: rotate(360deg);} }
        </style>
      </head>
      <body>
        <div class="wrap">
          <div class="card">
            <h2>SmaRI Control</h2>
            <div class="sub">Local web panel</div>

            <div class="row">
              <div class="label">Wi-Fi</div>
              <div id="wifi" class="value pill">—</div>
            </div>

            <div class="row">
              <div class="label">IP</div>
              <div id="ip" class="value">—</div>
            </div>

            <div class="row">
              <div class="label">RSSI</div>
              <div id="rssi" class="value">—</div>
            </div>

            <div class="row">
              <div class="label">Relay</div>
              <div id="relay" class="value pill">—</div>
            </div>

            <div class="grid">
              <button id="btn1" onclick="triggerRelay(1)">Garage</button>
              <button id="btn2" onclick="triggerRelay(2)">Pedestrian</button>
            </div>

            <div class="msg" id="msg"></div>
          </div>
        </div>

      <script>
      let busy = false;

      function setBusy(isBusy, remainingMs) {
        busy = isBusy;

        document.getElementById('btn1').disabled = busy;
        document.getElementById('btn2').disabled = busy;

        const relayEl = document.getElementById('relay');
        if (busy) {
          relayEl.innerHTML = 'BUSY <span class="spinner"></span> ' + (remainingMs || 0) + ' ms';
        } else {
          relayEl.textContent = 'IDLE';
        }
      }

      function setWifiPill(state) {
        const el = document.getElementById('wifi');
        const s = (state || '—').toString();
        el.textContent = s;

        // add color hint
        el.classList.remove('ok', 'bad');
        if (s === 'connected') el.classList.add('ok');
        if (s === 'failed') el.classList.add('bad');
      }

      async function refreshStatus() {
        try {
          const r = await fetch('/api/status', { cache: 'no-store' });
          const s = await r.json();

          setWifiPill(s.wifi_state);
          document.getElementById('ip').textContent = s.ip || '—';
          document.getElementById('rssi').textContent = (s.rssi !== undefined) ? s.rssi : '—';

          setBusy(!!s.relay_busy, s.relay_remaining_ms);

          document.getElementById('msg').textContent = '';
        } catch (e) {
          document.getElementById('msg').innerHTML = '<span class="bad">Status error</span>';
        }
      }

      async function triggerRelay(id) {
        if (busy) return;

        document.getElementById('msg').innerHTML = 'Sending... <span class="spinner"></span>';
        try {
          const r = await fetch('/api/relay?id=' + id, { cache: 'no-store' });
          const t = await r.text();

          if (r.ok) {
            document.getElementById('msg').innerHTML = '<span class="ok">OK</span>';
          } else {
            document.getElementById('msg').innerHTML = '<span class="bad">Error:</span> ' + t;
          }
        } catch (e) {
          document.getElementById('msg').innerHTML = '<span class="bad">Request failed</span>';
        }

        refreshStatus();
      }

      refreshStatus();
      setInterval(refreshStatus, 2000);
      </script>
      </body>
      </html>
      )HTML");
    });
}

void SmaRiWebServer::begin() {
  if (_running) return;

  registerRoutes();
  _server.begin();
  _running = true;
}

void SmaRiWebServer::end() {
  if (!_running) return;

  _server.stop();           // available on ESP32 WebServer
  _running = false;
}

void SmaRiWebServer::loop() {
  if (!_running) return;
  _server.handleClient();
}

bool SmaRiWebServer::isRunning() const {
  return _running;
}

void SmaRiWebServer::setStatusProvider(std::function<String()> provider) {
  _statusProvider = provider;
}

void SmaRiWebServer::setRelayCommandHandler(
  std::function<bool(uint8_t, uint32_t, String&)> handler
) {
  _relayHandler = handler;
}

bool SmaRiWebServer::requireAuth() {
  if (!WEB_AUTH_ENABLED) return true;

  const unsigned long now = millis();

  // Lockout active
  if (_lockoutUntilMs > now) {
    _server.send(403, "text/plain", "Temporarily locked");
    return false;
  }

  // Auth success
  if (_server.authenticate(WEB_USER, WEB_PASS)) {
    _authFailures = 0;
    return true;
  }

  // Auth failure
  _authFailures++;

  // Progressive delay
  if (_authFailures >= 4 && _authFailures < 7) {
    delay(2000); // short penalty
  }

  // Hard lockout
  if (_authFailures >= 7) {
    _lockoutUntilMs = now + (5UL * 60UL * 1000UL); // 5 minutes
  }

  _server.requestAuthentication();
  return false;
}
