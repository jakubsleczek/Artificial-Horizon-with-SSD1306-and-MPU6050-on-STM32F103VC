
#include "ssd1306.h"



void writeByte(uint8_t data, _Bool operation){
	//Control Co - 0, D/C# - 0 - commands
	//Control Co - 0, D/C# - 1 - data to GDDRAM
	if(operation){ // 1 - data to GDDRAM
		uint8_t gddram[2];
		gddram[0] = 0x40;
		gddram[1] = data;
		I2C1_burstWrite(0x3C, 2, gddram);
	}
	else{  				// 0 - data to register
		uint8_t command[2];
		command[0] = 0x00;
		command[1] = data;
		I2C1_burstWrite(0x3C, 2, command);
	}	
}

void ssd1306_init(void){
	i2c1_init();
	systickDelaysMs(10);
	//Display OFF
	writeByte(0xAE,0);	
	systickDelaysMs(10);
	//*************Hardware Configuration*****************//
	//Set multiplexer ratio - command
	writeByte(0xA8,0);
	systickDelaysMs(10);
	//Set multiplexer ratio - value 64MUX - maximum
	writeByte(0x3F,0);
	systickDelaysMs(10);
	//Set display offset - command
	writeByte(0xD3,0);
	systickDelaysMs(10);
	//Set display offset - value 0 - minimum
	writeByte(0x00,0);
	systickDelaysMs(10);
	//Set display Start Line - from 40h to 7fh - 40h is 0 and 7fh is 63
	writeByte(0x40,0);
	systickDelaysMs(10);
	//Set segment Re-map A0/A1 - A0-column address 0 is SEG0  A1-column address 127 is SEG0
	writeByte(0xA1,0);
	systickDelaysMs(10);
	//Set COM output scan direct C0/C8 C0-scan from COM0 to COM[N-1]   C8-scan from COM[N-1] to COM0
  writeByte(0xC0,0);
	systickDelaysMs(10);
	//Set COM Pins Hardware Congfig - command bit5[0]-disable COM l/r remap  bit5[1]-dnable COM l/r remap   bit4[0]-sequential COM config bit4[1]-alternative COM config 
	writeByte(0xDA,0);
	systickDelaysMs(10);
	//Set COM Pins Hardware Congfig - value (0x32, 0x22, 0x12, 0x02) bit5[0]-disable COM l/r remap  bit5[1]-dnable COM l/r remap   bit4[0]-sequential COM config bit4[1]-alternative COM config 
	writeByte(0x12,0);
	systickDelaysMs(10);
	//****************End of Hardware Configuration**************//
	//****************Fundamental Command Table******************//
	//Set Contrast - command
	writeByte(0x81,0);
	systickDelaysMs(10);
	//Set Contrast - value (0x00 to 0xFF) from 1 to 256 contrast step
	writeByte(0x7F,0);
	systickDelaysMs(10);
	//Entire Display ON A4h-Output content from RAM    A5h-Ignore RAM content, display entire pixels
	writeByte(0xA4,0);
	systickDelaysMs(10);
	//Set Normal/Inverse Display  A6h-normal display     A7h-inverse display(negative)
	writeByte(0xA6,0);
	systickDelaysMs(10);
	//****************End of Fundamental Command Table***********//
	//****************Timing & Driving Scheme Setting Command Table************//
	//Set Display Clock Divide Ratio/Oscillator Frequency - command
	writeByte(0xD5,0);
	systickDelaysMs(10);
	//Set Display Clock Divide Ratio/Oscillator Frequency - value bits[0:3] divide display CLK A[0:3]+1    bits[7:4]-oscillator freq
	writeByte(0x080,0);
	systickDelaysMs(10);
	//Set pre-charged period - command
	writeByte(0xD9,0);
	//Set pre-charged period - value A[0:3] phase 1 from 1 to 15    A[4:7] phase 2 form 1 to 15     - 0 is invalid
	writeByte(0x22,0);
	//Set Vcomh deselect level - command
	writeByte(0xDB,0);
	//Set Vcomh deselect level - value 00h-0.65Vcc   20h-0.77Vcc   30h-0.83Vcc
	writeByte(0x20,0);
	//****************End of Timing & Driving Scheme Setting Command Table************//
	//****************Addressing Setting Command TAable************//
	//Page, vertical or horizontal mode and config
	//****************End of Addressing Setting Command Table************//
	//****************Scrolling Command Table**********************//
	//config
	//****************End of Scrolling Command Table**********************//
	//Charge Bump Setting - command
	writeByte(0x8D,0);
	systickDelaysMs(10);
	//Charge Bump Setting - value 14h-enable charge pump(important)   10h-disable charge pump
	writeByte(0x14,0);
	systickDelaysMs(10);
	//Display ON
	writeByte(0xAF,0);
	
}


