#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include <AutoConnect.h>
#include <Adafruit_NeoPixel.h>

// ADDS 
const char* host = "aviationweather.gov";
const int httpsPort = 443;
const char fingerprint[] PROGMEM = "07e32864918f4238a0542a2ccdc17c98798ca1d8";
String identifier = "KNFL";
String url = "/adds/dataserver_current/httpparam?dataSource=metars&requestType=retrieve&format=xml&hoursBeforeNow=3&mostRecent=true&stationString=";

String flight_category = "";
String visibility = "";
String sky_cover = "";
String cloud_base = "";
int wind_speed = 0;
String ceiling = "";

ESP8266WebServer server(80);
AutoConnect Portal(server);
AutoConnectConfig config;

// intitialize timer for auto updates
bool timerActive;

// NeoPixels
#define LED_PIN    5
#define LED_COUNT  100

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// button
const int buttonPin = 13; // button for manual toggle
uint32_t green_strip = 0; // for color evaluation
uint32_t red_strip = 0;

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

// html and javascript
static const char html_1[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
  <head>
  <meta name='viewport' content='width=device-width, initial-scale=1.0'/>
  <meta charset='utf-8'>
  <style>
    body {font-size:140%;}
    #main {display: table; margin: auto; padding: 0 10px 0 10px; }
    h2 {text-align:center; }
    #LED_button { padding:10px 10px 10px 10px; width:100%; background-color: #50FF50; font-size: 120%;}
   </style>

<script>
  // set the button text to a pending status
  // read the station id from the form and use it in our request URL
  function fetchWeather() {
    document.getElementById("submit_button").value = "Fetching weather...";
    station_id = document.getElementById("identifier").value;
    ajaxLoad(station_id);
  }

  // initialize ajax request object
  var ajaxRequest = null;
  if (window.XMLHttpRequest) { ajaxRequest = new XMLHttpRequest(); }
                           { ajaxRequest = new ActiveXObject("Microsoft.XMLHTTP"); }

  // send a POST request to the server with station ID
  // parse the response and display the values in HTML
  function ajaxLoad(station_id) {
    if (!ajaxRequest) {
      alert("AJAX is not supported.");
      return;
    }

    ajaxRequest.open("POST", "station_id/", true);
    ajaxRequest.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    ajaxRequest.send("station_id=" + station_id);
    ajaxRequest.onreadystatechange = function () {
      if (ajaxRequest.readyState == 4 && ajaxRequest.status == 200) {
        var response = JSON.parse(ajaxRequest.responseText);
        document.getElementById("identifier").value = response.identifier;
        document.getElementById("flight_category").innerHTML = response.flight_category;
        document.getElementById("ceiling").innerHTML = response.ceiling;
        document.getElementById("wind_speed").innerHTML = response.wind_speed;
        document.getElementById("submit_button").value = "Fetch Weather";
        document.getElementById("toggle_button").value = (response.isDrinkingWeather ? "Set Flying Weather" : "Set Drinking Weather");
        document.getElementById("toggle_button").style.backgroundColor = (response.isDrinkingWeather ? "#4CAF50" : "#FF0000");
      }
    }
    ajaxRequest.send();
  }

  // manually toggle flying weather and drinking weather lights
  // change the button text based on the desired state
  // send the LED toggle request to the server
  function toggleLed () {
    var button_text = document.getElementById("toggle_button").value;
    if (button_text == "Set Drinking Weather") {
      document.getElementById("toggle_button").value = "Setting Drinking Weather";
      document.getElementById("toggle_button").style.backgroundColor = "#4CAF50";
      ajaxLED('DRINKING');
    } else {
      document.getElementById("toggle_button").value = "Setting Flying Weather";
      document.getElementById("toggle_button").style.backgroundColor = "#FF0000";
      ajaxLED('FLYING');
    }
  }

  function ajaxLED(ajaxURL) {
    if (!ajaxRequest) {
      alert("AJAX is not supported.");
      return;
    }

    ajaxRequest.open("GET", ajaxURL, true);
    ajaxRequest.onreadystatechange = function () {
      if (ajaxRequest.readyState == 4 && ajaxRequest.status == 200) {
        var ajaxResult = ajaxRequest.responseText;
        if (ajaxResult == "Flying Weather") {
          document.getElementById("toggle_button").value = "Set Drinking Weather";
          document.getElementById("toggle_button").style.backgroundColor = "#FF0000";
        } else if (ajaxResult == "Drinking Weather") {
          document.getElementById("toggle_button").value = "Set Flying Weather";
          document.getElementById("toggle_button").style.backgroundColor = "#4CAF50";
        }
      }
    }
    ajaxRequest.send();
  }

  // request the local IP address from the server
  function getIpAddress() {
    if (!ajaxRequest) {
      alert("AJAX is not supported.");
      return;
    }

    // display the local IP address in HTML
    ajaxRequest.open("GET", "ip_address", true);
    ajaxRequest.onreadystatechange = function () {
      if (ajaxRequest.readyState == 4 && ajaxRequest.status == 200) {
        var ajaxResult = ajaxRequest.responseText;
        console.log(ajaxResult);
        document.getElementById("ip_address").innerHTML = ajaxResult;
      }
    }
    ajaxRequest.send();
  }
</script>

  <title>Saints Weather Station</title>
</head>

 <style type="text/css">
    body {
   font-family: "Helvetica Neue", Helvetica, Arial, sans-serif;
}

.label {
  text-align: right;
}

.data {
  padding-left: 10px;
}
    
input[type=text], select {
   width: 100%;
   padding: 12px 20px;
   margin: 8px 0;
   display: inline-block;
   border: 1px solid #ccc;
   border-radius: 4px;
   box-sizing: border-box;
}

input[type=button] {
  -webkit-appearance: none;
  width: 100%;
  color: white;
  padding: 10px 10px;
  margin: 8px 0;
  border: none;
  border-radius: 4px;
  cursor: pointer;
}

input[type=button] {
  -webkit-appearance: none;
  background-color: #45a049;
}

div {
  border-radius: 5px;
  background-color: #f2f2f2;
  padding: 20px;
}
    
  </style>
</head>
<body onload="getIpAddress()">
  <div id='main'>
    
    <h2>Saints Weather Station</h2>
    <table>
      <tr>
        <td class="label">Flight Category: </td>
        <td class="data" id="flight_category"></td>
      </tr>
      <tr>
        <td class="label">Ceiling: </td>
        <td class="data" id="ceiling"></td>
      </tr>
      <tr>
        <td class="label">Wind Speed: </td>
        <td class="data" id="wind_speed"></td>
      </tr>   
    </table>
      <div>
        <form class="form "action="javascript:fetchWeather()">
          <label for="name">Identifier:</label>
          <input type="text" id="identifier" name="station_id">
          <input type="button" style="background-color:#4169E1" id="submit_button" onclick="fetchWeather()" value="Fetch Weather" />
        </form>
      </div>
     <input class="button" type="button" id="toggle_button" style="background-color:#4CAF50;" onclick="toggleLed()" value="Set Flying Weather"/>
  </div>
  <p style="padding-top:15px;text-align:center">
  <a href="/_ac"><img src="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABgAAAAYCAYAAADgdz34AAAC2klEQVRIS61VvWsUQRSfmU2pon9BUIkQUaKFaCBKgooSb2d3NSSFKbQR/KrEIiIKBiGF2CgRxEpjQNHs7mwOUcghwUQ7g58IsbGxEBWsb2f8zR177s3t3S2cA8ftzPu993vzvoaSnMu2vRKlaqgKp74Q/tE8qjQPyHGcrUrRjwlWShmDbFMURd/a6TcQwNiYUmpFCPElUebcuQ2vz6aNATMVReHEPwzfSSntDcNwNo2rI+DcvQzhpAbA40VKyV0p1Q9snzBG1qYVcYufXV1sREraDcxpyHdXgkfpRBj6Uwm2RsC5dxxmZ9pdOY9cKTISRcHTCmGiUCh4fYyplTwG2mAUbtMTBMHXOgK9QfyXEZr+TkgQ1oUwDA40hEgfIAfj+HuQRaBzAs9eKyUZ5Htx+T3ZODKG8DzOJMANhmGomJVMXPll+hx9UUAlzZrJJ4QNCDG3VEfguu7mcpmcB/gkBOtShhQhchAlu5jlLUgc9ENgyP5gf9+y6LTv+58p5zySkgwzLNOIGc8sEoT1Lc53NMlbCQQuvMxeCME1NNPVVkmH/i3IzzXDtCSA0qQQwZWOCJDY50jsQRjJmkslEOxvTcDRO6zPxOh5xZglKkYLhWM9jMVnkIsTyMT6NBj7IbOCEjm6HxNVVTo2WXqEWJZ1T8rytB6GxizyDkPhWVpBqfiXUtbo/HywYJSpA9kMamNNPZ71R9Hcm+TMHHZNGw3EuraXEUldbfvw25UdOjqOt+JhMwJd7+jSTpZaEiIcaCDwPK83jtWnTkwnunFMtxeL/ge9r4XItt1RNNaj/0GAcV2bR3U5sG3nEh6M61US+Qrfd9Bs31GGulI2GOS/8dgcQZV1w+ApjIxB7TDwF9GcNzJzoA+rD0/8HvPnXQJCt2qFCwbBTfRI7UyXumWVt+HJ9NO4XI++bdsb0YyrqXmlh+AWOLHaLqS5CLQR5EggR3YlcVS9gKeH2hnX8r8Kmi1CAsl36QAAAABJRU5ErkJggg==" border="0" title="AutoConnect menu" alt="AutoConnect menu"/></a>
</p>
<p id="ip_address">192.168.x.x</p>
</html>

)=====";

