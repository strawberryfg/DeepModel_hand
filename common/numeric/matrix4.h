#pragma once

#include "vector2.h"
#include "vector3.h"
#include "vector4.h"

#include <cmath>
#include <iostream>
using namespace std;

#define USE_RAW_MEM		// use raw memory or valarray for matrix representation

#ifndef USE_RAW_MEM
#include <vector>
#endif

namespace numeric
{
	template <class C> class Matrix4;
	typedef Matrix4<float> Matrix4f;
	typedef Matrix4<double> Matrix4d;


	enum matrix_operation
	{
		rot_x = 0, rot_y, rot_z, trans_x, trans_y, trans_z, Const_Matr
	};

	template <class C>
	class Matrix4 
	{	
	public:

#ifdef USE_RAW_MEM
		C v[16];
#else
		vector<C> v;
#endif

	public:
		typedef C value_type;

		inline friend ostream & operator<< (ostream & out, const Matrix4<C>& m)
		{ 
			for(int n = 0; n < 16; n++)	
				out << m.v[n] << " ";
			out << endl;
			return out;
		}

		inline friend ostream_binary & operator<< (ostream_binary& out, const Matrix4<C>& m)
		{
			for(int n = 0; n < 16; n++)	
				out << m.v[n];
			return out;
		}

		inline friend istream_binary & operator>> (istream_binary& in, Matrix4<C>& m)
		{
			for(int n = 0; n < 16; n++)	
				in >> m.v[n];
			return in;
		}

		inline friend istream & operator>> (istream & in, Matrix4<C>& m)
		{ 
			for(int n = 0; n < 16; n++) 
				in >> m.v[n];	
			return in;	
		}

		// constructors
	public:
		Matrix4()
		{
#ifndef USE_RAW_MEM
			v = vector<C>(16);
#endif
			SetIdentity();
		}

		//second-order gradient
		Matrix4(const matrix_operation &opt, value_type value) {    // value is in radian if matrix_operation is rotation			
			SetZero();			
			if (opt == rot_x)     //Rotate along axis X
			{
				v[5] = -cos(value);
				v[6] = sin(value);
				v[9] = -sin(value);
				v[10] = -cos(value);				
			}
			else if (opt == rot_y)  //Rotate along axis Y
			{
				v[0] = -cos(value);
				v[2] = sin(value);
				v[8] = -sin(value);
				v[10] = -cos(value);
			}
			else if (opt == rot_z)  //Rotate along axis Z
			{
				v[0] = -cos(value);
				v[1] = sin(value);
				v[4] = -sin(value);
				v[5] = -cos(value);
			}
			else                     //Translate along axis X, Y or Z
			{
				//do nothing 
			}
		}

//
		Matrix4(const matrix_operation &opt, value_type value, bool is_gradient) {    // value is in radian if matrix_operation is rotation
			if (is_gradient)
				SetZero();
			else
				SetIdentity();

			if (opt == rot_x)     //Rotate along axis X
			{
				v[0] = (!is_gradient) ? value_type(1)  :  value_type(0);
				v[5] = (!is_gradient) ? cos(value)     :  -sin(value);
				v[6] = (!is_gradient) ? -sin(value)    :  -cos(value);
				v[9] = (!is_gradient) ? sin(value)     :  cos(value);
				v[10] = (!is_gradient) ? cos(value)    :  -sin(value);
				v[15] = (!is_gradient) ? value_type(1) :  value_type(0);
			}
			else if (opt == rot_y)  //Rotate along axis Y
			{
				v[0] = (!is_gradient) ? cos(value)     :  -sin(value);
				v[2] = (!is_gradient) ? -sin(value)    :  -cos(value);
				v[5] = (!is_gradient) ? value_type(1)  :  value_type(0);
				v[8] = (!is_gradient) ? sin(value)     :  cos(value);
				v[10] = (!is_gradient) ? cos(value)    :  -sin(value);
				v[15] = (!is_gradient) ? value_type(1) :  value_type(0);
			}
			else if (opt == rot_z)  //Rotate along axis Z
			{
				v[0] = (!is_gradient) ? cos(value)     :  -sin(value);
				v[1] = (!is_gradient) ? -sin(value)    :  -cos(value);
				v[4] = (!is_gradient) ? sin(value)     :  cos(value);
				v[5] = (!is_gradient) ? cos(value)     :  -sin(value);
				v[10] = (!is_gradient) ? value_type(1) :  value_type(0);
				v[15] = (!is_gradient) ? value_type(1) :  value_type(0);
			}
			else                     //Translate along axis X, Y or Z
			{
				v[(opt - trans_x) * 4 + 3] = (!is_gradient) ? value : value_type(1);
			}
		}


