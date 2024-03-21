/**
 * @file Unit_Finger_M5StickCPlus.ino
 * @author SeanKwok (shaoxiang@m5stack.com)
 * @brief
 * @version 0.1
 * @date 2024-03-20
 *
 *
 * @Hardwares: M5StickC Plus + Hat Finger
 * @Platform Version: Arduino M5Stack Board Manager v2.1.1
 * @Dependent Library:
 * M5_FPC1020A: https://github.com/m5stack/M5-FPC1020A
 */

#include "M5Unified.h"
#include "M5_FPC1020A.h"

M5_FPC1020A finger;

bool add_user_process(uint8_t id, uint8_t permission) {
    M5.Lcd.fillRect(0, 0, 240, 135, BLACK);
    M5.Lcd.setCursor(0, 0);
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

    M5.Lcd.setRotation(1);
    M5.Lcd.setTextColor(GREEN);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.println("Finger Unit init...");
    if (!finger.begin(&Serial2, 26, 0, 19200)) {
        Serial.println("FPC1020A not found");
        while (1) delay(1);
    }
    M5.Lcd.fillRect(0, 0, 240, 135, BLACK);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.println("Finger Unit TEST");

    uint8_t userNum = finger.getUserCount();
    Serial.print("userNum:");
    Serial.println(userNum);

    M5.Lcd.println("Delete All User");
    finger.delAllFinger();

    M5.Lcd.println("Btn.A add a user");
    M5.Lcd.println("Btn.B verify user");
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

    if (M5.BtnB.wasPressed()) {
        M5.Lcd.fillRect(0, 0, 240, 135, BLACK);
        M5.Lcd.setCursor(0, 0);
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
}
