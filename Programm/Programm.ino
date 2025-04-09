#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <U8g2lib.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Update.h>
#include <EEPROM.h>

#define EEPROM_SIZE 128
#define EEPROM_SSID_ADDR 0
#define EEPROM_PASS_ADDR 64
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme;
U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

const char ssid[10] = "valerik";
const char password[2] = "";
String WLANSSID = "";
String WLANPassword = "";
WebServer server(80);

bool isAP = true;

unsigned long delayTime;
unsigned long lastSwitch = 0;
bool showSensorData = true;

void saveWiFiCredentials(const String& ssid, const String& pass) {
  EEPROM.begin(EEPROM_SIZE);
  for (int i = 0; i < 32; ++i) {
    EEPROM.write(EEPROM_SSID_ADDR + i, i < ssid.length() ? ssid[i] : 0);
    EEPROM.write(EEPROM_PASS_ADDR + i, i < pass.length() ? pass[i] : 0);
  }
  EEPROM.commit();
  EEPROM.end();
}

void loadWiFiCredentials(String& ssid, String& pass) {
  EEPROM.begin(EEPROM_SIZE);
  char ssidBuff[33], passBuff[33];
  for (int i = 0; i < 32; ++i) {
    ssidBuff[i] = EEPROM.read(EEPROM_SSID_ADDR + i);
    passBuff[i] = EEPROM.read(EEPROM_PASS_ADDR + i);
  }
  ssidBuff[32] = 0;
  passBuff[32] = 0;
  ssid = String(ssidBuff);
  pass = String(passBuff);
  EEPROM.end();
}

