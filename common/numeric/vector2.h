#pragma once

#include <cmath>
#include <vector>
#include <assert.h>

#include <Utility\iostream_binary.h>

namespace numeric
{
	template <class C> class Vector2;
	typedef Vector2<float>	Vector2f;
	typedef Vector2<double>	Vector2d;
	typedef Vector2<int>	Vector2i;
	typedef Vector2<short>	Vector2s;

	typedef Vector2f Point2D;
	typedef Vector2i Coord2D;

	typedef std::vector< Vector2f > Vector2fArray;
	typedef std::vector< Point2D > Point2DArray;
	typedef std::vector< Coord2D > Coord2DArray;

	template <class C>
	class Vector2 
	{	
	public:
		typedef C value_type;
		C x,y;

		inline static Vector2<C> Min(const Vector2<C> & r0, const Vector2<C> & r)
		{	return Vector2<C>(min(r0.x, r.x), min(r0.y, r.y));	}

		inline static Vector2<C> Max(const Vector2<C> & r0, const Vector2<C> & r)
		{	return Vector2<C>(max(r0.x, r.x), max(r0.y, r.y));	}

		inline static float Angle(const Vector2<C>& v0, const Vector2<C>& v1)
		{	return acos(v0.Dot(v1) / v0.L2Norm() / v1.L2Norm());	}

		// constructors
		inline Vector2() : x(0), y(0) {}
		inline explicit Vector2(const C & value) : x(value), y(value) {}
		inline Vector2(const C & _x, const C & _y) : x(_x), y(_y){}

		// unary operator
		inline Vector2<C> operator-() const { return Vector2<C>(-x, -y); }
		inline Vector2<C> & operator= (const Vector2<C> & r) { x = r.x; y = r.y; return *this; }

		// binary operator
		inline Vector2<C> operator+ (const Vector2<C> & r) const { return Vector2<C>(x + r.x, y + r.y); }
		inline Vector2<C> operator- (const Vector2<C> & r) const { return Vector2<C>(x - r.x, y - r.y); }
		inline Vector2<C> operator* (const C & r) const { return Vector2<C>(x * r, y * r); }
		inline friend Vector2<C> operator*(const C & l, const Vector2<C> & r)	{ return Vector2<C>(l * r.x, l * r.y);}
		inline Vector2<C> operator/ (const C & r) const { return Vector2<C>(x / r, y / r); }

		inline Vector2<C> & operator+= (const Vector2<C> & r) { return (*this) = (*this) + r; }
		inline Vector2<C> & operator-= (const Vector2<C> & r) { return (*this) = (*this) - r; }
		inline Vector2<C> & operator*= (const C & r) { return (*this) = (*this) * r; }
		inline Vector2<C> & operator/= (const C & r) { return (*this) = (*this) / r; }

		inline bool operator< (const Vector2<C> & r) const { return (x <  r.x) && (y <  r.y); }
		inline bool operator<=(const Vector2<C> & r) const { return (x <= r.x) && (y <= r.y); }
		inline bool operator> (const Vector2<C> & r) const { return (x >  r.x) && (y >  r.y); }
		inline bool operator>=(const Vector2<C> & r) const { return (x >= r.x) && (y >= r.y); }
		inline bool operator==(const Vector2<C> & r) const { return (x == r.x) && (y == r.y); }
		inline bool operator!=(const Vector2<C> & r) const { return (x != r.x) || (y != r.y); }

		// auxiliary functions
		inline C Dot(const Vector2<C> & r) const { return x * r.x + y * r.y; }
		inline C L1Norm() const { return fabs(x) + fabs(y); }
		inline C L2Norm() const { return sqrt(x * x + y * y); }
		inline C L2Norm2() const { return x*x+y*y; }
		inline C Distance(const Vector2<C> & r) const { return (*this-r).L2Norm(); }
		inline C Distance2(const Vector2<C> & r) const { return (*this-r).L2Norm2(); }
		inline Vector2<C> & Normalize() 
		{ 
			C norm = this->L2Norm();
			assert(norm > 1e-6);
			(*this) /= norm; 
			return *this;
		}

		// IO
		inline friend ostream& operator<< (ostream& out, const Vector2<C> & r) 
		{	
			out << r.x << " " << r.y;
			return out;
		}

		inline friend ostream_binary& operator<< (ostream_binary& out, const Vector2<C> & r) 
		{	
			out << r.x << r.y;
			return out;
		}

		template<class IStreamType>
		inline friend IStreamType& operator>> (IStreamType& in, Vector2<C> & r) 
		{	
			in >> r.x >> r.y;
			return in;
		}
	};

}