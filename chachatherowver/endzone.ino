#include "rowver.h"

#define FINAL_X 100
#define FINAL_Y 100

void clawGrab() {
    servo_bottom.write(0);
    delay(1000);
    servo_fl.write(180 - (100 + 10));
    servo_fr.write(100);
    delay(1000);
    servo_bottom.write(45);
}

void clawRelease() {
    servo_bottom.write(90);
    servo_fl.write(180);
    servo_fr.write(0);
}

void endzone() {
    // setupClaw();

    int distFromSideWall = -1;
    spinAndScan(&distFromSideWall);
    LOGF(true, "d to block %d\n", distFromSideWall);
    moveForward(0, 0);
    endzoneGoToObject(distFromSideWall, 270);
    // spinAndScan();

    // TODO
    // int d = 60, theta = -90;

    // if (endzoneGetResponse(&d, &theta) == false) {
    //    return;
    // }

    // endzoneGoToObject(d, theta);
    // endzoneGoToFinal(FINAL_X, FINAL_Y);
}

/*
bool endzoneGetResponse(int *d, int *theta) {
    #define MAX_BLOBS 32
    int resIdx = 0;
    int btResDistances[MAX_BLOBS];
    int btResThetas[MAX_BLOBS];

    while (true) {
        String buf = Serial.readStringUntil('\n');
        buf.trim();
        LOGF(LOG_EZ_BLUETOOTH, "Got Response %s\n", buf);

        if (buf.indexOf("[CMD::END]") != -1) {
            return;
        } else {
            if (buf.indexOf("[CMD::RES] " == -1) || buf.indexOf(',') == -1) {
                LOGF(LOG_EZ_BLUETOOTH, "Got Bad Response\n");
                return false;
            }
            int dStart = buf.indexOf(' ') + 1;
            int thStart = buf.indexOf(',') + 1;

            btResDistances[resIdx] = atoi(buf.substring(dStart, thStart - 2).c_str());
            btResThetas[resIdx] = atoi(buf.substring(thStart).c_str());
            resIdx += 1;

            if (resIdx >= MAX_BLOBS) {
                LOGF(LOG_EZ_BLUETOOTH, "Too many blobs sent!\n");
                break;
            }

            LOGF(LOG_EZ_BLUETOOTH, "Response: d: %d, theta: %d\n", btResDistances[resIdx], btResThetas[resIdx]);
        }
    }

    int minIt = 0;
    int minTheta = 0;

    for (int i = 0; i < resIdx; i++) {
        int distanceIt = btResDistances[i];
        int thetaIt = btResThetas[i];

        if (distanceIt % 360 >= -180 && thetaIt <= 0) {
            int dIt = distanceIt * sin(thetaIt);
            if (dIt < minTheta) {
                minIt = i;
                minTheta = thetaIt;
            }
        }
    }

    *d = btResDistances[minIt];
    *theta = btResThetas[minIt];

    LOGF(LOG_EZ_BLUETOOTH, "Final Choice: d: %d, theta: %d\n", *d, *theta);

    return true;
}
*/