void handleRoot() {
  float temperature = bme.readTemperature();
  float pressure = bme.readPressure() / 100.0F;
  float altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  float humidity = bme.readHumidity();

  String html = "<!DOCTYPE html><html><head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>BME280 Sensor</title>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; background: #f5f7fa; margin: 0; padding: 0; }";
  html += ".container { max-width: 1200px; margin: 0 auto; padding: 20px; }";
  html += "header { background: #4361ee; color: white; padding: 20px 0; text-align: center; border-radius: 0 0 10px 10px; }";
  html += ".sensor-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 15px; margin: 20px 0; }";
  html += ".card { background: white; border-radius: 8px; padding: 15px; box-shadow: 0 2px 5px rgba(0,0,0,0.1); }";
  html += ".card h3 { color: #4361ee; margin-top: 0; }";
  html += ".value { font-size: 24px; font-weight: bold; margin: 10px 0; }";
  html += ".unit { color: #666; }";
  html += "form { background: white; padding: 20px; border-radius: 8px; box-shadow: 0 2px 5px rgba(0,0,0,0.1); margin-bottom: 20px; }";
  html += "input[type='text'], input[type='password'], input[type='file'] { width: 100%; padding: 10px; margin: 5px 0 15px; border: 1px solid #ddd; border-radius: 4px; }";
  html += "button { background: #4361ee; color: white; border: none; padding: 10px 15px; border-radius: 4px; cursor: pointer; }";
  html += "button:hover { background: #3a56d4; }";
  html += "@media (max-width: 600px) { .sensor-grid { grid-template-columns: 1fr; } }";
  // NEU: GitHub-Corner CSS hinzufügen
  html += ".github-corner { position: absolute; top: 0; right: 0; border: 0; }";
  html += ".github-corner:hover .octo-arm { animation: octocat-wave 560ms ease-in-out; }";
  html += "@keyframes octocat-wave { 0%,100% { transform: rotate(0); } 20%,60% { transform: rotate(-25deg); } 40%,80% { transform: rotate(10deg); } }";
  html += "@media (max-width:500px) { .github-corner:hover .octo-arm { animation: none; } .github-corner .octo-arm { animation: octocat-wave 560ms ease-in-out; } }";
  html += "</style></head>";
  html += "<body><div class='container'>";
  html += "<header><h1>BME280 Sensor</h1><p>Echtzeit-Daten</p></header>";




  // GitHub Corner (rechte obere Ecke)
  html += "<a href='https://github.com/ValleST07/Umgebungssensor' class='github-corner' target='_blank' aria-label='View source on GitHub'>";
  html += "<svg width='80' height='80' viewBox='0 0 250 250' style='fill:#151513; color:#fff;'>";
  html += "<path d='M0,0 L115,115 L130,115 L142,142 L250,250 L250,0 Z'></path>";
  html += "<path d='M128.3,109.0 C113.8,99.7 119.0,89.6 119.0,89.6 C122.0,82.7 120.5,78.6 120.5,78.6 C119.2,72.0 123.4,76.3 123.4,76.3 C127.3,80.9 125.5,87.3 125.5,87.3 C122.9,97.6 130.6,101.9 134.4,103.2' fill='currentColor' style='transform-origin: 130px 106px;' class='octo-arm'></path>";
  html += "<path d='M115.0,115.0 C114.9,115.1 118.7,116.5 119.8,115.4 L133.7,101.6 C136.9,99.2 139.9,98.4 142.2,98.6 C133.8,88.0 127.5,74.4 143.8,58.0 C148.5,53.4 154.0,51.2 159.7,51.0 C160.3,49.4 163.2,43.6 171.4,40.1 C171.4,40.1 176.1,42.5 178.8,56.2 C183.1,58.6 187.2,61.8 190.9,65.4 C194.5,69.0 197.7,73.2 200.1,77.6 C213.8,80.2 216.3,84.9 216.3,84.9 C212.7,93.1 206.9,96.0 205.4,96.6 C205.1,102.4 203.0,107.8 198.3,112.5 C181.9,128.9 168.3,122.5 157.7,114.1 C157.9,116.9 156.7,120.9 152.7,124.9 L141.0,136.5 C139.8,137.7 141.6,141.9 141.8,141.8 Z' fill='currentColor' class='octo-body'></path>";
  html += "</svg>";
  html += "</a>";

  // Sensordaten-Karten
  html += "<div class='sensor-grid'>";
  html += "<div class='card'><h3>Temperatur</h3><div class='value' id='temp'>" + String(temperature, 1) + "</div><div class='unit'>°C</div></div>";
  html += "<div class='card'><h3>Luftfeuchtigkeit</h3><div class='value' id='hum'>" + String(humidity, 1) + "</div><div class='unit'>%</div></div>";
  html += "<div class='card'><h3>Druck</h3><div class='value' id='press'>" + String(pressure, 1) + "</div><div class='unit'>hPa</div></div>";
  html += "<div class='card'><h3>Höhe</h3><div class='value' id='alt'>" + String(altitude, 1) + "</div><div class='unit'>m</div></div>";
  html += "</div>";

  // WLAN-Konfiguration
  html += "<form method='POST' action='/config'>";
  html += "<h2>WLAN-Konfiguration</h2>";
  html += "<label for='ssid'>SSID:</label>";
  html += "<input type='text' name='ssid' value='" + WLANSSID + "' placeholder='WLAN-Name'>";
  html += "<label for='password'>Passwort:</label>";
  html += "<input type='password' name='password' value='" + WLANPassword + "' placeholder='WLAN-Passwort'>";
  html += "<button type='submit'>Einstellungen speichern</button>";
  html += "<p style='font-size: 12px; color: #666;'>Leer lassen für AP-Modus</p>";
  html += "</form>";

  // Firmware-Update
  html += "<form method='POST' action='/update' enctype='multipart/form-data'>";
  html += "<h2>Firmware-Update</h2>";
  html += "<input type='file' name='update'>";
  html += "<button type='submit'>Hochladen & Installieren</button>";
  html += "</form>";

  // JavaScript für Live-Updates
  html += "<script>";
  html += "setInterval(() => {";
  html += "fetch('/data').then(r => r.json()).then(d => {";
  html += "document.getElementById('temp').innerText = d.temp.toFixed(1);";
  html += "document.getElementById('hum').innerText = d.hum.toFixed(1);";
  html += "document.getElementById('press').innerText = d.press.toFixed(1);";
  html += "document.getElementById('alt').innerText = d.alt.toFixed(1);";
  html += "});";
  html += "}, 2000);";
  html += "</script>";

  // Optional: GitHub-Link auch in der Fußzeile
  html += "<footer style='text-align:center;margin-top:30px;color:#666;font-size:14px;'>";
  html += "<a href='https://github.com/ValleST07/Umgebungssensor' target='_blank' style='color:#4361ee;text-decoration:none;'>";
  html += "View project on GitHub";
  html += "</a></footer>";

  html += "</div></body></html>";

  server.send(200, "text/html", html);
}
void handleConfig() {
  if (server.method() == HTTP_POST) {
    WLANSSID = server.arg("ssid");
    WLANPassword = server.arg("password");
    Serial.println("Neues WLAN:");
    Serial.println(WLANSSID);
    Serial.println(WLANPassword);

    saveWiFiCredentials(WLANSSID, WLANPassword);
    // Stoppe Webserver, bevor WiFi-Modus geändert wird
    server.stop();

    // Wechsel vom Access Point zum Station-Modus
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_STA);

    WiFi.begin(WLANSSID.c_str(), WLANPassword.c_str());
    Serial.println("Verbinde mit neuem WLAN...");

    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 30000) {
      delay(500);
      Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\n✅ Verbindung erfolgreich!");
      Serial.print("IP: ");
      Serial.println(WiFi.localIP());
      isAP = false;
    } else {
      Serial.println("\n❌ Verbindung fehlgeschlagen.");
      // Optional: wieder Access Point starten
      WiFi.mode(WIFI_AP);
      WiFi.softAP(ssid, password);
      WiFi.softAPConfig(IPAddress(1, 1, 1, 1), IPAddress(1, 1, 1, 1), IPAddress(255, 255, 255, 0));
      isAP = true;
    }

    // Server neu starten nach Moduswechsel
    server_init();
  } else {
    server.send(405, "text/plain", "Method Not Allowed");
  }
}
void handleData() {
  String json = "{";
  json += "\"temp\":" + String(bme.readTemperature()) + ",";
  json += "\"press\":" + String(bme.readPressure() / 100.0F) + ",";
  json += "\"alt\":" + String(bme.readAltitude(SEALEVELPRESSURE_HPA)) + ",";
  json += "\"hum\":" + String(bme.readHumidity());
  json += "}";
  server.send(200, "application/json", json);
}
void handleUpdate() {
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    Serial.printf("Update: %s\n", upload.filename.c_str());
    if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
      Serial.printf("Update Begin Error\n");
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
      Serial.printf("Update Write Error\n");
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (Update.end(true)) {
      Serial.printf("Update Success\n");
    } else {
      Serial.printf("Update Failed\n");
    }
  }
  server.sendHeader("Connection", "close");
  server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
  delay(1000);
  ESP.restart();
}
void server_init() {
  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.on(
    "/update", HTTP_POST, []() {
      server.send(200, "text/plain", "OK");
    },
    handleUpdate);
  server.on("/config", HTTP_POST, handleConfig);  // Handle the Wi-Fi configuration form submission
  server.begin();
  Serial.println("HTTP server started");
}
void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println(F("BME280 test"));

  display.begin();

  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1) delay(10);
  }

  // Lade gespeicherte SSID & Passwort
  loadWiFiCredentials(WLANSSID, WLANPassword);
  Serial.println("Geladene WLAN-Daten:");
  Serial.println(WLANSSID);
  Serial.println(WLANPassword);

  if (WLANSSID.length() > 0) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WLANSSID.c_str(), WLANPassword.c_str());
    Serial.println("Versuche gespeicherte WLAN-Verbindung...");

    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
      delay(500);
      Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\n✅ Verbunden mit gespeichertem WLAN");
      Serial.print("IP: ");
      Serial.println(WiFi.localIP());
      isAP = false;
    }
  }

  // Falls nicht verbunden → Access Point starten
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);
    WiFi.softAPConfig(IPAddress(1, 1, 1, 1), IPAddress(1, 1, 1, 1), IPAddress(255, 255, 255, 0));
    Serial.println("\nAccess Point gestartet");
    Serial.print("IP: ");
    Serial.println(WiFi.softAPIP());
    Serial.println(WiFi.softAPSSID());
    isAP = true;
  }

  server_init();

  delayTime = 1000;
  lastSwitch = millis();
}

