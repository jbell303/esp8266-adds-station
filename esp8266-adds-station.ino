#include <ESP8266WiFi.h>

// WIFI
const char* ssid =  "JamesBell'sNetwork_2.4";
const char* password = "Ju51!3@Io";

// ADDS
const char* host = "aviationweather.gov";
const int httpsPort = 443;
const char* fingerprint = "1ce610e06d392674ee443a469b449977aca3d472";
const char* url = "/adds/dataserver_current/httpparam?dataSource=metars&requestType=retrieve&format=xml&hoursBeforeNow=3&mostRecent=true&stationString=KNFL";

void setup() {
  Serial.begin(115200);

  // initialize GPIO pins
  pinMode(4, OUTPUT);
  pinMode(16, OUTPUT);

  // connect to wifi
  connectToWifi();
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

  // Use WiFiClientSecure to create TLS connection
  WiFiClientSecure client;

  Serial.printf("\nConnecting to %s ...", host);

  // initialize SHA-256 fingerprint
  Serial.printf("Using fingerprint '%s'\n", fingerprint);
  client.setFingerprint(fingerprint);

  // connect to host
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }
  Serial.println("connected");
  
  // verify fingerprint from host
  if (client.verify(fingerprint, host)) {
     Serial.println("certificate matches");
     Serial.println("[Sending a request]");
     client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                  "Host: " + host + "\r\n" +
                  "User-Agent: BuildFailureDetectorESP8266\r\n" +
                  "Connection: close\r\n" +
                  "\r\n"
                  );

     // debug response to console
     Serial.println("[Response:]");
     String flight_category = "";
     String visibility = "";
     String sky_cover = "";
     String cloud_base = "";
     int wind_speed = 0;
     int ceiling = 50000;
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
          // get the ceiling
          value = getValueforParameter(line, "sky_cover");
          if (value.length() > 0) {
            sky_cover = value;
            String base = getValueforParameter(line, "cloud_base_ft_agl");
            if (base.length() > 0) {
              cloud_base = base;
              if (sky_cover == "BKN" || sky_cover == "OVC") {
                if (cloud_base.toInt() < ceiling) {
                  ceiling = cloud_base.toInt();
                }
              }
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
     Serial.println("ceiling: " +  (ceiling >= 50000 ? "none" : String(ceiling) + " feet"));
     Serial.println("wind speed: " + String(wind_speed) + " knots");
     Serial.println(isDrinkingWeather(flight_category, wind_speed, ceiling) ? "Drinking Weather" : "Flying Weather");
     client.stop();
     Serial.println("[Disconnected]"); 
  } else { // close the connection if certificate does not match
     Serial.println("certificate doesn't match");
     client.stop();
     Serial.println("\n[Disconnected]");
  }
}

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

bool isDrinkingWeather(String flight_category, int wind_speed, int ceiling) {
  if (flight_category == "IFR" || ceiling < 3000 || wind_speed > 25) {
    digitalWrite(4, LOW);    // turn the Red LED ON
    digitalWrite(16, HIGH);    // turn the Green LED OFF
    return true;
  }
  digitalWrite(4, HIGH);
  digitalWrite(16, LOW);
  return false;
}
void loop() {
  
}
