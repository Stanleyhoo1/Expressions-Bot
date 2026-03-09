#include "Arduino_LED_Matrix.h"
ArduinoLEDMatrix matrix; // initialise object of LED matrix class
int sensorValue = 0; // variable for raw sensor readings
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  matrix.begin(); // start LED matrix
}
void loop(){
  int sensorValue = analogRead(A0);
  Serial.println(sensorValue); // With resistor from 0-1000 so maybe mult by 10 to get resistance? No resistor just 1-20
  delay(100);
}