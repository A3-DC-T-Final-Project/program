#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "stm32f407xx.h"
#include "PB_LCD_Drivers.h"
#include "LED_PB_functions.h"
#include "multimeter_functions.h"
#include "digital_functions.h"
#include "math_functions.h"
#include "ranges.h"

// Int var for ADC conversions
uint32_t ADCconv;
// Int var for read mode
int read_mode;
// Int var for voltage range
int voltage_range;

// Conversion bounds
float conversion_upper_bound;
float conversion_lower_bound;

void initADC(void){
	// Initialise ADConv, read_mode variable
	ADCconv = 0;
	read_mode = 0;
	
	// Enables clock for required registers in GPIOC
	RCC->AHB1ENR = (RCC->AHB1ENR & ~0x0000004ul) | 0x00000004;
	// Sets the MODE to analogue input/output (0b11)
	GPIOC->MODER = (GPIOC->MODER & ~GPIO_MODER_MODER4_Msk) | (0x3 << GPIO_MODER_MODER4_Pos);
	// Enable the peripheral clock using RCC APB2 register
	RCC->APB2ENR = (RCC->APB2ENR & ~RCC_APB2ENR_ADC1EN_Msk) | (0x1 << RCC_APB2ENR_ADC1EN_Pos);
	// Configure channel 14 (E), 1 conversion(SQ1)
	// SQ1 is bits 4:0 so no shift
	ADC1->SQR3 = (ADC1->SQR3 & ~ADC_SQR3_SQ1) | 0xE;
	// Turn converter on (ADON)
	// Since ADON is bit 0 there is no shift
	ADC1->CR2 = (ADC1->CR2 & ~ADC_CR2_ADON_Msk) | 0x1;
	// Enable DISCEN for discontinuous mode on regular channels
	ADC1->CR1 = (ADC1->CR1 & ~ADC_CR1_DISCEN_Msk) | (0x1 << ADC_CR1_DISCEN_Pos);
	// Enable EOCS for End Of Conversion Selection
	// Enables overrun detection
	ADC1->CR2 = (ADC1->CR2 & ~ADC_CR2_EOCS_Msk) | (0x1 << ADC_CR2_EOCS_Pos);
	
	// Default to 10V range on startup
	changeVoltageRange(V_10_RANGE);
}

void initPinSelect(void){
	// Enables clock for required pins in GPIOB
	RCC->AHB1ENR = (RCC->AHB1ENR & ~RCC_AHB1ENR_GPIOBEN_Msk) | (0x1 << RCC_AHB1ENR_GPIOBEN_Pos);
	// Set mode of GPIO pin B8 to GP output
	GPIOB->MODER = (GPIOB->MODER & ~GPIO_MODER_MODER8_Msk) | (0x1 << GPIO_MODER_MODER8_Pos);
}

void waitForADCAndRead(void){
	// Starts a conversion for regular channels
	ADC1->CR2 = (ADC1->CR2 & ~ADC_CR2_SWSTART_Msk) | (0x1 << ADC_CR2_SWSTART_Pos);
	// Waits for the EOC flag to be triggered
	do {
		__asm("");
	} while(!((ADC1->SR >> 1) & 1));
	
	// Store the value in the ADCs Data Register to a local var
	ADCconv = ADC1->DR;
}

