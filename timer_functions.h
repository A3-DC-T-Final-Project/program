#include "stm32f407xx.h"

#ifndef timer_functions_FILE
#define timer_functions_FILE

void SysTick_Handler(void);
void waitInterval(uint32_t waitTime);
void initTimer(void);

extern uint32_t ourTick;
extern int led_cond;
uint32_t getTicks(void);

#endif
