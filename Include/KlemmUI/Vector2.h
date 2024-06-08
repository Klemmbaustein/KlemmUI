#pragma once
#include <string>
#include <cstdint>

template<class T, class U>
concept Derived = std::is_base_of<U, T>::value;

class _VecBase2
{
};

/**
 * @brief
 * 2d vector class.
 * 
 * Represents a point in 2d space, usually a point on the screen.
 * 
 * See: Vector2ui, Vector2i, Vector2f
 */
template<typename T>
class Vector2 : _VecBase2
{
public:
	/// X (first) value.
	T X = 0;
	/// Y (second) value.
	T Y = 0;

	Vector2();
	/// Initializes X and Y to the value of XY.
	Vector2(T XY);
	/// Initializes this->X to X and this->Y to Y.
	Vector2(T X, T Y);

	// Copies the value of another vector2 with a different value type to this one.
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

	/// Returns the length (or magnitude) of this vector.
	float Length() const;
	/// Returns a normalized version of this vector.
	Vector2<T> Normalize() const;

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

	/// Returns a string representation of this vector.
	std::string ToString() const;

	/// Returns a vector with the minimum values of both A and B.
	static Vector2<T> Min(Vector2<T> A, Vector2<T> B);
	/// Returns a vector with the maximum values of both A and B.
	static Vector2<T> Max(Vector2<T> A, Vector2<T> B);
	/// Returns a vector clamped between Min and Max.
	Vector2<T> Clamp(Vector2<T> Min, Vector2<T> Max);
};

/// A Vector2 with two unsigned ints.
typedef Vector2<uint64_t> Vector2ui;
/// A Vector2 with two signed ints.
typedef Vector2<int64_t> Vector2i;
/// A Vector2 with two floats.
typedef Vector2<float> Vector2f;
