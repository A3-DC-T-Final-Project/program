#ifndef MULTIMETER_FUNCTIONS_H
#define MULTIMETER_FUNCTIONS_H

#include "stm32f407xx.h"

#define DC_MODE 0
#define AC_MODE 1
#define I_MODE 2
#define R_MODE 3

void initADC(void);
void initPinSelect(void);
void waitForADCAndRead(void);
void OutputValue(void);
float DCVoltage(void);
float ACVoltage(void);
void switchMode(void);

extern uint32_t ADCconv;
extern int read_mode;

#endif
