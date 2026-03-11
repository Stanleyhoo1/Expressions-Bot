#include <Wire.h>        // Arduino library for I2C communication
#include <Motoron.h>     // Pololu Motoron motor controller library

MotoronI2C mc(0x11);     // Create a Motoron controller object using I2C address 0x11

// Encoder pins
const int encA = 2;      // Encoder channel A connected to Arduino pin 2 (interrupt pin)
const int encB = 4;      // Encoder channel B connected to Arduino pin 4

// Number of encoder counts per full output shaft rotation
// Determined experimentally (~144 counts per revolution)
const long countsPerRevolution = 144;

volatile long encoderCount = 0; // Stores the current encoder count
                                // 'volatile' because it is modified inside an interrupt

// Interrupt function that runs whenever encoder channel A has a rising edge
void handleEncA()
{
  // Check encoder channel B to determine direction
  if (digitalRead(encB) == HIGH) // If B is HIGH
    encoderCount++;              // motor rotating forward → increase count
  else
    encoderCount--;              // motor rotating backward → decrease count
}

void setup()
{
  Wire.begin();                  // Start I2C communication
  Serial.begin(115200);          // Start serial communication for debugging

  mc.reinitialize();             // Reset Motoron controller to default settings
  mc.disableCrc();               // Disable CRC checking for simpler communication
  mc.clearResetFlag();           // Clear reset error flag inside Motoron

  mc.setMaxAcceleration(1, 200); // Limit how fast motor 1 can accelerate
  mc.setMaxDeceleration(1, 300); // Limit how fast motor 1 can decelerate

  mc.disableCommandTimeout();    // Prevent Motoron from stopping motor if commands stop

  pinMode(encA, INPUT_PULLUP);   // Set encoder A as input with internal pull-up resistor
  pinMode(encB, INPUT_PULLUP);   // Set encoder B as input with internal pull-up resistor

  // Attach interrupt to encoder A
  // When pin 2 detects a rising edge, run handleEncA()
  attachInterrupt(digitalPinToInterrupt(encA), handleEncA, RISING);
}

void loop()
{
  mc.setSpeed(1, 0);             // Command motor 1 to stop (speed range is -800 to 800)

  long count;

  noInterrupts();                // Temporarily disable interrupts
  count = encoderCount;          // Copy encoder count safely
  interrupts();                  // Re-enable interrupts

  // Convert encoder counts to angle in degrees
  float angleDeg = (count * 360.0) / countsPerRevolution;

  Serial.print("Count: ");       // Print encoder count
  Serial.print(count);

  Serial.print("   Angle (deg): ");
  Serial.println(angleDeg);      // Print calculated angle

  delay(100);                    // Wait 100 ms before next update
}