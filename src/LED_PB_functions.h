#include "stm32f407xx.h"

#ifndef LED_PB_functions_FILE
#define LED_PB_functions_FILE

void initLEDs(void);
void initButton(void);
uint32_t ReadPushButton(void);
void LED_Turn_On(int colour);
void LED_Turn_Off(int colour);

#endif
