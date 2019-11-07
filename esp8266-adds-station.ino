
/*
 * Sketch: esp8266-adds-station
 * Intended to be run on an ESP8266
 * 
 * Uses station mode on ESP8266
 */

String header = "HTTP/1.1 200 OK\r\nContent-Type: JSON\r\n\r\n";
String textHeader = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";

String html_1 = R"=====(
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
  function fetchWeather() {
    document.getElementById("submit_button").value = "Fetching weather...";
    station_id = document.getElementById("identifier").value;
    ajaxLoad(station_id);
  }

var ajaxRequest = null;
if (window.XMLHttpRequest) { ajaxRequest = new XMLHttpRequest(); }
                           { ajaxRequest = new ActiveXObject("Microsoft.XMLHTTP"); }

function ajaxLoad(station_id) {
  if (!ajaxRequest) {
    alert("AJAX is not supported.");
    return;
  }

  ajaxRequest.open("POST", "station_id=" + station_id, true);
  ajaxRequest.onreadystatechange = function () {
    if (ajaxRequest.readyState == 4 && ajaxRequest.status == 200) {
      var response = JSON.parse(ajaxRequest.responseText);
      document.getElementById("identifier").value = response.identifier;
      document.getElementById("flight_category").innerHTML = "Flight Category: " + response.flight_category;
      document.getElementById("ceiling").innerHTML = "Ceiling: " + response.ceiling;
      document.getElementById("wind_speed").innerHTML = "Wind speed: " + response.wind_speed;
      document.getElementById("submit_button").value = "Fetch Weather";
      document.getElementById("toggle_button").value = (response.isDrinkingWeather ? "Set Flying Weather" : "Set Drinking Weather");
    }
  }
  ajaxRequest.send();
}

function toggleLed () {
  var button_text = document.getElementById("toggle_button").value;
  if (button_text == "Set Drinking Weather") {
    document.getElementById("toggle_button").value = "Setting Drinking Weather";
    ajaxLED('DRINKING');
  } else {
    document.getElementById("toggle_button").value = "Setting Flying Weather";
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
      } else if (ajaxResult == "Drinking Weather") {
        document.getElementById("toggle_button").value = "Set Flying Weather";
      }
    }
  }
  ajaxRequest.send();
}
</script>

  <title>esp8266 ADDS Station</title>
</head>

<body>
  <div id='main'>
    <h2>esp8266 ADDS Station</h2>
    <p id="flight_category">Flight Category: </p>
    <p id="ceiling">Ceiling: </p>
    <p id="wind_speed">Winds: </p>
    <form>
      <div>
        <form action="javascript:fetchWeather()">
          <label for="name">Identifier:</label>
          <input type="text" id="identifier" name="station_id">
          <input type="button" id="submit_button" onclick="fetchWeather()" value="Fetch Weather" />
        </form>
      </div>
     </form>
  </div>
  <div>
    <input type="button" id="toggle_button" onclick="toggleLed()" value="Set Flying Weather"/>
  </div>
</body>
</html>

)=====";

#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
//#include <ESP8266mDNS.h>

// WIFI
const char* ssid =  "JamesBell'sNetwork_2.4";
const char* password = "Ju51!3@Io";

WiFiServer server(80);

// ADDS
const char* host = "aviationweather.gov";
const int httpsPort = 443;
const char* fingerprint = "29f037bace863c943c3bdf2ce5303a243eaef491";
String identifier = "KNFL";
String url = "/adds/dataserver_current/httpparam?dataSource=metars&requestType=retrieve&format=xml&hoursBeforeNow=3&mostRecent=true&stationString=";

String flight_category = "";
String visibility = "";
String sky_cover = "";
String cloud_base = "";
int wind_speed = 0;
String ceiling = "";

// NeoPixels
#define LED_PIN    5
#define LED_COUNT    102

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// button
const int buttonPin = 13;
uint16_t n = 0;
uint32_t green_strip = 0;
uint32_t red_strip = 0;

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
  if (flight_category == "IFR" || ceiling.toInt() < 3000 || wind_speed > 25) {
    setDrinkingWeatherLights();
    return true;
  }
  setFlyingWeatherLights();
  return false;
}

