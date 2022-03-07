#include "stm32f407xx.h"

#include "digital_functions.h"

// Edge pins used: B(5 7 15) & E(3 4 5 6 7)
void initDigitalPins(){
	// Enable clock E&B
	RCC->AHB1ENR = (RCC->AHB1ENR & 0xFFFFFFFD) | 0x00000002;
	RCC->AHB1ENR = (RCC->AHB1ENR & 0xFFFFFFDF) | 0x00000020;
	
	// Change the MODE (01)
	GPIOB->MODER = (GPIOB->MODER & 0xBFFFBBFF) | 0x40004400;
	GPIOE->MODER = (GPIOE->MODER & 0xFFFFAABF) | 0x00005540;
	
	// Set PUPDR to PD (10)
	GPIOB->PUPDR = (GPIOB->PUPDR & 0x7FFF77FF) | 0x80008800;
	GPIOE->PUPDR = (GPIOE->PUPDR & 0xFFFF557F) | 0x0000AA80;
}

void setHigh(GPIO_TypeDef * port, int pin){
	port->BSRR = (port->BSRR & (0xFFFFFFFF - (0x1 << pin))) | (0x1 << pin);
}

void setLow(GPIO_TypeDef * port, int pin){
	port->BSRR = (port->BSRR & (0xFFFFFFFF - (0x1 << (pin + 16)))) | (0x1 << (pin + 16));
}