		// Post mult this matrix by a homogeneous rotation matrix along X axis
		Matrix4& RMult_RotateX(const value_type theta)
		{
			*this = (*this) * Matrix4(rot_x, theta, false);
			return *this;
		}

		// Post mult this matrix by a homogeneous rotation matrix along Y axis
		Matrix4& RMult_RotateY(const value_type theta)
		{
			*this = (*this) * Matrix4(rot_y, theta, false);
			return *this;
		}

		// Post mult this matrix by a homogeneous rotation matrix along Z axis
		Matrix4& RMult_RotateZ(const value_type theta)
		{
			*this = (*this) * Matrix4(rot_z, theta, false);
			return *this;
		}

		Matrix4& RMult_Rotate(const value_type roll, const value_type pitch, const value_type yaw)
		{
			(*this).RMult_RotateZ(roll).RMult_RotateX(pitch).RMult_RotateY(yaw);
			return *this;
		}

		Matrix4& RMult_TranslateXYZ(const value_type xdelta, const value_type ydelta, const value_type zdelta)
		{
			*this = (*this) * Matrix4f(matrix_operation::trans_x, xdelta, false);
			*this = (*this) * Matrix4f(matrix_operation::trans_y, ydelta, false);
			*this = (*this) * Matrix4f(matrix_operation::trans_z, zdelta, false);
			return *this;
		}


		// TODO: need to explicitly define copy constructor and assignment operator for raw memory?
#ifdef USE_RAW_MEM
		Matrix4(const Matrix4& m)
		{
			(*this) = m.v;
		}

		Matrix4<C>& operator= (const Matrix4& m)
		{
			if (this != &m)
				(*this) = m.v;
			return *this;
		}
#endif

		explicit Matrix4(const C* _v)
		{
#ifndef USE_RAW_MEM
			v = vector<C>(16);
#endif
			(*this) = _v;
		}

		// three rows correspond to the three points
		Matrix4(const Point3D& p0, const Point3D& p1, const Point3D& p2)
		{
#ifndef USE_RAW_MEM
			v = vector<C>(16);
#endif

			v[0] = p0.x[0];	v[1] = p0.x[1];	v[2] = p0.x[2];	 v[3] = 1;
			v[4] = p1.x[0];	v[5] = p1.x[1];	v[6] = p1.x[2];	 v[7] = 1;
			v[8] = p2.x[0];	v[9] = p2.x[1];	v[10] = p2.x[2]; v[11] = 1;
			v[12] = 1;		v[13] = 1;		v[14] = 1;		 v[15] = 1;
		}

		// a column vector * a row vector
		Matrix4(const Vector4<C>& v1, const Vector4<C>& v2)
		{
#ifndef USE_RAW_MEM
			v = vector<C>(16);
#endif

			v[0] = v1.x[0] * v2.x[0];		v[1] = v1.x[0] * v2.x[1];		v[2] = v1.x[0] * v2.x[2];		v[3] = v1.x[0] * v2.x[3];
			v[4] = v1.x[1] * v2.x[0];		v[5] = v1.x[1] * v2.x[1];		v[6] = v1.x[1] * v2.x[2];		v[7] = v1.x[1] * v2.x[3];
			v[8] = v1.x[2] * v2.x[0];		v[9] = v1.x[2] * v2.x[1];		v[10] = v1.x[2] * v2.x[2];	    v[11] = v1.x[2] * v2.x[3];
			v[12] = v1.x[3] * v2.x[0];	    v[13] = v1.x[3] * v2.x[1];	    v[14] = v1.x[3] * v2.x[2];	    v[15] = v1.x[3] * v2.x[3];
		}

		// three columns
		Matrix4(const Vector4<C>& c0, const Vector4<C>& c1, const Vector4<C>& c2, const Vector4<C>& c3)
		{
#ifndef USE_RAW_MEM
			v = vector<C>(16);
#endif

			SetCol(c0, c1, c2, c3);
		}

		Matrix4<C>& operator= (const C _v[16])
		{			
			//for(int n = 0; n < 16; n++)	v[n] = _v[n];			
			// TODO: the second method is better?
			v[0] = _v[0]; 	v[1] = _v[1]; 	v[2] = _v[2]; 	v[3] = _v[3];
			v[4] = _v[4]; 	v[5] = _v[5]; 	v[6] = _v[6]; 	v[7] = _v[7];
			v[8] = _v[8]; 	v[9] = _v[9]; 	v[10] = _v[10]; v[11] = _v[11];
			v[12] = _v[12]; v[13] = _v[13]; v[14] = _v[14]; v[15] = _v[15];
			return *this;
		}

