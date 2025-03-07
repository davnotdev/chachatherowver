#include "rowver.h"

#define FINAL_X 100
#define FINAL_Y 100

// TODO
void clawGrab() {
    // servo_bottom.write(90);
    servo_fl.write(180 - (100 + 10));
    servo_fr.write(100);
}

void clawRelease() {
    // servo_bottom.write(90);
    servo_fl.write(180);
    servo_fr.write(0);
}

void endzone() {
    spinAndScan();

    int d, theta;

    if (endzoneGetResponse(&d, &theta) == false) {
        return;
    }

    endzoneGoToObject(d, theta);
    endzoneGoToFinal(FINAL_X, FINAL_Y);
}

bool endzoneGetResponse(int *d, int *theta) {
    #define MAX_BLOBS 32
    int i = 0;
    int btresDistances[MAX_BLOBS];
    int btresThetas[MAX_BLOBS];

    while (true) {
        String buf = Serial.readStringUntil('\n');
        buf.trim();
        LOGF(LOG_EZ_BLUETOOTH, "Got Response %s\n", buf);

        if (buf.indexOf("[CMD::END]") != -1) {
            return;
        } else {
            if (buf.indexOf("[CMD::" == -1) || buf.indexOf(' ') == -1) {
                LOGF(LOG_EZ_BLUETOOTH, "Got Bad Response\n");
                return false;
            }
            int dStart = buf.indexOf(' ') + 1;
            int thStart = buf.indexOf(',') + 1;

            *d = atoi(buf.substring(dStart, thStart - 2).c_str());
            *theta = atoi(buf.substring(thStart).c_str());

            LOGF(LOG_EZ_BLUETOOTH, "Response: d: %d, theta: %d\n", *d, *theta);
        }
    }

    LOGF(LOG_EZ_BLUETOOTH, "Final Choice: d: %d, theta: %d\n", *d, *theta);

    return true;
}

void spinAndScan() {
    delay(3000);
    char b[64]; 

    int distanceCmFrontLeft = readDistanceSensor(fl_ultrasonic_echo, fl_ultrasonic_trigger);
    int distanceCmFrontRight = readDistanceSensor(fr_ultrasonic_echo, fr_ultrasonic_trigger);
    int targetDistance = (distanceCmFrontRight + distanceCmFrontLeft) / 2;
    int leftSpeed, rightSpeed;
    bool shouldMove;

    // Scan backward.
    resetEncoders();
    shouldMove = true;
    while (shouldMove) {
        getSpeedWithCourseCorrection(&leftSpeed, &rightSpeed);
        shouldMove = moveBackwardByCms(leftSpeed, rightSpeed, targetDistance);
    
        int left = readDistanceSensor(sl_ultrasonic_echo, sl_ultrasonic_trigger);
        int right = readDistanceSensor(sr_ultrasonic_echo, sr_ultrasonic_trigger);
        snprintf(b, 64, "[CMD::DATA] %d,0\n", left);
        bluetooth.print(b);
        snprintf(b, 64, "[CMD::DATA] %d,180\n", right);
        bluetooth.print(b);
    }
    resetEncoders();
    // Return.
    moveForward(0, 0);
    shouldMove = true;
    while (shouldMove) {
        getSpeedWithCourseCorrection(&leftSpeed, &rightSpeed);
        shouldMove = moveForwardByCms(leftSpeed, rightSpeed, targetDistance);
    }
    // Scan forward.
    resetEncoders();
    moveForward(0, 0);
    shouldMove = true;
    while (shouldMove) {
        getSpeedWithCourseCorrection(&leftSpeed, &rightSpeed);
        shouldMove = moveForwardByCms(leftSpeed, rightSpeed, targetDistance);
        int left = readDistanceSensor(sl_ultrasonic_echo, sl_ultrasonic_trigger);
        int right = readDistanceSensor(sr_ultrasonic_echo, sr_ultrasonic_trigger);
        snprintf(b, 64, "[CMD::DATA] %d,0\n", left);
        bluetooth.print(b);
        snprintf(b, 64, "[CMD::DATA] %d,180\n", right);
        bluetooth.print(b);
    }
    resetEncoders();
    // Return.
    moveForward(0, 0);
    shouldMove = true;
    while (shouldMove) {
        getSpeedWithCourseCorrection(&leftSpeed, &rightSpeed);
        shouldMove = moveBackwardByCms(leftSpeed, rightSpeed, targetDistance);
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
        getSpeedWithCourseCorrection(&leftSpeed, &rightSpeed);
        shouldMove = moveBackwardByCms(leftSpeed, rightSpeed, targetDistance);
    }
    resetEncoders();
    moveForward(0, 0);

    // clawGrab();

    // Move Back.
    resetEncoders();
    shouldMove = true;
    while (shouldMove) {
        getSpeedWithCourseCorrection(&leftSpeed, &rightSpeed);
        shouldMove = moveForwardByCms(leftSpeed, rightSpeed, targetDistance);
    }
    resetEncoders();
}

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
