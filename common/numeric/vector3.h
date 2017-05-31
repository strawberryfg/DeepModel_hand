#pragma once

#include <cmath>
#include <vector>
#include <assert.h>

#include <Utility\iostream_binary.h>

namespace numeric
{	
	template <class C> class Vector3;
	typedef Vector3<float> Vector3f;
	typedef Vector3<double> Vector3d;
	typedef Vector3<int> Vector3i;
	typedef Vector3<short>	Vector3s;

	typedef Vector3f Point3D;
	typedef Vector3i Coord3D;

	typedef std::vector< Vector3f > Vector3fArray;
	typedef std::vector< Point3D > Point3DArray;
	typedef std::vector< Coord3D > Coord3DArray;

	template <class C>
	class Vector3 
	{	
	public:
		typedef C value_type;
		C x,y,z;

		inline static Vector3<C> Min(const Vector3<C> & r0, const Vector3<C> & r)
		{	return Vector3<C>(min(r0.x, r.x), min(r0.y, r.y), min(r0.z, r.z));	}

		inline static Vector3<C> Max(const Vector3<C> & r0, const Vector3<C> & r)
		{	return Vector3<C>(max(r0.x, r.x), max(r0.y, r.y), max(r0.z, r.z));	}

		inline static float Angle(const Vector3<C>& v0, const Vector3<C>& v1)
		{	
			C c = Cos(v0, v1);
			if (c > 1) c = 1;
			if (c < -1) c = -1;
			return acos(c);
		}

		inline static float Cos(const Vector3<C>& v0, const Vector3<C>& v1)
		{
			return v0.Dot(v1) / sqrt(v0.L2Norm2()*v1.L2Norm2());
		}

		// constructors
		inline Vector3() : x(0), y(0), z(0) {}
		inline explicit Vector3(const C& value) : x(value), y(value), z(value) {}
		inline Vector3(const C & a, const C & b, const C & c) : x(a), y(b), z(c) {}

		operator float* () { return &x; }
		operator const float* () const { return &x; }

		// unary operator
		inline Vector3<C> operator-() const { return Vector3<C>(-x, -y, -z); }
		inline Vector3<C> & operator= (const Vector3<C> & r) { x = r.x; y = r.y; z = r.z; return *this; }
		inline Vector3<C> & operator= (const C& r) { x = y = z = r; return *this; }

		// binary operator
		inline Vector3<C> operator+ (const Vector3<C> & r) const { return Vector3<C>(x + r.x, y + r.y, z + r.z); }
		inline Vector3<C> operator- (const Vector3<C> & r) const { return Vector3<C>(x - r.x, y - r.y, z - r.z); }
		inline Vector3<C> operator* (const Vector3<C> & r) const { return Vector3<C>(x * r.x, y * r.y, z * r.z); }	
		inline Vector3<C> operator* (const C & r) const { return Vector3<C>(x * r, y * r, z * r); }	
		inline friend Vector3<C> operator*(const C & l, const Vector3<C> & r) { return Vector3<C>(l * r.x, l * r.y, l * r.z); }
		inline Vector3<C> operator/ (const Vector3<C> & r) const { return Vector3<C>(x / r.x, y / r.y, z / r.z); }
		inline Vector3<C> operator/ (const C & r) const { return Vector3<C>(x / r, y / r, z / r); }		

		inline Vector3<C> & operator+= (const Vector3<C> & r) { return (*this) = (*this) + r; }
		inline Vector3<C> & operator-= (const Vector3<C> & r) { return (*this) = (*this) - r; }
		inline Vector3<C> & operator*= (const C & r) { return (*this) = (*this) * r; }
		inline Vector3<C> & operator*= (const Vector3<C> & r) { return (*this) = (*this) * r; }
		inline Vector3<C> & operator/= (const C & r) { return (*this) = (*this) / r; }
		inline Vector3<C> & operator/= (const Vector3<C> & r) { return (*this) = (*this) / r; }

		inline bool operator< (const Vector3<C> & r) const { return (x <  r.x) && (y <  r.y) && (z <  r.z); }
		inline bool operator<=(const Vector3<C> & r) const { return (x <= r.x) && (y <= r.y) && (z <= r.z); }
		inline bool operator> (const Vector3<C> & r) const { return (x >  r.x) && (y >  r.y) && (z >  r.z); }
		inline bool operator>=(const Vector3<C> & r) const { return (x >= r.x) && (y >= r.y) && (z >= r.z); }
		inline bool operator==(const Vector3<C> & r) const { return (x == r.x) && (y == r.y) && (z == r.z); }
		inline bool operator!=(const Vector3<C> & r) const { return (x != r.x) || (y != r.y) || (z != r.z); }

		// auxiliary functions
		inline C Dot(const Vector3<C> & r) const { return x * r.x + y * r.y + z * r.z; }
		inline Vector3<C> Cross(const Vector3<C> & r) const { return Vector3<C>(y * r.z - z * r.y, z * r.x - x * r.z, x * r.y - y * r.x );	}
		inline C L1Norm() const { return fabs(x) + fabs(y) + fabs(z); }
		inline C L2Norm() const { return sqrt(Dot(*this)); }
		inline C L2Norm2() const { return Dot(*this); }
		inline C Distance(const Vector3<C> & r) const { return (*this-r).L2Norm(); }
		inline C Distance2(const Vector3<C> & r) const { return (*this-r).L2Norm2(); }
		inline Vector3<C> & Normalize() 
		{ 
			C norm = this->L2Norm(); 
			assert(norm > 1e-6);
			(*this) /= norm; 
			return *this;
		}

		// IO
		inline friend ostream& operator<< (ostream& out, const Vector3<C> & r) 
		{	
			out << r.x << " " << r.y << " " << r.z;
			return out;
		}

		inline friend ostream_binary& operator<< (ostream_binary& out, const Vector3<C> & r) 
		{	
			out << r.x << r.y << r.z;
			return out;
		}

		template<class IStreamType>
		inline friend IStreamType& operator>> (IStreamType& in, Vector3<C> & r) 
		{	
			in >> r.x >> r.y >> r.z;
			return in;
		}
	};
}