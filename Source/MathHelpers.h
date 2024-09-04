#pragma once
#include <KlemmUI/Vector2.h>
#include <string>
#include <algorithm>
namespace Math
{
	inline constexpr const float PI = 3.14159265359f;
	bool IsPointIn2DBox(Vector2f BoxA, Vector2f BoxB, Vector2f Point);
	bool NearlyEqual(float A, float B, float epsilon = 0.005f);
}