		void SetRow(const Vector4<C>& r0, const Vector4<C>& r1, const Vector4<C>& r2, const Vector4<C>& r3)
		{
			v[0] = r0.x[0]; 	v[1] = r0.x[1]; 	v[2] = r0.x[2]; 	v[3] = r0.x[3];
			v[4] = r1.x[0]; 	v[5] = r1.x[1]; 	v[6] = r1.x[2]; 	v[7] = r1.x[3];
			v[8] = r2.x[0]; 	v[9] = r2.x[1]; 	v[10] = r2.x[2]; 	v[11] = r2.x[3];
			v[12] = r3.x[0]; 	v[13] = r3.x[1]; 	v[14] = r3.x[2]; 	v[15] = r3.x[3];
		}

		void SetCol(const Vector4<C>& c0, const Vector4<C>& c1, const Vector4<C>& c2, const Vector4<C>& c3)
		{
			v[0] = c0.x[0]; 	v[1] = c1.x[0]; 	v[2] = c2.x[0]; 	v[3] = c3.x[0];
			v[4] = c0.x[1]; 	v[5] = c1.x[1]; 	v[6] = c2.x[1]; 	v[7] = c3.x[1];
			v[8] = c0.x[2]; 	v[9] = c1.x[2]; 	v[10] = c2.x[2]; 	v[11] = c3.x[2];
			v[12] = c0.x[3]; 	v[13] = c1.x[3]; 	v[14] = c2.x[3]; 	v[15] = c3.x[3];
		}

		// access
	public:
		C operator[](int n) const	{	return v[n];	}
		C& operator[](int n)		{	return v[n];	}
		C At(int row, int col) const	{	return v[4 * row + col];	}
		C& At(int row, int col)			{	return v[4 * row + col];	}

		C e00()	const	{	return v[0];	}
		C e01()	const	{	return v[1];	}
		C e02()	const	{	return v[2];	}
		C e03()	const	{	return v[3];	}
		C e10()	const	{	return v[4];	}
		C e11()	const	{	return v[5];	}
		C e12()	const	{	return v[6];	}
		C e13()	const	{	return v[7];	}
		C e20()	const	{	return v[8];	}
		C e21()	const	{	return v[9];	}
		C e22()	const	{	return v[10];	}
		C e23()	const	{	return v[11];	}
		C e30()	const	{	return v[12];	}
		C e31()	const	{	return v[13];	}
		C e32()	const	{	return v[14];	}
		C e33()	const	{	return v[15];	}

		Vector4<C> GetRow(int nRow) const
		{			
			assert((nRow >= 0) && (nRow < 4));
			int nIndex = 4 * nRow;
			return Vector4<C>(v[nIndex], v[nIndex+1], v[nIndex+2], v[nIndex+3]);
		}

		// arithmatic operations
	public:
		Vector4<C> operator* (const Vector4<C>& vec) const
		{
			return Vector4<C>(v[0]  * vec.x[0] + v[1]  * vec.x[1] + v[2]  * vec.x[2] + v[3]  * vec.x[3]
				             ,v[4]  * vec.x[0] + v[5]  * vec.x[1] + v[6]  * vec.x[2] + v[7]  * vec.x[3]
				             ,v[8]  * vec.x[0] + v[9]  * vec.x[1] + v[10] * vec.x[2] + v[11] * vec.x[3]
				             ,v[12] * vec.x[0] + v[13] * vec.x[1] + v[14] * vec.x[2] + v[15] * vec.x[3]);
		}

		// a row vector multiplies a matrix
		friend Vector4<C> operator*(const Vector4<C>& vec, const Matrix4<C>& m)
		{
			return Vector4<C>(vec.x[0] * m.v[0] + vec.x[1] * m.v[4] + vec.x[2] * m.v[8]  + vec.x[3] * m.v[12]
			                 ,vec.x[0] * m.v[1] + vec.x[1] * m.v[5] + vec.x[2] * m.v[9]  + vec.x[3] * m.v[13]
			                 ,vec.x[0] * m.v[2] + vec.x[1] * m.v[6] + vec.x[2] * m.v[10] + vec.x[3] * m.v[14]
			                 ,vec.x[0] * m.v[3] + vec.x[1] * m.v[7] + vec.x[2] * m.v[11] + vec.x[3] * m.v[15]);
		}		

