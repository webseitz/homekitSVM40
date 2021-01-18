# Tutorial: SVM40 Homekit Integration

## Summary

This tutorial enables you to setup a VOC-Monitor sending Temperature, Humidity and Volotile Organic Compound (VOC) measurements via WLAN to Apple Homekit. All steps necessary and links are provided here.

The tutorial is structured in 3 parts

* **Hardware Setup**: Learn how to wire the sensor to the development board
* **Software Setup**: Learn how to setup your computer to program the development board
* **Homekit Setup**: Add the new gadget to homekit

## Hardware Setup

To complete this tutorial, you'll need

* [ESP32 DevKitC](https://www.espressif.com/en/products/devkits/esp32-devkitc) (available [here](https://www.digikey.com/en/products/detail/espressif-systems/ESP32-DEVKITC-32D/9356990))
* [Sensirions SEK-SVM40](https://www.sensirion.com/en/environmental-sensors/evaluation-kit-sek-svm40/) (available [here](https://www.digikey.com/en/products/detail/sensirion-ag/SEK-SVM40/12820417?s=N4IgTCBcDaIMoDUCyAWADCAugXyA)
* USB cable to connect the ESP32 DevKitC module to your computer

Connect the SEK-SVM40 module to the ESP32 DevKitC as depicted below:

* **VDD** of the SEK-SVM40 to the **3.3V** of the ESP32
* **GND** of the SEK-SVM40 to the **GND** of the ESP32
* **SCL** of the SEK-SVM40 to the **GPIO 22** of the ESP32
* **SDA** of the SEK-SVM40 to the **GPIO 21** of the ESP32
* **SEL** of the SEK-SVM40 to the **GND** of the ESP32  (choosing the I2C interface)

<img src="images/SVM40_hardware_setup.png" width="500">

## Software Setup

### Setup the Arduino IDE for the ESP32 platform

The following instructions originate from [here](https://github.com/espressif/arduino-esp32)

1. Install the current version of the [Arduino IDE](http://www.arduino.cc/en/main/software).
2. Start the Arduino IDE and open the Preferences window.
3. Enter the following link into the *Additional Board Manager URLs* field. You can add multiple URLs, separating them with commas.
	* `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
4. Open the Boards Manager from `Tools > Board -> Board Manager` and install the *esp32* platform
5. Select your ESP32 board from the `Tools > Board` menu after the successfull installation.
	* E.g. `ESP32 Dev Module`

### Setup the requried libraries

We'll be installing one library. Click the link below and download the newest .zip release packages

* The [Arduino Homekit ESP8266 Library](https://github.com/Mixiaoxiao/Arduino-HomeKit-ESP8266/releases)

For the downloaded .zip file: In the Arduino IDE, select `Sketch -> include Library -> Add .zip Library` and select the downloaded .zip file.

<img src="images/Arduino-import-zip-lib.png" width="500">

Restart the Arduino IDE.

### Launch the Gadget Firmware

1. Open the Arduino IDE.
2. Go to `File -> Examples -> Sensirion Gadget BLE Lib -> Example5_SVM40_BLE_Gadget`.
3. Make sure the ESP32 is connected to your computer.
4. Press the Upload button on the top left corner of the Arduino IDE.

<img src="images/Arduino-upload-button.png" width="500">

## Homekit Integration

1. Open the Home App on your iPhone or iPad.
2. Use the "+"-sign to add a new device.
3. Chose "I do not have a code"
4. Select the SVM40-module
5. Insert 111-11-111 as the device code
6. Follow the final steps.

### Value plotting on your Computer

To verify that everything is working fine, open the Serial Plotter, while your ESP32 ist still connected to your computer to see the sensor values measured by the SVM40 module:

1. Go to `Tools -> Serial Plotter`
2. Make sure on the bottom left corner `115200 baud` is selected, as depicted in the image below

You should see the measured values plotted in the opened window. Alternatively you can choose `Tools -> Serial Monitor` to see the values in text form.