String getValueforTag(String line, String tag) {
  String matchString;
  if (line.endsWith("</" + tag + ">")) {
    matchString = line;
    int idx = matchString.indexOf(tag + ">");
    int lineStart = idx + tag.length() + 1;
    int lineEnd = matchString.indexOf("</" + tag + ">");
    return matchString.substring(lineStart, lineEnd);
  } else {
    return "";
  }
}

String getValueforParameter(String line, String param) {
  int idx = line.indexOf(param + "=");
  if (idx != -1) {
    int paramStart = idx + param.length() + 2;
    int paramEnd = line.indexOf('"', paramStart);
    return line.substring(paramStart, paramEnd);
  }
  return "";
}

bool isDrinkingWeather(String flight_category, int wind_speed, String ceiling) {
  if (flight_category == "IFR" || flight_category == "LIFR" || ceiling.toInt() < 3000 || wind_speed > 25) {
    setDrinkingWeatherLights();
    return true;
  }
  setFlyingWeatherLights();
  return false;
}

void fetchWeather() {
  // Use WiFiClientSecure to create TLS connection
  WiFiClientSecure client;

  Serial.printf("\nConnecting to %s ...", host);
  Serial.print("\nFetching weather for...");
  Serial.println(identifier);

  // initialize SHA-256 fingerprint
  Serial.printf("Using fingerprint '%s'\n", fingerprint);
  client.setFingerprint(fingerprint);

  // connect to host
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }
  
  Serial.println("connected");
  
  Serial.println("[Sending a request]");
     client.print(String("GET ") + url + identifier + " HTTP/1.1\r\n" +
                  "Host: " + host + "\r\n" +
                  "User-Agent: BuildFailureDetectorESP8266\r\n" +
                  "Connection: close\r\n" +
                  "\r\n"
                  );

     // debug response to console
     Serial.println("[Response:]");

     // parse the server response
     while (client.connected() || client.available()){
        if (client.available()){
          String line = client.readStringUntil('\r');
//          Serial.println(line);
          String value = getValueforTag(line, "flight_category");
          if (value.length() > 0) {
            flight_category = value;
          }
          value = getValueforTag(line, "visibility_statute_mi");
          if (value.length() > 0) {
            visibility = value;
          }
          // get the sky cover and cloud base
          value = getValueforParameter(line, "sky_cover");
          if (value.length() > 0) {
            sky_cover = value;
            String base = getValueforParameter(line, "cloud_base_ft_agl");
            ceiling = "50000"; //initialize celing to a high value
            // if the sky cover is broken or overcast, we have a ceiling
            if (sky_cover == "BKN" || sky_cover == "OVC") {
              if (base.length() > 0) {
                cloud_base = base;
                if (cloud_base.toInt() < ceiling.toInt()) {
                  ceiling = cloud_base;
                }
              }
            } else {
              cloud_base = base;
            }
          }
          value = getValueforTag(line, "wind_speed_kt");
          if (value.length() > 0) {
            wind_speed = value.toInt();
          }
        }
     } 
     
     // close the connection
     Serial.println("visibility: " + visibility + ", flight category: " + flight_category);
     Serial.println("sky cover: " + sky_cover + ", cloud base: " + cloud_base);
     Serial.println("ceiling: " +  (ceiling.toInt() >= 50000 ? "None" : ceiling + " feet"));
     Serial.println("wind speed: " + String(wind_speed) + " knots");
     Serial.println(isDrinkingWeather(flight_category, wind_speed, ceiling) ? "Drinking Weather" : "Flying Weather");
     client.stop();
     Serial.println("[Disconnected]");
}

