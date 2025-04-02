#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <U8g2lib.h>
#include <WiFi.h>
#include <WebServer.h>

#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme;
U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

const char ssid [10] = "valerik";
const char password [2] = "";
WebServer server(80);

unsigned long delayTime;

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
    html += "<script>setInterval(()=>{fetch('/data').then(res=>res.json()).then(data=>{document.getElementById('temp').innerText=data.temp;document.getElementById('press').innerText=data.press;document.getElementById('alt').innerText=data.alt;document.getElementById('hum').innerText=data.hum;});},2000);</script>";
    html += "</body></html>";
    
    server.send(200, "text/html", html);
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

void setup() {
    Serial.begin(9600);
    while(!Serial);
    Serial.println(F("BME280 test"));

    display.begin();

    if (!bme.begin(0x76)) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1) delay(10);
    }
    
    WiFi.softAP(ssid, password);
    WiFi.softAPConfig(IPAddress(1,1,1,1), IPAddress(1,1,1,1), IPAddress(255,255,255,0));
    Serial.println("Access Point started");
    Serial.print("IP Address: ");
    Serial.println(WiFi.softAPIP());
    Serial.println(WiFi.softAPSSID());
    
    server.on("/", handleRoot);
    server.on("/data", handleData);
    server.begin();
    Serial.println("HTTP server started");
    
    delayTime = 1000;
}

void loop() {
    server.handleClient();
    
    float temperature = bme.readTemperature();
    float pressure = bme.readPressure() / 100.0F;
    float altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
    float humidity = bme.readHumidity();
    
    display.clearBuffer();
    display.setFont(u8g2_font_ncenB08_tr);
    display.setCursor(0, 10);
    display.print("Temp: "); display.print(temperature); display.print(" C");
    display.setCursor(0, 20);
    display.print("Pressure: "); display.print(pressure); display.print(" hPa");
    display.setCursor(0, 30);
    display.print("Alt: "); display.print(altitude); display.print(" m");
    display.setCursor(0, 40);
    display.print("Humidity: "); display.print(humidity); display.print(" %");
    display.setCursor(0, 50);
    display.print("(c)by VS&ET");
    display.setCursor(0, 60);
    display.print(WiFi.softAPSSID()); display.print(WiFi.softAPIP());
    display.sendBuffer();
    
    delay(delayTime);
}
