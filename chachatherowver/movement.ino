#include "rowver.h"
void goForwardOneSide(int enA, int in1, int in2, int enB, int in3, int in4, int speed) {
    //Set forward rotation direction for MOTOR 1
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);

    //Set forward rotation direction for MOTOR 2
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);

    //Send signal to L298N Enable pin for MOTOR 1
    analogWrite(enA, speed);
    //Send signal to L298N Enable pin for MOTOR 2
    analogWrite(enB, speed);
}

void goBackwardOneSide(int enA, int in1, int in2, int enB, int in3, int in4, int speed) {
    //Set backward rotation direction for MOTOR 1
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);

    //Set backward rotation direction for MOTOR 2
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);

    //Send signal to L298N Enable pin for MOTOR 1
    analogWrite(enA, speed);
    //Send signal to L298N Enable pin for MOTOR 2
    analogWrite(enB, speed);
}

void oneSideForwardBack(int enA, int in1, int in2, int enB, int in3, int in4, int speed) {
    //Set forward rotation direction for MOTOR 1
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    
    //Set backward rotation direction for MOTOR 2
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);

    //Send signal to L298N Enable pin for MOTOR 1
    analogWrite(enA, speed);
    //Send signal to L298N Enable pin for MOTOR 2
    analogWrite(enB, speed);
}

void oneSideBackForward(int enA, int in1, int in2, int enB, int in3, int in4, int speed) {
    //Set backward rotation direction for MOTOR 1
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    
    //Set forward rotation direction for MOTOR 2
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);

    //Send signal to L298N Enable pin for MOTOR 1
    analogWrite(enA, speed);
    //Send signal to L298N Enable pin for MOTOR 2
    analogWrite(enB, speed);
}

static int encoderCounter = 0; 
static int encoderPrevReading = LOW;
static double encoderMagic = 30.0 / 40.0;
// static double encoderMagic = 3.5;

void updateEncoders() {
    int reading = digitalRead(r1_encoder);
    if (reading != encoderPrevReading) {
        encoderCounter += 1;
    }
    encoderPrevReading = reading;
}

double readEncoderCms() {
    return encoderMagic * encoderCounter;
}

void resetEncoders() {
    encoderCounter = 0;
}

void moveForward(float leftSpeed, float rightSpeed) {
    goForwardOneSide(
        l1_en,
        l1_in1,
        l1_in2,
        l2_en,
        l2_in1,
        l2_in2,
        leftSpeed
    );
    goForwardOneSide(
        r1_en,
        r1_in1,
        r1_in2,
        r2_en,
        r2_in1,
        r2_in2,
        rightSpeed
    );
}

void moveBackward(float leftSpeed, float rightSpeed) {
    goBackwardOneSide(
        l1_en,
        l1_in1,
        l1_in2,
        l2_en,
        l2_in1,
        l2_in2,
        leftSpeed
    );
    goBackwardOneSide(
        r1_en,
        r1_in1,
        r1_in2,
        r2_en,
        r2_in1,
        r2_in2,
        rightSpeed
    );
}

bool moveForwardByCms(float leftSpeed, float rightSpeed, float cms) {
    double reading = readEncoderCms();

    moveForward(leftSpeed, rightSpeed);
    updateEncoders();

    if (reading >= cms) {
        moveForward(0, 0);
        return false; 
    } else {
        return true;
    }
}

bool moveBackwardByCms(float leftSpeed, float rightSpeed, float cms) {
    double reading = readEncoderCms();

    moveBackward(leftSpeed, rightSpeed);
    updateEncoders();

    if (reading >= cms) {
        moveForward(0, 0);
        return false; 
    } else {
        return true;
    }
}

void moveSpin(int speed = default_speed) {
    goForwardOneSide(
        r1_en,
        r1_in1,
        r1_in2,
        r2_en,
        r2_in1,
        r2_in2,
        speed
    );

    goBackwardOneSide(
        l1_en, 
        l1_in1,
        l1_in2,
        l2_en,
        l2_in1,
        l2_in2,
        speed
    );  
}

void moveRSpin(int speed = default_speed) { // This is New
    goForwardOneSide(
        l1_en, 
        l1_in1,
        l1_in2,
        l2_en,
        l2_in1,
        l2_in2,
        speed
    );

    goBackwardOneSide(
        r1_en,
        r1_in1,
        r1_in2,
        r2_en,
        r2_in1,
        r2_in2,
        speed
    );  
}

// Don't Use This.
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

void spinToTargetAngle(int targetAngle) {  // new
    int currentYaw = (int)readGyroYaw() % 360;
    while (true) {
        currentYaw = (int)readGyroYaw() % 360;
        if (abs(targetAngle - currentYaw) < 2) {
            return;
        }

        if (targetAngle - currentYaw < 0) {
            moveRSpin();
        } else if (targetAngle - currentYaw > 0) {
            moveSpin();
        }
    }
    moveForward(0, 0);
}

void chachaAlign() {
    spinToTargetAngle(0);
}

void chachaLeft(float speed) {
    oneSideForwardBack(
        r1_en,
        r1_in1,
        r1_in2,
        r2_en,
        r2_in1,
        r2_in2,
        speed
    );
    oneSideBackForward(
        l1_en,
        l1_in1,
        l1_in2,
        l2_en,
        l2_in1,
        l2_in2,
        speed
    ); 
}

void chachaRight(float speed) {
    oneSideForwardBack(
        l1_en,
        l1_in1,
        l1_in2,
        l2_en,
        l2_in1,
        l2_in2,
        speed
    );
    oneSideBackForward(
        r1_en,
        r1_in1,
        r1_in2,
        r2_en,
        r2_in1,
        r2_in2,
        speed
    ); 
}
