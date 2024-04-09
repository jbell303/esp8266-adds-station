#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266WebServer.h>
 #include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include <AutoConnect.h>
#include <Adafruit_NeoPixel.h>
#include "html.h"
//#include "cert.h"

// initialize webserver
ESP8266WebServer server(80);
AutoConnect Portal(server);
AutoConnectConfig config;
// const char mdns_name[] PROGMEM = "weather"; // .local

// initialize ADDS parameters
const char* adds_host = "aviationweather.gov";
const uint16_t port = 443;
//X509List cert(cert_DigiCert_Global_Root_CA);
//String url = "/adds/dataserver_current/httpparam?dataSource=metars&requestType=retrieve&format=xml&hoursBeforeNow=3&mostRecent=true&stationString=";
const char url[] PROGMEM = "/api/data/dataserver?requestType=retrieve&dataSource=metars&format=xml&hoursBeforeNow=3&mostRecent=true&stationString=";

// initialize BearSSL client
WiFiClientSecure client;

// initialize METAR data
char identifier[5] = "KNFL";
char flight_category[5];
char visibility[10];
char sky_cover[10];
char cloud_base[10];
int wind_speed = 0;
int ceiling = 50000;

#define MIN_CEILING 3000;
#define MAX_WIND_SPEED 25;
int min_ceiling = MIN_CEILING;
int max_wind_speed = MAX_WIND_SPEED;

// intitialize timer for auto updates
bool timerActive;

// initialize NeoPixels
#define LED_PIN    5
#define LED_COUNT  100
uint32_t green_strip = 0; // for color evaluation
uint32_t red_strip = 0;

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// initialize button parameters
const int buttonPin = 13; // button for manual toggle

// set the lights for flying weather
void setFlyingWeatherLights () {
  strip.clear();
  strip.fill(strip.Color(255, 255, 255), 0, 25); // left strip white
  strip.fill(strip.Color(0, 255, 0), 25, 25); // top strip green
  strip.fill(strip.Color(255, 255, 255), 50, 24); // right strip white
  strip.fill(strip.Color(0, 0, 0), 74, 26); // bottom strip off
  strip.show();
}

// set the lights for drinking weather
void setDrinkingWeatherLights () {
  strip.clear();
  strip.fill(strip.Color(255, 255, 255), 0, 25); // left white
  strip.fill(strip.Color(0, 0, 0), 25, 25); // top off
  strip.fill(strip.Color(255, 255, 255), 50, 24); // right white
  strip.fill(strip.Color(255, 0, 0), 74, 26); // bottom red
  strip.show();
}

// parses XML data values based on tag provided
void getValueForTag(const char* line, const char* tag, char* value, int valueSize) {
  // Find the starting index of the tag
  const char* tagStart = strstr(line, tag);
  if (tagStart == nullptr) {
    value[0] = '\0'; // Empty string if tag is not found
    return;
  }

  // Find the starting index of the tag value
  const char* valueStart = strchr(tagStart, '>') + 1;
  if (valueStart == nullptr) {
    value[0] = '\0'; // Empty string if value is not found
    return;
  }

  // Find the ending index of the tag value
  const char* valueEnd = strstr(valueStart, "</");
  if (valueEnd == nullptr) {
    value[0] = '\0'; // Empty string if end tag is not found
    return;
  }

  // Calculate the length of the value
  int valueLength = valueEnd - valueStart;
  if (valueLength >= valueSize) {
    valueLength = valueSize - 1; // Ensure it fits in the provided buffer
  }

  // Copy the value to the provided buffer
  strncpy(value, valueStart, valueLength);
  value[valueLength] = '\0'; // Null-terminate the string
}

void getValueForParameter(const char* line, const char* param, char* value, int valueSize) {
  // Find the starting index of the parameter
  const char* paramStart = strstr(line, param);
  if (paramStart == nullptr) {
    value[0] = '\0'; // Empty string if parameter is not found
    return;
  }

  // Find the starting index of the parameter value
  const char* valueStart = strchr(paramStart, '=') + 1;
  if (valueStart == nullptr) {
    value[0] = '\0'; // Empty string if value is not found
    return;
  }

  // Find the ending index of the parameter value
  const char* valueEnd = strchr(valueStart, '"');
  if (valueEnd == nullptr) {
    value[0] = '\0'; // Empty string if end quote is not found
    return;
  }

  // Calculate the length of the value
  int valueLength = valueEnd - valueStart;
  if (valueLength >= valueSize) {
    valueLength = valueSize - 1; // Ensure it fits in the provided buffer
  }

  // Copy the value to the provided buffer
  strncpy(value, valueStart, valueLength);
  value[valueLength] = '\0'; // Null-terminate the string
}

bool isDrinkingWeather(const char* flight_category, int wind_speed, int ceiling) {
  if (flight_category == "IFR" || flight_category == "LIFR" || ceiling < min_ceiling || wind_speed > max_wind_speed) {
    setDrinkingWeatherLights();
    return true;
  }
  setFlyingWeatherLights();
  return false;
}

