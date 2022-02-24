#include "array-queue.h"

#ifndef multimeter_functions_FILE
#define multimeter_functions_FILE


#define DC_MODE 0
#define AC_MODE 1
#define I_MODE 2
#define R_MODE 3

void initADC(void);
void initDAC(void);
void initPinSelect(void);
void waitForADCAndRead(void);
void OutputValue(void);
float map(float x, float in_min, float in_max, float out_min, float out_max);
void performFunction(void);
void ACVoltage(Queue* readings);
void switchMode(void);

extern uint32_t ADCconv;
extern int read_mode;

#endif