void fetchWeather() {
  // Use WiFiClientSecure to create TLS connection
  WiFiClientSecure client;
//  BearSSL::WiFiClientSecure client;
//  client.setInsecure();

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
//   if (!client.connect(host, 80)) {
//    Serial.println("connection failed");
//    return;
//  }
  Serial.println("connected");
  
  // verify fingerprint from host
//  if (client.verify(fingerprint, host)) {
//     Serial.println("certificate matches");
//      
//  } else { // close the connection if certificate does not match
//     Serial.println("certificate doesn't match");
//     client.stop();
//     Serial.println("\n[Disconnected]");
//  }
  Serial.println("[Sending a request]");
     client.print(String("GET ") + url + identifier + " HTTP/1.1\r\n" +
                  "Host: " + host + "\r\n" +
                  "User-Agent: BuildFailureDetectorESP8266\r\n" +
                  "Connection: close\r\n" +
                  "\r\n"
                  );

     // debug response to console
     Serial.println("[Response:]");
     
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
     } // close the connection
     Serial.println("visibility: " + visibility + ", flight category: " + flight_category);
     Serial.println("sky cover: " + sky_cover + ", cloud base: " + cloud_base);
     Serial.println("ceiling: " +  (ceiling.toInt() >= 50000 ? "None" : ceiling + " feet"));
     Serial.println("wind speed: " + String(wind_speed) + " knots");
     Serial.println(isDrinkingWeather(flight_category, wind_speed, ceiling) ? "Drinking Weather" : "Flying Weather");
     client.stop();
     Serial.println("[Disconnected]");
}

void connectToWifi(){
  // connect to wifi
  Serial.println("connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // connect using wifimanager
//  WiFiManager wifiManager;
//  wifiManager.autoConnect("WeatherStation");
  
  Serial.println("Connected.");

  //try mDNS
//  if (!MDNS.begin("weatherstation"))  { Serial.println("Error setting up mDNS responder!"); }
//                                      { Serial.println("mDNS responder started!"); }
  
  // start a server
  server.begin();
  Serial.println("Server started"); 
}

void setFlyingWeatherLights () {
  strip.clear();
  strip.fill(strip.Color(2, 2, 2), 0, 25);
  strip.fill(strip.Color(0, 0, 0), 25, 26);
  strip.fill(strip.Color(2, 2, 2), 51, 25);
  strip.fill(strip.Color(0, 150, 0), 76, 26);
  strip.show();
}

void setDrinkingWeatherLights () {
  strip.clear();
  strip.fill(strip.Color(2, 2, 2), 0, 25);
  strip.fill(strip.Color(150, 0, 0), 25, 26);
  strip.fill(strip.Color(2, 2, 2), 51, 25);
  strip.fill(strip.Color(0, 0, 0), 76, 26);
  strip.show();
}

unsigned long timer;
bool timerActive;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(115200);

  String request = "";

  // initialize NeoPixels
  strip.begin();
  strip.show();
//  strip.setBrightness(50);

  // connect to wifi
  connectToWifi();

  // start the timer
  timerActive = true;
}

void loop() {

  // toggle button
  if (digitalRead(buttonPin) == LOW) {
    Serial.println("Button pressed...");
    timerActive = false;
    green_strip = strip.getPixelColor(76);
    red_strip = strip.getPixelColor(25);
//    Serial.print("Color: ");
//    Serial.println(green_strip);
//    Serial.println(red_strip);
//    n = strip.numPixels();
//    Serial.print("n: ");
//    Serial.println(n);
    if (red_strip == 0 && green_strip == 0){
      Serial.println("Setting drinking weather...");
      setDrinkingWeatherLights();
    } else if (red_strip == strip.Color(150, 0, 0)){
      Serial.println("Setting flying weather...");
      setFlyingWeatherLights();
    } else if (green_strip == strip.Color(0, 150, 0)){
      Serial.println("Clearing strip...");
      strip.clear();
      strip.show();
    }
    delay(500);
  }

  if (timerActive && (millis() % 60000) == 0) {
    fetchWeather();
  }
  
  // check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // read the first line of the request
  String request = client.readStringUntil('\r');

  Serial.print("request: ");
  Serial.println(request);

  if (request.indexOf("station_id") > 0) {
    // fetch weather for requested station id
    int idx = request.indexOf("station_id=");
    if (idx != -1) {
      int paramStart = idx + 11; // station_id=
      int paramEnd = paramStart + 4;
      identifier = request.substring(paramStart, paramEnd);
    }
    timerActive = true;
    fetchWeather();

    // JSON 
    const size_t capacity = JSON_OBJECT_SIZE(7);
    DynamicJsonDocument doc(capacity);
    
    // serialize json response
    doc["identifier"] = identifier;
    doc["flight_category"] = flight_category;
    doc["ceiling"] = (ceiling.toInt() >= 50000 ? "None" : ceiling + " feet");
    doc["wind_speed"] = String(wind_speed) + " knots";
    doc["isDrinkingWeather"] = isDrinkingWeather(flight_category, wind_speed, ceiling);

    serializeJson(doc, Serial);

    // send response to client
    client.print(header);
    serializeJson(doc, client); 
  } else if (request.indexOf("DRINKING") > 0) {
    timerActive = false;
    setDrinkingWeatherLights();
    client.print(textHeader);
    client.print("Drinking Weather");
  } else if (request.indexOf("FLYING") > 0) {
    timerActive = false;
    setFlyingWeatherLights();
    client.print(textHeader);
    client.print("Flying Weather");
  } else {
    client.flush();
    client.print(header);
    client.print(html_1);
    delay(5);
  }
}
