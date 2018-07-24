/*
    This is an example code using the I2C library to get the raw data from the MPU9250 or MPU6050.

    Please refer to https://longnight975551865.wordpress.com/2018/02/20/how-to-read-data-from-mpu9250/
    for detailed information.

    Feedback and contribution is welcome!

    Created by Yudi Ren, Feb 1, 2018.
    renyudicn@outlook.com
    Version 0.1
*/
#include <math.h>
#include <I2C.h>

//MPU9250
#define MPU9250_AD 0x68
#define FIFO_EN_AD 0x23
#define PWR_MGMT_1_AD 0x6B
#define ACCEL_XOUT_H_AD 0x3B
#define GYRO_XOUT_H_AD 0x43
#define EXT_SENS_DATA_00_AD 0x49
#define ACCEL_CONFIG_1_AD 0x1C
#define ACCEL_CONFIG_2_AD 0x1D
#define GYRO_CONFIG_AD 0x1B
#define CONFIG_AD 0x1A
#define I2C_MST_CTRL_AD 0x24
#define I2C_SLV0_ADDR_AD 0x25
#define I2C_SLV0_REG_AD 0x26
#define I2C_SLV0_CTRL_AD 0x27
#define INT_BYPASS_CONFIG_AD 0x37
#define USER_CTRL_AD 0x6A
#define ACCEL_SENS 8192.0f
#define GYRO_SENS 131.0f

//Magnetometer
#define MAG_AD 0xC
#define WIA_AD 0x00
#define INFO 0x01
#define STATUS_1_AD 0x02
#define HXL_AD 0x03    //X-axis measurement data lower 8bit
#define HXH_AD 0x04    //X-axis measurement data higher 8bit
#define HYL_AD 0x05    //Y-axis measurement data lower 8bit
#define HYH_AD 0x06    //Y-axis measurement data higher 8bit
#define HZL_AD 0x07    //Z-axis measurement data lower 8bit
#define HZH_AD 0x08    //Z-axis measurement data higher 8bit
#define STATUS_2_AD 0x09
#define CNTL1_AD 0x0A   //control 1
#define CNTL2_AD 0x0B   //control 2
#define ASTC_AD 0x0C    //Self-Test Control
#define TS1_AD 0x0D    //test 1
#define TS2_AD 0x0E   //test 2
#define I2CDIS_AD 0x0F    //I2C disable
#define ASAX_AD 0x10    //Magnetic sensor X-axis sensitivity adjustment value
#define ASAY_AD 0x11    //Magnetic sensor Y-axis sensitivity adjustment value
#define ASAZ_AD 0x12    //Magnetic sensor Z-axis sensitivity adjustment value
#define MAGNE_SENS 6.67f
#define SCALE 0.1499f  // 4912/32760 uT/tick
#define DATA_READY 0x01
#define MAGIC_OVERFLOW 0x8

//timer interrupt
#define OUTPUT_COMPARE 0x186A //6250, 0.025s

#define SELF_AD 0x18
#define I2C_FREQ 400

#define DRONE_AD 0x26

volatile float accelX,accelY,accelZ,gyroX,gyroY,gyroZ,magneX,magneY,magneZ,asax,asay,asaz;
bool updated = false;

using namespace TWI;

void setup() {
    Serial.begin(115200);
    I2CSetup(SELF_AD,I2C_FREQ);
    MPU9250Setup();
    timerInterruptSetup();
}

void loop(){
    if(updated){
        Serial.print("GYROX: ");
        Serial.print(gyroX/GYRO_SENS);
        Serial.print(" GYROY: ");
        Serial.print(gyroY/GYRO_SENS);
        Serial.print(" GYROZ: ");
        Serial.print(gyroZ/GYRO_SENS);
        Serial.print(" ACCELX: ");
        Serial.print(accelX/ACCEL_SENS);
        Serial.print(" ACCELY: ");
        Serial.print(accelY/ACCEL_SENS);
        Serial.print(" ACCELZ: ");
        Serial.println(accelZ/ACCEL_SENS);
        updated = false;
    }
}

ISR(TIMER1_COMPA_vect){
    updateGyroReading();
    updated = true;
}

void MPU9250Setup(){
    startTrans(MPU9250_AD);
    write(PWR_MGMT_1_AD);
    write(0x01,true); //set the clock reference to X axis gyroscope to get a better accuracy

    startTrans(MPU9250_AD);
    write(ACCEL_CONFIG_1_AD);
    write(0x08,true); //set the accel scale to 4g

    startTrans(MPU9250_AD);
    write(ACCEL_CONFIG_2_AD);
    //turn on the internal low-pass filter for accel with 5.05Hz bandwidth
    write(0x05,true);

    startTrans(MPU9250_AD);
    write(GYRO_CONFIG_AD);
    write(0x08,true); //set the gyro scale to 500 °/s and FCHOICE_B

    // turn on the internal low-pass filter for gyro with 10Hz bandwidth
    startTrans(MPU9250_AD);
    write(CONFIG_AD);
    write(0x05,true);
}

void updateGyroReading(){
    //read the accelerate
    startTrans(MPU9250_AD);
    write(ACCEL_XOUT_H_AD);
    requestFrom(MPU9250_AD,6,true);
    accelX = (readBuffer()<<8) | readBuffer();
    accelY = (readBuffer()<<8) | readBuffer();
    accelZ = (readBuffer()<<8) | readBuffer();

    //read the gyro
    startTrans(MPU9250_AD);
    write(GYRO_XOUT_H_AD);
    requestFrom(MPU9250_AD,6,true);
    gyroX = (readBuffer()<<8) | readBuffer();
    gyroY = (readBuffer()<<8) | readBuffer();
    gyroZ = (readBuffer()<<8) | readBuffer();
}

void timerInterruptSetup(){
    cli();  //disable the global interrupt
    //Timer/Counter 1
    TCCR1A = 0x00;
    TCCR1B = (_BV(WGM12)) | (_BV(CS11)) | (_BV(CS10));  //CTC mode, clk/64
    OCR1A = OUTPUT_COMPARE; //set to 200ms
    TCNT1 = 0x00; //initialise the counter
    TIMSK1 = _BV(OCIE1A);  //Output Compare A Match Interrupt Enable
    sei(); //enable global interrupt
}