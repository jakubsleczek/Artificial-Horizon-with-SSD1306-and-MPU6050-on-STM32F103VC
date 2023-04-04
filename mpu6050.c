#include "mpu6050.h"

char data;
char sensors_data[14];

void mpu_read_address (uint8_t reg){
	I2C2_byteRead(DEVICEID, reg, &data);
}

void mpu_write(uint8_t reg, char value){
	I2C2_burstWrite(DEVICEID, reg, 1, &value);
}

void mpu_read_values(uint8_t reg){
	I2C2_burstRead(DEVICEID, reg, 14, sensors_data);
}

void mpu_init(void){
	//Enable I2C 400kHz for 36MHz PLCK1 
	i2c2_init();
	//Read the DAVICE ADDRESS - 0x68
	mpu_read_address(WHO_AM_I);
	//Set ranges of gyro and acc
	mpu_write(GYRO_CONFIG, GYRO_SELFTEST);
	mpu_write(ACCEL_CONFIG, ACC_RANGE);
	//Set USER_CTRL, PWR_MGMT_1, PWR_MGMT_2 registers
	mpu_write(USER_CTRL, USET_CTRL_SET);
  mpu_write(PWR_MGMT_1, PWR_MGMT_1_SET);
	mpu_write(PWR_MGMT_2, PWR_MGMT_2_SET);
}
