#pragma once

#include <math.h>
#include <assert.h>

namespace geometry
{
	// TODO: use SSE
	// quaternion = [w, x, y, z]: w + x * i + y * j + z * k
	// it represents a rotation of angle theta around axis u, q = [cos(theta/2), u * sin(theta/2)]
	class Quaternion
	{
		typedef float value_type;
		value_type w, x, y, z;
		value_type Magnitude() const { return sqrt(w*w+x*x+y*y+z*z); }
		Quaternion& Normalize() { value_type m = Magnitude(); w /= m; x /= m; y /= m; z /= m; return *this; }

	public:
		Quaternion() : w(1), x(0), y(0), z(0) {} // the identity rotation

		// assume axis u is already normalized
		template<class Vector3>
		Quaternion(value_type theta, Vector3 u)
		{
			assert(fabs(u.x*u.x+u.y*u.y+u.z*u.z - 1) < 1e-6);
			w = cos(theta/2);
			value_type s2 = sin(theta/2);
			x = u.x * s2;
			y = u.y * s2;
			z = u.z * s2;
		}

		Quaternion(value_type _w, value_type _x, value_type _y, value_type _z, bool normalize) : w(_w), x(_x), y(_y), z(_z) 
		{
			if (normalize)
				Normalize();
			else
				assert(fabs(Magnitude()-1.0) < 1e-6);
		}

		const value_type& W() const { return w; }
		const value_type& X() const { return x; }
		const value_type& Y() const { return y; }
		const value_type& Z() const { return z; }
		
		//Quaternion Conjugate() const { return Quaternion(w, -x, -y, -z); }

		//friend Quaternion operator+(const Quaternion& q1, const Quaternion& q2)
		//{ return Quaternion(q1.w+q2.w, q1.x+q2.x, q1.y+q2.y, q1.z+q2.z); }

		friend Quaternion operator*(const Quaternion& q1, const Quaternion& q2)
		{
			//return Quaternion(q1.w*q2.w - q1.v.Dot(q2.v), q1.w*q2.v + q2.w*q1.v + q1.v.Cross(q2.v));
			value_type w, x, y, z;
			w = q1.w*q2.w - (q1.x*q2.x+q1.y*q2.y+q1.z*q2.z);
			x = q1.w*q2.x + q2.w*q1.x + q1.y * q2.z - q1.z * q2.y;
			y = q1.w*q2.y + q2.w*q1.y + q1.z * q2.x - q1.x * q2.z;
			z = q1.w*q2.z + q2.w*q1.z + q1.x * q2.y - q1.y * q2.x;

			// need normalization as the norm may no longer be 1 due to numerical error
			return Quaternion(w, x, y, z, true);
		}

		value_type Dot(const Quaternion& q) const { return w*q.w + x*q.x + y*q.y + z*q.z; }

		// http://ggt.sourceforge.net/html/group__Interp.html
		static Quaternion Slerp(float u, const Quaternion& from, const Quaternion& to, bool adjustSign = true)		
		{
			const Quaternion& p = from;
			Quaternion q;
			float cosom = from.Dot(to);

			if (adjustSign && (cosom < 0.0))
			{
				cosom = -cosom;
				q.w = -to.w; q.x = -to.x; q.y = -to.y; q.z = -to.z;
			}
			else q = to;

			float sclp, sclq;
			if ( (1.0 - cosom) > 1e-3 )
			{
				float omega = acos(cosom);
				float sinom = sin(omega);
				sclp = sin((1.0f - u) * omega) / sinom;
				sclq = sin(u * omega) / sinom;
			}
			else
			{
				sclp = 1.0f - u;
				sclq = u;
			}

			// TODO: does it need normalization here?
			return Quaternion(sclp*p.w+sclq*q.w, sclp*p.x+sclq*q.x, sclp*p.y+sclq*q.y, sclp*p.z+sclq*q.z, true);
		}
	};

	// Rodrigues's rotation formula
	// http://mathworld.wolfram.com/RotationFormula.html
	// http://mathworld.wolfram.com/EulerAngles.html
	// http://www.theory.org/software/qfa/writeup/node12.html
	// http://www.euclideanspace.com/maths/algebra/matrix/orthogonal/rotation/index.htm		
	// axis (x, y, z) should be normalized, cos and sin of angle is given
	inline void set_rodrigues_rotation(float x, float y, float z, float c, float s, float* r)
	{
		assert(fabs(x*x+y*y+z*z-1.0) < 1e-6);
		assert(fabs(c*c+s*s-1.0) < 1e-6);

		r[0] = c + (1-c)*x*x;		r[1] = (1-c)*x*y - s*z;		r[2] = (1-c)*x*z + s*y;
		r[3] = (1-c)*x*y + s*z;		r[4] = c + (1-c)*y*y;		r[5] = (1-c)*y*z - s*x;
		r[6] = (1-c)*x*z - s*y;		r[7] = (1-c)*y*z + s*x;		r[8] = c + (1-c)*z*z;
	}

