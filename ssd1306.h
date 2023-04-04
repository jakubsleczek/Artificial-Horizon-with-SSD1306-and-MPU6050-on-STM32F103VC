#ifndef _SSD1306_H_
#define _SSD1306_H_


#include "i2c.h"
#include "systick.h" 
#include "font.c"
#include <math.h>

#define PI  3.14F
#define WIDTH	128  //Dispaly width (px)
#define HEIGHT	64 // Display height (px)
#define PX180DEG_HORIZONTAL 96 //Number of px/180deg in horizontal move
#define PX180DEG_VERTICAL 192 // Number of px/180deg in vertical move
#define HELPER 	56		//angle (deg) when raduis is equal WIDTH of the display
#define FILL_DENSITY 4 // ground lines density

static unsigned char DispBuff[128][8];

void ssd1306_init(void);
void writeByte(uint8_t data, _Bool operation);
void DisplayCls(unsigned char fill);
void RefreshRAM(void);
void SetColStart(void);
void DrawPoint(unsigned char x ,unsigned char y, unsigned char p);
void DisplayChar(uint8_t x, uint8_t y, uint8_t Chr, uint8_t mode);
void DispTxt(uint8_t x, uint8_t y, const uint8_t *txt, uint8_t mode);
void DrawBmp(uint8_t x, uint8_t y, const uint8_t *bmp, uint8_t width, uint8_t height);
void drawLine(uint8_t x, uint8_t y, uint8_t length, int16_t angle_deg, int16_t pitch_deg);
void DrawPointNew(int16_t x ,int16_t y, unsigned char p);
void drawScreen(int16_t roll_deg, int16_t pitch_deg);
void drawAH(int16_t roll_deg, int16_t pitch_deg);
int16_t bCoefficientCalculator(int16_t roll_deg, int16_t pitch_deg);
int16_t PX180DEGCoefficientCalculator(int16_t roll_deg);
void drawCenter(void);

#endif //_SSD1306_H_
