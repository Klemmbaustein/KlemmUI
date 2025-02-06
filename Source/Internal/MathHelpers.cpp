#include "MathHelpers.h"
#include <cmath>

bool kui::internal::math::IsPointIn2DBox(Vec2f BoxA, Vec2f BoxB, Vec2f Point)
{
	if (BoxA.X > BoxB.X)
	{
		return (BoxA.X >= Point.X && BoxA.Y >= Point.Y && BoxB.X <= Point.X&& BoxB.Y <= Point.Y);
	}
	else
	{
		return (BoxA.X <= Point.X && BoxA.Y <= Point.Y&& BoxB.X >= Point.X&& BoxB.Y >= Point.Y);
	}
}

bool kui::internal::math::NearlyEqual(float A, float B, float epsilon)
{
	return (std::fabs(A - B) < epsilon);
}
