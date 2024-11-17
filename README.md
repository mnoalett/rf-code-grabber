# Esp-RF-Code-Catcher

This project allows you to utilize an ESP8266 in conjunction with a CC1101 radio module.

The firmware is primarily designed to listen for and log remote rf codes primarily used in home garage door and gate openers, smart plugs and others, while minimizing power consumption. After one minute of inactivity, the device enters light sleep mode, conserving battery life while maintaining its capability to capture signals.

## Features

After powering on, the ESP8266 will create an access point for 1 minute. After 1 minute of inactivity, if there is no client connected, the board enters light sleep and listens for remote codes, logging them for later access.

The default access point name is "ESP-Net" and password "01234567890" (without quotes).

Once connected, open a web browser and navigate to 192.168.4.1 to access the configuration interface.

Currently, the access point allows you to:

- Download the log
- Change the radio settings (frequency, bandwidth...)
- Change the board configuration (access point settings)
- Transmit remote codes

## Hardware:

- ESP8266 Module
- CC1101 Radio Module

Software:

Arduino IDE
ESP8266 Board Package
Libraries for CC1101 and Wi-Fi functionality
Installation
Clone the repository:

bash
Copia codice
git clone https://github.com/yourusername/your-repo.git
cd your-repo
Open in Arduino IDE:

Open the .ino file in the Arduino IDE.
Install Dependencies:

Ensure that you have the required libraries installed. Use the Library Manager in the Arduino IDE to install any missing libraries.
Upload to ESP8266:

Select the correct board and COM port.
Upload the firmware to your ESP8266.
Usage
Connecting to the Access Point:

After powering on, the ESP8266 will create an access point. Connect to it using the provided SSID.
Configuring Settings:

Open a web browser and navigate to 192.168.4.1 to access the configuration interface.
Adjust your settings as needed.
Logging and Transmitting Codes:

Use the interface to send codes and download logs.
Entering Light Sleep:

The ESP8266 will automatically enter light sleep after 1 minute of inactivity, listening for remote codes.
