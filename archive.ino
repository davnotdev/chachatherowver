#if 0

void oldloop() {
    int distanceCmFrontLeft = readDistanceSensor(fl_ultrasonic_echo, fl_ultrasonic_trigger);
    int distanceCmFrontRight = readDistanceSensor(fr_ultrasonic_echo, fr_ultrasonic_trigger);

    mpu.update();
    int yaw = readGyroYaw();

    int leftSpeed = default_speed;
    int rightSpeed = default_speed;

    bool shouldLeftOffsetSpeed = yaw <= -direction_correction_threshold;
    bool shouldRightOffsetSpeed = yaw >= direction_correction_threshold;

    if (yaw >= direction_correction_threshold) {
        rightSpeed -= offset_speed * yaw;
        leftSpeed += offset_speed * yaw;
    } else if (yaw <= -direction_correction_threshold) {
        rightSpeed -= offset_speed * yaw;
        leftSpeed += offset_speed * yaw;
    }

    /*
    Serial.print("Yaw: ");
    Serial.print(yaw);
    Serial.print(" Ultrasonic Left: ");
    Serial.print(distanceCmFrontLeft);
    Serial.print(" Ultrasonic Right: ");
    Serial.println(distanceCmFrontRight);
    */

    if (distanceCmFrontLeft < object_front_threshold || distanceCmFrontRight < object_front_threshold) {
      // NOTE: Change <= to < and the other to >= depending on obsticle course
      // By default, if both sensors detect object, go right.
        if (distanceCmFrontLeft <= distanceCmFrontRight) {
            /// Cha cha to right
            Serial.println("cha cha to right");
            /*
              oneSideForwardBack(
                  l1_en,
                  l1_in1,
                  l1_in2,
                  l2_en,
                  l2_in1,
                  l2_in2,
                  default_speed
              );
              oneSideBackForward(
                  r1_en,
                  r1_in1,
                  r1_in2,
                  r2_en,
                  r2_in1,
                  r2_in2,
                  default_speed
              ); 
            */
        } else if (distanceCmFrontLeft > distanceCmFrontRight){
            /// Cha cha to left
            Serial.println("cha cha to left");
            /*
            oneSideForwardBack(
                r1_en,
                r1_in1,
                r1_in2,
                r2_en,
                r2_in1,
                r2_in2,
                default_speed,   
            );

             oneSideForwardBack(
                r1_en, 
                r1_in1,
                r1_in2,
                r2_en,
                r2_in1,
                r2_in2,
                default_speed,
            );  
            */
        }
    } else {
        goForwardOneSide(
            l1_en,
            l1_in1,
            l1_in2,
            l2_en,
            l2_in1,
            l2_in2,
            default_speed
        );
        goForwardOneSide(
            r1_en,
            r1_in1,
            r1_in2,
            r2_en,
            r2_in1,
            r2_in2,
            default_speed
        );
    }

    goBackwardOneSide(
        r1_en,
        r1_in1,
        r1_in2,
        r2_en,
        r2_in1,
        r2_in2,
        default_speed
    );
    
    delay(3000);

}

#endif
