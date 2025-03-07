#include "Wire.h"
#include <MPU6050_light.h>

#define ECHO_PIN 11
#define TRIGGER_PIN 10

MPU6050 mpu(Wire);

int readGyroYaw() {
    int yaw = 0;
    mpu.update();
    yaw = mpu.getAngleZ();
    return yaw;
}

void setup() {
  Serial.begin(9600);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Wire.begin();
  byte status = mpu.begin();
  Serial.print("MPU6050 status: ");
  Serial.println(status);
  while (status != 0) {}
    
  Serial.println("Calculating offsets, do not move Cha Cha");
  delay(1000);
  mpu.calcOffsets();
  Serial.println("Done Gyro Calibration");
}

void loop() {
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(1);
  digitalWrite(TRIGGER_PIN, LOW);

  int duration = pulseIn(ECHO_PIN, HIGH); 
  int cm = (duration / 2) / 29.1;

  int yaw = readGyroYaw();

  Serial.print(cm);
  Serial.print("\t");
  Serial.print(yaw);
  Serial.print("\n");
}
