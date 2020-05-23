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
* The ESP8266 must be connected to WiFi for the lights to turn on, even with the button.
* If the sign is powered on and successully connects to WiFi, it will grab the weather for KNFL from ADDS and turn the lights on automatically.

### Weather Update Cylce 
* The ESP8266 grabs the current METAR once per minute.

### Manual Button
* The sign can be cycled through Drinking Weather->Flying Weather->OFF with the button on the back of the frame.

## Connecting via a web browser
* Open a web browser and navigate to [http://weather.local/](http://weather.local/)
* Note: the `weather.local` address uses mDNS and works best on iOS or Mac, Windows requires installation of Bonjour. Android is not supported.
* You can also connect via the local IP address (e.g. `192.168.x.x`)
* To find the local IP, navigate to [weather.local](http://weather.local/) on an iOS or Mac and check the local IP at the bottom of the screen.

## Checking the weather
* 


