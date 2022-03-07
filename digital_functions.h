#ifndef DIGITAL_FUNCTIONS_H
#define DIGITAL_FUNCTIONS_H

#include "stm32f407xx.h"

void initDigitalPins(void);
void setHigh(GPIO_TypeDef * port, int pin);
void setLow(GPIO_TypeDef * port, int pin);

#endif /* DIGITAL_FUNCTIONS_H */
