#include "i2c.h"

/* Pinout: SCL - PB6
					 SDA - PB7
					 APB1ENR - 36 000 000 Hz
*/

void i2c1_init(void){
	//Enable clk for PB6 and PB7
	RCC->APB2ENR |= APB2ENR_IOPBEN | APB2ENR_AFIOEN;
	//Set PB6 and PB7 as alternate function open drain 1111 50MHz
	GPIOB->CRL |= (1U<<24);
	GPIOB->CRL |= (1U<<25);
	GPIOB->CRL |= (1U<<26);
	GPIOB->CRL |= (1U<<27);
	GPIOB->CRL |= (1U<<28);
	GPIOB->CRL |= (1U<<29);
	GPIOB->CRL |= (1U<<30);
	GPIOB->CRL |= (1U<<31);
	//Enable clk for i2c_1
	RCC->APB1ENR |= APB1ENR_I2C1EN;
	//Peripherial disable
	I2C1->CR1 &= ~CR1_PE;
	//Reset and set mode
	I2C1->CR1 |= CR1_SWRST;
	I2C1->CR1 &= ~CR1_SWRST;
	//Set clock to 36MHz - 100100 - same as APB1
	I2C1->CR2 |= (1U<<5); 
	I2C1->CR2 &= ~(1U<<4);
	I2C1->CR2 &= ~(1U<<3);
	I2C1->CR2 |= (1U<<2); 
	I2C1->CR2 &= ~(1U<<1);
	I2C1->CR2 &= ~(1U<<0);
	//Set standard mode 100kHz
	//I2C1->CCR &= ~CCR_F_S; //SM explicitly
	//Set fast mode 400kHz 1/2
	I2C1->CCR |= CCR_F_S;
	//Set CCR to 180 (0b10110100 - 0xB4) - it makes 100kHz I2C clock
	//I2C1->CCR |= (1U<<2);
	//I2C1->CCR |= (1U<<4);
	//I2C1->CCR |= (1U<<5);
	//I2C1->CCR |= (1U<<7);
	//Set CR to 30 (0x1E) (0b00011110) it makes 400kHz I2c clock
	I2C1->CCR |= (1U<<1);
	I2C1->CCR |= (1U<<2);
	I2C1->CCR |= (1U<<3);
	I2C1->CCR |= (1U<<4);
	/*All CCR is 0b1000000000011110 - 0x801E*/
	//Set rise time 11 = 10,8 + 1
	I2C1->TRISE = SM_MAX_RISE_TIME;
	//Peripherial enable
	I2C1->CR1 |= CR1_PE;
}

void I2C1_byteRead(char saddr, char maddr, char* data){
	volatile int tmp;
	/*Request*/
	//Wait until not busy
	while(I2C1->SR2 & SR2_BUSY);
	//ACK enable
	I2C1->CR1 |= CR1_ACK;
	//Generate start - S
	I2C1->CR1 |= CR1_START;
	//Wait until start flag is set
	while(!(I2C1->SR1 & SR1_SB));
	//clear SB flag - EV5
	tmp = I2C1->SR1;
	tmp = I2C1->DR;
	//Transmit slave addres plus write mode - last bit 0 = write - Address
	I2C1->DR = saddr<<1 | 0;  //shift by 1 bit couse saddr is 7 bits to 8 bits space register DR 
	//wait until addr flag is set
	while(!(I2C1->SR1 & SR1_ADDR));
	//clear addr flag
	tmp = I2C1->SR1 | I2C1->SR2;
	//Wait until transmitter empty
	while(!(I2C1->SR1 & SR1_TXE));
	//send memory address
	I2C1->DR = maddr;

	
	/*Reception data*/
	//Generate restart
	I2C1->CR1 |= CR1_START;
	//Wait until start flag is set
	while(!(I2C1->SR1 & SR1_SB));
	//clear SB flag - EV5
	tmp = I2C1->SR1;
	tmp = I2C1->DR;
	//Transmit slave address plus Read mode - last bit 1 = read
	I2C1->DR = (saddr<<1) | 1;
	//Wait until addr flag is set
	while(!(I2C1->SR1 & SR1_ADDR));
	//Disable Acknowledge - for reading last (and only) byte
	I2C1->CR1 &= ~CR1_ACK;
	//Clear addr flag
	tmp = I2C1->SR1 | I2C1->SR2;
	//Generate stop after data received
	I2C1->CR1 |= CR1_STOP;
	//Wait until receiver is set
	while(!(I2C1->SR1 & SR1_RXNE));
	//Read data from DR
	*data++ = I2C1->DR;
}

