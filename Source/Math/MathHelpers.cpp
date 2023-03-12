#include <Math/MathHelpers.h>

bool Math::IsPointIn2DBox(Vector2f BoxA, Vector2f BoxB, Vector2f Point)
{
	bool InArea = false;
	if (BoxA.X > BoxB.X)
	{
		InArea = (BoxA.X > Point.X && BoxA.Y > Point.Y && BoxB.X < Point.X&& BoxB.Y < Point.Y);
	}
	else
	{
		InArea = (BoxA.X < Point.X&& BoxA.Y < Point.Y&& BoxB.X > Point.X&& BoxB.Y > Point.Y);
	}
	return InArea;
}

bool Math::NearlyEqual(float A, float B, float epsilon)
{
	return (fabs(A - B) < epsilon);
}