void DisplayCls(unsigned char fill)
{
 unsigned char i, j;

 for (i = 0; i < 8; i ++) {
	for (j = 0; j < 128; j ++) {
		DispBuff[j][i] = fill;
	}
 }
}


void RefreshRAM(void)
{
 uint8_t i, j;

 for (i = 0; i < 8; i ++) {
	writeByte(0xB0 + i,0);
	SetColStart();
	for (j = 0; j < 128; j ++) {
		writeByte(DispBuff[j][i],1);
	}
 }
}

void SetColStart(void){
 writeByte(0x00,0); //low
 writeByte(0x10,0); //high
}


void DrawPoint(unsigned char x ,unsigned char y, unsigned char p)
{
 uint8_t chPos, chBx, chTemp = 0;

 if (x > 127 || y > 63) return; //if over bounds do nothing
 chPos = 7 - y / 8;
 chBx = y % 8;
 chTemp = 1 << (7 - chBx);
 if (p) {
 DispBuff[x][chPos] |= chTemp; //light up pixel if p = 1
 } else {
 DispBuff[x][chPos] &= ~chTemp; //light down pixel if p = 0
 }
}

void DrawPointNew(int16_t x ,int16_t y, unsigned char p)
{
 int16_t chPos, chBx, chTemp = 0;

 if (x > 127 || y > 63) return; //if over bounds do nothing
 chPos = 7 - y / 8;
 chBx = y % 8;
 chTemp = 1 << (7 - chBx);
 if (p) {
 DispBuff[x][chPos] |= chTemp; //light up pixel if p = 1
 } else {
 DispBuff[x][chPos] &= ~chTemp; //light down pixel if p = 0
 }
}

void DisplayChar(uint8_t x, uint8_t y, uint8_t Chr, uint8_t mode)
{
	uint8_t i, j;
	uint8_t chTemp, reverse,chYpos0 = y;

	Chr = Chr - ' ';  // ASCI position - 32 to fit font array
	for (i = 0; i < 5; i ++){
		if (mode){
			chTemp = font[Chr][i];
		}else{
			chTemp = ~font[Chr][i];
		}
		reverse = chTemp;
		chTemp = 0;
		chTemp |= (reverse & 0x01)<<7;
		chTemp |= (reverse & 0x02)<<5;
		chTemp |= (reverse & 0x04)<<3;
		chTemp |= (reverse & 0x08)<<1;
		chTemp |= (reverse & 0x10)>>1;
		chTemp |= (reverse & 0x20)>>3;
		chTemp |= (reverse & 0x40)>>5;
		chTemp |= (reverse & 0x80)>>7;
		for (j = 0; j < 8; j ++){
			if (chTemp & 0x80){
				DrawPoint(x, y, 1);
			}else{
				DrawPoint(x, y, 0);
			}
			chTemp <<= 1;
			y ++;
			if ((y - chYpos0) == 8){
				y = chYpos0;
				x ++;
				break;
			}
		}
	}
}

