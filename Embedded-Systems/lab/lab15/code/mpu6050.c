/*
 * File:   mpu6050.c
 * Author: chris
 *
 * Created on May 18, 2020, 11:50 AM
 */

#include "./mcc_generated_files/mcc.h"
#include <xc.h>
#include "./mpu6050.h"


//----------------------------------------------------------------
// MPU6050_standby()
//----------------------------------------------------------------
void MPU6050_standby(void) {
    uint8_t c;
    
    c = I2C1_Read1ByteRegister(MPU6050_DEVICE_ADDRESS,PWR_MGMT_1);
    I2C1_Write1ByteRegister(MPU6050_DEVICE_ADDRESS, PWR_MGMT_1, c | 0b01000000);
    
} // end MPU6050_standby


//----------------------------------------------------------------
// MPU6050_active()
//----------------------------------------------------------------
void MPU6050_active(void) {
    uint8_t c;

    I2C1_Write1ByteRegister(MPU6050_DEVICE_ADDRESS, PWR_MGMT_1, 0x00); // Turn MPU6050 on
    
} // end MPU6050_active


//----------------------------------------------------------------
// MPU6050_deviceReset
//----------------------------------------------------------------
void MPU6050_deviceReset(void) {
    
    I2C1_Write1ByteRegister(MPU6050_DEVICE_ADDRESS, PWR_MGMT_1, 0b10000000); // Reset MPU
    
}


//----------------------------------------------------------------
// MPU6050_setAccelerationScale
// SET FULL-SCALE ACCELEROMETER RANGE
//	This function sets the full-scale range of the x, y, and z axis accelerometers.
//	Possible values for the fsr variable are SCALE_2G, SCALE_4G, SCALE_8G, or SCALE_16G.
//----------------------------------------------------------------
void MPU6050_setAccelerationScale(uint8_t fsr)
{
	uint8_t cfg;
    
    INTERRUPT_GlobalInterruptDisable();   
    cfg = I2C1_Read1ByteRegister(MPU6050_DEVICE_ADDRESS,ACCEL_CONFIG);    
	cfg &= 0b11100111;	   // Mask out scale bits    
	cfg |= (fsr << 3); 
    I2C1_Write1ByteRegister(MPU6050_DEVICE_ADDRESS, ACCEL_CONFIG, cfg);  
    INTERRUPT_GlobalInterruptEnable();		
    
} // end MPU6050_setAccelerationScale


//----------------------------------------------------------------
// MPU6050_readAccelerationScale
// READ FULL-SCALE ACCELEROMETER RANGE
//	This function read the full-scale range of the accelerometers.
//	Possible values for the fsr variable are SCALE_2G, SCALE_4G, SCALE_8G, or SCALE_16G.
//----------------------------------------------------------------
uint8_t MPU6050_readAccelerationScale(void)
{
	uint8_t cfg;

    INTERRUPT_GlobalInterruptDisable();    
    cfg = I2C1_Read1ByteRegister(MPU6050_DEVICE_ADDRESS,ACCEL_CONFIG);    	
	return((cfg &0b11100111) >> 3);
    INTERRUPT_GlobalInterruptEnable();	
    
} // end MPU6050_readAccelerationScale


//----------------------------------------------------------------
// MPU6050_setDlpfScale
// SET DIGITAL LOW PASS FILTER
//  0:  260 Hz bandwidth
//  1:  184 Hz bandwidth
//  2:   94 Hz bandwidth
//  3:   44 Hz bandwidth
//  4:   21 Hz bandwidth
//  5:   10 Hz bandwidth
//  6:    5 Hz bandwidth
//----------------------------------------------------------------
void MPU6050_setDlpfScale(uint8_t dlpf)
{
	uint8_t cfg;
    
    INTERRUPT_GlobalInterruptDisable();   
    cfg = I2C1_Read1ByteRegister(MPU6050_DEVICE_ADDRESS,CONFIG);    
    cfg &= 0b11111000;
    cfg |= dlpf;
    I2C1_Write1ByteRegister(MPU6050_DEVICE_ADDRESS, CONFIG, cfg);  
    INTERRUPT_GlobalInterruptEnable();		
    
} // end MPU6050_setAccelerationScale


//----------------------------------------------------------------
// MPU6050_setScale
// SET FULL-SCALE RANGE
//	This function sets the full-scale range of the x, y, and z axis accelerometers.
//	Possible values for the fsr variable are SCALE_2G, SCALE_4G, SCALE_8G, or SCALE_16G.
//----------------------------------------------------------------
void MPU6050_setGyroscopeScale(uint8_t fsr)
{
	uint8_t cfg;
    
    INTERRUPT_GlobalInterruptDisable(); 
    cfg = I2C1_Read1ByteRegister(MPU6050_DEVICE_ADDRESS,GYRO_CONFIG);    
	cfg &= 0b11100111;	   // Mask out scale bits    
	cfg |= (fsr << 3); 
    I2C1_Write1ByteRegister(MPU6050_DEVICE_ADDRESS, GYRO_CONFIG, cfg);  
    INTERRUPT_GlobalInterruptEnable();	
	
} // end MPU6050_setGyroscopeScale


//----------------------------------------------------------------
// MPU6050_readGyroscopeScale
// READ FULL-SCALE ACCELEROMETER RANGE
//	This function read the full-scale range of the accelerometers.
//	Possible values for the fsr variable are SCALE_2G, SCALE_4G, SCALE_8G, or SCALE_16G.
//----------------------------------------------------------------
uint8_t MPU6050_readGyroscopeScale(void)
{
	uint8_t cfg;

    INTERRUPT_GlobalInterruptDisable();     
    cfg = I2C1_Read1ByteRegister(MPU6050_DEVICE_ADDRESS,GYRO_CONFIG);    	
	return((cfg &0b11100111) >> 3);
    INTERRUPT_GlobalInterruptEnable();	
	
} // end MPU6050_readAccelerationScale



const char MPU6050RegisterNames[MPU6050_NUM_REGISTERS][MAX_REGISTER_NAME_LENGTH] = {
"SELF_TEST_X",
"SELF_TEST_Y",
"SELF_TEST_Z",
"SELF_TEST_A",
"SMPLRT_DIV",
"CONFIG",
"GYRO_CONFIG",
"ACCEL_CONFIG",
"FIFO_EN",
"INT_PIN_CFG",
"INT_ENABLE",
"INT_STATUS",
"ACCEL_XOUT_H",
"ACCEL_XOUT_L",
"ACCEL_YOUT_H",
"ACCEL_YOUT_L",
"ACCEL_ZOUT_H",
"ACCEL_ZOUT_L",
"TEMP_OUT_H",
"TEMP_OUT_L",
"GYRO_XOUT_H",
"GYRO_XOUT_L",
"GYRO_YOUT_H",
"GYRO_YOUT_L",
"GYRO_ZOUT_H",
"GYRO_ZOUT_L",
"USER_CTRL",
"PWR_MGMT_1",
"PWR_MGMT_2",
"WHO_AM_I_MPU6050"};


const uint8_t MPU6050RegisterAddress[MPU6050_NUM_REGISTERS] = {
0x0D,
0x0E,
0x0F,
0x10,
0x19,
0x1A,
0x1B,
0x1C,
0x23,
0x37,
0x38,
0x3A,
0x3B,
0x3C,
0x3D,
0x3E,
0x3F,
0x40,
0x41,
0x42,
0x43,
0x44,
0x45,
0x46,
0x47,
0x48,
0x6A,
0x6B,
0x6C,
0x75
};
