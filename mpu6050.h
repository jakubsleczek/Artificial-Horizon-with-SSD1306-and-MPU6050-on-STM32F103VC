#ifndef _MPU6050_H_
#define _MPU6050_H_

#include "i2c.h"

//Address 0b1101000 (0x68) - AD0 low
//Address 0b1101001 (0x69) - AD0 high
#define CONFIG				(0x1A) //not used
#define GYRO_CONFIG		(0x1B) 
#define ACCEL_CONFIG	(0x1C) 
#define DATA_START_R	(0x3B)
#define USER_CTRL			(0x6A)
#define PWR_MGMT_1		(0x6B)
#define PWR_MGMT_2		(0x6C)
#define WHO_AM_I			(0x75)

#define DEVICEID			(0x68)

#define CONFIG_SET 		 (0x00)	 //not used
#define GYRO_SELFTEST	 (0x08)  //Range +/-500st/s
#define ACC_RANGE			 (0x08)  //Range to +/-4g
#define USET_CTRL_SET	 (0x00)
#define PWR_MGMT_1_SET (0x01)  //CYCLE MODE on, PPL with X axis gyro reference as clock source, temperature ON
#define PWR_MGMT_2_SET (0x00)  //Wake up frequency 1.25Hz, acc and gyro all axes ON

void mpu_read_address (uint8_t reg);
void mpu_write(uint8_t reg, char value);
void mpu_read_values(uint8_t reg);
void mpu_init(void);

#endif //_MPU6050_H_
