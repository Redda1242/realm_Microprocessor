#include "stm32l031lib.h"
#include "realm.h"

void initSound(void);
	
int main()
{
	initClock(); // Set the system clock running at 16MHz
	RCC->IOPENR |= 2; // enable GPIOB
	pinMode(GPIOB,3,1); // Make GPIOB_3 (LD3) an output
	pinMode(GPIOB,4,0); // Make GPIOB_4 (Button) an input
	enablePullUp(GPIOB,4); // enable pull-up for GPIOB_4
	RCC->IOPENR |= (1<<0); // enable GPIOA
	pinMode(GPIOA,5,1); // Make GPIOA bit 5 an output
	pinMode(GPIOA,6,1); // Make GPIOA bit 6 an output
	pinMode(GPIOA,0,1); // Make PORTA Bit 0 an output
	pinMode(GPIOA,1,1); // Make PORTA Bit 1 an output
	initSound();
	initSerial();
	SysTick->LOAD = 16000;   // 16MHz / 16000 = 1kHz
	SysTick->CTRL = 7;       // enable systick counting and interrupts, use core clock
	enable_interrupts();
	GPIOA->ODR = 1;
	
	while(1)
	{
		//start game realm.c
		runGame();
		
	}
}
