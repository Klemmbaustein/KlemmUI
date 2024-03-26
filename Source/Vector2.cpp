#include <KlemmUI/Vector2.h>

template<typename T>
Vector2<T>::Vector2()
{
}

template<typename T>
Vector2<T>::Vector2(T XY)
{
	X = XY;
	Y = XY;
}

template<typename T>
Vector2<T>::Vector2(T X, T Y)
{
	this->X = X;
	this->Y = Y;
}

template<typename T>
inline Vector2<T> Vector2<T>::operator+(Vector2<T> b)
{
	return Vector2<T>(X + b.X, Y + b.Y);
}

template<typename T>
inline Vector2<T> Vector2<T>::operator-(Vector2<T> b)
{
	return Vector2<T>(X - b.X, Y - b.Y);
}

template<typename T>
inline Vector2<T> Vector2<T>::operator*(Vector2<T> b)
{
	return Vector2<T>(X * b.X, Y * b.Y);
}

template<typename T>
inline Vector2<T> Vector2<T>::operator/(Vector2<T> b)
{
	return Vector2<T>(X / b.X, Y / b.Y);
}

template<typename T>
float Vector2<T>::Length()
{
	return (float)(X*X+Y*Y);
}

template<typename T>
Vector2<T> Vector2<T>::Normalize()
{
	T l = (T)Length();
	if (l > 0.01)
	{
		return Vector2<T>(X / l, Y / l);
	}
	return Vector2<T>();
}

template<typename T>
std::string Vector2<T>::ToString()
{
	return std::to_string(X) + " " + std::to_string(Y);
}

template<typename T>
Vector2<T> Vector2<T>::Clamp(Vector2<T> Min, Vector2<T> Max)
{
	T NewX = std::min(Max.X, std::max(Min.X, X));
	T NewY = std::min(Max.Y, std::max(Min.Y, Y));
	return Vector2<T>(NewX, NewY);
}

template class Vector2<uint64_t>;
template class Vector2<int64_t>;
template class Vector2<int32_t>;
template class Vector2<float>;