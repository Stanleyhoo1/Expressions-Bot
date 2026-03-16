/*
The sensor outputs provided by the library are the raw
16-bit values obtained by concatenating the 8-bit high and
low accelerometer and gyro data registers. They can be
converted to units of g and dps (degrees per second) using
the conversion factors specified in the datasheet for your
particular device and full scale setting (gain).

Example: An LSM6DS33 gives an accelerometer Z axis reading
of 16276 with its default full scale setting of +/- 2 g. The
LA_So specification in the LSM6DS33 datasheet (page 15)
states a conversion factor of 0.061 mg/LSB (least
significant bit) at this FS setting, so the raw reading of
16276 corresponds to 16276 * 0.061 = 992.8 mg = 0.9928 g.
*/

#include <Wire.h>
#include <LSM6.h>

LSM6 imu;

char report[80];

// --- Define Thresholds (in units of 'g') ---
const float THRESHOLD_X = 0.3;     // Triggers if X goes above 1.5g or below -1.5g
const float THRESHOLD_Y = 0.3;     // Triggers if Y goes above 1.5g or below -1.5g
const float THRESHOLD_Z_LOW = 0.75; // Triggers if Z drops below 0.5g (e.g., freefall or tilt)

void setup()
{
  Serial.begin(9600);
  Wire.begin();

  if (!imu.init())
  {
    Serial.println("Failed to detect and initialize IMU!");
    while (1);
  }
  imu.enableDefault();
}

void loop()
{
  imu.read();

  // 1. Convert raw values to standard units (g and dps)
  float accel_x = imu.a.x * 0.061 / 1000.0;
  float accel_y = imu.a.y * 0.061 / 1000.0;
  float accel_z = imu.a.z * 0.061 / 1000.0;

  float gyro_x = imu.g.x * 8.75 / 1000.0;
  float gyro_y = imu.g.y * 8.75 / 1000.0;
  float gyro_z = imu.g.z * 8.75 / 1000.0;

  // 2. Print the standard report
  // snprintf(report, sizeof(report), "A: %6f %6f %6f    G: %6f %6f %6f",
  //   accel_x, accel_y, accel_z,
  //   gyro_x, gyro_y, gyro_z);
  // Serial.println(report);

  // 3. Check thresholds and print alerts
  if (abs(accel_x) > THRESHOLD_X) {
    Serial.print("ALERT: X-axis acceleration surpassed threshold! Value: ");
    Serial.println(accel_x);
  }

  if (abs(accel_y) > THRESHOLD_Y) {
    Serial.print("ALERT: Y-axis acceleration surpassed threshold! Value: ");
    Serial.println(accel_y);
  }

  if (accel_z < THRESHOLD_Z_LOW) {
    Serial.print("ALERT: Z-axis acceleration dropped below threshold! Value: ");
    Serial.println(accel_z);
  }

  delay(100);
}