#include "stm32f407xx.h"

#ifndef timer_functions_FILE
#define timer_functions_FILE

void SysTick_Handler(void);
void waitInterval(uint32_t waitTime);
void initTimer(void);

// Variable used to keep count of counter ticks
extern uint32_t ourTick;

uint32_t getTicks(void);

#endif
