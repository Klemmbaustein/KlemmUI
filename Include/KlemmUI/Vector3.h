#pragma once
#include <string>
#include <cstdint>

template<typename T>
class Vector3
{
public:
	T X = 0;
	T Y = 0;
	T Z = 0;

	Vector3();
	Vector3(T XYZ);
	Vector3(T X, T Y, T Z);
	template<typename T2>
	Vector3(Vector3<T2> b)
	{
		X = (T)b.X;
		Y = (T)b.Y;
		Z = (T)b.Z;
	}

	static Vector3 Lerp(Vector3 a, Vector3 b, float val);

	Vector3<T> operator+(Vector3<T> b);
	Vector3<T> operator-(Vector3<T> b);
	Vector3<T> operator*(Vector3<T> b);
	Vector3<T> operator/(Vector3<T> b);
	template<class T2> bool operator==(T2 a) const
	{
		return a.X == X && a.Y == Y && a.Z == Z;
	}
	bool operator==(float XYZ) const
	{
		return XYZ == X && XYZ == Y && XYZ == Z;
	}
	template<class T2> bool operator!=(T2 a) const
	{
		return a.X != X || a.Y != Y || a.Z != Z;
	}
	bool operator!=(float XYZ) const
	{
		return XYZ != X || XYZ != Y || XYZ != Z;
	}

	template<class T2> bool operator<(T2 a) const
	{
		return X * 0xffff + Y * 0xff + Z < a.X * 0xffff + a.Y * 0xff + a.Z;
	}
	float Length();
	Vector3<T> Normalize();
	static Vector3 FromString(std::string str);
};


typedef Vector3<uint64_t> Vector3ui;
typedef Vector3<int64_t> Vector3i;
typedef Vector3<float> Vector3f;