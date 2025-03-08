#include "rowver.h"

static bool testInit = false;

void testLoop() {
    if (!testInit) {
        Serial.println(R"(Test Mode
  Commands:
      w[t]\n - Test wheels for t secs
      u[t]\n - Read from ultrasonic sensors for t secs
      g[t]\n - Read from gyroscope yaw sensors for t secs
      s[x]\n - Spin for t secs     
      o[s]\n - Run wheels s secs
      c[c]\n - Go forward c cms
      x[t]\n - Move with course correction for t secs
      b[c]\n - Send an ascii code via bluetooth and read
      r[a]\n - Correct rotation
      z[a]\n - Check Cha Cha
)");
        testInit = true;
    }

    String buf = Serial.readStringUntil('\n');
    buf.trim();

    if (buf.length() == 0) {
        return;
    }
        
    char cmd = buf[0];
    int argument = atoi(buf.c_str() + 1);

    Serial.print("Got: ");
    Serial.print(cmd);
    Serial.print(" with ");
    Serial.println(argument);

    moveForward(0, 0);

    switch (cmd) {
        case 'f': // This is New
            Serial.println("this work");
            delay(3000);
            chachaAlign(120, 120);
            // moveForwardByCms(120, 120, 10);
            break;
        case 'a':
            spinAndScan();
            break;
        case 'w': {
            unsigned long start = millis();
            while (millis() - start < argument * 1000) {
                moveForward(default_speed, default_speed);
            }
            } break;
        case 'u': {
            unsigned long start = millis();
            while (millis() - start < argument * 1000) {
                double fl = readDistanceSensor(fl_ultrasonic_echo, fl_ultrasonic_trigger);
                double fr = readDistanceSensor(fr_ultrasonic_echo, fr_ultrasonic_trigger);
                double sl = readDistanceSensor(sl_ultrasonic_echo, sl_ultrasonic_trigger);
                double sr = readDistanceSensor(sr_ultrasonic_echo, sr_ultrasonic_trigger);

                Serial.print(fl);
                Serial.print('\t');
                Serial.print(fr);
                Serial.print('\t');
                Serial.print(sl);
                Serial.print('\t');
                Serial.print(sr);
                Serial.print('\n');
            }
            } break;
        case 'g': {
            unsigned long start = millis();
            while (millis() - start < argument * 1000) {
                Serial.println(readGyroYaw());
            }
            } break;
        case 's':
            for (int i = 0; i < argument; i++) {
                moveSpin();
            }
            break;
        case 'x': {
            int leftSpeed, rightSpeed;
            unsigned long start = millis();
            while (millis() - start < argument * 1000) {
                getSpeedWithCourseCorrection(&leftSpeed, &rightSpeed);
                moveForward(leftSpeed, rightSpeed);
            }
            moveForward(0, 0);
        } break;
        case 'r':
            correctRotation();
            break;
        case 'z':
            checkChaCha();
            break;
        case 'o': {
            unsigned long start = millis();
            while (millis() - start < argument * 1000) {
                moveForward(default_speed, default_speed);
            }
            moveForward(0, 0);
          } break;
        case 'c':
            double reading = 0.0;
              while (reading <= argument) {
                  moveForward(default_speed, default_speed);
                  reading = readEncoderCms();
                  Serial.println(reading);
                  updateEncoders();
              }
              moveForward(0, 0);
              break;
        case 'b':
            bluetooth.write((byte)argument);
            while (bluetooth.available()) {
                Serial.print("Got: ");
                Serial.println(bluetooth.read());
            }
            break;
        case 'z':
            break;
        default:
            Serial.print("Not a command: ");
            Serial.println(buf);
    }
}



