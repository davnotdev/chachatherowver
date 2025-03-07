#include "rowver.h"

#define FINAL_X 100
#define FINAL_Y 100

void endzone() {
    int d, theta;

    if (endzoneGetResponse(&d, &theta) == false) {
        return;
    }

    endzoneGoToObject(d, theta);
    endzoneGoToFinal();
}

bool endzoneGetResponse(int *d, int *theta) {
    String buf = Serial.readStringUntil('\n');
    buf.trim();
    
    LOGF(LOG_EZ_BLUETOOTH, "Got Response %s\n", buf);

    if (buf.indexOf("[CMD::RES] " == -1) || buf.indexOf(' ') == -1 || buf.indexOf(';') == -1) {
        LOGF(LOG_EZ_BLUETOOTH, "Got Bad Response\n");
        return false;
    }

    int dStart = buf.indexOf(' ') + 1;
    int thStart = buf.indexOf(';') + 1;

    *d = atoi(buf.substring(dStart, thStart - 2).c_str());
    *theta = atoi(buf.substring(thStart).c_str());

    LOGF(LOG_EZ_BLUETOOTH, "Final Reading: d: %d, theta: %d\n", *d, *theta);

    return true;
}

void endzoneGoToObject(int d, int theta) {
    bool shouldMove;
    int leftSpeed, rightSpeed;
    
    // Spin to orient back to object 
    int targetAngle = theta - 180;
    // TODO

    // Move to target distance
    int targetDistance = d - 25;
    resetEncoders();
    shouldMove = true;
    while (shouldMove) {
        getSpeedWithCourseCorrection(&leftSpeed, &rightSpeed);
        shouldMove = moveBackwardByCms(leftSpeed, rightSpeed, targetDistance);
    }
    resetEncoders();
    moveForward(0, 0);

    // Grab object.
    // TODO

    // Move Back.
    resetEncoders();
    shouldMove = true;
    while (shouldMove) {
        getSpeedWithCourseCorrection(&leftSpeed, &rightSpeed);
        shouldMove = moveForwardByCms(leftSpeed, rightSpeed, targetDistance);
    }
    resetEncoders();
}

void endzoneGoToFinal() {
    bool shouldMove;
    int leftSpeed, rightSpeed;

    // Convert d and theta to polar coordinates.
    int d = sqrt(FINAL_X * FINAL_X + FINAL_Y * FINAL_Y);
    int theta = (int)((atan((double)FINAL_X / (double)FINAL_Y) * 180.0) / 3.14);

    // Spin to orient back to final.
    int targetAngle = theta - 180;
    // TODO: Spin until targetAngle.

    int targetDistance = d - 50;
    resetEncoders();
    shouldMove = true;
    while (shouldMove) {
        getSpeedWithCourseCorrection(&leftSpeed, &rightSpeed);
        shouldMove = moveBackwardByCms(leftSpeed, rightSpeed, targetDistance);
    }
    resetEncoders();

    // Release object.
    // TODO
}
