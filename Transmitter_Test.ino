#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid     = "ESP_TEST_AP";
const char* password = "87654321";

const char* receiverIP = "192.168.4.1";

void setup() {
  Serial.begin(115200);
  delay(2000);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Serial.println("EA connected");
}

void loop() {
  WiFiClient client;
  HTTPClient http;

  long rssi = WiFi.RSSI();

  String url = String("http://") + receiverIP +
               "/data?rssi=" + String(rssi);

  http.begin(client, url);
  http.GET();
  http.end();

  delay(2000);
}