#include <kui/Vec2.h>

template<typename T>
Vec2<T>::Vec2()
{
}

template<typename T>
Vec2<T>::Vec2(T XY)
{
	X = XY;
	Y = XY;
}

template<typename T>
Vec2<T>::Vec2(T X, T Y)
{
	this->X = X;
	this->Y = Y;
}

template<typename T>
inline Vec2<T> Vec2<T>::operator+(Vec2<T> b)
{
	return Vec2<T>(X + b.X, Y + b.Y);
}

template<typename T>
inline Vec2<T> Vec2<T>::operator-(Vec2<T> b)
{
	return Vec2<T>(X - b.X, Y - b.Y);
}

template<typename T>
inline Vec2<T> Vec2<T>::operator*(Vec2<T> b)
{
	return Vec2<T>(X * b.X, Y * b.Y);
}

template<typename T>
inline Vec2<T> Vec2<T>::operator/(Vec2<T> b)
{
	return Vec2<T>(X / b.X, Y / b.Y);
}

template<typename T>
float Vec2<T>::Length() const
{
	return (float)(X*X+Y*Y);
}

template<typename T>
Vec2<T> Vec2<T>::Normalize() const
{
	T l = (T)Length();
	if (l > 0.01)
	{
		return Vec2<T>(X / l, Y / l);
	}
	return Vec2<T>();
}

template<typename T>
std::string Vec2<T>::ToString() const
{
	return std::to_string(X) + " " + std::to_string(Y);
}

template<typename T>
Vec2<T> Vec2<T>::Min(Vec2<T> A, Vec2<T> B)
{
	T NewX = std::min(A.X, B.X);
	T NewY = std::min(A.Y, B.Y);
	return Vec2<T>(NewX, NewY);
}

template<typename T>
Vec2<T> Vec2<T>::Max(Vec2<T> A, Vec2<T> B)
{
	T NewX = std::max(A.X, B.X);
	T NewY = std::max(A.Y, B.Y);
	return Vec2<T>(NewX, NewY);
}

template<typename T>
Vec2<T> Vec2<T>::Clamp(Vec2<T> Min, Vec2<T> Max)
{
	T NewX = std::min(Max.X, std::max(Min.X, X));
	T NewY = std::min(Max.Y, std::max(Min.Y, Y));
	return Vec2<T>(NewX, NewY);
}

template class Vec2<uint64_t>;
template class Vec2<int64_t>;
template class Vec2<int32_t>;
template class Vec2<float>;
template class Vec2<double>;