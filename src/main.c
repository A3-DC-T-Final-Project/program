#include <stdio.h>
#include <stdlib.h>

#include "stm32f407xx.h"
#include "PB_LCD_Drivers.h"
#include "LED_PB_functions.h"
#include "multimeter_functions.h"
#include "timer_functions.h"

int main (void){
	// Initialisation functions
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock/2);
	initTimer();
	initDAC();
	initADC();
	initLEDs();
	initButton();
	PB_LCD_Init();
	
	// Initial conditions
	PB_LCD_Clear();
	LED_Turn_On(4);
	
	// Read from ADC
	waitForADCAndRead();
	// Output value to LCD
	OutputValue();
}
