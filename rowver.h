#ifndef ROWVER_H
#define ROWVER_H

#include "Wire.h"
#include <MPU6050_light.h>
#include <SoftwareSerial.h>

// Motor L1 (enB, in3, in4)
#define r2_en 2
#define r2_in1 4
#define r2_in2 3

// Motor L2 (enA, in1, in2)
#define r1_en 5
#define r1_in1 6
#define r1_in2 7

// Motor R1 (enA, in1, in2)
#define l2_en 11
#define l2_in1 12
#define l2_in2 13

// Motor R2 (enB, in3, in4)
#define l1_en 8
#define l1_in1 10
#define l1_in2 9

// Ultrasonic Sensors Front
#define fl_ultrasonic_echo 25
#define fl_ultrasonic_trigger 24
#define fr_ultrasonic_echo 49
#define fr_ultrasonic_trigger 48

// Ultrasonic Sensors Side
#define sl_ultrasonic_echo 23
#define sl_ultrasonic_trigger 22
#define sr_ultrasonic_echo 51
#define sr_ultrasonic_trigger 50

// Course Correction Constants
#define default_speed 180
#define offset_speed 5
#define direction_correction_threshold 1
#define move_forward_amount 30

// Phase One Object Detection
#define object_front_threshold 15

// Scan Modulo
#define scan_modulo 2

// Filtered Logging
#define LOGF(LAYER, ...) if (LAYER) { char b[256]; snprintf(b, 256, __VA_ARGS__); Serial.print(b); }
#define LOG_SENSOR_READINGS false
#define LOG_COURSE_CORRECTION true
#define LOG_STEPS true

// Gyroscope Sensor
MPU6050 mpu(Wire);

// Bluetooth
SoftwareSerial BTserial(2, 3);
const long btBaudRate = 31250;

// Builtin
void setup();
void loop();

// Sensor
double readGyroYaw();
double readDistanceSensor(int echo, int trigger);

// Movement
void moveForward(float leftSpeed, float rightSpeed);
void moveSpin();
void chachaRight(float speed);
void chachaLeft(float speed);
void goForwardOneSide(int enA, int in1, int in2, int enB, int in3, int in4, int speed);

// Test Mode

#define test_mode false

void testLoop();

#endif
