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

// Reset encoders after.
bool moveGenericByCms(float leftSpeed, float rightSpeed, float cms, void(*callback)(int, int)) {
    double reading = readEncoderCms();

    callback(leftSpeed, rightSpeed);
    updateEncoders();

    if (reading >= cms) {
        moveForward(0, 0);
        return false; 
    } else {
        return true;
    }
}

bool moveForwardByCms(float leftSpeed, float rightSpeed, float cms) {
    return moveGenericByCms(leftSpeed, rightSpeed, cms, moveForward);
}

bool moveBackwardByCms(float leftSpeed, float rightSpeed, float cms) {
    return moveGenericByCms(leftSpeed, rightSpeed, cms, moveBackward);
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

void chachaAlign(float leftSpeed, float rightSpeed){ // This is new

  moveForward(0, 0);

  double y1l = readDistanceSensor(sl_ultrasonic_echo, sl_ultrasonic_trigger); // return cm
  double y1r = readDistanceSensor(sr_ultrasonic_echo , sr_ultrasonic_trigger);

  double cm = 10;
  // moveForwardByCms(leftSpeed, rightSpeed, cm); // move forward cm
  double reading = 0.0;
    while (reading <= cm) {
        moveForward(leftSpeed, rightSpeed);
        reading = readEncoderCms();
        Serial.println(reading);
        updateEncoders();
    }
    moveForward(0, 0);

  double y2l = readDistanceSensor(sl_ultrasonic_echo, sl_ultrasonic_trigger);
  double y2r = readDistanceSensor(sr_ultrasonic_echo , sr_ultrasonic_trigger);
  
  double d1 = atan((y2l-y1l)/10) * 180/3.141592;
  double d2 = atan((y2r-y1r)/-10) * 180/3.141592;
  
  double degrees = d1;
  Serial.print("y1l: ");
  Serial.println(y1l);
  Serial.print("y2l: ");
  Serial.println(y2l);

  // Serial.print("y1r: ");
  // Serial.println(y1r);
  // Serial.print("y2r: ");
  // Serial.println(y2r);

  // Serial.print("d1: ");
  // Serial.println(d1);
  
  // Serial.print("d2: ");
  // Serial.println(d2);
  
  Serial.print("Degrees: ");
  Serial.println(degrees);

  spinToTargetAngle((int)degrees);

  // if(degrees > 0){
  //   int yaw0 = readGyroYaw(); // get rid of intial yaw
  //   Serial.println(yaw0); // print statement
  //   // moveRSpin();

  //   while(true){
  //     Serial.println(((int)readGyroYaw() - yaw0)%360); // print statment
  //     if(degrees - ((int)readGyroYaw() - yaw0)%360 <= 0){ // stop when wanted Yaw correction is acheived
  //       // moveRSpin(0);
  //       break;
  //     }
  //   }
  // }

  // if(degrees < 0){
  //   int yaw0 = readGyroYaw();
  //   Serial.println(yaw0);  
  //   // moveSpin();

  //   while(true){
  //     Serial.println(((int)readGyroYaw() - yaw0)%360); // print statemet
  //     if(degrees - ((int)readGyroYaw() - yaw0)%360 >= 0){ // stop when wanted Yaw correction is acheived
  //       // moveSpin(0);
  //       break;
  //     }
  //   }

  // moveBackwardByCms(leftSpeed, rightSpeed, cm);
    reading = 0.0;
    while (reading <= cm) {
        moveBackward(leftSpeed, rightSpeed);
        reading = readEncoderCms();
        Serial.println(reading);
        updateEncoders();
    }
    moveBackward(0, 0);

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
