#include <Wire.h>
#include <Adafruit_BME280.h>

// Pins for Arduino MKR 1010
const int MOISTURE_PIN = A1; // SS49E Hall Effect or Probe
Adafruit_BME280 bme; 

// Calibration Constants
float tempCoefficient = 0.05; // Adjust this based on testing
float referenceTemp = 25.0;   // Standard room temp in Celsius

void setup() {
  Serial.begin(9600);
  Wire.begin(); // Initializes Pins 11 and 12

  if (!bme.begin(0x76)) {
    Serial.println("BME280 not found! Check SDA(11) and SCL(12)");
    while (1);
  }
}

void loop() {
  // 1. Read raw values
  float rawMoisture = analogRead(MOISTURE_PIN);
  float currentTemp = bme.readTemperature();

  // 2. Calculate Correction
  // If temp is higher than 25°C, this value is positive (subtracts from reading)
  // If temp is lower than 25°C, this value is negative (adds to reading)
  float correction = (currentTemp - referenceTemp) * tempCoefficient;
  
  // 3. Apply Calibration
  float correctedMoisture = rawMoisture - correction;

  // 4. Output to Serial
  Serial.print("Temp: "); Serial.print(currentTemp); Serial.print("C | ");
  Serial.print("Raw: "); Serial.print(rawMoisture); Serial.print(" | ");
  Serial.print("Corrected: "); Serial.println(correctedMoisture);

  delay(2000);
}
