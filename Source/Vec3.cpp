#include <kui/Vec3.h>
#include <vector>
#include <cmath>

using namespace kui;

template<typename T>
Vec3<T>::Vec3()
{
}

template<typename T>
Vec3<T>::Vec3(T XYZ)
{
	X = XYZ;
	Y = XYZ;
	Z = XYZ;
}

template<typename T>
Vec3<T>::Vec3(T X, T Y, T Z)
{
	this->X = X;
	this->Y = Y;
	this->Z = Z;
}

template<typename T>
Vec3<T> Vec3<T>::Lerp(Vec3 a, Vec3 b, float val)
{
	return Vec3(
		(T)std::lerp((float)a.X, (float)b.X, val), 
		(T)std::lerp((float)a.Y, (float)b.Y, val),
		(T)std::lerp((float)a.Z, (float)b.Z, val));
}

template<typename T>
inline Vec3<T> Vec3<T>::operator+(Vec3<T> b) const
{
	return Vec3<T>(X + b.X, Y + b.Y, Z + b.Z);
}

template<typename T>
inline Vec3<T> Vec3<T>::operator-(Vec3<T> b) const
{
	return Vec3<T>(X - b.X, Y - b.Y, Z - b.Z);
}

template<typename T>
inline Vec3<T> Vec3<T>::operator*(Vec3<T> b) const
{
	return Vec3<T>(X * b.X, Y * b.Y, Z * b.Z);
}

template<typename T>
inline Vec3<T> Vec3<T>::operator/(Vec3<T> b) const
{
	return Vec3<T>(X / b.X, Y / b.Y, Z / b.Z);
}

template<typename T>
float Vec3<T>::Length()
{
	return (float)(X*X+Y*Y+Z*Z);
}

template<typename T>
Vec3<T> Vec3<T>::Normalize()
{
	T l = (T)Length();
	if (l > 0.01)
	{
		return Vec3<T>(X / l, Y / l, Z / l);
	}
	return Vec3<T>();
}

template<typename T>
Vec3<T> Vec3<T>::FromString(std::string str)
{
	if (str.size() == 0)
	{
		return Vec3();
	}
	std::string myString = str;
	std::stringstream iss(myString);

	T number;
	std::vector<T> NumberVec;
	while (iss >> number)
		NumberVec.push_back(number);
	if (NumberVec.size() >= 3)
		return Vec3(NumberVec.at(0), NumberVec.at(1), NumberVec.at(2));
	return Vec3();
}

template class Vec3<uint64_t>;
template class Vec3<int64_t>;
template class Vec3<int32_t>;
template class Vec3<float>;