#include "M5_FPC1020A.h"

M5_FPC1020A::M5_FPC1020A() {
}

bool M5_FPC1020A::begin(HardwareSerial *serial, uint8_t rx, uint8_t tx,
                        unsigned long baud) {
    _serial = serial;
    _serial->begin(baud, SERIAL_8N1, rx, tx);

    if (getUserCount() != 0xff) {
        return true;
    }
    return false;
}

bool M5_FPC1020A::enableDebug(HardwareSerial *debug_serial) {
    _debug        = debug_serial;
    _enable_debug = true;
    return true;
}

bool M5_FPC1020A::setBaud(unsigned long baud) {
    uint8_t res;
    TxBuf[CMD] = CMD_BAUD;
    TxBuf[P1]  = 0;
    TxBuf[P2]  = 0;

    switch (baud) {
        case 9600:
            TxBuf[P3] = BAUD_9600;
            break;
        case 19200:
            TxBuf[P3] = BAUD_19200;
            break;
        case 38400:
            TxBuf[P3] = BAUD_38400;
            break;
        case 57600:
            TxBuf[P3] = BAUD_57600;
            break;
        case 115200:
            TxBuf[P3] = BAUD_115200;
            break;
        default:
            return false;
            break;
    }
    res = sendCMD(1500);
    if (res == ACK_SUCCESS) {
        return true;
    } else {
        return false;
    }
}

/*! @brief Send a message and wait for the specified time until the return value
 is received.*/
uint8_t M5_FPC1020A::sendCMD(uint16_t timeout) {
    uint8_t i, j;
    uint8_t checkSum = 0;
    memset(RxBuf, 0, 9);
    TxBuf[5] = 0;

    _serial->write(CMD_HEAD);

    if (_enable_debug) {
        _debug->println();
        _debug->println("Send start:");
        _debug->printf("%02X ", CMD_HEAD);
    }

    for (i = 1; i < 6; i++) {
        _serial->write(TxBuf[i]);
        if (_enable_debug) {
            _debug->printf("%02X ", TxBuf[i]);
        }
        checkSum ^= TxBuf[i];
    }
    _serial->write(checkSum);
    _serial->write(CMD_TAIL);
    if (_enable_debug) {
        _debug->printf("%02X ", checkSum);
        _debug->printf("%02X ", CMD_TAIL);
        _debug->println();
        _debug->println("Send end");
    }

    uint8_t ch;
    unsigned long start = millis();

    if (_enable_debug) {
        _debug->println();
        _debug->println("Receive start:");
    }

    uint8_t index = 0;
    memset(RxBuf, 0, 9);
    _serial->flush();

    while (1) {
        if (_serial->available()) {
            ch           = _serial->read();
            RxBuf[index] = ch;
            if (_enable_debug) {
                _debug->printf("%02X ", ch);
            }
            if (RxBuf[0] == 0xf5) {
                if (index < 7) {
                    index++;
                }
            } else {
                index = 0;
            }
            if (RxBuf[0] == 0xf5 && RxBuf[7] == 0xf5) {
                break;
            }
        } else if (millis() - start > timeout) {
            _serial->flush();
            break;
        }
    }

    if (_enable_debug) {
        _debug->println();
        _debug->println("Receive end");
    }

    if (RxBuf[HEAD] != CMD_HEAD) return ACK_FAIL;
    if (RxBuf[TAIL] != CMD_TAIL) return ACK_FAIL;
    if (RxBuf[CMD] != (TxBuf[CMD])) return ACK_FAIL;

    checkSum = 0;
    for (j = 1; j < CHK; j++) {
        checkSum ^= RxBuf[j];
    }
    if (checkSum != RxBuf[CHK]) {
        return ACK_FAIL;
    }
    return ACK_SUCCESS;
}

/*! @brief Putting the chip to sleep.*/
bool M5_FPC1020A::sleep(void) {
    uint8_t res;

    TxBuf[CMD] = CMD_SLEEP_MODE;
    TxBuf[P1]  = 0;
    TxBuf[P2]  = 0;
    TxBuf[P3]  = 0;

    res = sendCMD(1500);

    if (res == ACK_SUCCESS) {
        return true;
    } else {
        return false;
    }
}