void DispTxt(uint8_t x, uint8_t y, const uint8_t *txt, uint8_t mode)
{
 while (*txt != '\0'){ 
	 if (x > (128 - 5)){
		 x = 0; 
		 y += 8; 
		 if (y > (64 - 8)){
			y = x = 0;
			DisplayCls(0x00);
		 }
	 }
	 DisplayChar(x, y, *txt, mode);
	 x += 5;
	 txt ++;
 }
}

void DrawBmp(uint8_t x, uint8_t y, const uint8_t *bmp, uint8_t width, uint8_t height)
{
 uint16_t i, j, byteWidth = (width + 7) / 8;
 for(j = 0; j < height; j++){
	for(i = 0; i < width; i++) {
		if(*(bmp + j * byteWidth + i / 8) & (128 >> (i & 7))) {
			DrawPoint(x + i, y + j, 1);
		}
	}
 }
}




// roll +/- 180 degree    pitch +/-180degree     roll+ on the rigt wing, pitch+ towords ground
void drawAH(int16_t roll_deg, int16_t pitch_deg){
		pitch_deg %= 181;
		roll_deg %= 181;
		if(pitch_deg == -180)pitch_deg *= (-1);
	  if(roll_deg == -180)roll_deg *= (-1);
		drawScreen(roll_deg,pitch_deg);
}




int16_t bCoefficientCalculator(int16_t roll_deg, int16_t pitch_deg){
	uint8_t diagonal_angle = (uint8_t)(atan((double)HEIGHT/WIDTH) * (double)(180.0F/PI)); // because of rectangle screen
	float roll_rad;
	int16_t b;
	roll_rad = (float)roll_deg * (float)PI/180.0F;	
	//Horizontal
	if(roll_deg == 0 || roll_deg == 180)
		return (int16_t)(pitch_deg / 2 * cos(roll_rad)); //2deg is 1px
	//Vertical
	else if(roll_deg == 90 || roll_deg == -90)
		return (int16_t)(pitch_deg * 1 * sin(roll_rad)); //1deg is 1px
	//Rest
	else{
		//1st quarter
		if(roll_deg>0 && roll_deg <= HELPER) 
			b=(int16_t)((float)pitch_deg/180 * (PX180DEG_HORIZONTAL + PX180DEG_HORIZONTAL * sin(roll_rad)));
		else if(roll_deg>HELPER && roll_deg <= 90-diagonal_angle) 
			b= (int16_t)((float)pitch_deg/180 * (PX180DEG_HORIZONTAL/cos(roll_rad)));
		else if(roll_deg>90-diagonal_angle && roll_deg < 90)
			b= (int16_t)((float)pitch_deg/180 * (PX180DEG_VERTICAL + PX180DEG_HORIZONTAL * cos(roll_rad)));
		//2nd quarter
		else if(roll_deg>90 && roll_deg <= 90+diagonal_angle)
			b= (int16_t)((float)pitch_deg/180 * (PX180DEG_VERTICAL + PX180DEG_HORIZONTAL * sin(roll_rad - PI/2)));
		else if(roll_deg>90+diagonal_angle && roll_deg <= 180 - HELPER)  
			b= (int16_t)((float)pitch_deg/180 * (PX180DEG_HORIZONTAL/-cos(roll_rad)));
		else if(roll_deg>180 - HELPER && roll_deg < 180) 
			b=(int16_t)((float)pitch_deg/180 * (PX180DEG_HORIZONTAL + PX180DEG_HORIZONTAL * sin(roll_rad)));
		//4th quarter
		else if(roll_deg>=-HELPER && roll_deg < 0) 
			b= (int16_t)((float)pitch_deg/180 * (PX180DEG_HORIZONTAL + PX180DEG_HORIZONTAL*(-sin(roll_rad))));
		else if(roll_deg>=-90+diagonal_angle && roll_deg < -HELPER) 
			b= (int16_t)((float)pitch_deg/180 * (PX180DEG_HORIZONTAL/cos(roll_rad)));		
		else	if(roll_deg>-90 && roll_deg < -90 + diagonal_angle) 
			b= (int16_t)((float)pitch_deg/180 * (PX180DEG_VERTICAL + PX180DEG_HORIZONTAL*cos(roll_rad)));
		//3rd quarter
		else if(roll_deg>= -90-diagonal_angle && roll_deg < -90) 
			b= (int16_t)((float)pitch_deg/180 * (PX180DEG_VERTICAL + PX180DEG_HORIZONTAL*sin(roll_rad - PI/2)));
		else if(roll_deg>= -180+HELPER && roll_deg < -90-diagonal_angle) 
			b= (int16_t)((float)pitch_deg/180 * (PX180DEG_HORIZONTAL/-cos(roll_rad)));
		else if(roll_deg>-180 && roll_deg < -180+HELPER) 
			b= (int16_t)((float)pitch_deg/180 * (PX180DEG_HORIZONTAL +PX180DEG_HORIZONTAL*(-cos(roll_rad))));
}
	return b;
}

