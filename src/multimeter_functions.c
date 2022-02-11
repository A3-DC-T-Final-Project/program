#include <stdio.h>
#include <stdlib.h>

#include "stm32f407xx.h"
#include "PB_LCD_Drivers.h"
#include "multimeter_functions.h"

// Int var for ADC conversions
uint32_t static ADCconv;

void initTimer(void){
	// Enables clock for required registers in GPIOE
	RCC->AHB1ENR = (RCC->AHB1ENR & 0xFFFFFFEF) | 0x00000010;
	// Sets the MODE of E7 alternate function mode 0b10
	GPIOE->MODER = (GPIOE->MODER & ~GPIO_MODER_MODER7_Msk) | (0x2 << GPIO_MODER_MODER7_Pos);
	// Sets the low AFR register for GPIOE pin 7(low between 0 and 7) to mux select 1 for TIM1_ETR
	// 0x18 shifts selection 1 to position word 7 (4 bits wide)
	GPIOE->AFR[0] = (GPIOE->AFR[0] & ~GPIO_AFRL_AFRL7) | (GPIO_AFRL_AFSEL1_1 << 0x18);
}

void initADC(void){
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
	// Starts conversion for regular channels
	ADC1->CR2 = (ADC1->CR2 & ~ADC_CR2_SWSTART_Msk) | (0x1 << ADC_CR2_SWSTART_Pos);
}

void initDAC(void){
	// Enables clock for required registers in GPIOA
	RCC->AHB1ENR = (RCC->AHB1ENR & ~0x00000001ul) | 0x00000001;
	// Sets the MODE to analogue input/output (0b11)
	GPIOA->MODER = (GPIOA->MODER & ~GPIO_MODER_MODER4_Msk) | (0x3 << GPIO_MODER_MODER4_Pos);
}

void waitForADCAndRead(void){
	// Waits for the EOC flag to be triggered
	while(!((ADC1->SR >> 1) & 1)){
	}
	// Store the value in the ADCs Data Register to a local var
	ADCconv = ADC1->DR;
}

void OutputValue(){
	// Write to first line
	PB_LCD_GoToXY(0, 0);
	// Write the property (voltage) to LCD
	PB_LCD_WriteString("Voltage:", 0x8);
	// Write to second line
	PB_LCD_GoToXY(0, 1);
	// Map the value of the ADC to the correct numbers
	float mapped_value = map((float) ADCconv, 0, 4095, 0, 3);
	// Allocate memory and define string var for the LCD value buffer
	char* value = malloc(13*sizeof(char));
	// Put the value of the ADC into the value buffer
	snprintf(value, 13*sizeof(char), "%.5lf", (double) mapped_value);
	// Write value buffer to LCD
	PB_LCD_WriteString(value, 0x12);
	// Free the allocated memory
	free(value);
}

// Map function relates one set of numbers to another set
float map(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
