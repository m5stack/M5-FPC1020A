/**
 * @file Unit_Finger_M5Core.ino
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

#include "M5Unified.h"
#include "M5_FPC1020A.h"

M5_FPC1020A finger;

#define FINGER_TOUCH_INI_PIN 35

bool add_user_process(uint8_t id, uint8_t permission) {
    M5.Lcd.fillRect(0, 0, 320, 200, BLACK);
    M5.Lcd.setCursor(0, 20);
    M5.Lcd.println("add finger process:");
    M5.Lcd.println("put your finger on the sensor");
    for (uint8_t i = 0; i < 6; i++) {
        while (!finger.addFinger(id, permission, i)) {
            Serial.printf("Finger ID: %d Finger Record:%d error\r\n", id, i);
            Serial.println("Retry...");
            delay(1000);
        };
        M5.Lcd.printf("add finger count : %d/6\r\n", i + 1);
        Serial.printf("Finger ID: %d Finger Record:%d ok\r\n", id, i);
    }
    return true;
}

void setup() {
    M5.begin();

    // init face finger panel power
    pinMode(26, OUTPUT);
    digitalWrite(26, 1);
    // init touch ic power
    pinMode(5, OUTPUT);
    digitalWrite(5, 1);

    delay(100);

    pinMode(FINGER_TOUCH_INI_PIN, INPUT);

    M5.Lcd.setTextColor(GREEN);
    M5.Lcd.setTextSize(2);

    M5.Lcd.drawString("Add User", 10, 210);
    M5.Lcd.drawString("Verify", 125, 210);
    M5.Lcd.drawString("Del User", 220, 210);

    M5.Lcd.setCursor(0, 20);
    M5.Lcd.println("Finger Unit init...");
    while (!finger.begin(&Serial2, 16, 17, 19200)) {
        Serial.println("FPC1020A not found");
        delay(1000);
    }

    M5.Lcd.fillRect(0, 0, 320, 200, BLACK);

    M5.Lcd.setCursor(0, 20);
    M5.Lcd.println("Finger Unit TEST");
    M5.Lcd.println("1. delete all user");
    M5.Lcd.println("2. add a user fingerprint");
    M5.Lcd.println("3. verify user permission");

    uint8_t userNum = finger.getUserCount();
    Serial.print("userNum:");
    Serial.println(userNum);
}

uint8_t new_user_id = 1;

void loop() {
    M5.update();
    if (M5.BtnA.wasPressed()) {
        // user id: 1 ~ 0xfff
        if (add_user_process(new_user_id, 1)) {
            Serial.println("add user success");
            M5.Lcd.println("add user success");
            new_user_id++;
        }
    }
    if (M5.BtnB.wasPressed() || digitalRead(FINGER_TOUCH_INI_PIN)) {
        M5.Lcd.fillRect(0, 0, 320, 200, BLACK);
        M5.Lcd.setCursor(0, 20);
        uint8_t res = finger.available();
        if (res == ACK_SUCCESS) {
            M5.Lcd.println("Success");
            Serial.println("Success");
            M5.Lcd.print("User ID: ");
            Serial.print("User ID: ");
            M5.Lcd.println(finger.getFingerID());
            Serial.println(finger.getFingerID());
            M5.Lcd.print("User Permission: ");
            Serial.print("User Permission: ");
            M5.Lcd.println(finger.getFingerPermission());
            Serial.println(finger.getFingerPermission());
        } else {
            Serial.println("No User");
            M5.Lcd.println("No User");
        }
    }

    if (M5.BtnC.wasPressed()) {
        M5.Lcd.fillRect(0, 0, 320, 200, BLACK);
        M5.Lcd.setCursor(0, 20);
        Serial.println("Delete All User");
        M5.Lcd.println("Delete All User");
        finger.delAllFinger();
    }
}
