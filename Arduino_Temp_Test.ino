#include <WiFiNINA.h>
#include <Wire.h>
#include "SparkFunBME280.h"

BME280 mySensor;

// WiFi (must match EB AP)
const char* ssid = "ESP_TEST_AP";
const char* password = "87654321";

// EB IP
const char* ebIP = "192.168.4.1";

WiFiClient client;

void setup() {
  Serial.begin(115200);
  delay(2000);

  // Initialize I2C + sensor
  Wire.begin();
  if (mySensor.beginI2C() == false) {
    Serial.println("BME280 not detected. Check wiring.");
    while (1);
  }

  Serial.println("Connecting to AP...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nCONNECTED (Arduino)");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {

  // -------- READ ACTUAL TEMPERATURE --------
  float tempC = mySensor.readTempC();   // use readTempF() if you want Fahrenheit

  // -------- SEND TO EB --------
  if (client.connect(ebIP, 80)) {

    String request = String("GET /temp?value=") + tempC + " HTTP/1.1\r\n" +
                     "Host: " + ebIP + "\r\n" +
                     "Connection: close\r\n\r\n";

    client.print(request);

    Serial.print("Sent temperature: ");
    Serial.println(tempC);

    delay(100);  // allow transmission

    while (client.available()) {
      client.read();  // clear response
    }

    client.stop();
  } else {
    Serial.println("Connection to EB failed");
  }

  delay(2000);  // send every 2 seconds (same as before)
}