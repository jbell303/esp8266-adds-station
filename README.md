# ESP8266 Weather Station with Neopixels
Uses the ESP8266 to query Aviation Digital Data Service (aviationweather.gov/adds) to display current weather status with Adafruit NeoPixels

![animation](https://github.com/jbell303/esp8266-weather-station-with-neopixels/blob/master/media/sign_animation.gif)
## Requirements
### Hardware
* [Adafruit ESP8266 Feather HUZZAH with headers](https://www.adafruit.com/product/3046)
* [Adafruit Terminal Block Breakout Featherwing](https://www.adafruit.com/product/2926)
* [74AHCT125 Quad Level-shifter](https://www.adafruit.com/product/1787)
* [470 Ohm Resistor](https://www.adafruit.com/product/2781)
* [1N4001 Diode](https://www.adafruit.com/product/755)
* [IC Socket for 0.3" Chips](https://www.adafruit.com/product/2203)
* [2-Pin JST plug](https://www.adafruit.com/product/2880)
* [2.1mm jack to screw terminal block](https://www.adafruit.com/product/368)

### Basic Wiring

#### Components
Here is the basic layout for the electronics:  
![ESP8266 Sketch](https://github.com/jbell303/esp8266-weather-station-with-neopixels/blob/master/media/esp8266_adds_station_sketch.png)

I used a breadboard in the diagram for simplification, however, the actual assembly was mounted on the terminal block Featherwing
depicted in the lower-left corner. You can mount the 74AHCT125 in the center of the Featherwing and the ESP8266 will still fit on top.
the diode and resistor can be mounted on the far left of the Featherwing. I mounted the capacitor in the DC terminal block as it would not
fit anywhere else without interfering with the USB port on the ESP8266.

I used these diagrams for reference:
1. [Adafruit Neopixel Uberguide](https://learn.adafruit.com/adafruit-neopixel-uberguide/basic-connections)
2. [Neopixel Raspberry Pi Wiring](https://learn.adafruit.com/neopixels-on-raspberry-pi/raspberry-pi-wiring)
3. [ESP8266 Neopixel Wiring](https://forums.adafruit.com/viewtopic.php?t=112430)
