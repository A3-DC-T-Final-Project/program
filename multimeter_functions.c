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
// Int var for current range
int current_range;

// Conversion bounds
float conversion_upper_bound;
float conversion_lower_bound;
// Range bounds
float range_max;
float range_min;

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
	// Default to 100mA range on startup
	changeCurrentRange(I_100M_RANGE);
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
	float total = (ADC_MAX / 2); // Placeholder value
	float mapped_value = 0;
	
	// Allocate memory and define string var for the LCD value buffer
	char* value = malloc(13*sizeof(char));
	
	// Flag to keep track of correct measuring
	bool conversion = false;
	// Flag to keep track of voltage measuring
	bool isVoltageReading = false;

	// Switch to write output
	switch(read_mode){
		case DC_MODE:	
			while (!conversion) {
					// Map the value of the ADC to the correct numbers
					total = DCVoltage(&conversion);
				}
			isVoltageReading = true;
			break;
		case AC_MODE:
			while (!conversion) {
					// Map the value of the ADC to the correct numbers
					total = ACVoltage(&conversion);
				}
			isVoltageReading = true;
			break;
		case I_MODE:
			while (!conversion) {
				total = Current(&conversion);
			}
			switch (current_range) {
				case I_10M_RANGE:
					mapped_value = map(total, range_min, range_max, -10, 10);
					break;
				case I_25M_RANGE:
					mapped_value = map(total, range_min, range_max, -25, 25);
					break;
				case I_50M_RANGE:
					mapped_value = map(total, range_min, range_max, -50, 50);
					break;
				default:
				case I_100M_RANGE:
					mapped_value = map(total, range_min, range_max, -100, 100);
					break;
			}
			snprintf(value, 13*sizeof(char), "%.3f mA", mapped_value);
			break;
		case R_MODE:
			// Resistance map
			break;
		default:
			break;
	}
	
	if (isVoltageReading) {
		switch (voltage_range) {
				// TODO: Do not repeat snprintf calls
				case V_100M_RANGE:
					mapped_value = map(total, range_min, range_max, -100, 100);
					// Put the value of the ADC into the value buffer
					snprintf(value, 13*sizeof(char), "%.3f mV", mapped_value);
					break;
				case V_1_RANGE:
					mapped_value = map(total, range_min, range_max, -1, 1);
					snprintf(value, 13*sizeof(char), "%.3f V", mapped_value);
					break;
				case V_5_RANGE:
					mapped_value = map(total, range_min, range_max, -5, 5);
					snprintf(value, 13*sizeof(char), "%.3f V", mapped_value);
					break;
				default:
				case V_10_RANGE:
					mapped_value = map(total, range_min, range_max, -10, 10);
					snprintf(value, 13*sizeof(char), "%.3f V", mapped_value);
					break;
			}
	}

	// Write value buffer to LCD
	PB_LCD_WriteString(value, 0xC);

	// Free the allocated memory
	free(value);
} 

void checkIfInRange(float value, bool * conversion) {
	if((value > conversion_lower_bound) && (value < conversion_upper_bound)) {
		// If the mean is between the bounds of another range
		// switch to said range;
		(*conversion) = false;
		changeVoltageRange(voltage_range - 1);
	} else if((value > range_max) && (voltage_range != V_10_RANGE)) {
		// If value overshoots, switch to upper range
		(*conversion) = false;
		changeVoltageRange(voltage_range + 1);
	} else if((value < range_min) && (voltage_range != V_10_RANGE)) {
		// If value undershoots, switch to lower range
		(*conversion) = false;
		changeVoltageRange(voltage_range - 1);
	} else {
		// Successful conversion
		(*conversion) = true;
	}
}

void checkIfInRangeCurrent(float value, bool * conversion) {
	if((value > conversion_lower_bound) && (value < conversion_upper_bound)) {
		// If the mean is between the bounds of another range
		// switch to said range;
		(*conversion) = false;
		changeVoltageRange(voltage_range - 1);
	} else if((value > range_max) && (current_range != I_100M_RANGE)) {
		// If value overshoots, switch to upper range
		(*conversion) = false;
		changeVoltageRange(voltage_range + 1);
	} else if((value < range_min) && (current_range != I_100M_RANGE)) {
		// If value undershoots, switch to lower range
		(*conversion) = false;
		changeVoltageRange(voltage_range - 1);
	} else {
		// Successful conversion
		(*conversion) = true;
	}
}

