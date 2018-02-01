/*
    I2C.h - A free library for I2C communication of Arduino Nano or Mega. I haven't Test
    it on other boards but any one uses ATmega48A/PA/88A/PA/168A/PA/328/P
    or Atmel ATmega640/V-1280/V-1281/V-2560/V-2561/V chip should work.

    Feedback and contribution is welcome!

    Created by Yudi Ren, Feb 1, 2018.
    renyudicn@outlook.com
    Version 0.1
*/

#ifndef I2C_H_
#define I2C_H_

#include <Arduino.h>

//I2C status codes
#define CPU_FREQ 16000000
#define STATUS_CODE_MASK 0xF8
#define START 0x08
#define RE_START 0x10
#define MT_SLA_W_ACK 0x18
#define MT_SLA_W_NACK 0x20
#define MT_DATA_ACK 0x28
#define MT_DATA_NACK 0x30
#define ARB_LOST 0x38
#define MR_SLA_R_ACK 0x40
#define MR_SLA_R_NACK 0x48
#define MR_DATA_ACK 0x50
#define MR_DATA_NACK 0x58
#define SR_AD_RECEIVED 0x60
#define SR_ARB_AD 0x68
#define GENERAL_CALL 0x70
#define SR_ARB_GC 0x78
#define SR_PRE_AD_DATA_ACK 0x80
#define SR_PRE_AD_DATA_NACK 0x88
#define SR_PRE_GC_DATA_ACK 0x90
#define SR_PRE_GC_DATA_NACK 0x98
#define SR_STOP 0xA0

#define RX_BUFFER_SIZE 32
#define WRITE 0x00
#define READ 0x01
#define RESPOND_GC 0x01
#define NOT_RESPOND_GC 0x00

namespace TWI{
    //unit: khz, the max freq for ATMEL328p is 400khz
    void I2CSetup(uint8_t selfAddress,int i2cFreq=100,bool slave = false,bool generalCall = false, bool interrupt = false);
    bool startTrans(uint8_t address, uint8_t type = WRITE, bool repeatStart = false);
    void write(uint8_t data,bool stop = false);
    void stopTrans();
    void error(uint8_t type, uint8_t info = 0x00);
    void requestFrom(uint8_t deviceAd, uint8_t num, bool stop, bool repeatStart = true);
    uint8_t readBuffer();
    void setFreq(int freq);  //unit: khz
    void receive();
    void interrupt(bool on);
    uint8_t* getBuffer();
}

#endif
