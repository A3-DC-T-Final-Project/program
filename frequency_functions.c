#include "stm32f407xx.h"

#include "frequency_functions.h"




void initFrequencyPin(){
	// Enable clock A
	//RCC->AHB1ENR = (RCC->AHB1ENR & 0xFFFFFFFE) | 0x00000001;
	
	// Port E clock already enabled in digital functions.
	// Set alternate function MODE (01)
	// E0 
	GPIOE->MODER = (GPIOE->MODER & 0xFFFFFFFE) | 0x00000001;
	// Set TIM4 alternate function[high] (AF2 0010)
	GPIOE->AFR[0] = (GPIOE->AFR[0] & 0xFFFFFFFD) | 0x00000002;
	
	
	
	// Sets TIM4EN (bit 0)
	RCC->APB1ENR = (RCC->APB1ENR & ~RCC_APB1ENR_TIM4EN_Msk) | (0x1 << RCC_APB1ENR_TIM4EN_Pos);
	// Set ECE bit in TIM4_SMCR to enable clock mode 2
	TIM4->SMCR = (TIM4->SMCR & ~TIM_SMCR_ECE_Msk) | (0x1 << TIM_SMCR_ECE_Pos);
	// Set CEN bit in TIM4_CR1 to enable counter
	TIM4->CR1 = (TIM4->CR1 & ~TIM_CR1_CEN_Msk) | (0x1 << TIM_CR1_CEN_Pos);
	
	
	
	// Enable clock Timer 2
	/*RCC->APB1ENR = (RCC->APB1ENR & 0xFFFFFFFE) | 0x00000001;
	
	// Set alternate function MODE (01)
	// A15
	GPIOA->MODER = (GPIOA->MODER & 0x7FFFFFFF) | 0x80000000;
	// Set TIM2 alternate function[high] (AF1 0001)
	GPIOA->AFR[1] = (GPIOA->AFR[1] & 0xEFFFFFFF) | 0x10000000;
	
	// Set ECE bit in TIM2_SMCR to enable clock mode 2
	TIM2->SMCR = (TIM2->SMCR & ~TIM_SMCR_ECE_Msk) | (0x1 << TIM_SMCR_ECE_Pos);
	// Set CEN bit in TIM2_CR1 to enable counter
	TIM2->CR1 = (TIM2->CR1 & ~TIM_CR1_CEN_Msk) | (0x1 << TIM_CR1_CEN_Pos);*/	
}
