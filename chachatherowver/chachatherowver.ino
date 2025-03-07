#include "rowver.h"

inline void getSpeedWithCourseCorrection(int *leftSpeedOut, int *rightSpeedOut, int speed = default_speed) {
  /*
    *leftSpeedOut = default_speed;
    *rightSpeedOut = default_speed;
    return; 
    */

    double yaw = readGyroYaw();

    // Course Correction
    int leftSpeed = speed;
    int rightSpeed = speed;

    bool shouldLeftOffsetSpeed = yaw <= -direction_correction_threshold;
    bool shouldRightOffsetSpeed = yaw >= direction_correction_threshold;

    // TODO: Figure out yaw factor.
    if (shouldRightOffsetSpeed) {
        rightSpeed -= offset_speed;
        //leftSpeed += offset_speed;
    } else if (shouldLeftOffsetSpeed) {
        leftSpeed -= offset_speed;
        //leftSpeed += offset_speed;
    }

    LOGF(LOG_COURSE_CORRECTION, "Should Yah %d Left Speed: %d, Left Speed %d, Right Speed: %d\n", (short)yaw, (short)shouldLeftOffsetSpeed, leftSpeed, rightSpeed);

    *leftSpeedOut = leftSpeed;
    *rightSpeedOut = rightSpeed;
}

void checkChaCha() {
    int distanceCmFrontLeft = readDistanceSensor(fl_ultrasonic_echo, fl_ultrasonic_trigger);
    int distanceCmFrontRight = readDistanceSensor(fr_ultrasonic_echo, fr_ultrasonic_trigger);
    int distanceCmSideLeft = readDistanceSensor(sl_ultrasonic_echo, sl_ultrasonic_trigger);
    int distanceCmSideRight = readDistanceSensor(sr_ultrasonic_echo, sr_ultrasonic_trigger);

    // Let left be negative.
    float nudge = 0.0f;

    if (distanceCmFrontLeft < object_front_threshold || distanceCmFrontRight < object_front_threshold) {
        if (distanceCmSideLeft > distanceCmSideRight) {
            nudge = -1.0f;
        } else if (distanceCmSideLeft == distanceCmSideRight) {
            if (distanceCmSideRight <= object_front_threshold) {
                nudge = -1.0f;
            } else {
                nudge = 1.0f;
            }
        } else {
            nudge = 1.0f;
        }

        // Dodge the Block
        while (distanceCmFrontLeft < object_front_threshold || distanceCmFrontRight < object_front_threshold) {
            LOGF(LOG_STEPS, "Dodging Dist FL: %d FR: %d SL: %d SR: %d\n", distanceCmFrontLeft, distanceCmFrontRight, distanceCmSideLeft, distanceCmSideRight);
            if (nudge < 0) {
                chachaLeft(default_speed);
            } else {
                chachaRight(default_speed);
            }

            distanceCmFrontLeft = readDistanceSensor(fl_ultrasonic_echo, fl_ultrasonic_trigger);
            distanceCmFrontRight = readDistanceSensor(fr_ultrasonic_echo, fr_ultrasonic_trigger);
        }
    
        // Final Nudge to account for blind spot
        if (nudge < 0) {
            chachaLeft(default_speed);
        } else {
            chachaRight(default_speed);
        }
        delay(600);
    }

    moveForward(0, 0);
}

void takeStep() {
    // 1. Move until there is nothing in front.
    checkChaCha();
    LOGF(LOG_STEP_PROC, "[1] Initial Check for Block\n");

    // 2. Check Blind Spots
    float frontLeftReading = readDistanceSensor(fl_ultrasonic_echo, fl_ultrasonic_trigger);
    float frontRightReading = readDistanceSensor(fr_ultrasonic_echo, fr_ultrasonic_trigger);
    float sideLeftReading = readDistanceSensor(sl_ultrasonic_echo, sl_ultrasonic_trigger);
    float sideRightReading = readDistanceSensor(sr_ultrasonic_echo, sr_ultrasonic_trigger);

    unsigned int startTime;

    if (frontLeftReading > object_front_threshold && frontRightReading > object_front_threshold) {
        if (sideLeftReading > sideRightReading) {

            LOGF(LOG_STEP_PROC, "[2.5] Check Left Blindspot\n");
            startTime = millis();
            while (millis() - startTime < CMS_TO_TIME(18)) {
                chachaLeft(default_speed);
            }
            moveForward(0, 0);

            delay(100);
            
            float newReading = readDistanceSensor(sl_ultrasonic_echo, sl_ultrasonic_trigger);
            if (newReading <= object_front_threshold) {
                checkChaCha();
            } else {
                startTime = millis();
                while (millis() - startTime < CMS_TO_TIME(10)) {
                    chachaRight(default_speed);
                }
                moveForward(0, 0);
            }
        } else {
            LOGF(LOG_STEP_PROC, "[2.5] Check Right Blindspot\n");
            startTime = millis();
            while (millis() - startTime < CMS_TO_TIME(18)) {
                chachaRight(default_speed);
            }
            moveForward(0, 0);

            delay(100);
            
            float newReading = readDistanceSensor(sr_ultrasonic_echo, sr_ultrasonic_trigger);
            if (newReading <= object_front_threshold) {
                checkChaCha();
            } else {
                startTime = millis();
                while (millis() - startTime < CMS_TO_TIME(10)) {
                    chachaLeft(default_speed);
                }
                moveForward(0, 0);
            }
        }
    }

    moveForward(0, 0);

    // 3. Move One Step.
    LOGF(LOG_STEP_PROC, "[3] Move One Step\n");
    bool shouldForward = true;
    int leftSpeed = default_speed, rightSpeed = default_speed;

    resetEncoders();
    while (shouldForward) {
        getSpeedWithCourseCorrection(&leftSpeed, &rightSpeed);
        shouldForward = moveBackwardByCms(leftSpeed, rightSpeed, move_forward_cm);
    }
    resetEncoders();
    moveForward(0, 0);
}

void spinAndScan() {
    delay(3000);
    int yaw = (int)readGyroYaw();
    int originalYaw = yaw;
    char b[64]; 

    while (yaw - originalYaw <= 360) {
        moveSpin(150);

        int scanDistance = readDistanceSensor(sl_ultrasonic_echo, sl_ultrasonic_trigger);
        yaw = readGyroYaw();
        // int scanDistance = readDistanceSensor(sr_ultrasonic_echo, sr_ultrasonic_trigger);

        // More optimimal to do it manually
        // LOGF(LOG_STEP_PROC, "[CMD::DATA] %d,%d\n", scanDistance, yaw);
        snprintf(b, 64, "[CMD::DATA] %d,%d\n", scanDistance, yaw);
        bluetooth.print(b);
    }
    moveForward(0, 0);
}

static bool done = false;

void loop() {
    if (done) {
        return;
    }

    if (test_mode) {
        return testLoop();
    }

    moveForward(0, 0);

    // Obstacle Jungle
    for (int i = 0; i < 4; i++) {
        LOGF(LOG_STEPS, "Doing step %d\n", i);
        takeStep();
    }

    moveForward(0, 0);

    // End Zone Code
    endzone();

    done = true;
    LOGF(LOG_STEPS, "Done.");


}
