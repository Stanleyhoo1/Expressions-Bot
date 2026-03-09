// Pololu Distance Sensor with Pulse Width Output - 300 cm
// Smoothed + quadratic calibration fit from your measured data

const uint8_t sensorPin = 5;

// ---------- FILTER SETTINGS ----------
const int MEDIAN_SAMPLES = 7;   // must be odd
const int AVG_SAMPLES = 5;

int avgBuffer[AVG_SAMPLES];
int avgIndex = 0;
bool avgFilled = false;

// ---------- SORT HELPER ----------
void sortArray(int arr[], int n) {
  for (int i = 0; i < n - 1; i++) {
    for (int j = i + 1; j < n; j++) {
      if (arr[j] < arr[i]) {
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
      }
    }
  }
}

// ---------- RAW SENSOR READING ----------
int readRawDistance() {
  unsigned long t = pulseIn(sensorPin, HIGH, 50000); // 50 ms timeout

  if (t == 0) return -1;        // timeout
  if (t > 1850) return -1;      // no detection

  int d = (int)(t - 1000) * 4;  // Pololu raw conversion in mm
  if (d < 0) d = 0;

  return d;
}

// ---------- MEDIAN FILTER ----------
int readMedianDistance() {
  int readings[MEDIAN_SAMPLES];
  int validCount = 0;

  for (int i = 0; i < MEDIAN_SAMPLES; i++) {
    int val = readRawDistance();
    if (val >= 0) {
      readings[validCount++] = val;
    }
    delay(5);
  }

  if (validCount == 0) return -1;

  sortArray(readings, validCount);
  return readings[validCount / 2];
}

// ---------- MOVING AVERAGE ----------
int movingAverage(int newValue) {
  if (newValue < 0) return -1;

  avgBuffer[avgIndex] = newValue;
  avgIndex++;

  if (avgIndex >= AVG_SAMPLES) {
    avgIndex = 0;
    avgFilled = true;
  }

  int count = avgFilled ? AVG_SAMPLES : avgIndex;
  long sum = 0;
  for (int i = 0; i < count; i++) {
    sum += avgBuffer[i];
  }

  return sum / count;
}

// ---------- QUADRATIC CALIBRATION ----------
// expected_mm = a*raw^2 + b*raw + c
// Fit from your measured data:
float calibrateDistance(float raw) {
  float corrected = -0.000023 * raw * raw + 0.5275 * raw + 7.4;

  if (corrected < 0) corrected = 0;
  return corrected;
}

// ---------- OPTIONAL POWER CALIBRATION ----------
// Uncomment this and use it instead if you want to test power fit
/*
float calibrateDistance(float raw) {
  if (raw <= 0) return 0;
  float corrected = 1.536 * pow(raw, 0.812);
  if (corrected < 0) corrected = 0;
  return corrected;
}
*/

void setup() {
  pinMode(sensorPin, INPUT);
  Serial.begin(115200);

  for (int i = 0; i < AVG_SAMPLES; i++) {
    avgBuffer[i] = 0;
  }
}

void loop() {
  int medianRaw = readMedianDistance();

  if (medianRaw < 0) {
    Serial.println(-1);
    delay(100);
    return;
  }

  int smoothRaw = movingAverage(medianRaw);
  float corrected = calibrateDistance((float)smoothRaw);

  Serial.print(corrected, 1);
  Serial.println(" mm");

  delay(50);
}