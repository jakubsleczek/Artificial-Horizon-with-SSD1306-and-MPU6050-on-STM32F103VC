#ifndef _I2C_H_
#define _I2C_H_

#include "stm32f10x.h"                  // Device header
#include <stdint.h>

#define APB1ENR_I2C1EN		(1U<<21)
#define APB2ENR_IOPBEN		(1U<<3)
#define APB2ENR_AFIOEN		(1U<<0)
#define CR1_SWRST					(1U<<15)
#define CCR_F_S						(1U<<15)
#define I2C_100kHZ				180
#define SM_MAX_RISE_TIME	11
#define CR1_PE 						(1U<<0)
#define SR2_BUSY					(1U<<1)
#define CR1_START					(1U<<8)
#define SR1_SB						(1U<<0)
#define SR1_ADDR					(1U<<1)
#define SR1_TXE						(1U<<7)
#define CR1_ACK						(1U<<10)
#define CR1_STOP					(1U<<9)
#define SR1_RXNE					(1U<<6)
#define SR1_BTF						(1U<<2)

#define APB1ENR_I2C2EN		(1U<<22)

void i2c1_init(void);
void I2C1_byteRead(char saddr, char maddr, char* data);
void I2C1_burstRead(char saddr, char maddr, uint32_t n, char* data);
void I2C1_burstWrite(char saddr, uint32_t n, uint8_t* data);


void i2c2_init(void);
void I2C2_byteRead(char saddr, char maddr, char* data);
void I2C2_burstRead(char saddr, char maddr, uint32_t n, char* data);
void I2C2_burstWrite(char saddr, char maddr, uint32_t n, char* data);

#endif //_I2C_H_
