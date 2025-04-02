#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <U8g2lib.h>
#include <WiFi.h>
#include <WebServer.h>

#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme;
U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

const char* ssid = "HTL-IoT";
const char* password = "Internet0fThings!";
WebServer server(80);

unsigned long delayTime;

void handleRoot() {
    float temperature = bme.readTemperature();
    float pressure = bme.readPressure() / 100.0F;
    float altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
    float humidity = bme.readHumidity();
    
    String html = "<html><head><title>BME280 Sensor Data</title></head><body>";
    html += "<h1>Sensor Daten</h1>";
    html += "<p>Temperatur: " + String(temperature) + " &deg;C</p>";
    html += "<p>Druck: " + String(pressure) + " hPa</p>";
    html += "<p>H&ouml;he: " + String(altitude) + " m</p>";
    html += "<p>Luftfeuchtigkeit: " + String(humidity) + " %</p>";
    html += "</body></html>";
    
    server.send(200, "text/html", html);
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
    
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    
    server.on("/", handleRoot);
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
    dispay.print(WiFi.localIP());
    display.sendBuffer();
    
    delay(delayTime);
}