void loop() {
  server.handleClient();

  if (millis() - lastSwitch > (showSensorData ? 30000 : 5000)) {
    showSensorData = !showSensorData;
    lastSwitch = millis();
  }

  display.clearBuffer();
  display.setFont(u8g2_font_ncenB08_tr);

  if (showSensorData) {
    float temperature = bme.readTemperature();
    float pressure = bme.readPressure() / 100.0F;
    float altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
    float humidity = bme.readHumidity();

    display.setCursor(10, 10);
    display.print("Temp: ");
    display.print(temperature, 1);
    display.print(" °C");

    display.setCursor(10, 22);
    display.print("Pressure: ");
    display.print(pressure, 1);
    display.print(" hPa");

    display.setCursor(10, 34);
    display.print("Altitude: ");
    display.print(altitude, 1);
    display.print(" m");

    display.setCursor(10, 46);
    display.print("Humidity: ");
    display.print(humidity, 1);
    display.print(" %");

    display.setFont(u8g2_font_5x7_tr);  // Kleine Schrift für Fußzeile
    display.setCursor(40, 62);
    display.print("(c) VS&ET");

  } else {
    display.setFont(u8g2_font_ncenB08_tr);

    display.setCursor(30, 20);
    if (isAP) display.print("AP-Mode");
    else display.print("ST-Mode");

    display.setCursor(10, 34);
    if (isAP) {
      display.print("SSID: ");
      display.print(WiFi.softAPSSID());
    }

    display.setCursor(10, 46);
    display.print("IP: ");
    if (isAP) display.print(WiFi.softAPIP());
    else display.print(WiFi.localIP());

    display.setFont(u8g2_font_5x7_tr);
    display.setCursor(40, 62);
    display.print("(c) VS&ET");
  }

  display.sendBuffer();

  delay(delayTime);
}