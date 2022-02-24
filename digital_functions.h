#ifndef _DIGITAL_FUNCTIONS_H_
#define _DIGITAL_FUNCTIONS_H_

#include "stm32f407xx.h"

void initDigitalPins(void);
void setHigh(GPIO_TypeDef * port, int pin);
void setLow(GPIO_TypeDef * port, int pin);

#endif /* _DIGITAL_FUNCTIONS_H_ */
