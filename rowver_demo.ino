#include "rowver.h"

inline void getSpeedWithCourseCorrection(int *leftSpeedOut, int *rightSpeedOut) {
  /*
    *leftSpeedOut = default_speed;
    *rightSpeedOut = default_speed;
    return; 
    */

    double yaw = readGyroYaw();

    // Course Correction
    int leftSpeed = default_speed;
    int rightSpeed = default_speed;

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

void correctRotation() {
    int yaw = (int)readGyroYaw() % 360;

    LOGF(LOG_STEP_PROC, "YAW: %d\n", (int)yaw);

    int incToCheck = 5;
    int i = 0;

    while ((yaw >= 1 || yaw <= -1) && (i % incToCheck) == 0) {
        LOGF(LOG_STEP_PROC, "YAW: %d\n", (int)yaw);
        yaw = (int)readGyroYaw() % 360;
        moveSpin();
    }
    moveForward(0, 0);
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
    float sideLeftReading = readDistanceSensor(sl_ultrasonic_echo, sl_ultrasonic_trigger);
    float sideRightReading = readDistanceSensor(sr_ultrasonic_echo, sr_ultrasonic_trigger);

    unsigned int startTime;

    if (sideLeftReading > sideRightReading) {
        LOGF(LOG_STEP_PROC, "[2.5] Check Left Blindspot\n");
        startTime = millis();
        while (millis() - startTime < 600) {
            chachaLeft(default_speed);
        }
        moveForward(0, 0);

        
        float newReading = readDistanceSensor(sl_ultrasonic_echo, sl_ultrasonic_trigger);
        if (newReading <= object_front_threshold) {
            checkChaCha();
        }
    } else {
        LOGF(LOG_STEP_PROC, "[2.5] Check Right Blindspot\n");
        startTime = millis();
        while (millis() - startTime < 600) {
            chachaRight(default_speed);
        }
        moveForward(0, 0);
        
        float newReading = readDistanceSensor(sr_ultrasonic_echo, sr_ultrasonic_trigger);
        if (newReading <= object_front_threshold) {
            checkChaCha();
        }
    }

    moveForward(0, 0);

    // 3. Move One Step.
    LOGF(LOG_STEP_PROC, "[3] Move One Step\n");
    bool shouldForward = true;
    int leftSpeed = default_speed, rightSpeed = default_speed;

    int incToCheck = 1, i = 0;

    resetEncoders();
    while (shouldForward) {
        if ((i % incToCheck) == 0) {
            getSpeedWithCourseCorrection(&leftSpeed, &rightSpeed);
        }
        shouldForward = moveForwardByCms(leftSpeed, rightSpeed, 100);
        i++;
    }
    resetEncoders();

    // Move forward
    // LOGF(LOG_STEPS, "Moving Forward Now\n");
    // int leftSpeed, rightSpeed;
    // int encoderReading = readEncoderCms();
    // for (int i = 0; i < 10; i++) {
    //     while (encoderReading <= move_forward_cm / 10.0) {
    //         int oldEncoderReading = encoderReading;
    //         checkChaCha();
    //         encoderReading = oldEncoderReading;
    //         updateEncoders();
    //         encoderReading = readEncoderCms();
    //         getSpeedWithCourseCorrection(&leftSpeed, &rightSpeed);
    //         moveForward(leftSpeed, rightSpeed);
    //     }
    //     resetEncoders();
    // }
    
    // moveForward(0, 0);
}

void spinAndScan() {
    delay(3000);
    int yaw = (int)readGyroYaw();
    int originalYaw = yaw;

    while (yaw - originalYaw <= 360) {
        moveSpin();

        int scanDistance = readDistanceSensor(sl_ultrasonic_echo, sl_ultrasonic_trigger);
        yaw = readGyroYaw();
        // int scanDistance = readDistanceSensor(sr_ultrasonic_echo, sr_ultrasonic_trigger);
        LOGF(LOG_STEP_PROC, "[CMD::DATA] %d\t%d\n", scanDistance, yaw);
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
    done = true;
    Serial.println("Done.");
}