float ACVoltage(bool * conversion){
	double squares_total = 0;
	int i;
	for(i=0; i<1000; i++){
		waitForADCAndRead();
		squares_total += pow(ADCconv, 2);
	}

	float rms_total;
	rms_total = RMSAverage(squares_total, 1000);

	checkIfInRange(rms_total, conversion);

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
	
	checkIfInRange(mean_total, conversion);
	
	return mean_total;
}

float Current(bool * conversion) {
	double total = 0;
	int i;
	for(i=0; i<1000; i++){
		waitForADCAndRead();
		total += ADCconv;
	}

	float mean_total;
	mean_total = MeanAverage(total, 1000);
	
	checkIfInRangeCurrent(mean_total, conversion);
	//(*conversion) = true;
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
			changeVoltageRange(V_10_RANGE);
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
			range_max = V_100m_MAX;
			range_min = V_100m_MIN;
			break;
		case V_1_RANGE:
			setHigh(GPIOE, 3);
			setLow(GPIOE, 4);
			conversion_lower_bound = V_1_M100mV;
			conversion_upper_bound = V_1_P100mV;
			range_max = V_1_MAX;
			range_min = V_1_MIN;
			break;
		case V_5_RANGE:
			setLow(GPIOE, 3);
			setHigh(GPIOE, 4);
			conversion_lower_bound = V_5_M1V;
			conversion_upper_bound = V_5_P1V;
			range_max = V_5_MAX;
			range_min = V_5_MIN;
			break;
		case V_10_RANGE:
			setHigh(GPIOE, 3);
			setHigh(GPIOE, 4);
			conversion_lower_bound = V_10_M5V;
			conversion_upper_bound = V_10_P5V;
			range_max = V_10_MAX;
			range_min = V_10_MIN;
			break;
		default:
			voltage_range = V_10_RANGE;
			conversion_lower_bound = V_10_M5V;
			conversion_upper_bound = V_10_P5V;
			range_max = V_10_MAX;
			range_min = V_10_MIN;
			setHigh(GPIOE, 3);
			setHigh(GPIOE, 4);
			break;
	}
}

void changeCurrentRange(int range) {
	current_range = range;
	switch (range) {
		case I_10M_RANGE:
			setLow(GPIOE, 5);
			setLow(GPIOE, 6);
			conversion_lower_bound = -1;
			conversion_upper_bound = -1;
			range_max = I_10M_MAX;
			range_min = I_10M_MIN;
			break;
		case I_25M_RANGE:
			setHigh(GPIOE, 5);
			setLow(GPIOE, 6);
			conversion_lower_bound = I_25M_M10M;
			conversion_upper_bound = I_25M_P10M;
			range_max = I_25M_MAX;
			range_min = I_25M_MIN;
			break;
		case I_50M_RANGE:
			setLow(GPIOE, 5);
			setHigh(GPIOE, 6);
			conversion_lower_bound = I_50M_M25M;
			conversion_upper_bound = I_50M_P25M;
			range_max = I_50M_MAX;
			range_min = I_50M_MIN;
			break;
		case I_100M_RANGE:
			setHigh(GPIOE, 5);
			setHigh(GPIOE, 6);
			conversion_lower_bound = I_100M_M50M;
			conversion_upper_bound = I_100M_P50M;
			range_max = I_100M_MAX;
			range_min = I_100M_MIN;
			break;
		default:
			current_range = I_100M_RANGE;
			setHigh(GPIOE, 5);
			setHigh(GPIOE, 6);
			conversion_lower_bound = I_100M_M50M;
			conversion_upper_bound = I_100M_P50M;
			range_max = I_100M_MAX;
			range_min = I_100M_MIN;
			break;
	}
}
