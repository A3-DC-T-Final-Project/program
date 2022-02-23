#include "stm32f407xx.h"
#include "timer_functions.h"

// Global Variables
// Int var for tick counter
uint32_t static volatile ticks = 0;

void SysTick_Handler(void) {
	ticks++;
}

// Waits for some ticks 
void waitInterval(uint32_t waitTime){
	uint32_t whenStarted = ticks;
	while (ticks - whenStarted < waitTime);
}

void initTimer(void){
	// Enables clock for required registers in GPIOE
	RCC->AHB1ENR = (RCC->AHB1ENR & ~RCC_AHB1ENR_GPIOEEN_Msk) | (0x1 << RCC_AHB1ENR_GPIOEEN_Pos);
	// Sets the MODE of E7 alternate function mode 0b10
	GPIOE->MODER = (GPIOE->MODER & ~GPIO_MODER_MODER7_Msk) | (0x2 << GPIO_MODER_MODER7_Pos);
	// Sets the low AFR register for GPIOE pin 7(low between 0 and 7) to mux select 1 for TIM1_ETR
	// 0x18 shifts selection 1 to position word 7 (4 bits wide)
	GPIOE->AFR[0] = (GPIOE->AFR[0] & ~GPIO_AFRL_AFRL7) | (GPIO_AFRL_AFSEL1_1 << 0x17);
	
	// Sets TIM1EN (bit 0)
	RCC->APB2ENR = (RCC->APB2ENR & ~RCC_APB2ENR_TIM1EN_Msk) | (0x1 << RCC_APB2ENR_TIM1EN_Pos);
	// Set ECE bit in TIM1_SMCR to enable clock mode 2
	TIM1->SMCR = (TIM1->SMCR & ~TIM_SMCR_ECE_Msk) | (0x1 << TIM_SMCR_ECE_Pos);
	// Set CEN bit in TIM1_CR1 to enable counter
	TIM1->CR1 = (TIM1->CR1 & ~TIM_CR1_CEN_Msk) | (0x1 << TIM_CR1_CEN_Pos);	
}

uint32_t getTicks(){
	return ticks;
}
