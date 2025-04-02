> [!IMPORTANT]
> Um ein Programm auf den ESP32-C3 zu flashen, muss in den bootloader-Mode gewechselt werden!\
> [Hier](https://www.edgemicrotech.com/esp32-c3-super-mini-arduino-ide-quick-start-guide/) ein einfaches Tutorial um den ESP32-C3 SuperMini richtig zu flashen.

# Umgebungssensor Projekt
## ESP32-C3 SuperMini
### Pin Out zum Board
![ESP32-C3 SuperMini Pin Out](/doc/esp32-c3-supermini_pinout.png)
Wichtige PINs für das Projekt:
- GPIO8: I²C SDA
- GPIO9: I²C SCK
- GND: Ground Potential
- 3V3: 3,3V Potential für BME280 und SH1106
### Vorbereitung zur Programmierung ([ArduinoIDE](https://www.arduino.cc/en/software/))
- Wenn du bereits ESP32 programmiert hast, kannst du diesen Punkt überspringen!\
Hier bitte noch den passenden Treiber herrunterladen: [CP210x USB to UART Bridge VCP Drivers](https://www.silabs.com/developer-tools/usb-to-uart-bridge-vcp-drivers?tab=downloads)
- Im Board-Manager die ESP32 Boards installieren: 'https://dl.espressif.com/dl/package_esp32_index.json'
- Als Board zum Programmiern: ESP32C3 Dev Module

> [!NOTE]
> In der IDE in der Menubar unter Tools: USB CDC on Boot auf "Enable".

Somit sollten alle Vorbereitung zur Programmierung getroffen worden sein.