int16_t PX180DEGCoefficientCalculator(int16_t roll_deg){
		uint8_t diagonal_angle = (uint8_t)(atan((double)HEIGHT/WIDTH) * (double)(180.0F/PI)); // because of rectangle screen
	float roll_rad;
	roll_rad = (float)roll_deg * (float)PI/180.0F;	

		//1st quarter
		if(roll_deg>0 && roll_deg <= HELPER) 
			return (int16_t)(PX180DEG_HORIZONTAL + PX180DEG_HORIZONTAL * sin(roll_rad));
		else if(roll_deg>HELPER && roll_deg <= 90-diagonal_angle) 
			return (int16_t)(PX180DEG_HORIZONTAL/cos(roll_rad));
		else if(roll_deg>90-diagonal_angle && roll_deg < 90)
			return (int16_t)(PX180DEG_VERTICAL + PX180DEG_HORIZONTAL * cos(roll_rad));
		//2nd quarter
		else if(roll_deg>90 && roll_deg <= 90+diagonal_angle)
			return (int16_t)(PX180DEG_VERTICAL + PX180DEG_HORIZONTAL * sin(roll_rad - PI/2));
		else if(roll_deg>90+diagonal_angle && roll_deg <= 180 - HELPER)  
			return (int16_t)(PX180DEG_HORIZONTAL/-cos(roll_rad));
		else if(roll_deg>180 - HELPER && roll_deg < 180) 
			return (int16_t)(PX180DEG_HORIZONTAL + PX180DEG_HORIZONTAL * sin(roll_rad));
		//4th quarter
		else if(roll_deg>=-HELPER && roll_deg < 0) 
			return (int16_t)(PX180DEG_HORIZONTAL + PX180DEG_HORIZONTAL*(-sin(roll_rad)));
		else if(roll_deg>=-90+diagonal_angle && roll_deg < -HELPER) 
			return (int16_t)(PX180DEG_HORIZONTAL/cos(roll_rad));		
		else	if(roll_deg>-90 && roll_deg < -90 + diagonal_angle) 
			return (int16_t)(PX180DEG_VERTICAL + PX180DEG_HORIZONTAL*cos(roll_rad));
		//3rd quarter
		else if(roll_deg>= -90-diagonal_angle && roll_deg < -90) 
			return (int16_t)(PX180DEG_VERTICAL + PX180DEG_HORIZONTAL*sin(roll_rad - PI/2));
		else if(roll_deg>= -180+HELPER && roll_deg < -90-diagonal_angle) 
			return (int16_t)(PX180DEG_HORIZONTAL/-cos(roll_rad));
		else if(roll_deg>-180 && roll_deg < -180+HELPER) 
			return (int16_t)(PX180DEG_HORIZONTAL + PX180DEG_HORIZONTAL*(-cos(roll_rad)));
}





