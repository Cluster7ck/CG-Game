#ifndef Utility_len_h
#define Utility_len_h

#define _USE_MATH_DEFINES
#include <random>

float map(float s, float a1, float a2, float b1, float b2);
float randomFloat(float min, float max);
void replaceFilename(const char* Filename, const char* replacer, char* destination);
//catch 0 case
float cosinus(int degree);
float sinus(int degree);

#endif
