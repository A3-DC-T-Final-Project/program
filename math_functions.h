#ifndef __MATH_FUNCTIONS_H
#define __MATH_FUNCTIONS_H

#include "stm32f407xx.h"

float map(float x, float in_min, float in_max, float out_min, float out_max);
float RMSAverage(double squares_total, int samples);
float MeanAverage(double total, int samples);
#endif
