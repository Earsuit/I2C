/*
    I2C.cpp - a free library for I2C communication of Arduino Nano or Mega. I haven't
    tested it on other boards but any one uses ATmega48A/PA/88A/PA/168A/PA/328/P
    or Atmel ATmega640/V-1280/V-1281/V-2560/V-2561/V chip should work.

    Please refer to https://longnight975551865.wordpress.com/2018/02/11/write-your-own-i%c2%b2c-library/ for more information.

    Feedback and contribution is welcome!

    Created by Yudi Ren, Feb 1, 2018.
    renyudicn@outlook.com
    Version 0.1
*/

#include "I2C.h"
#include <Arduino.h>

//buffer
uint8_t rxBuffer[RX_BUFFER_SIZE];
uint8_t rxBufferIndex;
uint8_t rxBufferLength;

/*
    Initialise the I2C interface

    @param selfAddress: the self selfAddress
    @param i2cFreq: the I2C bus clock frequency, default is 100, unit KHz
    @param slave: does this device work as a slave, default is false
    @param generalCall: does this device response to general call, default is false
    @param interrupt: does this device use I2C interrupt,default is false
*/
void TWI::I2CSetup(uint8_t selfAddress, int i2cFreq, bool slave, bool generalCall, bool interrupt){
    //turn on the internal pull-up resistor
    pinMode(SDA, INPUT_PULLUP);
    pinMode(SCL, INPUT_PULLUP);

    /*
        The formula to calculate the SCL freq is on p212 of the datasheet of
        the ATMEL328p.
    */
    TWBR = (CPU_FREQ/i2cFreq/1000-16)/2;
    //set the address
    if(generalCall)
        TWAR = (selfAddress<<1) | RESPOND_GC;
    else
        TWAR = (selfAddress<<1) | NOT_RESPOND_GC;

    if(slave)
        TWCR = (_BV(TWEA)) | (_BV(TWEN));

    if(interrupt){
        TWCR |= _BV(TWIE);
        sei();
    }

    rxBufferIndex = 0;
    rxBufferLength = 0;
}

/*
    Change the I2C bus clock frequency

    @param freq: I2C bus frequency, unit is KHz
*/
void TWI::setFreq(int freq){
    TWBR = (CPU_FREQ/freq/1000-16)/2;
}

/*
    A switch for I2C interface Interrupt

    @param on: on and off of the interrupt
*/
void TWI::interrupt(bool on){
    if(on)  //enable the TWI interrupt
        TWCR |= _BV(TWIE);
    else  //switch off the TWI interrupt
        TWCR &= (~(_BV(TWIE)));
}

/*
    Master sends START information, slave address and WRITE/READ bit

    @param address: slave address
    @param type: WRITE or READ, predefined in I2C.h file
    @param repeatStart: whether it's a repeat start
    @return whether the operation is successful
*/
bool TWI::startTrans(uint8_t address, uint8_t type, bool repeatStart){
    //send start condition
    TWCR = (_BV(TWINT)) | (_BV(TWSTA)) | (_BV(TWEN));
    while(!(TWCR & (_BV(TWINT))));
    uint8_t status = TWSR & STATUS_CODE_MASK;
    if(!repeatStart && status != START){
        error(START);
        return false;
    }else if(repeatStart && status != RE_START){
        error(RE_START);
        return false;
    }

    //send the address and WRITE/READ bit
    switch(type){
        case WRITE:
            TWDR = (address<<1) | WRITE;
            TWCR = (_BV(TWINT)) | (_BV(TWEN));
            while (!(TWCR & (_BV(TWINT))));
            if((TWSR & STATUS_CODE_MASK) != MT_SLA_W_ACK){
                error(MT_SLA_W_ACK);
                return false;
            }
            break;
        case READ:
            TWDR = (address<<1) | READ;
            TWCR = (_BV(TWINT)) | (_BV(TWEN));
            while (!(TWCR & (_BV(TWINT))));
            if((TWSR & STATUS_CODE_MASK) != MR_SLA_R_ACK){
                error(MR_SLA_R_ACK);
                return false;
            }
            break;
    }
    return true;
}

/*
    Master writes data to the slave, this function can only send one byte at a
    time, if you want to use burst write mode, just set the stop parameter to false

    @param data: data to be sent
    @param stop: does it send STOP signal
*/
void TWI::write(uint8_t data, bool stop){
    TWDR = data;
    TWCR = (_BV(TWINT)) | (_BV(TWEN));
    while(!(TWCR & (_BV(TWINT))));
    if((TWSR & STATUS_CODE_MASK) != MT_DATA_ACK)
        error(MT_DATA_ACK);

    if(stop)
        stopTrans();
}

