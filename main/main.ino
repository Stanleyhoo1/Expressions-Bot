#include <Wire.h>
#include <Motoron.h>

MotoronI2C mc(0x11);

// ======================================================
// MOTOR + ENCODER SETUP
// ======================================================

const long countsPerRevolution = 144;   // measured experimentally

struct Motor
{
  int motorID;              // Motoron motor number: 1, 2, or 3
  int encA;                 // Encoder channel A pin
  int encB;                 // Encoder channel B pin
  volatile long count;      // Encoder count
};

// Motor 1 uses encoder pins D2 and D4
Motor motor1 = {1, 2, 4, 0};
Motor motor2 = {2, 12, 13, 0};

// Interrupt handler for motor 1 encoder
void handleEncA1()
{
  if (digitalRead(motor1.encB) == HIGH)
    motor1.count++;
  else
    motor1.count--;
}

// Rotate one motor by a given number of degrees
// direction = 1 for one direction, -1 for the other
void rotateMotor(Motor &m, int direction, float degrees)
{
  long startCount;
  noInterrupts();
  startCount = m.count;
  interrupts();

  long targetCounts = (abs(degrees) / 360.0) * countsPerRevolution;
  int speed = 300 * direction;

  mc.setSpeed(m.motorID, speed);

  while (true)
  {
    long currentCount;
    noInterrupts();
    currentCount = m.count;
    interrupts();

    if (abs(currentCount - startCount) >= targetCounts)
      break;
  }

  mc.setSpeed(m.motorID, 0);
}

// ======================================================
// DISTANCE SENSOR SETUP
// ======================================================

// Pololu Distance Sensor with Pulse Width Output
const uint8_t sensorPin = 5;

// Filter settings
const int MEDIAN_SAMPLES = 7;   // must be odd
const int AVG_SAMPLES = 5;

int avgBuffer[AVG_SAMPLES];
int avgIndex = 0;
bool avgFilled = false;

// Sort helper for median filter
void sortArray(int arr[], int n)
{
  for (int i = 0; i < n - 1; i++)
  {
    for (int j = i + 1; j < n; j++)
    {
      if (arr[j] < arr[i])
      {
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
      }
    }
  }
}

// Raw distance reading from pulse width output
int readRawDistance()
{
  unsigned long t = pulseIn(sensorPin, HIGH, 50000); // 50 ms timeout

  if (t == 0) return -1;        // timeout
  if (t > 1850) return -1;      // no detection

  int d = (int)(t - 1000) * 4;  // Pololu raw conversion in mm
  if (d < 0) d = 0;

  return d;
}

// Median filter
int readMedianDistance()
{
  int readings[MEDIAN_SAMPLES];
  int validCount = 0;

  for (int i = 0; i < MEDIAN_SAMPLES; i++)
  {
    int val = readRawDistance();
    if (val >= 0)
      readings[validCount++] = val;

    delay(5);
  }

  if (validCount == 0) return -1;

  sortArray(readings, validCount);
  return readings[validCount / 2];
}

// Moving average filter
int movingAverage(int newValue)
{
  if (newValue < 0) return -1;

  avgBuffer[avgIndex] = newValue;
  avgIndex++;

  if (avgIndex >= AVG_SAMPLES)
  {
    avgIndex = 0;
    avgFilled = true;
  }

  int count = avgFilled ? AVG_SAMPLES : avgIndex;
  long sum = 0;

  for (int i = 0; i < count; i++)
    sum += avgBuffer[i];

  return sum / count;
}

// Quadratic calibration
float calibrateDistance(float raw)
{
  float corrected = -0.000023 * raw * raw + 0.5275 * raw + 7.4;

  if (corrected < 0) corrected = 0;
  return corrected;
}

// ======================================================
// SETUP
// ======================================================

void setup()
{
  Wire.begin();
  Serial.begin(115200);

  mc.reinitialize();
  mc.disableCrc();
  mc.clearResetFlag();

  mc.setMaxAcceleration(1, 200);
  mc.setMaxDeceleration(1, 300);

  mc.disableCommandTimeout();

  pinMode(motor1.encA, INPUT_PULLUP);
  pinMode(motor1.encB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(motor1.encA), handleEncA1, RISING);

  pinMode(sensorPin, INPUT);

  for (int i = 0; i < AVG_SAMPLES; i++)
    avgBuffer[i] = 0;
}

// ======================================================
// LOOP
// ======================================================

void loop()
{
  // ------------------------------
  // Distance sensor reading
  // ------------------------------
  int medianRaw = readMedianDistance();
  float corrected = -1;

  if (medianRaw >= 0)
  {
    int smoothRaw = movingAverage(medianRaw);
    corrected = calibrateDistance((float)smoothRaw);

    Serial.print("Distance: ");
    Serial.print(corrected);
    Serial.println(" mm");
  }
  else
  {
    Serial.println("Distance: no reading");
  }

  // ------------------------------
  // Motor behavior based on distance
  // ------------------------------
  if (corrected > 600 || corrected == -1) {
    mc.setSpeed(1, 0);
    mc.setSpeed(2, 800);
  } else {
    mc.setSpeed(1, 800);
    mc.setSpeed(2, 0);
  }

  // ------------------------------
  // Encoder angle reading
  // ------------------------------
  long count;
  noInterrupts();
  count = motor1.count;
  interrupts();

  float angleDeg = (count * 360.0) / countsPerRevolution;

  Serial.print("Count: ");
  Serial.print(count);
  Serial.print("   Angle (deg): ");
  Serial.println(angleDeg);

  delay(50);
}