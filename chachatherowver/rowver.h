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
#define fl_ultrasonic_echo 23
#define fl_ultrasonic_trigger 22
#define fr_ultrasonic_echo 25
#define fr_ultrasonic_trigger 24

// Ultrasonic Sensors Side
#define sl_ultrasonic_echo 49
#define sl_ultrasonic_trigger 48
#define sr_ultrasonic_echo 51
#define sr_ultrasonic_trigger 50

// Encoder Pins
#define l1_encoder 35
#define l2_encoder 37
#define r1_encoder 34
#define r2_encoder 36

// Bluetooth
#define bt_tx 29
#define bt_rx 28

SoftwareSerial bluetooth(bt_tx, bt_rx);

// Course Correction Constants
static int default_speed = 100;
static int offset_speed = 0;

#define CMS_TO_TIME(D) ((D * 1000) / default_speed)

#define target_speed_cmps 10
#define offset_speed_magic 6
#define direction_correction_threshold 1
#define move_forward_cm 100

// Phase One Object Detection
#define object_front_threshold 30


// Scan Modulo
#define scan_modulo 2

// Filtered Logging
#define LOGF(LAYER, ...) if (LAYER) { char b[128]; snprintf(b, 128, __VA_ARGS__); bluetooth.print(b); Serial.print(b); }
#define LOG_CALIBRATE true
#define LOG_SENSOR_READINGS false
#define LOG_COURSE_CORRECTION false
#define LOG_STEPS true
#define LOG_STEP_PROC true

#define LOG_EZ_BLUETOOTH true

// Gyroscope Sensor
MPU6050 mpu(Wire);

// Builtin
void setup();
void loop();

// Sensor
double readGyroYaw();
double readDistanceSensor(int echo, int trigger);

// Movement
void moveForward(float leftSpeed, float rightSpeed);
void moveSpin(int speed);
void chachaRight(float speed);
void chachaLeft(float speed);
void goForwardOneSide(int enA, int in1, int in2, int enB, int in3, int in4, int speed);

// Test Mode

#define test_mode true

void testLoop();

#endif
