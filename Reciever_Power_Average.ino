#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <math.h>

const char* ap_ssid     = "Bowles1";
const char* ap_password = "87654321";

ESP8266WebServer server(80);

const int numReadings = 300;
float readings[numReadings];
int readIndex = 0;
float total = 0;
float latestRawRSSI = 0;
float averageRSSI = 0;
float power_mW = 0;
float moisture = 0;
float temperature = 0;

void handleRoot() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<title>Moisture Monitor</title>";
  html += "<style>body{font-family:sans-serif; text-align:center; padding-top:50px;} .box{border:1px solid #ccc; padding:20px; display:inline-block; border-radius:10px; background:#f9f9f9;} #val{font-size:1.2em; color:#333; text-align:left;}</style>";
  html += "<script>";
  html += "function update(){";
  html += "fetch('/power').then(r=>r.text()).then(data=>{";
  html += "document.getElementById('val').innerHTML = data;";
  html += "});}";
  html += "setInterval(update, 2000);";
  html += "</script>";
  html += "</head><body onload='update()'>";
  html += "<h2>Moisture Analytics (2s Refresh)</h2>";
  html += "<div class='box'><div id='val'>Waiting for data...</div></div>";
  html += "<p style='color:grey;'>30s rolling average</p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handlePower() {
  String response = "<b>Raw RSSI:</b> " + String(latestRawRSSI, 1) + " dBm<br>";
  response += "<b>Avg RSSI:</b> " + String(averageRSSI, 2) + " dBm<br>";
  response += "<b>Power:</b> " + String(power_mW, 6) + " mW<br>";
  response += "<b>Moisture:</b> " + String(moisture, 2) + " %<br>";
  response += "<b>Temperature:</b> " + String(temperature, 2) + " &deg;C";
  server.send(200, "text/plain", response);
}

void handleData() {
  if (server.hasArg("rssi")) {
    latestRawRSSI = server.arg("rssi").toFloat();

    total = total - readings[readIndex];
    readings[readIndex] = latestRawRSSI;
    total = total + readings[readIndex];
    readIndex = (readIndex + 1) % numReadings;

    averageRSSI = total / numReadings;
    power_mW = pow(10.0, averageRSSI / 10.0);

    if (power_mW > 0) {
      moisture = 8.52 * log(0.25 / power_mW);
      if (moisture < 0) moisture = 0;
      if (moisture > 100) moisture = 100;
    }
  }
  server.send(200, "text/plain", "OK");
}

// --- New temperature endpoint ---
void handleTemp() {
  if (server.hasArg("value")) {
    temperature = server.arg("value").toFloat();
  }
  server.send(200, "text/plain", "OK");
}

void setup() {
  for (int i = 0; i < numReadings; i++) readings[i] = 0;

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_password);

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.on("/power", handlePower);
  server.on("/temp", handleTemp);  // <-- New route

  server.begin();
}

void loop() {
  server.handleClient();
}