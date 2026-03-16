#include <Wire.h>
#include <Motoron.h>
#include <LSM6.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>
#include <math.h>

#include "heheheha.h"
#include "mimimimi.h"
#include "hmm.h"
#include "uwu.h"
#include "wilheim_scream.h"
#include "yawn.h"
#include "snore.h"
#include "bruh.h"
#include "annoyed_hmm.h"

#define TFT_CS   10
#define TFT_DC   9
#define TFT_RST  8
#define CLOSE_DISTANCE 200      // mm
#define DARK_THRESHOLD 500      // LDR value
#define BORED_TIME 30 * 1000    // 30 seconds
#define SAD_TIME 60*2*1000      // 2 minutes
#define IMU_SPIKE_THRESHOLD 20  // deg/s or similar

LSM6 imu;
MotoronI2C mc(0x11);

// ======================================================
// MOTOR + ENCODER SETUP
// ======================================================

float happyTarget = 25.0;
int happyDirection = 1;   // 1 means heading toward +45, -1 means heading toward -45
bool motor1Returning = false;
bool motor2Returning = false;
const float THRESHOLD_X = 0.3;
const float THRESHOLD_Y = 0.3;
const float THRESHOLD_Z_LOW = 0.75;

const long countsPerRevolution = 144;   // measured experimentally

struct Motor
{
  int motorID;              // Motoron motor number: 1, 2, or 3
  int encA;                 // Encoder channel A pin
  int encB;                 // Encoder channel B pin
  volatile long count;      // Encoder count
};

// Motor 1 uses encoder pins D2 and D4
Motor motor1 = {1, 2, 3, 0};
Motor motor2 = {2, 12, 4, 0};

const unsigned char* const sound_table[] PROGMEM = {
  he_he_he_ha_clash_royale_deep_fried,
  minecraft_villager_sound_effect,
  cute_uwu,
  VOXScrm_Wilhelm_scream_ID_0477__BigSoundBank_com,
  clash_royale_yawning_emote,
  man_snoring_meme_ctrllNn_1_,
  movie_1,
  mimimi_clash_royale,
  new2009pro_minecraft_villager_sound_15065
};

const unsigned int sound_lengths[] PROGMEM = {
  sizeof(he_he_he_ha_clash_royale_deep_fried),
  sizeof(mimimi_clash_royale),
  sizeof(minecraft_villager_sound_effect),
  sizeof(cute_uwu),
  sizeof(VOXScrm_Wilhelm_scream_ID_0477__BigSoundBank_com),
  sizeof(clash_royale_yawning_emote),
  sizeof(man_snoring_meme_ctrllNn_1_),
  sizeof(movie_1),
  sizeof(new2009pro_minecraft_villager_sound_15065)
};

const int TOTAL_SOUNDS = sizeof(sound_table) / sizeof(sound_table[0]);

void playSound(int soundIndex) {
  // Safety check: Make sure the index actually exists
  if (soundIndex < 0 || soundIndex >= TOTAL_SOUNDS) {
    Serial.println("Error: Sound index out of range!");
    return;
  }

  // Fetch pointer and length from PROGMEM
  const unsigned char* current_sound = (const unsigned char*) pgm_read_ptr(&sound_table[soundIndex]);
  unsigned int current_length = pgm_read_word(&sound_lengths[soundIndex]);

  // Print the results
  // Serial.print("Ready to play sound #");
  // Serial.print(soundIndex);
  // Serial.print(" | Size: ");
  // Serial.print(current_length);
  // Serial.println(" bytes");

  // FIX: Don't use sizeof() here! We already fetched the exact length from PROGMEM above.
  unsigned int totalSamples = current_length; 
  
  for (long i = 0; i < totalSamples; i++) {
    // FIX: Use pgm_read_byte to safely extract the audio data from Flash memory
    byte audioSample = pgm_read_byte(&current_sound[i]);
    
    analogWrite(A0, audioSample); // Send the audio wave to pin A0
    delayMicroseconds(125);       // Wait exactly the right amount of time for 8000Hz
  }
}


// Interrupt handler for motor 1 encoder
void handleEncA1()
{
  if (digitalRead(motor1.encB) == HIGH)
    motor1.count--;
  else
    motor1.count++;
}

// Interrupt handler for motor 2 encoder
void handleEncA2()
{
  if (digitalRead(motor2.encB) == HIGH)
    motor2.count--;
  else
    motor2.count++;
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

  // 1. Kick-start motor
  mc.setSpeed(m.motorID, 600 * direction);
  // delay(100);   // adjust: 20–100 ms depending on motor

  // // 2. Run at slower speed for accuracy
  // mc.setSpeed(m.motorID, 300 * direction);

  while (true)
  {
    long currentCount;
    noInterrupts();
    currentCount = m.count;
    interrupts();

    long moved = abs(currentCount - startCount);

    if (moved >= targetCounts)
      break;
  }

  mc.setSpeed(m.motorID, 0);
}

