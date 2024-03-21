
/**
 * @file Sample.ino
 * @author SeanKwok (shaoxiang@m5stack.com)
 * @brief
 * @version 0.1
 * @date 2024-03-20
 *
 *
 * @Hardwares: M5Core + Unit Finger
 * @Platform Version: Arduino M5Stack Board Manager v2.1.1
 * @Dependent Library:
 * M5_FPC1020A: https://github.com/m5stack/M5-FPC1020A
 */

#include "M5_FPC1020A.h"

M5_FPC1020A finger;

bool add_user_process(uint8_t id, uint8_t permission) {
    for (uint8_t i = 0; i < 6; i++) {
        while (!finger.addFinger(id, permission, i)) {
            Serial.printf("Finger ID: %d Finger Record:%d error\r\n", id, i);
            Serial.println("Retry...");
            delay(1000);
        };
        Serial.printf("Finger ID: %d Finger Record:%d ok\r\n", id, i);
    }
    return true;
}

uint8_t new_user_id = 1;

void setup() {
    // wait for unit boot
    delay(100);
    Serial.begin(115200);
    while (!finger.begin(&Serial2, 16, 17, 19200)) {
        Serial.println("FPC1020A not found");
        delay(1000);
    }

    Serial.println("Delete All User");
    finger.delAllFinger();

    Serial.println("Start Add User Process");
    Serial.println("Now Keep Your Finger on the Sensor...");
    add_user_process(new_user_id, 1);
    // if you wanna check the debug message, you can enable it
    // finger.enableDebug(&Serial);

    uint8_t userNum = finger.getUserCount();
    Serial.print("userNum:");
    Serial.println(userNum);
}

void loop() {
    if (finger.available() == ACK_SUCCESS) {
        Serial.println("Success");
        Serial.print("User ID: ");
        Serial.println(finger.getFingerID());
        Serial.print("User Permission: ");
        Serial.println(finger.getFingerPermission());
    }
}