#include <Math/Vector2.h>

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

template class Vector2<uint64_t>;
template class Vector2<int64_t>;
template class Vector2<int32_t>;
template class Vector2<double>;
template class Vector2<float>;