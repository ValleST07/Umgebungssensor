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
U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

const char ssid [10] = "valerik";
const char password [2] = "";
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
    
    String html = "<html><head><title>BME280 Sensor Data</title>";
    html += "<style>body{font-family:Arial, sans-serif;background:#f4f4f4;margin:0;padding:0;text-align:center;}";
    html += "h1{background:#007BFF;color:#fff;padding:15px;}";
    html += "div.container{margin:20px auto;padding:20px;width:80%;max-width:600px;background:#fff;box-shadow:0px 0px 10px rgba(0,0,0,0.1);border-radius:10px;}";
    html += "p{font-size:18px;line-height:1.6;}";
    html += "</style></head><body>";
    html += "<h1>BME280 Sensor Daten</h1>";
    html += "<div class='container'>";
    html += "<p><strong>Temperatur:</strong> <span id='temp'>" + String(temperature) + "</span> &deg;C</p>";
    html += "<p><strong>Druck:</strong> <span id='press'>" + String(pressure) + "</span> hPa</p>";
    html += "<p><strong>H&ouml;he:</strong> <span id='alt'>" + String(altitude) + "</span> m</p>";
    html += "<p><strong>Luftfeuchtigkeit:</strong> <span id='hum'>" + String(humidity) + "</span> %</p>";
    html += "</div>";
    html += "<h2>Firmware Update</h2>";
    html += "<form method='POST' action='/update' enctype='multipart/form-data'>";
    html += "<input type='file' name='update'>";
    html += "<input type='submit' value='Update'>";
    html += "</form>";
    // Form for Wi-Fi configuration
    html += "<h2>Wi-Fi Configuration</h2>";
    html += "<form method='POST' action='/config'>";
    html += "<label for='ssid'>SSID:</label><br><input type='text' name='ssid' value='" + WLANSSID + "'><br><br>";
    html += "<label for='password'>Password:</label><br><input type='password' name='password' value='" + WLANPassword + "'><br><br>";
    html += "<input type='submit' value='Update Wi-Fi'>";
    html += "</form>";
    html += "<br>Config Leer: AP-Mode</br>";
    html += "<script>setInterval(()=>{fetch('/data').then(res=>res.json()).then(data=>{document.getElementById('temp').innerText=data.temp;document.getElementById('press').innerText=data.press;document.getElementById('alt').innerText=data.alt;document.getElementById('hum').innerText=data.hum;});},2000);</script>";
    html += "</body></html>";
    
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
            isAP=false;
        } else {
            Serial.println("\n❌ Verbindung fehlgeschlagen.");
            // Optional: wieder Access Point starten
            WiFi.mode(WIFI_AP);
            WiFi.softAP(ssid, password);
            WiFi.softAPConfig(IPAddress(1,1,1,1), IPAddress(1,1,1,1), IPAddress(255,255,255,0));
            isAP=true;
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
void server_init()
{
  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.on("/update", HTTP_POST, [](){ server.send(200, "text/plain", "OK"); }, handleUpdate);
  server.on("/config", HTTP_POST, handleConfig); // Handle the Wi-Fi configuration form submission
  server.begin();
  Serial.println("HTTP server started");
}
void setup() {
    Serial.begin(9600);
    while(!Serial);
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
        
        display.setCursor(0, 10);
        display.print("Temp: "); display.print(temperature); display.print(" C");
        display.setCursor(0, 20);
        display.print("Pressure: "); display.print(pressure); display.print(" hPa");
        display.setCursor(0, 30);
        display.print("Alt: "); display.print(altitude); display.print(" m");
        display.setCursor(0, 40);
        display.print("Humidity: "); display.print(humidity); display.print(" %");
        display.setCursor(0, 60);
        display.print("(c)by VS&ET");
    } else {
        display.setCursor(0, 20);
        display.print("SSID: "); display.print(WiFi.softAPSSID());
        display.setCursor(0, 30);
        display.print("IP: "); 
        if (isAP) display.print(WiFi.softAPIP());
        else display.print(WiFi.localIP());
        display.setCursor(0, 60);
        display.print("(c)by VS&ET");
    }
    
    display.sendBuffer();
    delay(delayTime);
}