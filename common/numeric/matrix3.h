#pragma once

//#include <numeric\vector2.h>
//#include <numeric\vector3.h>
#include "vector2.h"
#include "vector3.h"

#include <cmath>
#include <iostream>
using namespace std;

#define USE_RAW_MEM		// use raw memory or valarray for matrix representation

#ifndef USE_RAW_MEM
#include <vector>
#endif

namespace numeric
{
	template <class C> class Matrix3;
	typedef Matrix3<float> Matrix3f;
	typedef Matrix3<double> Matrix3d;

	template <class C>
	class Matrix3 
	{	
	public:

#ifdef USE_RAW_MEM
		C v[9];
#else
		vector<C> v;
#endif

	public:
		typedef C value_type;

		inline friend ostream & operator<< (ostream & out, const Matrix3<C>& m)
		{ 
			for(int n = 0; n < 9; n++)	
				out << m.v[n] << " ";
			out << endl;
			return out;
		}

		inline friend ostream_binary & operator<< (ostream_binary& out, const Matrix3<C>& m)
		{
			for(int n = 0; n < 9; n++)	
				out << m.v[n];
			return out;
		}

		inline friend istream_binary & operator>> (istream_binary& in, Matrix3<C>& m)
		{
			for(int n = 0; n < 9; n++)	
				in >> m.v[n];
			return in;
		}

		inline friend istream & operator>> (istream & in, Matrix3<C>& m)
		{ 
			for(int n = 0; n < 9; n++) 
				in >> m.v[n];	
			return in;	
		}

		// constructors
	public:
		Matrix3()
		{
#ifdef USE_RAW_MEM
			v[0] = v[1] = v[2] = v[3] = v[4] = v[5] = v[6] = v[7] = v[8] = 0;
#else
			v = vector<C>(9, 0);
#endif			
		}

		// TODO: need to explicitly define copy constructor and assignment operator for raw memory?
#ifdef USE_RAW_MEM
		Matrix3(const Matrix3& m)
		{
			(*this) = m.v;
		}

		Matrix3<C>& operator= (const Matrix3& m)
		{
			if (this != &m)
				(*this) = m.v;
			return *this;
		}
#endif

		explicit Matrix3(const C* _v)
		{
#ifndef USE_RAW_MEM
			v = vector<C>(9);
#endif
			(*this) = _v;
		}

		// three rows correspond to the three points
		Matrix3(const Point2D& p0, const Point2D& p1, const Point2D& p2)
		{
#ifndef USE_RAW_MEM
			v = vector<C>(9);
#endif

			v[0] = p0.x;	v[1] = p0.y;	v[2] = 1;
			v[3] = p1.x;	v[4] = p1.y;	v[5] = 1;
			v[6] = p2.x;	v[7] = p2.y;	v[8] = 1;
		}

		// a column vector * a row vector
		Matrix3(const Vector3<C>& v1, const Vector3<C>& v2)
		{
#ifndef USE_RAW_MEM
			v = vector<C>(9);
#endif

			v[0] = v1.x * v2.x;	v[1] = v1.x * v2.y;	v[2] = v1.x * v2.z;
			v[3] = v1.y * v2.x;	v[4] = v1.y * v2.y;	v[5] = v1.y * v2.z;
			v[6] = v1.z * v2.x;	v[7] = v1.z * v2.y;	v[8] = v1.z * v2.z;
		}

		// three columns
		Matrix3(const Vector3<C>& c0, const Vector3<C>& c1, const Vector3<C>& c2)
		{
#ifndef USE_RAW_MEM
			v = vector<C>(9);
#endif

			SetCol(c0, c1, c2);
		}

		Matrix3<C>& operator= (const C _v[9])
		{			
			//for(int n = 0; n < 9; n++)	v[n] = _v[n];			
			// TODO: the second method is better?
			v[0] = _v[0]; v[1] = _v[1]; v[2] = _v[2];
			v[3] = _v[3]; v[4] = _v[4]; v[5] = _v[5];
			v[6] = _v[6]; v[7] = _v[7]; v[8] = _v[8];

			return *this;
		}

		void SetRow(const Vector3<C>& r0, const Vector3<C>& r1, const Vector3<C>& r2)
		{
			v[0] = r0.x; v[1] = r0.y; v[2] = r0.z;
			v[3] = r1.x; v[4] = r1.y; v[5] = r1.z;
			v[6] = r2.x; v[7] = r2.y; v[8] = r2.z;
		}

		void SetCol(const Vector3<C>& c0, const Vector3<C>& c1, const Vector3<C>& c2)
		{
			v[0] = c0.x; v[1] = c1.x; v[2] = c2.x;
			v[3] = c0.y; v[4] = c1.y; v[5] = c2.y;
			v[6] = c0.z; v[7] = c1.z; v[8] = c2.z;
		}

		// access
	public:
		C operator[](int n) const	{	return v[n];	}
		C& operator[](int n)		{	return v[n];	}
		C At(int row, int col) const	{	return v[3*row+col];	}
		C& At(int row, int col)			{	return v[3*row+col];	}

		C e00()	const	{	return v[0];	}
		C e01()	const	{	return v[1];	}
		C e02()	const	{	return v[2];	}
		C e10()	const	{	return v[3];	}
		C e11()	const	{	return v[4];	}
		C e12()	const	{	return v[5];	}
		C e20()	const	{	return v[6];	}
		C e21()	const	{	return v[7];	}
		C e22()	const	{	return v[8];	}