void handleRoot() {
  server.send(200, "text/html", html_1);
}

void handleGetIp() {
  server.send(200, "text/plain", WiFi.localIP().toString());
}

void handleDrinkingToggle() {
  timerActive = false;
  setDrinkingWeatherLights();
  Serial.println("setting drinking weather...");
  server.send(200, "text/plain", "Drinking Weather");
}

void handleFlyingToggle() {
  timerActive = false;
  setFlyingWeatherLights();
  Serial.println("setting flying weather...");
  server.send(200, "text/plain", "Flying Weather");
}

void handleWeatherForm() {
   if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
    
  } else {

    // fetch weather from ADDS server
    if (server.argName(0) == "station_id") {
      identifier = server.arg(0);
      Serial.println("fetching weather for: " + identifier); 
      fetchWeather();
      
      // serialize response to JSON 
      const size_t capacity = JSON_OBJECT_SIZE(7);
      DynamicJsonDocument doc(capacity);
    
      doc["identifier"] = identifier;
      doc["flight_category"] = flight_category;
      doc["ceiling"] = (ceiling.toInt() >= 50000 ? "None" : ceiling + " feet");
      doc["wind_speed"] = String(wind_speed) + " knots";
      doc["isDrinkingWeather"] = isDrinkingWeather(flight_category, wind_speed, ceiling);

      serializeJson(doc, Serial);
      String docString;
      serializeJson(doc, docString);

      // send response to client
      server.send(200, "JSON", docString);
    }
  }
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void) {
  // set button
  pinMode(buttonPin, INPUT_PULLUP);
  
  // initialize serial port
  Serial.begin(115200);
  Serial.println("");

  // initialize NeoPixels
  strip.begin();
  strip.show();
  
  // initialize response handlers
  server.on("/", handleRoot);

  server.on("/station_id/", handleWeatherForm);

  server.on("/DRINKING", handleDrinkingToggle);

  server.on("/FLYING", handleFlyingToggle);

  server.on("/ip_address", handleGetIp);

  server.onNotFound(handleNotFound);

  // configure OTA updates
  config.ota = AC_OTA_BUILTIN;
  Portal.config(config);
  
  // start http server
  Portal.begin();
  Serial.println("HTTP server started: " +
          WiFi.localIP().toString());

  // start MDNS server
  if (MDNS.begin("weather")) {
    Serial.println("MDNS responder started");
  }

  // start timer
  timerActive = true;
}

void loop(void) {
  
  // if the button is pressed, manually toggle the 
  // flying and drinking lights
  if (digitalRead(buttonPin) == LOW) {
    Serial.println("Button pressed...");
    
    // stop weather updates
    timerActive = false;

    // toggle the drinking and flying weather lights
    green_strip = strip.getPixelColor(25); // 1st LED top strip
    red_strip = strip.getPixelColor(74); // 1st LED bottom strip
    
    if (red_strip == 0 && green_strip == 0){
      Serial.println("Setting drinking weather...");
      setDrinkingWeatherLights();
    } else if (red_strip == strip.Color(255, 0, 0)){
      Serial.println("Setting flying weather...");
      setFlyingWeatherLights();
    } else if (green_strip == strip.Color(0, 255, 0)){
      Serial.println("Clearing strip...");
      strip.clear();
      strip.show();
    }
    // delay a short time to avoid invoking twice
    delay(500);
  }

  // check the weather once every minute
  if (timerActive && (millis() % 60000) == 0) {
    fetchWeather();
  }

  // check for client requests
  Portal.handleClient();
  MDNS.update();
}
