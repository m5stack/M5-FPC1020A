/*!
 * @brief A fingerprint sensor From M5Stack
 * @copyright Copyright (c) 2022 by M5Stack[https://m5stack.com]
 *
 * @Links [Unit Finger](https://docs.m5stack.com/en/unit/finger)
 * @Links [Hat Finger](https://docs.m5stack.com/en/hat/hat-finger)
 * @Links [Module Finger](https://docs.m5stack.com/en/module/faces_finger)
 * @version  V0.0.1
 * @date  2022-07-18
 */
#ifndef _M5_FPC1020A_H_
#define _M5_FPC1020A_H_

#include "Arduino.h"
#include "M5_FPC1020A_CMD.h"

typedef enum {
    FINGER_NOT_REPEAT,
    FINGER_ALLOW_REPEAT
} fpc1020a_add_finger_mode_t;

typedef enum {
    BAUD_9600 = 1,
    BAUD_19200,
    BAUD_38400,
    BAUD_57600,
    BAUD_115200,
} fpc1020a_uart_baud_t;

class M5_FPC1020A {
   private:
    HardwareSerial *_serial;
    HardwareSerial *_debug;
    bool _enable_debug = false;
    uint8_t tx;
    uint8_t rx;

   public:
    M5_FPC1020A();
    bool begin(HardwareSerial *serial = &Serial2, uint8_t rx = 16,
               uint8_t tx = 17, unsigned long baud = 19200);
    bool enableDebug(HardwareSerial *debug_serial);
    uint8_t sendCMD(uint16_t timeout);
    bool sleep(void);

    bool setFingerMode(fpc1020a_add_finger_mode_t mode = FINGER_NOT_REPEAT);
    uint8_t getFingerMode();

    uint16_t getUserCount(void);
    bool addFinger(uint8_t id, uint8_t permission, uint8_t adding_count);
    bool delFinger(uint8_t id);
    bool delAllFinger();

    uint8_t available(time_t timeout = 500);
    uint8_t getFingerID(void);
    uint8_t getFingerPermission(void);

    bool setBaud(unsigned long baud = 19200);

   public:
    uint8_t TxBuf[9];
    uint8_t RxBuf[9];
};

#endif
