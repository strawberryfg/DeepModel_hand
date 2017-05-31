#pragma once

#include <cmath>
#include <vector>
#include <assert.h>

#include <Utility\iostream_binary.h>

namespace numeric
{
	template <class C> class Vector4;
	typedef Vector4<float> Vector4f;
	typedef Vector4<double> Vector4d;
	typedef Vector4<int> Vector4i;
	typedef Vector4<short>	Vector4s;
	
	template <class C>
	class Vector4
	{	
	public:
		typedef C value_type;
		C x[4];
		
		// constructors
		
		inline Vector4(){ x[0] = x[1] = x[2] = x[3] = value_type(0); }
		inline C operator [](int u) {
				return x[u];
			}
		inline explicit Vector4(const C & value)  { x[0] = x[1] = x[2] = x[3] = value; }
		inline Vector4(const C & _x, const C & _y, const C & _z, const C & _w) { x[0] = _x; x[1] = _y; x[2] = _z; x[3] = _w; }

		// unary operator
		inline Vector4<C> operator-() const { return Vector4<C>(-x[0], -x[1], -x[2], -x[3]); }
		inline Vector4<C> & operator= (const Vector4<C> & r) { x[0] = r.x[0]; x[1] = r.x[1]; x[2] = r.x[2]; x[3] = r.x[3]; return *this; }

		// binary operator
		inline Vector4<C> operator+ (const Vector4<C> & r) const { return Vector4<C>(x[0] + r.x[0], x[1] + r.x[1], x[2] + r.x[2], x[3] + r.x[3]); }
		inline Vector4<C> operator- (const Vector4<C> & r) const { return Vector4<C>(x[0] - r.x[0], x[1] - r.x[1], x[2] - r.x[2], x[3] - r.x[3]); }
		inline Vector4<C> operator* (const C & r) const { return Vector4<C>(x[0] * r, x[1] * r, x[2] * r, x[3] * r); }
		inline friend Vector4<C> operator*(const C & l, const Vector4<C> & r)	{ return Vector4<C>(l * r.x[0], l * r.x[1], l * r.x[2], l * r.x[3]);}
		inline Vector4<C> operator/ (const C & r) const { return Vector4<C>(x[0] / r, x[1] / r, x[2] / r, x[3] / r); }

		inline Vector4<C> & operator+= (const Vector4<C> & r) { return (*this) = (*this) + r; }
		inline Vector4<C> & operator-= (const Vector4<C> & r) { return (*this) = (*this) - r; }
		inline Vector4<C> & operator*= (const C & r) { return (*this) = (*this) * r; }
		inline Vector4<C> & operator/= (const C & r) { return (*this) = (*this) / r; }

		inline bool operator< (const Vector4<C> & r) const { return (x[0] <  r.x[0]) && (x[1] <  r.x[1]) && (x[2] <  r.x[2]) && (x[3] <  r.x[3]); }
		inline bool operator<=(const Vector4<C> & r) const { return (x[0] <= r.x[0]) && (x[1] <= r.x[1]) && (x[2] <= r.x[2]) && (x[3] <= r.x[3]); }
		inline bool operator> (const Vector4<C> & r) const { return (x[0] >  r.x[0]) && (x[1] >  r.x[1]) && (x[2] >  r.x[2]) && (x[3] >  r.x[3]); }
		inline bool operator>=(const Vector4<C> & r) const { return (x[0] >= r.x[0]) && (x[1] >= r.x[1]) && (x[2] >= r.x[2]) && (x[3] >= r.x[3]); }
		inline bool operator==(const Vector4<C> & r) const { return (x[0] == r.x[0]) && (x[1] == r.x[1]) && (x[2] == r.x[2]) && (x[3] == r.x[3]); }
		inline bool operator!=(const Vector4<C> & r) const { return (x[0] != r.x[0]) || (x[1] != r.x[1]) || (x[2] != r.x[2]) || (x[3] != r.x[3]); }

		// auxiliary functions
		inline C Dot(const Vector4<C> & r) const { return x[0] * r.x[0] + x[1] * r.x[1] + x[2] * r.x[2] + x[3] * r.x[3]; }
		inline C L1Norm() const { return fabs(x[0]) + fabs(x[1]) + fabs(x[2]) + fabs(x[3]); }
		inline C L2Norm() const { return sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2] + x[3] * x[3]); }
		inline C L2Norm2() const { return x[0] * x[0] + x[1] * x[1] + x[2] * x[2] + x[3] * x[3]; }
		inline C Distance(const Vector4<C> & r) const { return (*this-r).L2Norm(); }
		inline C Distance2(const Vector4<C> & r) const { return (*this-r).L2Norm2(); }
		inline Vector4<C> & Normalize() 
		{ 
			C norm = this->L2Norm();
			assert(norm > 1e-6);
			(*this) /= norm; 
			return *this;
		}

		// IO
		inline friend ostream& operator<< (ostream& out, const Vector4<C> & r) 
		{	
			out << r.x[0] << " " << r.x[1] << " " << r.x[2] << " " << r.x[3];
			return out;
		}

		inline friend ostream_binary& operator<< (ostream_binary& out, const Vector4<C> & r) 
		{	
			out << r.x[0] << r.x[1] << r.x[2] << r.x[3];
			return out;
		}

		template<class IStreamType>
		inline friend IStreamType& operator>> (IStreamType& in, Vector4<C> & r) 
		{	
			in >> r.x[0] >> r.x[1] >> r.x[2] >> r.x[3];
			return in;
		}
	};
}