#include <stdio.h>
#include <stdlib.h>

#include "stm32f407xx.h"
#include "PB_LCD_Drivers.h"
#include "LED_PB_functions.h"
#include "multimeter_functions.h"
#include "timer_functions.h"
#include "digital_functions.h"
#include "frequency_functions.h"

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
	initFrequencyPin();
	
	// Initial conditions
	PB_LCD_Clear();
	
	// Later add this into while loop
	waitForADCAndRead();
	
	uint32_t incr = 0;
	
	// Output value to LCD
	while(1){
		//OutputValue();
		
		/*if (readPushButton()){
			switchMode();
		}*/
		
		PB_LCD_Clear();
		PB_LCD_GoToXY(0, 0);
		char * output = malloc(13 * sizeof(char));
		
		uint32_t test = TIM4->CNT;
		snprintf(output, 13 * sizeof(char), "%u", test);
		
		PB_LCD_WriteString(output, 12);
		
		snprintf(output, 13 * sizeof(char), "%u", incr);
		PB_LCD_GoToXY(0, 1);
		PB_LCD_WriteString(output, 12);
		incr++;
		
		free(output);
		
		waitInterval(DEBOUNCE_TIME);
	}
}
