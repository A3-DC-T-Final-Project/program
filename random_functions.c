#include "stm32f407xx.h"
#include "random_functions.h"

void initRandom(){
	// Enable clock for 
	RCC->AHB2ENR = (RCC->AHB2ENR & ~ 0xFFFFFFBF) | 0x40;
	RNG->CR = (RNG->CR & ~RNG_CR_RNGEN_Msk) | (0x1 << RNG_CR_RNGEN_Pos);
}

/* TODO::
 *  - Check random number has been generated from flag in status register (SR)
 *  - Read random number from data register (DR)
 */
