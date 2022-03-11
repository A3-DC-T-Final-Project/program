#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	
	initPinSelect();
	initLEDs();
	initButton();
	PB_LCD_Init();
	initDigitalPins();
	initADC();
	//setLow(GPIOE, 3);
	//setLow(GPIOE, 4);
	
	// Initial conditions
	PB_LCD_Clear();
	
	//waitInterval(500);
	
	// Later add this into while loop
	/*waitForADCAndRead();
	
	char * message = malloc(13*sizeof(char));
	
	snprintf(message, 13*sizeof(char), "%u", ADCconv);
	
	PB_LCD_GoToXY(0, 0);
	PB_LCD_WriteString(message, 0xC);*/
	
	// Output value to LCD
	while(1){
		OutputValue();
		
		waitInterval(DEBOUNCE_TIME);
	}
}