void I2C1_burstRead(char saddr, char maddr, uint32_t n, char* data){
		volatile int tmp;
	/*Request*/
	//Wait until not busy
	while(I2C1->SR2 & SR2_BUSY);
	//ACK enable
	I2C1->CR1 |= CR1_ACK;
	//Generate start
	I2C1->CR1 |= CR1_START;
	//Wait until start flag is set
	while(!(I2C1->SR1 & SR1_SB));
	//clear SB flag - EV5
	tmp = I2C1->SR1;
	tmp = I2C1->DR;
	//Transmit slave addres plus write mode - last bit 0 = write
	I2C1->DR = saddr<<1;  //shift by 1 bit couse saddr is 7 bits to 8 bits space register DR 
	//wait until addr flag is set
	while(!(I2C1->SR1 & SR1_ADDR));
	//clear addr flag
	tmp = I2C1->SR1;
	tmp = I2C1->SR2; 
	//Wait until transmitter (data register) empty
	while(!(I2C1->SR1 & SR1_TXE));
	//send memory address
	I2C1->DR = maddr;
	//Wait until transmitter empty
	while(!(I2C1->SR1 & SR1_TXE));
	
	/*Reception*/
	//Generate restart
	I2C1->CR1 |= CR1_START;
	//Wait until start flag is set
	while(!(I2C1->SR1 & SR1_SB));
	//Transmit slave address plus Read mode - last bit 1 = read
	I2C1->DR = (saddr<<1) | 1;
	//Wait until addr flag is set
	while(!(I2C1->SR1 & SR1_ADDR));
	//Enable Acknowledge - for reading more then one byte
	I2C1->CR1 |= CR1_ACK;
	//Clear addr flag
	tmp = I2C1->SR1;
	tmp = I2C1->SR2;
	while(n > 0){
		//if one byte
		if(n == 1U){
			//Disable acknowledge - for reading last byte
			I2C1->CR1 &= ~CR1_ACK;
			//Generate stop after data received
			I2C1->CR1 |= CR1_STOP;
			//Wait until receiver (data register) is set
			while(!(I2C1->SR1 & SR1_RXNE));
			//Read data from DR
			*data++ = I2C1->DR;
			n--;
		}
		else{
			//Wait until receiver is set
			while(!(I2C1->SR1 & SR1_RXNE));
			//Read data from DR
			*data++ = I2C1->DR;		
			//Decrement number of bytes to read
			n--;
		}
	}
}

void I2C1_burstWrite(char saddr, uint32_t n, uint8_t* data){
	volatile int tmp;
	/*Request*/
	//Wait until not busy
	while(I2C1->SR2 & SR2_BUSY);
	//ACK enable
	I2C1->CR1 |= CR1_ACK;
	//Generate start
	I2C1->CR1 |= CR1_START;
	//Wait until start flag is set
	while(!(I2C1->SR1 & SR1_SB));
	//Transmit slave addres plus write mode - last bit 0 = write
	I2C1->DR = saddr<<1;  //shift by 1 bit couse saddr is 7 bits to 8 bits space register DR 
	//wait until addr flag is set
	while(!(I2C1->SR1 & SR1_ADDR));
	//clear addr flag
	tmp = I2C1->SR1;
	tmp = I2C1->SR2; // sr1 czy sr2 czy wszystko jedno
	//Wait until transmitter (data register) empty
	//while(!(I2C1->SR1 & SR1_TXE));
	//send memory address
	//I2C1->DR = maddr;
	//Wait until transmitter empty
	//while(!(I2C1->SR1 & SR1_TXE));
	
	/*Transmission*/
	for(int i = 0; i< n; i++){
		//Wait until transmitter (data register) empty
		while(!(I2C1->SR1 & SR1_TXE));
		//Write data
		I2C1->DR = *data++;
	}
	//Wait until transfer finished
	while(!(I2C1->SR1 & SR1_BTF));
	//Generate stop after data received
	I2C1->CR1 |= CR1_STOP;
}



