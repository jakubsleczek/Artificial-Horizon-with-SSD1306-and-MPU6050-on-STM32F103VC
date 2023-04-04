#include <stdint.h>
#include "ssd1306.h"
#include "stm32f10x.h"                  // Device header
#include "i2c.h"
#include "systick.h" 
#include "bmp.c"
#include "mpu6050.h"
#include <math.h>

extern char sensors_data[14];
static int16_t ax, ay, az;
static float acc_x, acc_y, acc_z;
static int16_t pitch=0, roll=0;

int main(void){
	systickDelaysMs(10);
	ssd1306_init();
	mpu_init();
	while(1){
		mpu_read_values(DATA_START_R);
		ax = ((int16_t)sensors_data[0]<<8)  | (int16_t)sensors_data[1];
		ay = ((int16_t)sensors_data[2]<<8)  | (int16_t)sensors_data[3];
		az = ((int16_t)sensors_data[4]<<8)  | (int16_t)sensors_data[5];
		acc_x = (float)ax/8192;
		acc_y = (float)ay/8192;
		acc_z = (float)az/8192;
		roll = atan2(acc_y, acc_z) * 57.3;
		pitch = atan2(-acc_x, sqrt((acc_y * acc_y) + (acc_z * acc_z))) * 57.3;		
		DisplayCls(0x00);
		drawAH(roll, pitch);
		RefreshRAM();
	}	
}
