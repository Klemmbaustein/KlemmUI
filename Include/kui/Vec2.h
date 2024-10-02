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
 * See: Vec2ui, Vec2i, Vec2f
 */
template<typename T>
class Vec2 : _VecBase2
{
public:
	/// X (first) value.
	T X = 0;
	/// Y (second) value.
	T Y = 0;

	Vec2();
	/// Initializes X and Y to the value of XY.
	Vec2(T XY);
	/// Initializes this->X to X and this->Y to Y.
	Vec2(T X, T Y);

	// Copies the value of another Vec2 with a different value type to this one.
	template<typename T2>
	Vec2(Vec2<T2> b)
	{
		X = (T)b.X;
		Y = (T)b.Y;
	}

	Vec2<T> operator+(Vec2<T> b) const;
	Vec2<T> operator-(Vec2<T> b) const;
	Vec2<T> operator*(Vec2<T> b) const;
	Vec2<T> operator/(Vec2<T> b) const;

	/// Returns the length (or magnitude) of this vector.
	float Length() const;
	/// Returns a normalized version of this vector.
	Vec2<T> Normalize() const;

	template<Derived<_VecBase2> T2>
	bool operator==(T2 a) const
	{
		return a.X == X && a.Y == Y;
	}
	bool operator==(float XY) const
	{
		return XY == X && XY == Y;
	}
	bool operator!=(float XY) const
	{
		return XY != X || XY != Y;
	}
	template<Derived<_VecBase2> T2>
	bool operator!=(T2 a) const
	{
		return a.X != X || a.Y != Y;
	}
	template<Derived<_VecBase2> T2>
	Vec2<T>& operator+=(T2 a)
	{
		X += a.X;
		Y += a.Y;
		return *this;
	}

	/// Returns a string representation of this vector.
	std::string ToString() const;

	/// Returns a vector with the minimum values of both A and B.
	static Vec2<T> Min(Vec2<T> A, Vec2<T> B);
	/// Returns a vector with the maximum values of both A and B.
	static Vec2<T> Max(Vec2<T> A, Vec2<T> B);
	/// Returns a vector clamped between Min and Max.
	Vec2<T> Clamp(Vec2<T> Min, Vec2<T> Max);
};

/// A Vec2 with two unsigned ints.
typedef Vec2<uint64_t> Vec2ui;
/// A Vec2 with two signed ints.
typedef Vec2<int64_t> Vec2i;
/// A Vec2 with two floats.
typedef Vec2<float> Vec2f;