/***************************************************I2C_2*****************************************/


/* Pinout: SCL - PB10
					 SDA - PB11
					 APB1ENR - 36 000 000 Hz
*/

void i2c2_init(void){
	//Enable clk for PB10 and PB11
	RCC->APB2ENR |= APB2ENR_IOPBEN | APB2ENR_AFIOEN;
	//Set PB6 and PB7 as alternate function open drain 1111 50MHz
	GPIOB->CRH |= (1U<<8);
	GPIOB->CRH |= (1U<<9);
	GPIOB->CRH |= (1U<<10);
	GPIOB->CRH |= (1U<<11);
	GPIOB->CRH |= (1U<<12);
	GPIOB->CRH |= (1U<<13);
	GPIOB->CRH |= (1U<<14);
	GPIOB->CRH |= (1U<<15);
	//Enable clk for i2c_1
	RCC->APB1ENR |= APB1ENR_I2C2EN;
	//Peripherial disable
	I2C2->CR1 &= ~CR1_PE;
	//Reset and set mode
	I2C2->CR1 |= CR1_SWRST;
	I2C2->CR1 &= ~CR1_SWRST;
	//Set clock to 36MHz - 100100 - same as APB1
	I2C2->CR2 |= (1U<<5); 
	I2C2->CR2 &= ~(1U<<4);
	I2C2->CR2 &= ~(1U<<3);
	I2C2->CR2 |= (1U<<2); 
	I2C2->CR2 &= ~(1U<<1);
	I2C2->CR2 &= ~(1U<<0);
	//Set standard mode 100kHz
	//I2C1->CCR &= ~CCR_F_S; //SM explicitly
	//Set fast mode 400kHz 1/2
	I2C2->CCR |= CCR_F_S;
	//Set CCR to 180 (0b0000000010110100 - 0x00B4) - it makes 100kHz I2C clock
	//I2C1->CCR |= (1U<<2);
	//I2C1->CCR |= (1U<<4);
	//I2C1->CCR |= (1U<<5);
	//I2C1->CCR |= (1U<<7);
	//Set CR to 30 (0x1E) (0b00011110) it makes 400kHz I2c clock
	I2C2->CCR |= (1U<<1);
	I2C2->CCR |= (1U<<2);
	I2C2->CCR |= (1U<<3);
	I2C2->CCR |= (1U<<4);
	/*All CCR is 0b1000000000011110 - 0x801E*/
	//Set rise time 11 = 10,8 + 1
	I2C2->TRISE = SM_MAX_RISE_TIME;
	//Peripherial enable
	I2C2->CR1 |= CR1_PE;
}

void I2C2_byteRead(char saddr, char maddr, char* data){
	volatile int tmp;
	/*Request*/
	//Wait until not busy
	while(I2C2->SR2 & SR2_BUSY);
	//ACK enable
	I2C2->CR1 |= CR1_ACK;
	//Generate start - S
	I2C2->CR1 |= CR1_START;
	//Wait until start flag is set
	while(!(I2C2->SR1 & SR1_SB));
	//clear SB flag - EV5
	tmp = I2C2->SR1;
	tmp = I2C2->DR;
	//Transmit slave addres plus write mode - last bit 0 = write - Address
	I2C2->DR = saddr<<1 | 0;  //shift by 1 bit couse saddr is 7 bits to 8 bits space register DR 
	//wait until addr flag is set
	while(!(I2C2->SR1 & SR1_ADDR));
	//clear addr flag
	tmp = I2C2->SR1 | I2C2->SR2;
	//Wait until transmitter empty
	while(!(I2C2->SR1 & SR1_TXE));
	//send memory address
	I2C2->DR = maddr;

	
	/*Reception data*/
	//Generate restart
	I2C2->CR1 |= CR1_START;
	//Wait until start flag is set
	while(!(I2C2->SR1 & SR1_SB));
	//clear SB flag - EV5
	tmp = I2C2->SR1;
	tmp = I2C2->DR;
	//Transmit slave address plus Read mode - last bit 1 = read
	I2C2->DR = (saddr<<1) | 1;
	//Wait until addr flag is set
	while(!(I2C2->SR1 & SR1_ADDR));
	//Disable Acknowledge - for reading last (and only) byte
	I2C2->CR1 &= ~CR1_ACK;
	//Clear addr flag
	tmp = I2C2->SR1 | I2C2->SR2;
	//Generate stop after data received
	I2C2->CR1 |= CR1_STOP;
	//Wait until receiver is set
	while(!(I2C2->SR1 & SR1_RXNE));
	//Read data from DR
	*data++ = I2C2->DR;
}

