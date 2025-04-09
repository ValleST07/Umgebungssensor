> [!IMPORTANT]
> Um ein Programm auf den ESP32-C3 zu flashen, muss in den bootloader-Mode gewechselt werden!\
> [Hier](https://www.edgemicrotech.com/esp32-c3-super-mini-arduino-ide-quick-start-guide/) ein einfaches Tutorial um den ESP32-C3 SuperMini richtig zu flashen.

# Umgebungssensor Projekt
## ESP32-C3 SuperMini
### Pin Out zum Board
![ESP32-C3 SuperMini Pin Out](/docs/esp32-c3-supermini_pinout.png)
Wichtige PINs für das Projekt:
- GPIO8: I²C SDA
- GPIO9: I²C SCK
- GND: Ground Potential
- 3V3: 3,3V Potential für BME280 und SH1106
### Vorbereitung zur Programmierung ([ArduinoIDE](https://www.arduino.cc/en/software/))
- Wenn du bereits ESP32 programmiert hast, kannst du diesen Punkt überspringen!
    - Hier bitte noch den passenden Treiber herrunterladen: [CP210x USB to UART Bridge VCP Drivers](https://www.silabs.com/developer-tools/usb-to-uart-bridge-vcp-drivers?tab=downloads)
    - Im Board-Manager die ESP32 Boards installieren oder folgenden Link anführen: `https://dl.espressif.com/dl/package_esp32_index.json`
- Als Board zum Programmiern: ESP32C3 Dev Module

> [!NOTE]
> In der IDE in der Menubar unter "Tools" → "USB CDC on Boot" → "Enable" einstellen. Somit sollte später keine Probleme auftreten, wenn du die Serielleschnittstelle benutzen möchtest.

Somit sollten alle Vorbereitung zur Programmierung getroffen worden sein.

## Verdrahtung und PCB
### Verdrahtung
![Steckbrettaufbau](/docs/Steckplatine.png)
### PCB
![2D-View: PCB](/docs/PCB_2D_View.png){height: 200px}
![3D-View: PCB](/docs/PCB_3D_View.png){height: 200px}