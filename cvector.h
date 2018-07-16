#pragma once

#include <math.h>

class cVector
{
public:
	// прямоугольные координаты для упрощения расчётов
	float x;
	float y;
	float z;

	/*constructor*/ cVector(float x_ = 0, float y_ = 0, float z_ = 0) : x(x_), y(y_), z(z_) {}
	/*destructor*/ ~cVector() {}

	float length2()
	{
		return sqrt(x * x + y * y);
	}

	float length3()
	{
		return sqrt(x * x + y * y);
	}

	cVector operator - (const cVector &op) const
	{
		return cVector(x - op.x, y - op.y, z - op.z);
	}

	cVector operator + (const cVector &op) const
	{
		return cVector(x + op.x, y + op.y, z + op.z);
	}

	cVector operator * (float factor) const
	{
		return cVector(x * factor, y * factor, z * factor);
	}
};
