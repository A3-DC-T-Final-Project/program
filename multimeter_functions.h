#include "array-queue.h"

#ifndef multimeter_functions_FILE
#define multimeter_functions_FILE

void initADC(void);
void initDAC(void);
void initPinSelect(void);
void waitForADCAndRead(void);
void OutputValue(void);
float map(float x, float in_min, float in_max, float out_min, float out_max);
void performFunction(void);
void ACVoltage(Queue* readings);
void switchMode(void);

#endif
