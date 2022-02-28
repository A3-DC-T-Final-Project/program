#include <math.h>

#include "math_functions.h"

// Map function relates one set of numbers to another set
float map(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float RMSAverage(double squares_total, int samples){
	return (float) sqrt(squares_total / samples);
}

float MeanAverage(double total, int samples){
	return (float) (total / samples);
}
