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
