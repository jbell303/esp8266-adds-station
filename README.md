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
The diode and resistor can be mounted on the far left of the Featherwing. I mounted the capacitor in the DC terminal block as it would not fit anywhere else without interfering with the USB port on the ESP8266.

The NeoPixel strips were divided into four sections around the wood frame.
![NeoPixel Layout](https://github.com/jbell303/esp8266-weather-station-with-neopixels/blob/master/media/neopixel_wiring.png)
Note that the Neopixel strip can terminate without any additional connections. Make sure to use flexible wiring as the copper connectors on the NeoPixels are delicate. Also, note the number of Neopixels you use in each strip as this will be important when writing the Arduino sketch.

Reference:
1. [Adafruit Neopixel Uberguide](https://learn.adafruit.com/adafruit-neopixel-uberguide/basic-connections)
2. [Neopixel Raspberry Pi Wiring](https://learn.adafruit.com/neopixels-on-raspberry-pi/raspberry-pi-wiring)
3. [ESP8266 Neopixel Wiring](https://forums.adafruit.com/viewtopic.php?t=112430)

### Software
[Arduino](https://www.arduino.cc/)
Required Libraries:
* ESP8266 WiFi
* ESP8266 WebServer
* ESP8266 mDNS
* [ArduinoJSON](https://arduinojson.org/)
* [AutoConnect](https://github.com/Hieromon/AutoConnect)
* Adafruit Neopixel

## Installation
Simply open Arduino and add the required libraries. Clone this repo for the `.ino` sketch and load onto the board.
You will need to make modifications to the NeoPixel variables and GPIO ports to reflect your own wiring. 
```
// NeoPixels
#define LED_PIN    5
#define LED_COUNT  100
```
```
void setFlyingWeatherLights () {
  strip.clear();
  strip.fill(strip.Color(255, 255, 255), 0, 25); // left strip
  strip.fill(strip.Color(0, 255, 0), 25, 25); // top strip
  strip.fill(strip.Color(255, 255, 255), 50, 24); // right strip
  strip.fill(strip.Color(0, 0, 0), 74, 26); // bottom strip
  strip.show();
}

void setDrinkingWeatherLights () {
  strip.clear();
  strip.fill(strip.Color(255, 255, 255), 0, 25);
  strip.fill(strip.Color(0, 0, 0), 25, 25);
  strip.fill(strip.Color(255, 255, 255), 50, 24);
  strip.fill(strip.Color(255, 0, 0), 74, 26);
  strip.show();
}
```
You can learn more about NeoPixel programming in the [Uberguide](https://learn.adafruit.com/adafruit-neopixel-uberguide/python-circuitpython)

You will not need
to provide your SSID or Password if you use AutoConnect. I recommend looking at this [Getting Started](https://hieromon.github.io/AutoConnect/gettingstarted.html) guide to learn more about how to use AutoConnect.

## Usage
Use the Serial Monitor (Tools->Serial Monitor) when you first use the sketch. It will output the local IP address (mDNS is `esp8266.local`) and the status of the server and requests. Navigate to the server's local IP on a web browser (e.g. 192.168.x.x) and you should see an html form. Enter a station identifier (e.g. KLAX for Los Angeles Int'l Airport) and press `Fetch Weather`. The weather will return with `Drinking Weather` if the conditions are IFR, ceiling less than 2000' or winds >25 knots. Feel free to change this in the code as it is strictly for entertainment. There is also an HTML button to manually toggle drinking/flying weather. Finally, a button can be connected to a GPIO port to toggle the lights drinking/flying/off without the need to connect a web client. Enjoy!
