#include "stm32f407xx.h"
#include "LED_PB_functions.h"
#include "Board_LED.h"

void initLEDs(void){
	// Enables clock for required registers in GPIOD
	RCC->AHB1ENR = (RCC->AHB1ENR & ~0x00000008ul) | 0x00000008;
	// Sets the mode for all LEDs
	GPIOD->MODER = (GPIOD->MODER & 0x00FFFFFF) | 0x55000000;
}

void initButton(void){
	// Enables clock for required registers in GPIOA 
	RCC->AHB1ENR = (RCC->AHB1ENR & 0xFFFFFFFE) | 0x00000001;
	// Defines pull-down nature
	GPIOA->PUPDR = (GPIOA->PUPDR & ~GPIO_PUPDR_PUPD0_Msk) | (0x2 << GPIO_PUPDR_PUPD0_Pos);
	// Sets MODE of push button
	// Ensures MODE is 0 so pin is an input not an output
	GPIOA->MODER = GPIOA->MODER & 0xFFFFFFFC;
}

uint32_t ReadPushButton(void){
	// Reads in the GPIOA IDR then isolates the last bit (push button state)
	uint32_t LastBit = GPIOA->IDR & (1 << 1) -1;
	return LastBit;
}

void LED_Turn_On(int colour){
	switch (colour){
		case 0:
			//setting green LED PD12
			GPIOD->BSRR = 0x00001000;
			break;
		
		case 1:
			//setting orange LED PD13
			GPIOD->BSRR = 0x00002000;
			break;
	
		case 2:
			//setting red LED PD14
			GPIOD->BSRR = 0x00004000;
			break;
		
		case 3:
			//setting blue LED PD15
			GPIOD->BSRR = 0x00008000;
			break;
		
		default:
			//setting all LEDS
			GPIOD->BSRR = 0x0000F000;
			break;
	} 
	
}

void LED_Turn_Off(int colour){
	switch (colour){
		case 0:
			//resetting green LED PD12
			GPIOD->BSRR = 0x10000000;
			break;
		
		case 1:
			//resetting orange LED PD13
			GPIOD->BSRR = 0x20000000;
			break;
	
		case 2:
			//resetting red LED PD14
			GPIOD->BSRR = 0x40000000;
			break;
		
		case 3:
			//resetting blue LED PD15
			GPIOD->BSRR = 0x80000000;
			break;
		
		default:
			//resetting all LEDS
			GPIOD->BSRR = 0xF0000000;
			break;
	}
}
