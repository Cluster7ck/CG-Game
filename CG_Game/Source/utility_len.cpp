#include "..\Header\utility_len.h"

float map(float s, float a1, float a2, float b1, float b2)
{
	return b1 + (s - a1)*(b2 - b1) / (a2 - a1);
}

float randomFloat(float min, float max)
{
	float r = (float)rand() / (float)RAND_MAX;
	return min + r * (max - min);
}

void replaceFilename(const char* Filename, const char* replacer, char* destination) {
	char charPointer;
	strcpy(destination, Filename);
	//Last occurence of slash
	char* backslashPointer = strrchr(destination, '/');
	//copy material filename behind slash
	strcpy(backslashPointer + 1, replacer);
}

Vector rotateVectorByAxis(Vector RotVector, Vector Axis, float degree) {
	Matrix rotM;
	rotM.rotationAxis(Axis, degree);
	return rotM * RotVector;
}