/*! @brief Set mode to add mode.*/
bool M5_FPC1020A::setFingerMode(fpc1020a_add_finger_mode_t mode) {
    uint8_t res;

    TxBuf[CMD] = CMD_ADD_MODE;
    TxBuf[P1]  = 0;
    TxBuf[P2]  = mode;
    TxBuf[P3]  = 0;

    res = sendCMD(1200);

    if (res == ACK_SUCCESS && RxBuf[Q3] == ACK_SUCCESS) {
        return true;
    } else {
        return false;
    }
}

uint8_t M5_FPC1020A::getFingerMode(void) {
    TxBuf[CMD] = CMD_ADD_MODE;
    TxBuf[P1]  = 0;
    TxBuf[P2]  = 0;
    TxBuf[P3]  = 0x01;

    if (sendCMD(1200) == ACK_SUCCESS) {
        return RxBuf[Q2];
    }
    return 0xff;
}

/*! @brief Get the user's number.*/
uint16_t M5_FPC1020A::getUserCount(void) {
    uint8_t res;

    TxBuf[CMD] = CMD_USER_CNT;
    TxBuf[P1]  = 0;
    TxBuf[P2]  = 0;
    TxBuf[P3]  = 0;

    res = sendCMD(1200);

    if (res == ACK_SUCCESS && RxBuf[Q3] == ACK_SUCCESS) {
        return RxBuf[Q2];
    } else {
        return 0xFF;
    }
}

/*! @brief Initialize the EXTIO2.*/
bool M5_FPC1020A::delAllFinger(void) {
    uint8_t res;

    TxBuf[CMD] = CMD_DEL_ALL;
    TxBuf[P1]  = 0;
    TxBuf[P2]  = 0;
    TxBuf[P3]  = 0;

    res = sendCMD(1200);

    if (res == ACK_SUCCESS && RxBuf[Q3] == ACK_SUCCESS) {
        return true;
    } else {
        return false;
    }
}

/*! @brief Delete all user information.*/
bool M5_FPC1020A::delFinger(uint8_t id) {
    uint8_t res;

    TxBuf[CMD] = CMD_DEL;
    TxBuf[P1]  = 0;
    TxBuf[P2]  = id;
    TxBuf[P3]  = 0;

    res = sendCMD(1200);

    if (res == ACK_SUCCESS && RxBuf[Q3] == ACK_SUCCESS) {
        return true;
    } else {
        return false;
    }
}

/*! @brief Add User.*/
bool M5_FPC1020A::addFinger(uint8_t id, uint8_t permission,
                            uint8_t adding_count) {
    uint8_t res;

    if (adding_count == 0) {
        TxBuf[CMD] = CMD_ADD_1;
    } else if (adding_count < 5) {
        TxBuf[CMD] = CMD_ADD_2;
    } else {
        TxBuf[CMD] = CMD_ADD_3;
    }

    TxBuf[P1] = 0;
    TxBuf[P2] = id;
    TxBuf[P3] = permission;
    res       = sendCMD(2000);
    if (res == ACK_SUCCESS) {
        return true;
    }
    return false;
}

/*! @brief Compare fingerprint information.*/
uint8_t M5_FPC1020A::available(time_t timeout) {
    uint8_t res;

    TxBuf[CMD] = CMD_MATCH;
    TxBuf[P1]  = 0;
    TxBuf[P2]  = 0;
    TxBuf[P3]  = 0;

    res = sendCMD(timeout);

    if (res == ACK_SUCCESS) {
        if (RxBuf[Q3] == ACK_NOUSER) {
            return ACK_NOUSER;
        }
        if (RxBuf[Q3] == ACK_TIMEOUT) {
            return ACK_TIMEOUT;
        }
        if ((RxBuf[Q2] != 0) &&
            (RxBuf[Q3] == 1 || RxBuf[Q3] == 2 || RxBuf[Q3] == 3)) {
            return ACK_SUCCESS;
        }
    }
    return res;
}

/*! @brief Get the user's id.*/
uint8_t M5_FPC1020A::getFingerID(void) {
    return RxBuf[Q1] << 8 | RxBuf[Q2];
}

/*! @brief Get the user's Permission.*/
uint8_t M5_FPC1020A::getFingerPermission(void) {
    return RxBuf[Q3];
}
