#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <U8g2lib.h>

#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme; // I2C

U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

unsigned long delayTime;

void setup() {
    Serial.begin(9600);
    while(!Serial);
    Serial.println(F("BME280 test"));

    display.begin();

    unsigned status;
    status = bme.begin(0x76);
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1) delay(10);
    }
    
    Serial.println("-- Default Test --");
    delayTime = 1000;
}

void loop() {
    float temperature = bme.readTemperature();
    float pressure = bme.readPressure() / 100.0F;
    float altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
    float humidity = bme.readHumidity();
    
    Serial.print("Temperature = "); Serial.print(temperature); Serial.println(" °C");
    Serial.print("Pressure = "); Serial.print(pressure); Serial.println(" hPa");
    Serial.print("Approx. Altitude = "); Serial.print(altitude); Serial.println(" m");
    Serial.print("Humidity = "); Serial.print(humidity); Serial.println(" %");
    Serial.println();

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
    display.sendBuffer();
    
    delay(delayTime);
}