#include <KlemmUI/Math/MathHelpers.h>
#include <cmath>

bool Math::IsPointIn2DBox(Vector2f BoxA, Vector2f BoxB, Vector2f Point)
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

bool Math::NearlyEqual(float A, float B, float epsilon)
{
	return (std::fabs(A - B) < epsilon);
}