void drawScreen(int16_t roll_deg, int16_t pitch_deg){
	uint8_t x = WIDTH/2;
	uint8_t y = HEIGHT/2;
	float a1, a2, roll_rad, sin_roll, cos_roll;
	int16_t b;
	int16_t PX180DEG;
	uint8_t diagonal_angle, diagonal_angle1;
	int16_t i, j;
	roll_rad = (float)roll_deg * (float)PI/180.0F;
	diagonal_angle = (uint8_t)(atan((double)HEIGHT/WIDTH) * (double)(180.0F/PI));
	a1 = (float)-tan((double)roll_rad);
	a2 = 1/a1;	
	sin_roll = sin(roll_rad);
	cos_roll = cos(roll_rad);
  b = bCoefficientCalculator(roll_deg, pitch_deg);
	if(roll_deg % 90 != 0)
			PX180DEG = PX180DEGCoefficientCalculator(roll_deg);
	int16_t x0_p, x0_n, y0_p, y0_n, next_line_sin, next_line_cos;
	diagonal_angle1 = diagonal_angle;
	
	/****************************Vertical line - START******************************************/
	if(roll_deg == 90 || roll_deg == -90)
	{ 
		// Vertical line
		for(i = 0; i < (HEIGHT/2); ++i){
			if((x-b>=0) && (x-b<WIDTH)){  // Vertical line at pitch 0 deg
				DrawPointNew(x-b, y+i, 1);
				DrawPointNew(x-b, y-i, 1);
			}
			if((x-b-PX180DEG_VERTICAL>=0) && (x-b-PX180DEG_VERTICAL<WIDTH)){  // Vertical line at pitch 180 deg
				DrawPointNew(x-b-PX180DEG_VERTICAL, y+i, 1);
				DrawPointNew(x-b-PX180DEG_VERTICAL, y-i, 1);
			}
			if((x-b+PX180DEG_VERTICAL>=0) && (x-b+PX180DEG_VERTICAL<WIDTH)){  // Vertical line at pitch -180 deg
				DrawPointNew(x-b+PX180DEG_VERTICAL, y+i, 1);
				DrawPointNew(x-b+PX180DEG_VERTICAL, y-i, 1);
			}
			if(roll_deg == 90){
				//fill ground to right
				for(j = 0 ; j < PX180DEG_VERTICAL; j+=FILL_DENSITY){
					if((x-b+j>=0) && (x-b+j<WIDTH)){ 
						DrawPointNew(x-b+j, y+i, 1);
						DrawPointNew(x-b+j, y-i, 1);							
					}
				}
				//fill ground from -180 deg 
				for(j = 0; j <= WIDTH/2; j+=FILL_DENSITY){
					if((x-b-PX180DEG_VERTICAL-j>=0) && (x-b-PX180DEG_VERTICAL-j<WIDTH)){ // Horizon line at pitch -180 deg
						DrawPointNew(x-b-PX180DEG_VERTICAL-j, y+i, 1);
						DrawPointNew(x-b-PX180DEG_VERTICAL-j, y-i, 1);							
					}
				}
			}
			if(roll_deg == -90){
				//fill ground to left
				for(j = 0 ; j < PX180DEG_VERTICAL; j+=FILL_DENSITY){
					if((x-b-j>=0) && (x-b-j<WIDTH)){ 
						DrawPointNew(x-b-j, y+i, 1);
						DrawPointNew(x-b-j, y-i, 1);							
					}
				}
				//fill ground from 180 deg 
				for(j = 0; j <= WIDTH/2; j+=FILL_DENSITY){
					if((x-b+PX180DEG_VERTICAL+j>=0) && (x-b+PX180DEG_VERTICAL+j<WIDTH)){ // Horizon line at pitch -180 deg
						DrawPointNew(x-b+PX180DEG_VERTICAL+j, y+i, 1);
						DrawPointNew(x-b+PX180DEG_VERTICAL+j, y-i, 1);							
					}
				}
			}
		}
	}
	/****************************Vertical line - END*******************************************/
	
	/****************************Horizontal line - START***************************************/
	else if(roll_deg == 180 || roll_deg == 0)
	{
		for(i = 0; i < (WIDTH/2); ++i){
					if((y-b>=0) && (y-b<HEIGHT)){  // Horizon line at pitch 0 deg if pitch_deg grows than horizon line goes up otherwise goes down
						DrawPointNew(x+i, y-b, 1);
						DrawPointNew(x-i, y-b, 1);
					}
					if((y-b-PX180DEG_HORIZONTAL>=0) && (y-b-PX180DEG_HORIZONTAL<HEIGHT)){ // Horizon line at pitch -180 deg
						DrawPointNew(x+i, y-b-PX180DEG_HORIZONTAL, 1);
						DrawPointNew(x-i, y-b-PX180DEG_HORIZONTAL, 1);
					}
					if((y-b+PX180DEG_HORIZONTAL>=0) && (y-b+PX180DEG_HORIZONTAL)){ // Horizon line at pitch 180 deg
						DrawPointNew(x+i, y-b+PX180DEG_HORIZONTAL, 1);
						DrawPointNew(x-i, y-b+PX180DEG_HORIZONTAL, 1);							
					}
					if(roll_deg == 0){
						//fill ground to 180 deg
						for(j = 0 ; j < PX180DEG_HORIZONTAL; j+=FILL_DENSITY){
							if((y-b+j>=0) && (y-b+j<HEIGHT)){ 
								DrawPointNew(x+i, y-b+j, 1);
								DrawPointNew(x-i, y-b+j, 1);							
							}
						}
						//fill ground from -180 deg 
						for(j = 0; j <= HEIGHT/2; j+=FILL_DENSITY){
							if((y-b-PX180DEG_HORIZONTAL-j>=0) && (y-b-PX180DEG_HORIZONTAL-j<HEIGHT)){ // Horizon line at pitch -180 deg
								DrawPointNew(x+i, y-b-PX180DEG_HORIZONTAL-j, 1);
								DrawPointNew(x-i, y-b-PX180DEG_HORIZONTAL-j, 1);							
							}
						}
					}
				if(roll_deg == 180){
					//fill ground to -180 deg
					for(j = 0 ; j < PX180DEG_HORIZONTAL; j+=FILL_DENSITY){
						if((y-b-j>=0) && (y-b-j<HEIGHT)){ 
							DrawPointNew(x+i, y-b-j, 1);
							DrawPointNew(x-i, y-b-j, 1);							
						}
					}
					//fill ground from 180 deg 
					for(j = 0; j <= HEIGHT/2; j+=FILL_DENSITY){
						if((y-b+PX180DEG_HORIZONTAL+j>=0) && (y-b+PX180DEG_HORIZONTAL+j<HEIGHT)){ // Horizon line at pitch -180 deg
							DrawPointNew(x+i, y-b+PX180DEG_HORIZONTAL+j, 1);
							DrawPointNew(x-i, y-b+PX180DEG_HORIZONTAL+j, 1);							
						}
					}
				}											 							
		}		
	}
	/****************************Horizontal line - END*****************************************/
	
		
	/***************************Line with max vertical span - START****************************/
	else if(((roll_deg>=diagonal_angle && roll_deg<=(180-diagonal_angle)) && roll_deg != 90) || ((roll_deg>=(-180+diagonal_angle) && roll_deg<=(-diagonal_angle)) && roll_deg != -90)) 
	{
			
		for(i = 0; i < (HEIGHT); ++i){
			//0
			x0_p = i*a2+x-b*sin_roll;
			y0_p = y+i-b*cos_roll;
			x0_n = -i*a2+x-b*sin_roll;
			y0_n = y-i-b*cos_roll;
			next_line_sin = PX180DEG*sin_roll;
			next_line_cos = PX180DEG*cos_roll;
			if((x0_p>=0) && (x0_p<WIDTH) && (y0_p>=0) && (y0_p<HEIGHT))  // Line at pitch 0 deg
					DrawPointNew(x0_p, y0_p, 1);
			if((x0_n>=0) && (x0_n<WIDTH) && (y0_n>=0) && (y0_n<HEIGHT)) 
					DrawPointNew(x0_n, y0_n, 1);
			//-180
			if((x0_p-next_line_sin>=0) && (x0_p-next_line_sin < WIDTH)  &&  (y0_p-next_line_cos >= 0)  &&  (y0_p-next_line_cos < HEIGHT))  // Line at pitch 180 deg
				DrawPointNew(x0_p-next_line_sin, y0_p-next_line_cos, 1);
			if((x0_n-next_line_sin>=0) && (x0_n-next_line_sin<WIDTH) && (y0_n-next_line_cos>=0) && (y0_n-next_line_cos<HEIGHT)) 
				DrawPointNew(x0_n-next_line_sin, y0_n-next_line_cos, 1);
			//180
			if((x0_p+next_line_sin>=0) && (x0_p+next_line_sin<WIDTH) && (y0_p+next_line_cos>=0) && (y0_p+next_line_cos<HEIGHT))  // Line at pitch -180 deg
				DrawPointNew(x0_p+next_line_sin, y0_p+next_line_cos, 1);
			if((x0_n+next_line_sin>=0) && (x0_n+next_line_sin<WIDTH) && (y0_n+next_line_cos>=0) && (y0_n+next_line_cos<HEIGHT)) 
				DrawPointNew(x0_n+next_line_sin, y0_n+next_line_cos, 1);

				//fill ground right
				for(j = 0; j<PX180DEG; j+=FILL_DENSITY){
					if((x0_p+j*sin_roll>=0) && (x0_p+j*sin_roll<WIDTH) && (y0_p+j*cos_roll>=0) && (y0_p+j*cos_roll<HEIGHT))  // Line at pitch 0 deg
						DrawPointNew(x0_p+j*sin_roll, y0_p+j*cos_roll, 1);
					if((x0_n+j*sin_roll>=0) && (x0_n+j*sin_roll<WIDTH) && (y0_n+j*cos_roll>=0) && (y0_n+j*cos_roll<HEIGHT)) 
						DrawPointNew(x0_n+j*sin_roll, y0_n+j*cos_roll, 1);
				}
				for(j = 0; j<PX180DEG; j+=FILL_DENSITY){
					if((x0_p-next_line_sin-j*sin_roll>=0) && (x0_p-next_line_sin-j*sin_roll < WIDTH)  &&  (y0_p-next_line_cos-j*cos_roll >= 0)  &&  (y0_p-next_line_cos-j*cos_roll < HEIGHT))  // Line at pitch 180 deg
						DrawPointNew(x0_p-next_line_sin-j*sin_roll, y0_p-next_line_cos-j*cos_roll, 1);
					if((x0_n-next_line_sin-j*sin_roll>=0) && (x0_n-next_line_sin-j*sin_roll < WIDTH) &&   (y0_n-next_line_cos-j*cos_roll >= 0)  &&  (y0_n-next_line_cos-j*cos_roll < HEIGHT)) 
						DrawPointNew(x0_n-next_line_sin-j*sin_roll, y0_n-next_line_cos-j*cos_roll, 1);
				}

		}		
	}
	/***************************Line with max vertical span - END******************************/
		
	
	/***************************Line with max horizontal span - START**************************/
	else if(((roll_deg>-diagonal_angle && roll_deg<diagonal_angle) && roll_deg != 0) || (roll_deg>(180-diagonal_angle) && roll_deg<180) || (roll_deg<(-180+diagonal_angle) && roll_deg>-180)) 
	{
			
		for(i = 0; i < (WIDTH/2); ++i){

			x0_p = x+i-b*sin_roll;
			y0_p = i*a1+y-b*cos_roll;
			x0_n = x-i-b*sin_roll;
			y0_n = -i*a1+y-b*cos_roll;
			next_line_sin = PX180DEG*sin_roll;
			next_line_cos = PX180DEG*cos_roll;
			
			if((x0_p>=0) && (x0_p<WIDTH) && (y0_p>=0) && (y0_p<HEIGHT))  // Line at pitch 0 deg
					DrawPointNew(x0_p, y0_p, 1);
			if((x0_n>=0) && (x0_n<WIDTH) && (y0_n>=0) && (y0_n<HEIGHT)) 
					DrawPointNew(x0_n, y0_n, 1);
			//-180
			if((x0_p-next_line_sin>=0) && (x0_p-next_line_sin < WIDTH)  &&  (y0_p-next_line_cos >= 0)  &&  (y0_p-next_line_cos < HEIGHT))  // Line at pitch 180 deg
				DrawPointNew(x0_p-next_line_sin, y0_p-next_line_cos, 1);
			if((x0_n-next_line_sin>=0) && (x0_n-next_line_sin<WIDTH) && (y0_n-next_line_cos>=0) && (y0_n-next_line_cos<HEIGHT)) 
				DrawPointNew(x0_n-next_line_sin, y0_n-next_line_cos, 1);
			//180
			if((x0_p+next_line_sin>=0) && (x0_p+next_line_sin<WIDTH) && (y0_p+next_line_cos>=0) && (y0_p+next_line_cos<HEIGHT))  // Line at pitch -180 deg
				DrawPointNew(x0_p+next_line_sin, y0_p+next_line_cos, 1);
			if((x0_n+next_line_sin>=0) && (x0_n+next_line_sin<WIDTH) && (y0_n+next_line_cos>=0) && (y0_n+next_line_cos<HEIGHT)) 
				DrawPointNew(x0_n+next_line_sin, y0_n+next_line_cos, 1);
			
				//fill ground right
				for(j = 0; j<PX180DEG; j+=FILL_DENSITY){
					if((x0_p+j*sin_roll>=0) && (x0_p+j*sin_roll<WIDTH) && (y0_p+j*cos_roll>=0) && (y0_p+j*cos_roll<HEIGHT))  // Line at pitch 0 deg
						DrawPointNew(x0_p+j*sin_roll, y0_p+j*cos_roll, 1);
					if((x0_n+j*sin_roll>=0) && (x0_n+j*sin_roll<WIDTH) && (y0_n+j*cos_roll>=0) && (y0_n+j*cos_roll<HEIGHT)) 
						DrawPointNew(x0_n+j*sin_roll, y0_n+j*cos_roll, 1);
				}
				for(j = 0; j<PX180DEG; j+=FILL_DENSITY){
					if((x0_p-next_line_sin-j*sin_roll>=0) && (x0_p-next_line_sin-j*sin_roll < WIDTH)  &&  (y0_p-next_line_cos-j*cos_roll >= 0)  &&  (y0_p-next_line_cos-j*cos_roll < HEIGHT))  // Line at pitch 180 deg
						DrawPointNew(x0_p-next_line_sin-j*sin_roll, y0_p-next_line_cos-j*cos_roll, 1);
					if((x0_n-next_line_sin-j*sin_roll>=0) && (x0_n-next_line_sin-j*sin_roll < WIDTH) &&   (y0_n-next_line_cos-j*cos_roll >= 0)  &&  (y0_n-next_line_cos-j*cos_roll < HEIGHT)) 
						DrawPointNew(x0_n-next_line_sin-j*sin_roll, y0_n-next_line_cos-j*cos_roll, 1);
				}
		}		
	}
		/*************************Line with max horizontal span - END****************************/
	drawCenter();
}


void drawCenter(void){
		for(int i = 0; i < 16; ++i){
			for(int j = 0; j<3;j++){
				DrawPointNew(64+i, 31+j, 1);
				DrawPointNew(64-i, 31+j, 1);
			}
		}	
		for(int i = 0; i < 8; ++i){
			for(int j = 0; j<3;j++){
				DrawPointNew(63+j, 32-i, 1);
			}
		}
}
