const int HALL_ANALOG_PIN = A1; // SS49E connected here
const int THRESHOLD = 15;       // Sensitivity: lower is more sensitive
int baseValue = 0;              // The "quiet" state of the biased sensor

void setup() {
  Serial.begin(9600);
  
  // Calibrate the base field (ensure no metal is near at startup)
  Serial.println("Calibrating base magnetic field...");
  long sum = 0;
  for(int i = 0; i < 50; i++) {
    sum += analogRead(HALL_ANALOG_PIN);
    delay(10);
  }
  baseValue = sum / 50;
  Serial.print("Base Value Set: "); Serial.println(baseValue);
}

void loop() {
  int currentValue = analogRead(HALL_ANALOG_PIN);
  int difference = abs(currentValue - baseValue);

  // If the change is greater than our threshold, metal is detected!
  if (difference > THRESHOLD) {
    Serial.print("!!! METAL DETECTED !!! Change: ");
    Serial.println(difference);
    // You could trigger a buzzer or stop the grain flow here
  }

  delay(50); // Small delay for stability
}
