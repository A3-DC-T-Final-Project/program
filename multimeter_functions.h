#ifndef MULTIMETER_FUNCTIONS_H
#define MULTIMETER_FUNCTIONS_H

#include <stdbool.h>

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

#define I_10M_RANGE 0
#define I_25M_RANGE 1
#define I_50M_RANGE 2
#define I_100M_RANGE 3

void initADC(void);
void initPinSelect(void);
void waitForADCAndRead(void);
void OutputValue(void);
float DCVoltage(bool * conversion);
float ACVoltage(bool * conversion);
float Current(bool * conversion);
void switchMode(void);

void changeVoltageRange(int range);
void checkIfInRange(float value, bool * conversion);

void changeCurrentRange(int range);
void checkIfInRangeCurrent(float value, bool * conversion);

extern uint32_t ADCconv;
extern int read_mode;
extern int voltage_range;
extern float conversion_upper_bound;
extern float conversion_lower_bound;
extern float range_max;
extern float range_min;

extern int current_range;

#endif
