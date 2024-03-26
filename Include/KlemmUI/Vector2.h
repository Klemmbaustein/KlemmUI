#pragma once
#include <string>
#include <cstdint>

template<class T, class U>
concept Derived = std::is_base_of<U, T>::value;

class _VecBase2
{
};

template<typename T>
class Vector2 : _VecBase2
{
public:
	T X = 0;
	T Y = 0;

	Vector2();
	Vector2(T XY);
	Vector2(T X, T Y);
	template<typename T2>
	Vector2(Vector2<T2> b)
	{
		X = (T)b.X;
		Y = (T)b.Y;
	}

	Vector2<T> operator+(Vector2<T> b);
	Vector2<T> operator-(Vector2<T> b);
	Vector2<T> operator*(Vector2<T> b);
	Vector2<T> operator/(Vector2<T> b);

	float Length();
	Vector2<T> Normalize();

	template<Derived<_VecBase2> T2> bool operator==(T2 a)
	{
		return a.X == X && a.Y == Y;
	}
	bool operator==(float XY)
	{
		return XY == X && XY == Y;
	}
	bool operator!=(float XY)
	{
		return XY != X || XY != Y;
	}
	template<Derived<_VecBase2> T2> bool operator!=(T2 a)
	{
		return a.X != X || a.Y != Y;
	}
	template<Derived<_VecBase2> T2>
	Vector2<T>& operator+=(T2 a)
	{
		X += a.X;
		Y += a.Y;
		return *this;
	}

	std::string ToString();

	Vector2<T> Clamp(Vector2<T> Min, Vector2<T> Max);
};

typedef Vector2<uint64_t> Vector2ui;
typedef Vector2<int64_t> Vector2i;
typedef Vector2<float> Vector2f;