		Matrix4<C>& operator+= (const Matrix4<C>& mat)
		{ for(int n = 0; n < 16; n++)	v[n] += mat.v[n]; return *this; }

		Matrix4<C>& operator-= (const Matrix4<C>& mat)
		{ for(int n = 0; n < 16; n++)	v[n] -= mat.v[n]; return *this; }

		Matrix4<C>& operator*= (const Matrix4<C>& mat)
		{
			C p[16];

			p[0] = v[0] * mat.v[0] + v[1] * mat.v[4] + v[2] * mat.v[8]+ v[3] * mat.v[12];
			p[1] = v[0] * mat.v[1] + v[1] * mat.v[5] + v[2] * mat.v[9]+ v[3] * mat.v[13];
			p[2] = v[0] * mat.v[2] + v[1] * mat.v[6] + v[2] * mat.v[10]+ v[3] * mat.v[14];
			p[3] = v[0] * mat.v[3] + v[1] * mat.v[7] + v[2] * mat.v[11]+ v[3] * mat.v[15];
			p[4] = v[4] * mat.v[0] + v[5] * mat.v[4] + v[6] * mat.v[8]+ v[7] * mat.v[12];
			p[5] = v[4] * mat.v[1] + v[5] * mat.v[5] + v[6] * mat.v[9]+ v[7] * mat.v[13];
			p[6] = v[4] * mat.v[2] + v[5] * mat.v[6] + v[6] * mat.v[10]+ v[7] * mat.v[14];
			p[7] = v[4] * mat.v[3] + v[5] * mat.v[7] + v[6] * mat.v[11]+ v[7] * mat.v[15];
			p[8] = v[8] * mat.v[0] + v[9] * mat.v[4] + v[10] * mat.v[8]+ v[11] * mat.v[12];
			p[9] = v[8] * mat.v[1] + v[9] * mat.v[5] + v[10] * mat.v[9]+ v[11] * mat.v[13];
			p[10] = v[8] * mat.v[2] + v[9] * mat.v[6] + v[10] * mat.v[10]+ v[11] * mat.v[14];
			p[11] = v[8] * mat.v[3] + v[9] * mat.v[7] + v[10] * mat.v[11]+ v[11] * mat.v[15];
			p[12] = v[12] * mat.v[0] + v[13] * mat.v[4] + v[14] * mat.v[8]+ v[15] * mat.v[12];
			p[13] = v[12] * mat.v[1] + v[13] * mat.v[5] + v[14] * mat.v[9]+ v[15] * mat.v[13];
			p[14] = v[12] * mat.v[2] + v[13] * mat.v[6] + v[14] * mat.v[10]+ v[15] * mat.v[14];
			p[15] = v[12] * mat.v[3] + v[13] * mat.v[7] + v[14] * mat.v[11]+ v[15] * mat.v[15];

			v[0] = p[0];	v[1] = p[1];	v[2] = p[2];	v[3] = p[3];
			v[4] = p[4];	v[5] = p[5];	v[6] = p[6];	v[7] = p[7];
			v[8] = p[8];	v[9] = p[9];	v[10] = p[10];	v[11] = p[11];
			v[12] = p[12];	v[13] = p[13];	v[14] = p[14];	v[15] = p[15];

			return *this;
		}

		friend Matrix4<C> operator+ (const Matrix4<C>& m1, const Matrix4<C>& m2)
		{	return Matrix4<C>(m1) += m2;	}

		friend Matrix4<C> operator- (const Matrix4<C>& m1, const Matrix4<C>& m2)
		{	return Matrix4<C>(m1) -= m2;	}

		friend Matrix4<C> operator* (const Matrix4<C>& m1, const Matrix4<C>& m2)
		{	return Matrix4<C>(m1) *= m2;	}

		// common matrix operations
	public:
		C L2Norm() const
		{ 		
			C s = 0.0;
			for(int n = 0; n < 16; n++) 
				s += v[n] * v[n];
			return sqrt(s);
		}

		C Trace() const { 	return v[0] + v[5] + v[10] + v[15];	}


		C Distance(const Matrix4<C>& m) const
		{	return (*this - m).L2Norm();	}	
		
		void SetIdentity()
		{
			v[1] = v[2] = v[3] = v[4] = v[6] = v[7] = v[8] = v[9] = v[11] = v[12] = v[13] = v[14] = 0;
			v[0] = v[5] = v[10] = v[15] = 1;
		}

		void SetZero()
		{
			for (int i = 0; i < 16; i++) v[i] = 0;
		}

		template<class Point3D>
		Point3D Translation() const { return Point3D(v[3], v[7], v[11]); }
	};
}