		Vector3<C> GetRow(int nRow) const
		{			
			assert((nRow >= 0) && (nRow < 3));
			int nIndex = 3*nRow;
			return Vector3<C>(v[nIndex], v[nIndex+1], v[nIndex+2]);
		}

		// arithmatic operations
	public:
		Vector3<C> operator* (const Vector3<C>& vec) const
		{
			return Vector3<C>(v[0]*vec.x + v[1]*vec.y + v[2]*vec.z
				,v[3]*vec.x + v[4]*vec.y + v[5]*vec.z
				,v[6]*vec.x + v[7]*vec.y + v[8]*vec.z);
		}

		// a row vector multiplies a matrix
		friend Vector3<C> operator*(const Vector3<C>& vec, const Matrix3<C>& m)
		{
			return Vector3<C>(vec.x*m.v[0] + vec.y*m.v[3] + vec.z*m.v[6]
			,vec.x*m.v[1] + vec.y*m.v[4] + vec.z*m.v[7]
			,vec.x*m.v[2] + vec.y*m.v[5] + vec.z*m.v[8]);
		}		

		Matrix3<C>& operator+= (const Matrix3<C>& mat)
		{ for(int n = 0; n < 9; n++)	v[n] += mat.v[n]; return *this; }

		Matrix3<C>& operator-= (const Matrix3<C>& mat)
		{ for(int n = 0; n < 9; n++)	v[n] -= mat.v[n]; return *this; }

		Matrix3<C>& operator*= (const Matrix3<C>& mat)
		{
			C p[9];

			p[0] = v[0]*mat.v[0] + v[1]*mat.v[3] + v[2]*mat.v[6];
			p[1] = v[0]*mat.v[1] + v[1]*mat.v[4] + v[2]*mat.v[7];
			p[2] = v[0]*mat.v[2] + v[1]*mat.v[5] + v[2]*mat.v[8];
			p[3] = v[3]*mat.v[0] + v[4]*mat.v[3] + v[5]*mat.v[6];
			p[4] = v[3]*mat.v[1] + v[4]*mat.v[4] + v[5]*mat.v[7];
			p[5] = v[3]*mat.v[2] + v[4]*mat.v[5] + v[5]*mat.v[8];
			p[6] = v[6]*mat.v[0] + v[7]*mat.v[3] + v[8]*mat.v[6];
			p[7] = v[6]*mat.v[1] + v[7]*mat.v[4] + v[8]*mat.v[7];
			p[8] = v[6]*mat.v[2] + v[7]*mat.v[5] + v[8]*mat.v[8];			

			v[0] = p[0]; v[1] = p[1]; v[2] = p[2];
			v[3] = p[3]; v[4] = p[4]; v[5] = p[5];
			v[6] = p[6]; v[7] = p[7]; v[8] = p[8];

			return *this;
		}

		friend Matrix3<C> operator+ (const Matrix3<C>& m1, const Matrix3<C>& m2)
		{	return Matrix3<C>(m1) += m2;	}

		friend Matrix3<C> operator- (const Matrix3<C>& m1, const Matrix3<C>& m2)
		{	return Matrix3<C>(m1) -= m2;	}

		friend Matrix3<C> operator* (const Matrix3<C>& m1, const Matrix3<C>& m2)
		{	return Matrix3<C>(m1) *= m2;	}

		// common matrix operations
	public:
		C L2Norm() const
		{ 		
			C s = 0.0;
			for(int n = 0; n < 9; n++) 
				s += v[n] * v[n];
			return sqrt(s);
		}

		C Trace() const { 	return v[0] + v[4] + v[8];	}

		C Det() const
		{
			return v[0]*v[4]*v[8] + v[1]*v[5]*v[6] + v[2]*v[3]*v[7]	
			- v[2]*v[4]*v[6] - v[0]*v[5]*v[7] - v[1]*v[3]*v[8];
		}

		C Distance(const Matrix3<C>& m) const
		{	return (*this - m).L2Norm();	}	
		
		void SetIdentity()
		{
			v[1] = v[2] = v[3] = v[5] = v[6] = v[7] = 0;
			v[0] = v[4] = v[8] = 1;
		}

		Matrix3<C>& Normalize()
		{
			C d = Det();
			for(int n = 0; n < 9; n++) v[n] /= d;
			return *this;
		}

		Matrix3<C>& Invert()
		{	
			C d = Det();

			C _v[9];
			_v[0] = v[4]*v[8] - v[5]*v[7];
			_v[1] = v[7]*v[2] - v[1]*v[8];
			_v[2] = v[1]*v[5] - v[4]*v[2];	
			_v[3] = v[5]*v[6] - v[3]*v[8];
			_v[4] = v[0]*v[8] - v[6]*v[2];
			_v[5] = v[3]*v[2] - v[0]*v[5];
			_v[6] = v[3]*v[7] - v[4]*v[6];
			_v[7] = v[6]*v[1] - v[0]*v[7];
			_v[8] = v[4]*v[0] - v[3]*v[1];

			for(int n = 0; n < 9; n++)	v[n] = _v[n] / d;
			return *this;
		}

		Matrix3<C>& Transpose()
		{
			std::swap(v[1], v[3]);
			std::swap(v[2], v[6]);
			std::swap(v[5], v[7]);
			return *this;
		}
	};
}