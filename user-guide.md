# User Guide
[Getting Started](#Getting-Started)  
[Connecting to WiFi](#Connecting-to-WiFi-using-AutoConnect)  
[Checking the Weather via a Web Browser](#Checking-the-weather-via-a-web-browser)  
[Setting the Lights Manually](#Setting-the-lights-manually)  
[Updating the Software](#Updating-the-Software)  
[Troubleshooting](#Troubleshooting-the-hardware)  
[Common Issues](#Common-Issues)


## Getting Started
#### Power Requirements
* You must use the DC 5V adapter to power the NeoPixels.  
* A micro USB cable can be used to power the ESP8266 microcontroller in order to upload an new sketch.

#### WiFi
* The ESP8266 **must be connected to WiFi** for the lights to turn on, even with the manual toggle button.
* If the sign is powered on and successully connects to WiFi, it will grab the weather for KNFL from ADDS and turn the lights on automatically.


### Connecting to WiFi using AutoConnect
Follow [this guide](https://hieromon.github.io/AutoConnect/gettingstarted.html#run-at-first) to connect the board to a new access point using your phone.

### Checking the weather via a web browser  
* Open a web browser and navigate to [http://weather.local/](http://weather.local/)  
*Note: the `weather.local` address uses [mDNS](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266mDNS) and works best on iOS or Mac. Windows requires installation of [Bonjour](https://support.apple.com/kb/DL999?viewlocale=en_US&locale=en_US). Android is not supported.*  
* You can also connect via the local IP address (e.g. `192.168.x.x`)  
* To find the local IP, navigate to [weather.local](http://weather.local/) on an iOS or Mac and check the local IP at the bottom of the screen. 
![screenshot](https://github.com/jbell303/esp8266-weather-station-with-neopixels/blob/master/media/wx_screen.PNG) 

### Fetching the weather
Type the four-letter identifier of the airport you want to check and press ![Fetch Weather](https://github.com/jbell303/esp8266-weather-station-with-neopixels/blob/master/media/fetch_button.PNG). 

#### Weather Update Cylce 
* The ESP8266 grabs the current METAR once per minute.

#### Drinking Weather Parameters
* IFR or LIFR  
* Ceiling < 3000' AGL  
* Wind Speed > 25 knots  

*To change these parameters see [Updating the Software](#Updating-the-Software)*

### Setting 'Flying Weather' or 'Drinking Weather' via a web browser
Press the ![Set Flying Weather](https://github.com/jbell303/esp8266-weather-station-with-neopixels/blob/master/media/fly_button.PNG) or ![Set Drinking Weather](https://github.com/jbell303/esp8266-weather-station-with-neopixels/blob/master/media/drink_button.PNG) button on the screen to toggle the lights.

### Setting the lights manually
The sign can be cycled through `Drinking Weather` -> `Flying Weather` -> `OFF` with the button on the back of the frame.  
*Note: the sign must be connected to WiFi*


## Updating the Software  
<img src=https://3dprint.com/wp-content/uploads/2015/06/3dp_hexapod_arduino_logo.png alt="Arduino Logo" width="100" height="95"></img>  
### 1. Clone this Repo
Using  `terminal` on a Mac, or `CMD` in Windows, navigate to the directory you want to install the software and type: 
```
git clone https://github.com/jbell303/esp8266-weather-station-with-neopixels.git
```  
*Note you may need to first install [Git](https://git-scm.com/).

### 2. Install Arduino 
Navigate to [https://www.arduino.cc](https://www.arduino.cc) and install Arduino.

### 3. Install the necessary ESP8266 drivers and package
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

#### Example: Update the `.local` address:
Open the `esp8266-adds-station.ino` file in Arduino.
Change the `mdns_name[]` variable from `weather` to the name you want (no whitespace).

#### Example: Update the drinking weather parameters:
Open the `esp8266-adds-station.ino` file in Arduino.
Change the `MIN_CEILING` and `MAX_WIND_SPEED` parameters to the desired values (e.g. `#define MIN_CEILING 2000`).

### 7. Upload the new sketch to the ESP8266
There are two ways to upload the sketch:  
1. **Using a micro-USB cable**. The previous [guide](https://learn.adafruit.com/adafruit-io-basics-esp8266-arduino/using-arduino-ide) covers how to upload a sketch in Arduino using a USB cable.

2. **Over the Air (OTA)** 
* To update a sketch OTA, the sketch first has to be compiled to a binary. In Arduino, go to `Sketch -> Export compiled binary` to build the sketch. Once finished, there should be a `.bin` file in the same directory as the `.ino` skech file.
* Using a web browser, navigate to `weather.local` or the local IP of the ESP8266.
* Click on the ![gear icon](https://github.com/jbell303/esp8266-weather-station-with-neopixels/blob/master/media/gear.PNG) at the botton of the screen to go to the Autoconnect menu.
* At the top-right of the Autoconnect menu, click on `Update`.
* Click on `Choose File...` and select the binary `.bin` file created earlier.
* Click `Update`. Once the firmware is done updating, the board will reset automatically, there should be no need to refresh the browser.


## Troubleshooting the hardware
### Parts
Functionally, there are two electronic parts:  
1. The ESP8266 microcontroller (i.e. the computer)  
![electronics](https://github.com/jbell303/esp8266-weather-station-with-neopixels/blob/master/media/electronics.png)
2. The NeoPixels (i.e. the lights)  
![neopixel-frame](https://github.com/jbell303/esp8266-weather-station-with-neopixels/blob/master/media/frame_open.png)

### Opening the frame
* Unplug the unit and place it face-down on a table.
* Pivot the latches so the back of the frame can be removed.
* Carefully lift the edge where the power cord is sticking out.
* Disconnect the two JST connectors connecting the NeoPixels.  
![jst connectors](https://github.com/jbell303/esp8266-weather-station-with-neopixels/blob/master/media/jst.png)
* Open the lid like a book cover, being careful not to damage the Neopixel connections on the left edge.  
![neopixels](https://github.com/jbell303/esp8266-weather-station-with-neopixels/blob/master/media/neopixel.png)
![open-frame](https://github.com/jbell303/esp8266-weather-station-with-neopixels/blob/master/media/frame_open.png)


### Removing the ESP8266
* Take note of the wiring, use a screwdriver to loosen the connections to the terminal block if required.  
![electronics](https://github.com/jbell303/esp8266-weather-station-with-neopixels/blob/master/media/electronics.png)
* Remove the electronics assembly from the housing on the frame.

### Fixing or replacing hardware
* The ESP8266 can be pulled out of the electronics assembly without tools.
* There is a parts list and wiring schematic in the [README](https://github.com/jbell303/esp8266-weather-station-with-neopixels/blob/master/README.md).

## Common Issues
### The unit won't turn on: 
* Plug in the sign using the wall adapter. Check to see if any lights blink on the ESP8266.  
* If there is a blinking amber light, the ESP8266 is getting power, but either:
1. The ESP8266 isn't connected to WiFi. - Follow the [autoconnect](#Connecting-to-WiFi-using-AutoConnect) instructions.
2. The NeoPixels aren't getting power - Check the connections to the NeoPixels. The `red` wire should be connected directly to the 5V DC power jack.  
![dc_jack](https://github.com/jbell303/esp8266-weather-station-with-neopixels/blob/master/media/dc_jack.png)  
You can test the lights manually with the button on the back of the sign.

### I can't access the webpage:
* Check that the board is connected to [WiFi](#Connecting-to-WiFi-using-AutoConnect)  
* Try to access the page using the local [IP](#Connecting-via-a-web-browser)

### The weather won't update
It is possible that the [SHA-1 fingerprint](https://en.wikipedia.org/wiki/Public_key_fingerprint) of `aviationweather.gov` has changed.  
* Follow [this](https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/client-secure-examples.html#how-to-verify-server-s-identity) guide to find the new fingerprint and [update](#Updating-the-Software) the value of `fingerprint` in `esp8266-adds-station.ino`.








