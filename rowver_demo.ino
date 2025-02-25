#include "rowver.h"

void getSpeedWithCourseCorrection(int *leftSpeedOut, int *rightSpeedOut) {
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
        for (int i = 0; i < 10; i++) {
            if (nudge < 0) {
                chachaLeft(default_speed);
            } else {
                chachaRight(default_speed);
            }
            delay(100);
        }
    }

    moveForward(0, 0);
}

void takeStep(int stepAmount) {
    //checkChaCha();

    // Move forward
    LOGF(LOG_STEPS, "Moving Forward Now by amount: %d\n", stepAmount);
    for (int i = 0; i < stepAmount; i++) {
        int leftSpeed, rightSpeed;
        getSpeedWithCourseCorrection(&leftSpeed, &rightSpeed);
        moveForward(leftSpeed, rightSpeed);
        delay(2000);
        moveForward(0, 0);
        checkChaCha();
    }
    moveForward(0, 0);
}

void spinAndScan(int spinAmount) {
    for (int i = 0; i < spinAmount; i++) {
        moveSpin();

        if (i % scan_modulo == 0) {
            int scanDistance = readDistanceSensor(sl_ultrasonic_echo, sl_ultrasonic_trigger);
            int yaw = readGyroYaw();
            // int scanDistance = readDistanceSensor(sr_ultrasonic_echo, sr_ultrasonic_trigger);
            Serial.print("[CMD::DATA] "); 
            Serial.print(scanDistance);
            Serial.print("\t");
            Serial.println(yaw);
        }

        delay(10);
    }
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
        int stepAmount = move_forward_amount;

        // Take an walk a bit more on first step.
        if (i == 1) {
            stepAmount -= 1;
        } 

        LOGF(LOG_STEPS, "Doing step %d amount: %d\n", i, stepAmount);
        takeStep(stepAmount);
    }

    moveForward(0, 0);

    // End Zone Code
    done = true;
    Serial.println("Done.");
}
