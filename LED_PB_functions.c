#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "stm32f407xx.h"
#include "Board_LED.h"
#include "LED_PB_functions.h"
#include "PB_LCD_Drivers.h"
#include "multimeter_functions.h"
#include "timer_functions.h"

// Counter for debouncing button
static uint32_t debounce_counter = 0;

void initLEDs(void){
	// Enables clock for required pins in GPIOD
	RCC->AHB1ENR = (RCC->AHB1ENR & ~0x00000008ul) | 0x00000008;
	// Sets the mode for all LEDs
	GPIOD->MODER = (GPIOD->MODER & 0x00FFFFFF) | 0x55000000;
}

void initButton(void){
	// Enables clock for required registers in GPIOA 
	RCC->AHB1ENR = (RCC->AHB1ENR & 0xFFFFFFFE) | 0x00000001;
	// Defines pull-up pull-down nature (PU)
	GPIOA->PUPDR = (GPIOA->PUPDR & ~GPIO_PUPDR_PUPD0_Msk) | (0x0 << GPIO_PUPDR_PUPD0_Pos);
	// Sets MODE as 0 so pin (A0) is an input not an output
	GPIOA->MODER = GPIOA->MODER & 0xFFFFFFFC;

	// CONFIGURES PUSH BUTTON AS INTERRUPT:
	// Enable clock for SYSCFG_EXTICR using the RCC->APB2ENR register
	RCC->APB2ENR = (RCC->APB2ENR & ~RCC_APB2ENR_SYSCFGEN_Msk) | (0x1 << RCC_APB2ENR_SYSCFGEN_Pos);
	// SYSCFG_EXTICR should already be enabled for PA0 as it is the reset value
	// Set bit in EXTI_IMR to enable interrupts in the external interrupt controller for the correct pin
	EXTI->IMR = (EXTI->IMR & ~EXTI_IMR_MR0_Msk) | (0x1 << EXTI_IMR_MR0_Pos);
	// Use EXTI_RTST and EXTI_FTSR to make interrupts occur on the falling edge
	//  Rising trigger disabled on reset
	EXTI->FTSR = (EXTI->FTSR & ~EXTI_FTSR_TR0_Msk) | (0x1 << EXTI_FTSR_TR0_Pos);
	// Use NVIC_EnableIRQ(param) to enable interrupts in the main interrupt controller
	//  Find correct param in STM32F407xx.h
	NVIC_EnableIRQ(EXTI0_IRQn);
	// The button will now call the interrupt controller of the same name
	NVIC_SetPriority(EXTI0_IRQn, 0);
}

// Push button interrupt controller
void EXTI0_IRQHandler(void){
	// Clear pending interrupt on line 0
	EXTI->PR = EXTI_PR_PR0;
	
	if((ourTick - debounce_counter) > (DEBOUNCE_TIME)) {
		debounce_counter = ourTick;
		
		switchMode();		
	}
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
