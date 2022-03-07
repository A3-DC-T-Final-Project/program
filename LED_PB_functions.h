#ifndef LED_PB_FUNCTIONS_H
#define LED_PB_FUNCTIONS_H

#include "stm32f407xx.h"

#define DEBOUNCE_TIME 200

void initLEDs(void);
void initButton(void);
void EXTI0_IRQHandler(void);
void LED_Turn_On(int colour);
void LED_Turn_Off(int colour);

#endif
