# User Guide

## Parts
Functionally, there are two electronic parts:  
1. The ESP8266 microcontroller (i.e. the computer)  
2. The NeoPixels (i.e. the lights)

## Powering On
### Requirements
* You must use the DC 5V adapter to power the NeoPixels.  
* A micro USB cable can be used to power the ESP8266 microcontroller in order to upload an new sketch.

### WiFi
* The ESP8266 **must be connected to WiFi** for the lights to turn on, even with the manual toggle button.
* If the sign is powered on and successully connects to WiFi, it will grab the weather for KNFL from ADDS and turn the lights on automatically.

### Weather Update Cylce 
* The ESP8266 grabs the current METAR once per minute.

### Setting the lights manually
* The sign can be cycled through Drinking Weather -> Flying Weather -> OFF with the button on the back of the frame.

## Connecting via a web browser
* Open a web browser and navigate to [http://weather.local/](http://weather.local/)  
*Note: the `weather.local` address uses mDNS and works best on iOS or Mac, Windows requires installation of [Bonjour](https://support.apple.com/kb/DL999?viewlocale=en_US&locale=en_US). Android is not supported.*  
* You can also connect via the local IP address (e.g. `192.168.x.x`)  
* To find the local IP, navigate to [weather.local](http://weather.local/) on an iOS or Mac and check the local IP at the bottom of the screen.  

## Checking the weather
Type the four-letter identifier of the airport you want to check and press `Fetch Weather`.  

## Setting 'Flying Weather' or 'Drinking Weather' via a web browser
Press the `Set Flying Weather` or `Set Drinking Weather` button on the screen to toggle the lights.

## Updating the Software (Arduino sketch)
### 1. Clone this Repo
Using  `terminal` on a Mac, or `CMD` in Windows, navigate to the directory you want to install the software and type `git clone https://github.com/jbell303/esp8266-weather-station-with-neopixels.git`
*Note you may need to first install [Git](https://git-scm.com/).

### 2. Install Arduino 
Navigate to [https://www.arduino.cc](https://www.arduino.cc) and install Arduino.

### 3. Install the necessary ESP8266 drivers and packages
Use [this guide](https://learn.adafruit.com/adafruit-io-basics-esp8266-arduino/using-arduino-ide) to add the ESP8266 to Arduino.

### 4. Install the required libraries
Go to `Tools -> Manage Libraries`
Install the following libraries (you can use the search bar at the top to find the required libraries):
* ArduinoJson
* AutoConnect
* Adafruit NeoPixel

### 5. Open the sketch
Open the `.ino` file from the repo cloned earlier.

### 6. Make the required changes
Make the required code changes then hit `verify` (the checkmark).  
Be sure to save your work.

### 7. Upload the new sketch to the ESP8266
There are two ways to upload the sketch:  
1. **Using a micro-USB cable**. The previous [guide](https://learn.adafruit.com/adafruit-io-basics-esp8266-arduino/using-arduino-ide) covers how to upload a sketch in Arduino using a USB cable.

2. **Over the Air (OTA)** 
* To update a sketch OTA, the sketch first has to be compiled to a binary. In Arduino, go to `Sketch -> Export compiled binary` to build the sketch. Once finished, there should be a `.bin` file in the same directory as the `.ino` skech file.
* Using a web browser, navigate to `weather.local` or the local IP of the ESP8266.
* At the botton of the screen there is a `gear icon`. Click on it to go to the Autoconnect menu.
* At the top-right of the Autoconnect menu, click on `Update`.
* Click on `Choose File...` and select the binary `.bin` file created earlier.
* Click `Update`. Once the firmware is done updating, the board will reset automatically, there should be no need to refresh the browser.

## Troubleshooting the hardware
### Opening the frame
* Unplug the unit and place it face-down on a table.
* Pivot the latches so the back of the frame can be removed.
* Carefully lift the edge where the power cord is sticking out.
* Disconnect the two JST connectors connecting the NeoPixels.
* Open the lid like a book cover, being careful not to damage the Neopixel connections on the left edge.

### Removing the ESP8266
* Take note of the wiring, use a screwdriver to loosen the connections to the terminal block if required.
* Remove the electronics assembly from the housing on the frame.

### Fixing or replacing hardware
* The ESP8266 can be pulled out of the electronics assembly without tools.
* There is a parts list and wiring schematic in the [README](https://github.com/jbell303/esp8266-weather-station-with-neopixels/blob/master/README.md).

### Common Issues
* The unit won't turn on: Plug in the sign using the wall adapter. Check to see if any lights blink on the ESP8266.
If there is a blinking amber light, the ESP8266 is getting power, but either:
1. The NeoPixels aren't getting power OR
2. The ESP8266 isn't connecting to WiFi








