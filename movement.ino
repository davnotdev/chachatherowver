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

static int wheelTimer = 0;
static int wheelLastTime = -1;

void moveForward(float leftSpeed, float rightSpeed) {
    if (leftSpeed != 0 || rightSpeed != 0) {
        int timeNow = millis();
        if (wheelLastTime == -1) {
            wheelLastTime = timeNow;
        }
        wheelTimer += timeNow - wheelLastTime;
    }

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

void moveSpin() {
    goForwardOneSide(
        r1_en,
        r1_in1,
        r1_in2,
        r2_en,
        r2_in1,
        r2_in2,
        default_speed
    );

    goBackwardOneSide(
        l1_en, 
        l1_in1,
        l1_in2,
        l2_en,
        l2_in1,
        l2_in2,
        default_speed
    );  
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