	// If quaternion = [W, X, Y, Z], then its corresponding matrix is
	// |1-2Y^2-2Z^2  2XY-2ZW      2XZ+2YW  |
	// | 2XY+2ZW    1-2X^2-2Z^2   2YZ-2XW  |
	// | 2XZ-2YW     2YZ+2XW    1-2X^2-2Y^2|
	// http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/index.htm
	inline void quaternion_to_rotation(float w, float x, float y, float z, float* r)
	{
		assert(fabs(sqrt(w*w+x*x+y*y+z*z)-1.0f) < 1e-6);
		float xx(2*x*x), yy(2*y*y), zz(2*z*z);
		float xy(2*x*y), xz(2*x*z), yz(2*y*z), xw(2*x*w), yw(2*y*w), zw(2*z*w);
		r[0] = 1 - yy - zz;		r[1] = xy - zw;			r[2] = xz + yw;
		r[3] = xy + zw;			r[4] = 1 - xx - zz;		r[5] = yz - xw;
		r[6] = xz - yw;			r[7] = yz + xw;			r[8] = 1 - xx - yy;
	}

	// according to http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/index.htm
	// TODO: see a complete implementation in
	// http://www.geometrictools.com/LibMathematics/Algebra/Wm5Quaternion.inl
	inline void rotation_to_quaternion(const float* r, float& w, float& x, float& y, float& z)
	{
		float trace = r[0] + r[4] + r[8];
		if (trace > 0.0f)
		{
			float s = 0.5f / sqrtf(trace+1.0f);
			w = 0.25f / s;
			x = (r[7]-r[5]) * s;
			y = (r[2]-r[6]) * s;
			z = (r[3]-r[1]) * s;
		}
		else 
		{
			if ( (r[0] > r[4]) && (r[0] > r[8]) ) 
			{
				float s = 2.0f * sqrtf( 1.0f + r[0] - r[4] - r[8]);
				w = (r[7] - r[5]) / s;
				x = 0.25f * s;
				y = (r[1] + r[3]) / s;
				z = (r[2] + r[6]) / s;
			}
			else if (r[4] > r[8])
			{
				float s = 2.0f * sqrtf( 1.0f + r[4] - r[0] - r[8]);
				w = (r[2] - r[6]) / s;
				x = (r[1] + r[3]) / s;
				y = 0.25f * s;
				z = (r[5] + r[7]) / s;
			} 
			else 
			{
				float s = 2.0f * sqrtf( 1.0f + r[8] - r[0] - r[4]);
				w = (r[3] - r[1]) / s;
				x = (r[2] + r[6]) / s;
				y = (r[5] + r[7]) / s;
				z = 0.25f * s;
			}
		}
	}

	template<class T>
	void quaternion_to_HomRotationMatrix(T w, T x, T y, T z, T m[16])
    {
        const T len2 = w*w + x*x + y*y + z*z;

        if(len2 != T(0))
        {
            const T len = sqrt( len2 );

            const T uw = w/len, ux = x/len, uy = y/len, uz = z/len;

            const T twoux = ux+ux, twouy = uy+uy, twouz = uz+uz;

            const T twouwux = uw * twoux;
            const T twouwuy = uw * twouy;
            const T twouwuz = uw * twouz;

            const T twouxux = ux * twoux;
            const T twouxuy = ux * twouy;
            const T twouxuz = ux * twouz;
            const T twouyuy = uy * twouy;
            const T twouyuz = uy * twouz;
            const T twouzuz = uz * twouz;

            /* the matrix m will be set to:

               [ 1-twouyuy-twouzuz,   twouxuy-twouwuz,   twouxuz+twouwuy,   0 ]
               |   twouxuy+twouwuz, 1-twouxux-twouzuz,   twouyuz-twouwux,   0 |
               |   twouxuz-twouwuy,   twouyuz+twouwux, 1-twouxux-twouyuy,   0 |
               [                 0,                 0,                 0,   1 ]
            */

            T *p = m;

            p[ 0] = T(1) - twouyuy - twouzuz;
            p[ 1] =        twouxuy + twouwuz;
            p[ 2] =        twouxuz - twouwuy;

            p[ 4] =        twouxuy - twouwuz;
            p[ 5] = T(1) - twouxux - twouzuz;
            p[ 6] =        twouyuz + twouwux;

            p[ 8] =        twouxuz + twouwuy;
            p[ 9] =        twouyuz - twouwux;
            p[10] = T(1) - twouxux - twouyuy;

            p[ 3] = p[ 7] = p[11] = p[12] = p[13] = p[14] = T(0);

            p[15] = T(1);
        }
        else
        {
            m[ 0] = T(1);  m[ 4] = T(0);  m[ 8] = T(0);  m[12] = T(0);
            m[ 1] = T(0);  m[ 5] = T(1);  m[ 9] = T(0);  m[13] = T(0);
            m[ 2] = T(0);  m[ 6] = T(0);  m[10] = T(1);  m[14] = T(0);
            m[ 3] = T(0);  m[ 7] = T(0);  m[11] = T(0);  m[15] = T(1);
        }
    }
}