void I2C2_burstRead(char saddr, char maddr, uint32_t n, char* data){
		volatile int tmp;
	/*Request*/
	//Wait until not busy
	while(I2C2->SR2 & SR2_BUSY);
	//ACK enable
	I2C2->CR1 |= CR1_ACK;
	//Generate start
	I2C2->CR1 |= CR1_START;
	//Wait until start flag is set
	while(!(I2C2->SR1 & SR1_SB));
	//clear SB flag - EV5
	tmp = I2C2->SR1;
	tmp = I2C2->DR;
	//Transmit slave addres plus write mode - last bit 0 = write
	I2C2->DR = saddr<<1;  //shift by 1 bit couse saddr is 7 bits to 8 bits space register DR 
	//wait until addr flag is set
	while(!(I2C2->SR1 & SR1_ADDR));
	//clear addr flag
	tmp = I2C2->SR1;
	tmp = I2C2->SR2; 
	//Wait until transmitter (data register) empty
	while(!(I2C2->SR1 & SR1_TXE));
	//send memory address
	I2C2->DR = maddr;
	//Wait until transmitter empty
	while(!(I2C2->SR1 & SR1_TXE));
	
	/*Reception*/
	//Generate restart
	I2C2->CR1 |= CR1_START;
	//Wait until start flag is set
	while(!(I2C2->SR1 & SR1_SB));
	//Transmit slave address plus Read mode - last bit 1 = read
	I2C2->DR = (saddr<<1) | 1;
	//Wait until addr flag is set
	while(!(I2C2->SR1 & SR1_ADDR));
	//Enable Acknowledge - for reading more then one byte
	I2C2->CR1 |= CR1_ACK;
	//Clear addr flag
	tmp = I2C2->SR1;
	tmp = I2C2->SR2;
	while(n > 0){
		//if one byte
		if(n == 1U){
			//Disable acknowledge - for reading last byte
			I2C2->CR1 &= ~CR1_ACK;
			//Generate stop after data received
			I2C2->CR1 |= CR1_STOP;
			//Wait until receiver (data register) is set
			while(!(I2C2->SR1 & SR1_RXNE));
			//Read data from DR
			*data++ = I2C2->DR;
			n--;
		}
		else{
			//Wait until receiver is set
			while(!(I2C2->SR1 & SR1_RXNE));
			//Read data from DR
			*data++ = I2C2->DR;		
			//Decrement number of bytes to read
			n--;
		}
	}
}

void I2C2_burstWrite(char saddr, char maddr, uint32_t n, char* data){
	volatile int tmp;
	/*Request*/
	//Wait until not busy
	while(I2C2->SR2 & SR2_BUSY);
	//ACK enable
	I2C2->CR1 |= CR1_ACK;
	//Generate start
	I2C2->CR1 |= CR1_START;
	//Wait until start flag is set
	while(!(I2C2->SR1 & SR1_SB));
	//Transmit slave addres plus write mode - last bit 0 = write
	I2C2->DR = saddr<<1;  //shift by 1 bit couse saddr is 7 bits to 8 bits space register DR 
	//wait until addr flag is set
	while(!(I2C2->SR1 & SR1_ADDR));
	//clear addr flag
	tmp = I2C2->SR1;
	tmp = I2C2->SR2; // sr1 czy sr2 czy wszystko jedno
	//Wait until transmitter (data register) empty
	while(!(I2C2->SR1 & SR1_TXE));
	//send memory address
	I2C2->DR = maddr;
	//Wait until transmitter empty
	//while(!(I2C1->SR1 & SR1_TXE));
	
	/*Transmission*/
	for(int i = 0; i< n; i++){
		//Wait until transmitter (data register) empty
		while(!(I2C2->SR1 & SR1_TXE));
		//Write data
		I2C2->DR = *data++;
	}
	//Wait until transfer finished
	while(!(I2C2->SR1 & SR1_BTF));
	//Generate stop after data received
	I2C2->CR1 |= CR1_STOP;
}
