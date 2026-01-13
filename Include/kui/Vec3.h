#pragma once
#include <string>
#include <cstdint>

namespace kui
{
	template<typename T>
	class Vec3
	{
	public:
		T X = 0;
		T Y = 0;
		T Z = 0;

		constexpr Vec3()
		{
		}

		constexpr Vec3(T XYZ)
		{
			X = XYZ;
			Y = XYZ;
			Z = XYZ;
		}

		constexpr Vec3(T X, T Y, T Z)
		{
			this->X = X;
			this->Y = Y;
			this->Z = Z;
		}

		template<typename T2>
		constexpr Vec3(Vec3<T2> b)
		{
			X = (T)b.X;
			Y = (T)b.Y;
			Z = (T)b.Z;
		}

		static Vec3 Lerp(Vec3 a, Vec3 b, float val);

		Vec3<T> operator+(Vec3<T> b) const;
		Vec3<T> operator-(Vec3<T> b) const;
		Vec3<T> operator*(Vec3<T> b) const;
		Vec3<T> operator/(Vec3<T> b) const;
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
		Vec3<T> Normalize();
		static Vec3 FromString(std::string str);
	};


	typedef Vec3<uint64_t> Vec3ui;
	typedef Vec3<int64_t> Vec3i;
	typedef Vec3<float> Vec3f;

}