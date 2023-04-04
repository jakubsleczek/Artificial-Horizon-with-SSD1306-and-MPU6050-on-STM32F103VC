
#include "systick.h"                  // Device header

//intercnal RC is 9MHz
//external OSC is 72MHz
#define SYSTICK_LOAD_VAL		72000
#define CTRL_ENABLE					(1U<<0)
#define CTRL_CLKSRC					(1U<<2)
#define CTRL_COUNTERFLAG		(1U<<16)

void systickDelaysMs(int n){
	// Setup LOAD value to be 1ms, enable , external clk
	SysTick->LOAD = SYSTICK_LOAD_VAL; 
	SysTick->CTRL |= CTRL_ENABLE;
	SysTick->CTRL |= CTRL_CLKSRC;
	// current value
	SysTick->VAL = 0;
	// 1ms x n
	for(int i=0; i<n; ++i){
		while(!(SysTick->CTRL & CTRL_COUNTERFLAG));
	}
	SysTick->CTRL=0;
}

