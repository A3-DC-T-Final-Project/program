#ifndef TIMER_FUNCTIONS_H
#define TIMER_FUNCTIONS_H

#include "stm32f407xx.h"

void initTimer(void);
void SysTick_Handler(void);
void waitInterval(uint32_t waitTime);

// Variable used to keep count of counter ticks
extern uint32_t ourTick;

uint32_t getTicks(void);

#endif
