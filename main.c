#include <stdio.h>
#include <stdlib.h>

#include "stm32f407xx.h"
#include "PB_LCD_Drivers.h"
#include "LED_PB_functions.h"
#include "multimeter_functions.h"
#include "timer_functions.h"
#include "digital_functions.h"

int main (void){
	// Initialisation functions
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock/1000);
	initTimer();
	initDAC();
	initADC();
	initPinSelect();
	initLEDs();
	initButton();
	PB_LCD_Init();
	initDigitalPins();
	
	// Initial conditions
	PB_LCD_Clear();
	
	// Later add this into while loop
	waitForADCAndRead();
	
	// Output value to LCD
	while(1){
		OutputValue();
		
		/*if (readPushButton()){
			switchMode();
		}*/
		
		waitInterval(DEBOUNCE_TIME);
	}
}
