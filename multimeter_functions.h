#ifndef MULTIMETER_FUNCTIONS_H
#define MULTIMETER_FUNCTIONS_H

#include "stm32f407xx.h"

#define DC_MODE 0
#define AC_MODE 1
#define I_MODE 2
#define R_MODE 3

#define ADC_MIN 0
#define ADC_MAX 4095

#define V_100M_RANGE 0
#define V_1_RANGE 1
#define V_5_RANGE 2
#define V_10_RANGE 3

void initADC(void);
void initPinSelect(void);
void waitForADCAndRead(void);
void OutputValue(void);
float DCVoltage(void);
float ACVoltage(void);
void switchMode(void);

void changeVoltageRange(int range);

extern uint32_t ADCconv;
extern int read_mode;
extern int voltage_range;

#endif
