#pragma once
#include <kui/Vec2.h>
#include <string>
#include <algorithm>
namespace kui::internal::math
{
	bool IsPointIn2DBox(Vec2f BoxA, Vec2f BoxB, Vec2f Point);
	bool NearlyEqual(float A, float B, float epsilon = 0.005f);
}