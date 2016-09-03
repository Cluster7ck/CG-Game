#ifndef Utility_len_h
#define Utility_len_h

#define _USE_MATH_DEFINES
#include <random>
#include "vector.h"
#include "Matrix.h"

template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}
Vector rotateVectorByAxis(Vector RotVector, Vector Axis, float degree);
float map(float s, float a1, float a2, float b1, float b2);
float randomFloat(float min, float max);
void replaceFilename(const char* Filename, const char* replacer, char* destination);



#endif