/*
    Master request data from the slave. It will send a START signal so you need
    to specify whether it's a repeat start

    @param slaveAddress: the slave address
    @param num: number of bytes requested from the slave, it should be less than
                RX_BUFFER_SIZE
    @param stop: does it send a STOP signal
    @param repeatStart: is this a repeat start
*/
void TWI::requestFrom(uint8_t slaveAddress, uint8_t num, bool stop, bool repeatStart){
    //initialise the buffer
    rxBufferLength = num;
    rxBufferIndex = 0;

    startTrans(slaveAddress,READ,repeatStart);

    //if the requested number of bytes is larger than RX_BUFFER_SIZE,
    //truncate it to RX_BUFFER_SIZE
    if(num>RX_BUFFER_SIZE)
        num = RX_BUFFER_SIZE;

    for(uint8_t n=0;n<num-1;n++){
        TWCR = (_BV(TWEA)) | (_BV(TWINT)) | (_BV(TWEN));
        while(!(TWCR & (_BV(TWINT))));
        if((TWSR & STATUS_CODE_MASK) != MR_DATA_ACK)
             error(MR_DATA_ACK);
        rxBuffer[n] = TWDR;
    }

    //last byte, no TWEA (generate NACK)
    TWCR = (_BV(TWINT)) | (_BV(TWEN));
    while(!(TWCR & (_BV(TWINT))));
    if((TWSR & STATUS_CODE_MASK) != MR_DATA_NACK)
         error(MR_DATA_NACK);
    rxBuffer[num-1] = TWDR;

    if(stop)
        stopTrans();
}

/*
    Read the receive buffer

    @return one byte in the buffer
*/
uint8_t TWI::readBuffer(){
    if(rxBufferLength > 0){
        rxBufferLength--;
        return rxBuffer[rxBufferIndex++];
    }
    return 0;
}

/*
    get the rxBuffer array

    @return the rxBuffer array pointer
*/
uint8_t* TWI::getBuffer(){
    return rxBuffer;
}

/*
    Send STOP signal
*/
void TWI::stopTrans(){
    TWCR = (_BV(TWINT))|(_BV(TWEN)) | (_BV(TWSTO));
    while(!(TWCR & (_BV(TWSTO))));
}

/*
    Function to print the status code it should be when an error happens
*/
void TWI::error(uint8_t type,uint8_t info){
    switch(type){
        case START:
            Serial.println("START ");
            break;
        case RE_START:
            Serial.println("RE_START");
            break;
        case ARB_LOST:
            Serial.println("ARB_LOST");
            break;
        case MT_SLA_W_ACK:
            Serial.println("MT_SLA_W_ACK ");
            break;
        case MT_SLA_W_NACK:
            Serial.println("MT_SLA_W_NACK");
            break;
        case MT_DATA_ACK:
            Serial.println("MT_DATA_ACK");
            break;
        case MT_DATA_NACK:
            Serial.println("MT_DATA_NACK");
            break;
        case MR_SLA_R_ACK:
            Serial.print("MR_SLA_R_ACK ");
            Serial.println(info,HEX);
            break;
        case MR_SLA_R_NACK:
            Serial.println("MR_SLA_R_NACK");
            break;
        case MR_DATA_ACK:
            Serial.println("MR_DATA_ACK");
            break;
        case MR_DATA_NACK:
            Serial.println("MR_DATA_NACK");
            break;
        case SR_AD_RECEIVED:
            Serial.println("SR_AD_RECEIVED");
            break;
        case SR_ARB_AD:
            Serial.println("SR_ARB_AD");
            break;
        case GENERAL_CALL:
            Serial.println("GENERAL_CALL");
            break;
        case SR_ARB_GC:
            Serial.println("SR_ARB_GC");
            break;
        case SR_PRE_AD_DATA_ACK:
            Serial.println("SR_PRE_AD_DATA_ACK");
            break;
        case SR_PRE_AD_DATA_NACK:
            Serial.println("SR_PRE_AD_DATA_NACK");
            break;
        case SR_PRE_GC_DATA_ACK:
            Serial.println("SR_PRE_GC_DATA_ACK");
            break;
        case SR_PRE_GC_DATA_NACK:
            Serial.println("SR_PRE_GC_DATA_NACK");
            break;
        case SR_STOP:
            Serial.println("SR_STOP");
            break;
        default:
            Serial.println("Unknown error");
            break;
    }
}

/*
    Slave receives data from a master. To use this function, you need to initialise
    the chip to a slave by using the I2CSetup(). One needs to make sure that the
    number of bytes to be sent should not exceed RX_BUFFER_SIZE.
*/
void TWI::receive(){
    uint8_t backup = TWCR;
    //if the own address has been received
    uint8_t status = TWSR & STATUS_CODE_MASK;
    if(status == SR_AD_RECEIVED || status == GENERAL_CALL){
        TWCR = (_BV(TWINT)) | (_BV(TWEN)) | (_BV(TWEA));
        //initialise the buffer
        rxBufferIndex = 0;
        rxBufferLength = 0;
        while(!(TWCR & (_BV(TWINT))));

        status = TWSR & STATUS_CODE_MASK;
        while(status == SR_PRE_AD_DATA_ACK || status == SR_PRE_GC_DATA_ACK){
            rxBuffer[rxBufferIndex++] = TWDR;
            rxBufferLength++;
            TWCR = (_BV(TWINT)) | (_BV(TWEN)) | (_BV(TWEA));
            while(!(TWCR & (_BV(TWINT))));
            status = TWSR & STATUS_CODE_MASK;
        }

        //if received a STOP signal
        if(status == SR_STOP){
            rxBufferIndex = 0;
        }else{
            error(status);
        }
    }

    TWCR = backup;
}
