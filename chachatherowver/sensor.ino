#define DIST_SENSOR_MAX_DISTANCE 100.0

double readGyroYaw() {
    mpu.update();
    double reading = mpu.getAngleZ();
    LOGF(LOG_SENSOR_READINGS, "Yaw Reading %d \n", reading);
    return reading;
}

double readRawDistanceSensor(int echo, int trigger) {
    digitalWrite(trigger, LOW);
    delayMicroseconds(2);
    digitalWrite(trigger, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigger, LOW);

    unsigned long duration = pulseIn(echo, HIGH); 
    double cm = ((double)duration / 2.0) / 29.1;
    return cm;
}

double readDistanceSensor(int echo, int trigger) {
    /*
    int readCount = 2;
    double readingSum = 0.0;

    for (int i = 0; i < readCount; i++) {
        double reading = readRawDistanceSensor(echo, trigger);
        LOGF(LOG_SENSOR_READINGS, "Distance Readings IDX: %d VALUE: %d \n", i, reading);

        // Discard the reading if it is negative.
        if (reading < 0.0 || reading >= 2000) {
            readCount -= 1;
        } else {
            readingSum += reading;
        }
    }

    double ret = readingSum / readCount;
    */
    double ret = readRawDistanceSensor(echo, trigger);
    LOGF(LOG_SENSOR_READINGS, "Avg Distance Reading VALUE: %d \n-----", ret);
    return ret;

    // If all readings were negative, then there is nothing in front of us.
    // return readCount == 0 ? DIST_SENSOR_MAX_DISTANCE : ret;
}
