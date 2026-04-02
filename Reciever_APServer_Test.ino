#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ap_ssid     = "ESP_TEST_AP";
const char* ap_password = "87654321";

ESP8266WebServer server(80);

// -------- STORED VALUES --------
float latestTemp = 0;     // holds last temperature received
float latestRSSI = 0;     // holds last RSSI received

// -------- TEMPERATURE (Arduino) --------
void handleTemp() {
  Serial.println("TEMP endpoint hit");

  if (server.hasArg("value")) {
    latestTemp = server.arg("value").toFloat();  // store value

    Serial.print("Temperature: ");
    Serial.println(latestTemp);
  } else {
    Serial.println("No temp value received");
  }

  server.send(200, "text/plain", "OK");
}

// -------- RSSI (EA) --------
void handleData() {
  Serial.println("RSSI endpoint hit");

  if (server.hasArg("rssi")) {
    latestRSSI = server.arg("rssi").toFloat();  // store value

    Serial.print("RSSI: ");
    Serial.println(latestRSSI);
  } else {
    Serial.println("No RSSI value received");
  }

  server.send(200, "text/plain", "OK");
}

// -------- WEBPAGE --------
void handleRoot() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<title>Live Monitor</title>";
  html += "<script>";
  html += "function update(){";
  html += "fetch('/status').then(r=>r.text()).then(data=>{";
  html += "document.getElementById('data').innerHTML = data;";
  html += "});}";
  html += "setInterval(update,1000);";  // refresh every 1 sec
  html += "</script>";
  html += "</head><body onload='update()'>";
  html += "<h2>ESP8266 Live Data</h2>";
  html += "<div id='data'>Waiting for data...</div>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

// -------- DATA ENDPOINT FOR WEBPAGE --------
void handleStatus() {
  String response = "Temperature: " + String(latestTemp, 2) + " C<br>";
  response += "RSSI: " + String(latestRSSI, 0) + " dBm";

  server.send(200, "text/html", response);
}

void setup() {
  Serial.begin(115200);
  delay(2000);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_password);

  Serial.println("AP ready");
  Serial.println(WiFi.softAPIP());

  // Existing endpoints
  server.on("/temp", handleTemp);
  server.on("/data", handleData);

  // New endpoints for webpage
  server.on("/", handleRoot);
  server.on("/status", handleStatus);

  server.begin();
}

void loop() {
  server.handleClient();
}