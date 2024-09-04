#include <KlemmUI/Vector3.h>
#include <vector>
#include <sstream>
#include <cmath>

template<typename T>
Vector3<T>::Vector3()
{
}

template<typename T>
Vector3<T>::Vector3(T XYZ)
{
	X = XYZ;
	Y = XYZ;
	Z = XYZ;
}

template<typename T>
Vector3<T>::Vector3(T X, T Y, T Z)
{
	this->X = X;
	this->Y = Y;
	this->Z = Z;
}

template<typename T>
Vector3<T> Vector3<T>::Lerp(Vector3 a, Vector3 b, float val)
{
	return Vector3(
		(T)std::lerp((float)a.X, (float)b.X, val), 
		(T)std::lerp((float)a.Y, (float)b.Y, val),
		(T)std::lerp((float)a.Z, (float)b.Z, val));
}

template<typename T>
inline Vector3<T> Vector3<T>::operator+(Vector3<T> b)
{
	return Vector3<T>(X + b.X, Y + b.Y, Z + b.Z);
}

template<typename T>
inline Vector3<T> Vector3<T>::operator-(Vector3<T> b)
{
	return Vector3<T>(X - b.X, Y - b.Y, Z - b.Z);
}

template<typename T>
inline Vector3<T> Vector3<T>::operator*(Vector3<T> b)
{
	return Vector3<T>(X * b.X, Y * b.Y, Z * b.Z);
}

template<typename T>
inline Vector3<T> Vector3<T>::operator/(Vector3<T> b)
{
	return Vector3<T>(X / b.X, Y / b.Y, Z / b.Z);
}

template<typename T>
float Vector3<T>::Length()
{
	return (float)(X*X+Y*Y+Z*Z);
}

template<typename T>
Vector3<T> Vector3<T>::Normalize()
{
	T l = (T)Length();
	if (l > 0.01)
	{
		return Vector3<T>(X / l, Y / l, Z / l);
	}
	return Vector3<T>();
}

template<typename T>
Vector3<T> Vector3<T>::FromString(std::string str)
{
	if (str.size() == 0)
	{
		return Vector3();
	}
	std::string myString = str;
	std::stringstream iss(myString);

	T number;
	std::vector<T> NumberVec;
	while (iss >> number)
		NumberVec.push_back(number);
	if (NumberVec.size() >= 3)
		return Vector3(NumberVec.at(0), NumberVec.at(1), NumberVec.at(2));
	return Vector3();
}

template class Vector3<uint64_t>;
template class Vector3<int64_t>;
template class Vector3<int32_t>;
template class Vector3<float>;