void spinAndScan(int* distFromSideWall) {
    delay(2000);
    // char b[64]; 
    
    int distanceCmFrontLeft = readDistanceSensor(fl_ultrasonic_echo, fl_ultrasonic_trigger);
    int targetDistance = distanceCmFrontLeft - 10;
    int leftSpeed, rightSpeed;
    bool shouldMove;

    int distToFrontWall = 0;
    *distFromSideWall = 99;
    int wallDist = readDistanceSensor(sr_ultrasonic_echo, sr_ultrasonic_trigger);

    // Scan backward.
    resetEncoders();
    shouldMove = true;
    while (shouldMove) {
        getSpeedWithCourseCorrection(&leftSpeed, &rightSpeed, default_speed, true);
        shouldMove = moveBackwardByCms(leftSpeed, rightSpeed, 100 - (targetDistance + 40));
    
        // int left = readDistanceSensor(sl_ultrasonic_echo, sl_ultrasonic_trigger);
        distanceCmFrontLeft = readDistanceSensor(fl_ultrasonic_echo, fl_ultrasonic_trigger);
        int right = readDistanceSensor(sr_ultrasonic_echo, sr_ultrasonic_trigger);
        // snprintf(b, 64, "[CMD::DATA] %d,0\n", left, -readEncoderCms());
        // bluetooth.print(b);
        // snprintf(b, 64, "[CMD::DATA] %d,%d\n", right, -readEncoderCms());
        // bluetooth.print(b);
        if (right < wallDist - 5 && wallDist > 5) {
            if (right < *distFromSideWall) {
                *distFromSideWall = right;
                distToFrontWall = distanceCmFrontLeft;
            }
        }
    }
    resetEncoders();
    // Return.
    moveForward(0, 0);
    shouldMove = true;
    while (shouldMove) {
        getSpeedWithCourseCorrection(&leftSpeed, &rightSpeed);
        shouldMove = moveForwardByCms(leftSpeed, rightSpeed, 100 - (targetDistance + 40));
    }
    // Scan forward.
    resetEncoders();
    moveForward(0, 0);
    shouldMove = true;
    while (shouldMove) {
        getSpeedWithCourseCorrection(&leftSpeed, &rightSpeed);
        shouldMove = moveForwardByCms(leftSpeed, rightSpeed, targetDistance);
        distanceCmFrontLeft = readDistanceSensor(fl_ultrasonic_echo, fl_ultrasonic_trigger);
        int left = readDistanceSensor(sl_ultrasonic_echo, sl_ultrasonic_trigger);
        int right = readDistanceSensor(sr_ultrasonic_echo, sr_ultrasonic_trigger);
        // snprintf(b, 64, "[CMD::DATA] %d,0\n", left, readEncoderCms());
        // bluetooth.print(b);
        // snprintf(b, 64, "[CMD::DATA] %d,%d\n", right, readEncoderCms());
        // bluetooth.print(b);
        if (right < wallDist - 5 && wallDist > 5) {
            if (right < *distFromSideWall) {
                *distFromSideWall = right;
                distToFrontWall = distanceCmFrontLeft;
            }
        }
    }
    resetEncoders();
    moveForward(0, 0);

    LOGF(true, "d to front wall: %d, d to block: %d", distToFrontWall, *distFromSideWall)

    // Return.
    shouldMove = true;
    while (shouldMove) {
        getSpeedWithCourseCorrection(&leftSpeed, &rightSpeed, default_speed, true);
        shouldMove = moveBackwardByCms(leftSpeed, rightSpeed, distToFrontWall - 10);
    }
    resetEncoders();
    moveForward(0, 0);
}

void endzoneGoToObject(int d, int theta) {
    bool shouldMove;
    int leftSpeed, rightSpeed;
    
    // Spin to orient back to object 
    int targetAngle = theta - 180;
    spinToTargetAngle(targetAngle);

    // Move to target distance
    int targetDistance = d;
    resetEncoders();
    shouldMove = true;
    while (shouldMove) {
        getSpeedWithCourseCorrection(&leftSpeed, &rightSpeed, default_speed, true, targetAngle);
        shouldMove = moveBackwardByCms(leftSpeed, rightSpeed, targetDistance);
    }
    resetEncoders();
    moveForward(0, 0);

    delay(2000);
    clawGrab();
    delay(1500);

    // Move Back.
    resetEncoders();
    shouldMove = true;
    while (shouldMove) {
        getSpeedWithCourseCorrection(&leftSpeed, &rightSpeed, default_speed, false, targetAngle);
        shouldMove = moveForwardByCms(leftSpeed, rightSpeed, targetDistance + 115);
    }
    resetEncoders();

    clawRelease();
}

/*
void endzoneGoToFinal(int final_x, int final_y) {
    bool shouldMove;
    int leftSpeed, rightSpeed;

    // Convert d and theta to polar coordinates.
    int d = sqrt(final_x * final_x + final_y * final_y);
    int theta = (int)((atan((double)final_x / (double)final_y) * 180.0) / 3.14);

    // Spin to orient back to final.
    int targetAngle = theta - 180;
    spinToTargetAngle(targetAngle);

    int targetDistance = abs(d);

    LOGF(true, "speed %d\n", default_speed);

    resetEncoders();
    shouldMove = true;
    while (shouldMove) {
        getSpeedWithCourseCorrection(&leftSpeed, &rightSpeed);
        shouldMove = moveBackwardByCms(leftSpeed, rightSpeed, targetDistance);
    }
    resetEncoders();

    clawRelease();
}
*/
