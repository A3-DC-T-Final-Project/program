#include "stm32f407xx.h"

#ifndef LED_PB_functions_FILE
#define LED_PB_functions_FILE

#define DEBOUNCE_TIME 200

void initLEDs(void);
void initButton(void);
void EXTI0_IRQHandler(void);
uint32_t readPushButton(void);
void LED_Turn_On(int colour);
void LED_Turn_Off(int colour);

#endif