void fetchWeather() {
  // Use WiFiClientSecure to create TLS connection
  Serial.printf("\nConnecting to %s ...\n", adds_host);

  // connect using root certificate
//   Serial.printf("Using certificate: %s\n", cert_DigiCert_Global_Root_CA);
//   client.setTrustAnchors(&cert);
   client.setInsecure();

  // debug: get the free heap
  Serial.print("[FetchWeather()] Free heap is: ");
  Serial.println(ESP.getFreeHeap());

  // set buffer sizes
  client.setBufferSizes(1024, 1024);

  // connect to host
  if (!client.connect(adds_host, port)) {
    Serial.println("connection failed");
    return;
  }
  
  Serial.println("connected");
  
  Serial.print("\nFetching weather for...");
  Serial.println(identifier);
  Serial.println("[Sending a request]");
  
  // Construct the request message
  const char* getRequestTemplate = "GET %s%s HTTP/1.1\r\nHost: %s\r\nUser-Agent: WeatherFetcherESP8266\r\nConnection: close\r\n\r\n";
  char getRequest[strlen(url) + strlen(identifier) + strlen(adds_host) + strlen(getRequestTemplate)];
  sprintf(getRequest, getRequestTemplate, url, identifier, adds_host);
  client.print(getRequest);

  // debug response to console
  Serial.println("[Response:]");

  // Parse the server response
  char line[256]; // Buffer for each line
  char value[256]; // Buffer for tag value
  while (client.connected() || client.available()) {
    if (client.available()) {
      memset(line, 0, sizeof(line)); // Clear the line buffer
      size_t bytesRead = client.readBytesUntil('\r', line, sizeof(line) - 1);
      if (bytesRead == 0) {
        break; // No more data available
      }
      line[bytesRead] = '\0'; // Null-terminate the string

      getValueForTag(line, "flight_category", value, sizeof(value));
      if (value[0] != '\0') {
        strcpy(flight_category, value);
      }
      getValueForTag(line, "visibility_statute_mi", value, sizeof(value));
      if (value[0] != '\0') {
        strcpy(visibility, value);
      }
      // Get the sky cover and cloud base
      getValueForParameter(line, "sky_cover", value, sizeof(value));
      if (value[0] != '\0') {
        strcpy(sky_cover, value);
        getValueForParameter(line, "cloud_base_ft_agl", value, sizeof(value));
        if (value[0] != '\0') {
          strcpy(cloud_base, value);
          // If the sky cover is broken or overcast, we have a ceiling
          if (sky_cover == "BKN" || sky_cover == "OVC") {
            if (atoi(cloud_base) < ceiling) {
              ceiling = atoi(cloud_base);
            }
          }
        }
      }
      getValueForTag(line, "wind_speed_kt", value, sizeof(value));
      if (value[0] != '\0') {
        wind_speed = atoi(value);
      }
    }
  } 
  
  // Close the connection
  Serial.println("visibility: " + String(visibility) + ", flight category: " + String(flight_category));
  Serial.println("sky cover: " + String(sky_cover) + ", cloud base: " + String(cloud_base));
  Serial.println("ceiling: " +  (ceiling >= 50000 ? "None" : String(ceiling)) + " feet");
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
      server.arg(0).toCharArray(identifier, sizeof(identifier));
      Serial.println("fetching weather for: " + String(identifier)); 
      fetchWeather();
      
      // serialize response to JSON 
      const size_t capacity = JSON_OBJECT_SIZE(9);
      DynamicJsonDocument doc(capacity);
    
      doc["identifier"] = identifier;
      doc["flight_category"] = flight_category;
      doc["ceiling"] = (ceiling >= 50000 ? "None" : ceiling + " feet");
      doc["visibility"] = String(visibility) + " mi";
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

void setup() {
  // set button
  pinMode(buttonPin, INPUT_PULLUP);
  
  // initialize serial port
  Serial.begin(115200);
  Serial.println("");

  // debug: get the free heap
  Serial.print("[Setup()] Free heap is: ");
  Serial.println(ESP.getFreeHeap());

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

  // Set time via NTP, as required for x.509 validation
//   configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
//
//   Serial.print("Waiting for NTP time sync: ");
//   time_t now = time(nullptr);
//   while (now < 8 * 3600 * 2) {
//     delay(500);
//     Serial.print(".");
//     now = time(nullptr);
//   }
//   Serial.println("");
//   struct tm timeinfo;
//   gmtime_r(&now, &timeinfo);
//   Serial.print("Current time: ");
//   Serial.print(asctime(&timeinfo));

  // start MDNS server
   if (!MDNS.begin("weather")) {
     Serial.println("Error setting up MDNS responder!");
     while (1) {
       delay(1000);
     }
   }
   Serial.println("mDNS responder started");

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