void OutputValue(void){
	// Update ADC value
	waitForADCAndRead();

	// Reset the LCD
	PB_LCD_Clear();
	// Write to first line
	PB_LCD_GoToXY(0, 0);
	// Write the property to LCD
	switch(read_mode){
		case 0:
			PB_LCD_WriteString("DC Voltage:", 0xB);
			break;
		case 1:
			PB_LCD_WriteString("AC Voltage:", 0xB);
			break;
		case 2:
			PB_LCD_WriteString("Current:", 0x8);
			break;
		case 3:
			PB_LCD_WriteString("Resistance:", 0xB);
			break;
		default:
			PB_LCD_WriteString("Problem:", 0x8);
			break;
	}
	// Write to second line
	PB_LCD_GoToXY(0, 1);
	// Compile time variables
	float total;
	float mapped_value = 0;
	
	// Allocate memory and define string var for the LCD value buffer
	char* value = malloc(13*sizeof(char));
	
	// Flag to keep track of correct measuring
	bool conversion = false;

	// Switch to write output
	switch(read_mode){
		case DC_MODE:
			// Placeholder value 
			total = (ADC_MAX / 2);	
			while (!conversion) {
					// Map the value of the ADC to the correct numbers
					total = DCVoltage(&conversion);
			}		
			switch (voltage_range) {
				// TODO: Do not repeat snprintf calls
				case V_100M_RANGE:
					mapped_value = map(total, ADC_MIN, ADC_MAX, -100, 100);
					// Put the value of the ADC into the value buffer
					snprintf(value, 13*sizeof(char), "%.3f mV", mapped_value);
					break;
				case V_1_RANGE:
					mapped_value = map(total, ADC_MIN, ADC_MAX, -1, 1);
					snprintf(value, 13*sizeof(char), "%.3f V", mapped_value);
					break;
				case V_5_RANGE:
					mapped_value = map(total, ADC_MIN, ADC_MAX, -5, 5);
					snprintf(value, 13*sizeof(char), "%.3f V", mapped_value);
					break;
				default:
				case V_10_RANGE:
					mapped_value = map(total, ADC_MIN, ADC_MAX, -10, 10);
					snprintf(value, 13*sizeof(char), "%.3f V", mapped_value);
					break;
			}
			
			break;
		case AC_MODE:
			// AC voltage map
			total = ACVoltage();
			mapped_value = map(total, 0, 4095, 0, 3);
			break;
		case I_MODE:
			// Current map
			break;
		case R_MODE:
			// Resistance map
			break;
		default:
			break;
	}

	// Write value buffer to LCD
	PB_LCD_WriteString(value, 0xC);

	// Free the allocated memory
	free(value);
} 

float DCVoltage(){
	double total = 0;
	int i;
	for(i=0; i<1000; i++){
		waitForADCAndRead();
		total += ADCconv;
	}

	float mean_total;
	mean_total = MeanAverage(total, 1000);
	return mean_total;
}

float ACVoltage(){
	double squares_total = 0;
	int i;
	for(i=0; i<1000; i++){
		waitForADCAndRead();
		squares_total += pow(ADCconv, 2);
	}

	float rms_total;
	rms_total = RMSAverage(squares_total, 1000);
	return rms_total;
}

float DCVoltage(bool * conversion){
	double total = 0;
	int i;
	for(i=0; i<1000; i++){
		waitForADCAndRead();
		total += ADCconv;
	}

	float mean_total;
	mean_total = MeanAverage(total, 1000);
	
	// If the mean is between the bounds of another range
	// switch to said range;
	if((mean_total > conversion_lower_bound) && (mean_total < conversion_upper_bound)) {
		(*conversion) = false;
		changeVoltageRange(voltage_range - 1);
	} else {
		(*conversion) = true;
	}
	
	// If the mean is ADC_MAX, use the upper range (max range 10V, so no upper range for 10V)
	if(((uint32_t) mean_total == ADC_MAX) && (voltage_range != V_10_RANGE)) {
		(*conversion) = false;
		changeVoltageRange(voltage_range + 1);
	}
	
	return mean_total;
}

void switchMode(void){
	read_mode++;
	if(read_mode == (R_MODE + 1)){
		read_mode = DC_MODE;
	}
	
	// TODO: Hard-wire to ground
	setLow(GPIOB, 7);

	switch(read_mode) {
		case R_MODE:
		case AC_MODE:
		case DC_MODE:
			setLow(GPIOB, 5);
			break;
		case I_MODE:
			setHigh(GPIOB, 5);
			break;
		default:
			break;
	}
}

void changeVoltageRange(int range) {
	voltage_range = range;
	switch (range) {
		case V_100M_RANGE:
			setLow(GPIOE, 3);
			setLow(GPIOE, 4);
			conversion_lower_bound = -1;
			conversion_upper_bound = -1;
			break;
		case V_1_RANGE:
			setHigh(GPIOE, 3);
			setLow(GPIOE, 4);
			conversion_lower_bound = V_1_M100mV;
			conversion_upper_bound = V_1_P100mV;
			break;
		case V_5_RANGE:
			setLow(GPIOE, 3);
			setHigh(GPIOE, 4);
			conversion_lower_bound = V_5_M1V;
			conversion_upper_bound = V_5_P1V;
			break;
		case V_10_RANGE:
			setHigh(GPIOE, 3);
			setHigh(GPIOE, 4);
			conversion_lower_bound = V_10_M5V;
			conversion_upper_bound = V_10_P5V;
			break;
		default:
			voltage_range = V_10_RANGE;
			conversion_lower_bound = V_10_M5V;
			conversion_upper_bound = V_10_P5V;
			setHigh(GPIOE, 3);
			setHigh(GPIOE, 4);
			break;
	}
}
