void setupDistanceSensor(int echo, int trigger) {
    pinMode(trigger, OUTPUT);
    pinMode(echo, INPUT);
}

void setupEncoders() {
    pinMode(l1_encoder, INPUT);
    pinMode(l2_encoder, INPUT);
    pinMode(r1_encoder, INPUT);
    pinMode(r2_encoder, INPUT);
}

void setupGyroscope() {
    Wire.begin();
    byte status = mpu.begin();
    Serial.print("MPU6050 status: ");
    Serial.println(status);
    while (status != 0) {}
    
    Serial.println("Calculating offsets, do not move Cha Cha");
    delay(1000);
    mpu.calcOffsets(true, true);
    mpu.setFilterGyroCoef(1.0);
    Serial.println("Done Gyro Calibration");
}

void setupBT() {
    bluetooth.begin(9600);
    Serial.print("Bluetooth started");
}

void setupClaw() {
    servo_bottom.attach(claw_bottom);
    servo_fl.attach(claw_front_left);
    servo_fr.attach(claw_front_right);
    clawRelease();
}

void setupMotor(int en, int in1, int in2) {
    pinMode(en, OUTPUT);
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);
}

double calibrateCheckSpeed(int speed) {
    int leftSpeed, rightSpeed;
    unsigned long start = millis();

    LOGF(LOG_CALIBRATE, "Trying %d\n", speed);

    resetEncoders();
    while (millis() - start < 1000) {
        updateEncoders();
        offset_speed = speed / 6;
        getSpeedWithCourseCorrection(&leftSpeed, &rightSpeed, speed);
        moveForward(leftSpeed, rightSpeed);
    }
    moveForward(0, 0);

    double ret = readEncoderCms();
    LOGF(LOG_CALIBRATE, "Done trying, got %d cm/s\n", ret);

    if ((int)ret == 0) {
        return 0.0;
    }

    resetEncoders();

    bool shouldMove = true;

    /*
    resetEncoders();
    while (shouldMove) {
        getSpeedWithCourseCorrection(&leftSpeed, &rightSpeed);
        shouldMove = moveBackwardByCms(leftSpeed, rightSpeed, speed);
    }
    resetEncoders();
    */
    moveForward(0, 0);

    return ret;
}

void setupCalibrateSpeed() {
    int defaultTrySpeed = 75;
    int trySpeed = defaultTrySpeed;
    int speed = 0;
    
    while (speed <= target_speed_cmps) {
        if (trySpeed >= 300) {
            LOGF(true, "Could not find suitable speed, stopping\n");
            while (true) {}
        }

        speed = calibrateCheckSpeed(trySpeed);
        trySpeed += 25;
    }

    LOGF(LOG_CALIBRATE, "Final Answer %d cm/s %d wheel speed\n", speed, trySpeed);

    default_speed = trySpeed;
    default_speed_cmps = speed;
    offset_speed = trySpeed / offset_speed_magic;
}

void setup() {
    Serial.begin(9600);

    moveForward(0, 0);

    setupGyroscope();
    setupBT();
    // setupClaw();
    setupEncoders();

    moveForward(0, 0);

    setupMotor(l1_en, l1_in1, l1_in2);
    moveForward(0, 0);
    setupMotor(l2_en, l2_in1, l2_in2);
    moveForward(0, 0);
    setupMotor(r1_en, r1_in1, r1_in2);
    moveForward(0, 0);
    setupMotor(r2_en, r2_in1, r2_in2);
    moveForward(0, 0);

    setupDistanceSensor(fl_ultrasonic_echo, fl_ultrasonic_trigger);
    setupDistanceSensor(fr_ultrasonic_echo, fr_ultrasonic_trigger);
    setupDistanceSensor(sl_ultrasonic_echo, sl_ultrasonic_trigger);
    setupDistanceSensor(sr_ultrasonic_echo, sr_ultrasonic_trigger);

    moveForward(0, 0);
    if (!skip_speed_calibration) {
        setupCalibrateSpeed();
    }

    moveForward(0, 0);
}