void returnToNeutral(Motor &m, bool &returningToNeutral)
{
  long count;
  noInterrupts();
  count = m.count;
  interrupts();
  int tolerance = 10;

  float angleDeg = (count * 360.0) / countsPerRevolution;

  float error = 0 - angleDeg;

  if (abs(error) < tolerance)
  {
    mc.setSpeed(m.motorID, 0);
    returningToNeutral = false;   // finished
    return;
  }

  int speed = (abs(error) > 10) ? 600 : 300;

  if (error > 0)
    mc.setSpeed(m.motorID, speed);
  else
    mc.setSpeed(m.motorID, -speed);
}

void bobMotor(Motor &m)
{
  long count;
  noInterrupts();
  count = m.count;
  interrupts();

  float angleDeg = (count * 360.0) / countsPerRevolution;

  if (angleDeg >= 25.0)
    happyTarget = -25.0;
  else if (angleDeg <= -25.0)
    happyTarget = 25.0;

  float error = happyTarget - angleDeg;

  if (error > 0)
    mc.setSpeed(m.motorID, 600);
  else
    mc.setSpeed(m.motorID, -600);
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
// LIGHT SENSOR SETUP
// ======================================================
int lightSensorValue = 0; // variable for raw sensor readings
const int lightPin = A0;

// ======================================================
// EMOTION LOGIC
// ======================================================
unsigned long idleStart = 0;
bool isIdle = false;

enum Emotion{
    HAPPY, // Default, normal/bright light, moderate distance (more than 200mm away)
    CURIOUS, // Detects object moderate distance away (200-1000mm)
    SHY, // Person too close and bright (less than 200mm away)
    STARTLED, // Dark and too close (less than 200mm away)
    TIRED, // Dark and idle (30 secs)
    SLEEPING, // Dark and idle for long period (2 mins)
    BORED, // No interaction for a period, bright (30 secs)
    SAD, // Long inactivity, bright (2 mins)
    ANNOYED // Picked up or moved suddenly (IMU spike)
};

Emotion determineEmotion(unsigned long idleTime, float distance, int lightSensorValue, bool spiked);
void printEmotion(Emotion e);

int consecutiveValidReadings = 0; // distance
const int VALID_READINGS_THRESHOLD = 3; // require 3 in a row

int consecutiveSpikeReadings = 0; // IMU
const int SPIKE_THRESHOLD_COUNT = 3;

Emotion emotion = HAPPY;

bool isDark(int lightSensorValue){
  if (lightSensorValue < 600){
    return true;
  } else {
    return false;
  }
}

bool imuSpikeDetected() {
  imu.read();

  float accel_x = imu.a.x * 0.061 / 1000.0;
  float accel_y = imu.a.y * 0.061 / 1000.0;
  float accel_z = imu.a.z * 0.061 / 1000.0;

  bool spiked = false;

  if (abs(accel_x) > THRESHOLD_X) {
    // Serial.print("ALERT: X-axis acceleration surpassed threshold! Value: ");
    // Serial.println(accel_x);
    spiked = true;
  } else if (abs(accel_y) > THRESHOLD_Y) {
    // Serial.print("ALERT: Y-axis acceleration surpassed threshold! Value: ");
    // Serial.println(accel_y);
    spiked = true;
  } else if (accel_z < THRESHOLD_Z_LOW) {
    // Serial.print("ALERT: Z-axis acceleration dropped below threshold! Value: ");
    // Serial.println(accel_z);
    spiked = true;
  }

  if (spiked) {
    consecutiveSpikeReadings++;
  } else {
    consecutiveSpikeReadings = 0;  // reset on any calm reading
  }

  return consecutiveSpikeReadings >= SPIKE_THRESHOLD_COUNT;
}


Emotion determineEmotion(unsigned long idleTime, float distance, int lightSensorValue, bool spiked) {
  bool dark = isDark(lightSensorValue);

    if (spiked) {
        return ANNOYED;
    }

    if (distance < CLOSE_DISTANCE && distance != -1) {
        if (dark) {
            return STARTLED;
        } else {
            return SHY;
        }
    }

    if (dark && idleTime > SAD_TIME) {
        return SLEEPING;
    }

    if (dark && idleTime > BORED_TIME) {
        return TIRED;
    }

    if (idleTime > SAD_TIME) {
        return SAD;
    }

    if (idleTime > BORED_TIME) {
        return BORED;
    }

    if (distance < 1000 && distance != -1) {
        return CURIOUS;
    }

    return HAPPY; // default
}

void printEmotion(Emotion e) {
  const char* names[] = {
    "HAPPY", "CURIOUS", "SHY", "STARTLED",
    "TIRED", "SLEEPING", "BORED", "SAD", "ANNOYED"
  };
  Serial.println(names[e]);
}

// ======================================================
// LED DISPLAY SETUP
// ======================================================
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

void drawEmotion(Emotion emotion);

// Helper functions
void drawSmile(int x, int y, int r) {
  for (int angle = 20; angle <= 160; angle += 2) {
    float rad = angle * DEG_TO_RAD;
    int px = x + r * cos(rad);
    int py = y + r * sin(rad);
    tft.fillCircle(px, py, 4, ILI9341_BLACK);
  }
}

void drawFrown(int x, int y, int r) {
  for (int angle = 200; angle <= 340; angle += 2) {
    float rad = angle * DEG_TO_RAD;
    int px = x + r * cos(rad);
    int py = y + r * sin(rad);
    tft.fillCircle(px, py, 4, ILI9341_BLACK);
  }
}

void drawClosedEye(int x, int y) {
  tft.drawLine(x - 12, y, x - 4, y - 6, ILI9341_BLACK);
  tft.drawLine(x - 4, y - 6, x + 4, y, ILI9341_BLACK);
  tft.drawLine(x + 4, y, x + 12, y - 6, ILI9341_BLACK);
}

void drawSmallMouth(int x, int y) {
  tft.drawLine(x - 10, y, x + 10, y, ILI9341_BLACK);
  tft.drawLine(x - 8, y + 1, x + 8, y + 1, ILI9341_BLACK);
}

void drawStressMark(int x, int y) {
  tft.drawLine(x, y, x + 6, y - 8, ILI9341_BLACK);
  tft.drawLine(x + 6, y - 8, x + 12, y, ILI9341_BLACK);
}

// Main draw function
void drawEmotion(Emotion emotion) {
  tft.fillScreen(ILI9341_YELLOW);
  int cx = 160;
  int cy = 120;

  switch (emotion) {
    case HAPPY:
      tft.fillCircle(cx - 60, cy - 40, 20, ILI9341_BLACK);
      tft.fillCircle(cx + 60, cy - 40, 20, ILI9341_BLACK);
      drawSmile(cx, cy + 10, 90);
      break;

    case CURIOUS:
      // Raised left eyebrow
      tft.fillRect(cx - 90, cy - 75, 40, 6, ILI9341_BLACK);
      // Normal right eyebrow
      tft.fillRect(cx + 50, cy - 60, 40, 6, ILI9341_BLACK);
      // Eyes with whites
      tft.fillCircle(cx - 60, cy - 30, 18, ILI9341_WHITE);
      tft.drawCircle(cx - 60, cy - 30, 18, ILI9341_BLACK);
      tft.fillCircle(cx - 60, cy - 30, 8, ILI9341_BLACK);
      tft.fillCircle(cx + 60, cy - 30, 18, ILI9341_WHITE);
      tft.drawCircle(cx + 60, cy - 30, 18, ILI9341_BLACK);
      tft.fillCircle(cx + 60, cy - 30, 8, ILI9341_BLACK);
      // Tilted mouth
      tft.drawLine(cx - 30, cy + 50, cx + 30, cy + 40, ILI9341_BLACK);
      tft.drawLine(cx - 30, cy + 51, cx + 30, cy + 41, ILI9341_BLACK);
      break;

    case SAD:
      tft.fillCircle(cx - 60, cy - 40, 20, ILI9341_BLACK);
      tft.fillCircle(cx + 60, cy - 40, 20, ILI9341_BLACK);
      drawFrown(cx, cy + 100, 90);
      break;

    case SHY:
      // Blush cheeks
      tft.fillCircle(cx - 85, cy + 5, 20, ILI9341_RED);
      tft.fillCircle(cx + 85, cy + 5, 20, ILI9341_RED);
      // Closed eyes
      drawClosedEye(cx - 50, cy - 35);
      drawClosedEye(cx + 50, cy - 35);
      // Stress marks
      drawStressMark(cx - 95, cy - 55);
      drawStressMark(cx + 75, cy - 55);
      // Small mouth
      drawSmallMouth(cx, cy + 45);
      break;

    case ANNOYED:
      // Slanted eyebrows
      tft.drawLine(cx - 90, cy - 65, cx - 40, cy - 75, ILI9341_BLACK);
      tft.drawLine(cx + 40, cy - 70, cx + 90, cy - 70, ILI9341_BLACK);
      // Eyes
      tft.fillCircle(cx - 60, cy - 30, 20, ILI9341_WHITE);
      tft.drawCircle(cx - 60, cy - 30, 20, ILI9341_BLACK);
      tft.fillCircle(cx + 60, cy - 30, 20, ILI9341_WHITE);
      tft.drawCircle(cx + 60, cy - 30, 20, ILI9341_BLACK);
      // Side-looking pupils
      tft.fillCircle(cx - 70, cy - 30, 9, ILI9341_BLACK);
      tft.fillCircle(cx + 50, cy - 30, 9, ILI9341_BLACK);
      // Half-closed eyelids
      tft.fillRect(cx - 80, cy - 45, 40, 12, ILI9341_YELLOW);
      tft.fillRect(cx + 40, cy - 45, 40, 12, ILI9341_YELLOW);
      // Flat mouth
      tft.fillRect(cx - 40, cy + 55, 80, 8, ILI9341_BLACK);
      break;

    case STARTLED:
      // Eyebrows
      tft.fillRect(cx - 85, cy - 75, 35, 6, ILI9341_BLACK);
      tft.fillRect(cx + 50, cy - 75, 35, 6, ILI9341_BLACK);
      // Big wide eyes
      tft.fillCircle(cx - 65, cy - 30, 26, ILI9341_WHITE);
      tft.drawCircle(cx - 65, cy - 30, 26, ILI9341_BLACK);
      tft.fillCircle(cx - 65, cy - 30, 10, ILI9341_BLACK);
      tft.fillCircle(cx + 65, cy - 30, 26, ILI9341_WHITE);
      tft.drawCircle(cx + 65, cy - 30, 26, ILI9341_BLACK);
      tft.fillCircle(cx + 65, cy - 30, 10, ILI9341_BLACK);
      // Shocked O mouth
      tft.fillCircle(cx, cy + 55, 24, ILI9341_BLACK);
      tft.fillCircle(cx, cy + 55, 12, ILI9341_RED);
      break;

    case TIRED:
    case BORED:
    case SLEEPING:
      // TODO
      break;
  }
}

// ======================================================
// SETUP
// ======================================================

void setup()
{
  Wire.begin();
  Serial.begin(115200);

  if (!imu.init()) {
    Serial.println("Failed to detect and initialize IMU!");
    while (1);
  }

  imu.enableDefault();

  tft.begin();
  tft.setRotation(1);

  mc.reinitialize();
  mc.disableCrc();
  mc.clearResetFlag();

  mc.setMaxAcceleration(1, 200);
  mc.setMaxDeceleration(1, 300);

  mc.setMaxAcceleration(2, 200);
  mc.setMaxDeceleration(2, 300);

  mc.disableCommandTimeout();

  pinMode(motor1.encA, INPUT_PULLUP);
  pinMode(motor1.encB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(motor1.encA), handleEncA1, RISING);

  pinMode(motor2.encA, INPUT_PULLUP);
  pinMode(motor2.encB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(motor2.encA), handleEncA2, RISING);

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
  float corrected_distance = -1;

  lightSensorValue = analogRead(lightPin);

  // Serial.print("Light Sensor: ");
  // Serial.println(lightSensorValue);
  bool spiked = imuSpikeDetected();


  if (medianRaw >= 0)
  {
    int smoothRaw = movingAverage(medianRaw);
    corrected_distance = calibrateDistance((float)smoothRaw);

    // Serial.print("Distance: ");
    // Serial.print(corrected);
    // Serial.println(" mm");
  }
  // Serial.println(corrected_distance);

  // ------------------------------
  // Behavior based on emotion
  // ------------------------------
  if (corrected_distance == -1) {
    consecutiveValidReadings = 0;  // reset streak on bad reading
    if (!isIdle) {
      idleStart = millis();
      isIdle = true;
    }
  } else {
    consecutiveValidReadings++;
    if (consecutiveValidReadings >= VALID_READINGS_THRESHOLD) {
      // Serial.println("TRUE");
      isIdle = false;
      idleStart = millis();  // only reset idle if reading is sustained
    }
  }

  if (spiked) {
    // Serial.println("TRUE");
    isIdle = false;
    idleStart = millis();
  }

  unsigned long idleTime = isIdle ? (millis() - idleStart) : 0;

  Serial.print("Idle time: ");
  Serial.println(idleTime/1000.0);

  emotion = determineEmotion(idleTime, corrected_distance, lightSensorValue, spiked);

  printEmotion(emotion);
  drawEmotion(emotion);
  playSound(emotion);

  if (emotion == HAPPY)
  {
    bobMotor(motor2);
    motor2Returning = true;   // remember that we left center
  }
  else
  {
    if (motor2Returning)
      returnToNeutral(motor2, motor2Returning);
    else
      mc.setSpeed(motor2.motorID, 0);
  }

  if (emotion == STARTLED)
  {
    bobMotor(motor1);
    motor1Returning = true;   // remember that we left center
  }
  else
  {
    if (motor1Returning)
      returnToNeutral(motor1, motor1Returning);
    else
      mc.setSpeed(motor1.motorID, 0);
  }

  delay(50);
}