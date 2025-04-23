#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <U8g2lib.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Update.h>
#include <EEPROM.h>
#include "index.h"

#define EEPROM_SIZE 128
#define EEPROM_SSID_ADDR 0
#define EEPROM_PASS_ADDR 64
#define SEALEVELPRESSURE_HPA (1013.25)

// Forward Declaration
void server_init();
Adafruit_BME280 bme;
U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

String html = String(MAIN_page);

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
  /*float temperature = bme.readTemperature();
  float pressure = bme.readPressure() / 100.0F;
  float altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  float humidity = bme.readHumidity();
  
  html.replace(%Temp%, temperature);
  html.replace(%Pressure%, pressure);
  html.replace(%Altitude%, altitude);
  html.replace(%Humidity%, humidity);*/
  //html.replace("%CHART_JS%", CHART_JS);
  server.send_P(200, "text/html", html.c_str());

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