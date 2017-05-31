#pragma once

#include <geometry\Quaternion.h>

#include <numeric\vector3.h>
#include <numeric\matrix3.h>
using namespace numeric;

#include <assert.h>

namespace geometry
{
	class Rotation3 : public Matrix3f
	{
		// quaternion should be normalized, but this is not imposed in the class Quaternion
		// so this constructor is only for internal use
		explicit Rotation3(const Quaternion& q) { SetQuaternion(q); }

		// construction and access
	public:
		Rotation3() { SetIdentity(); }
		explicit Rotation3(const Matrix3f& m) : Matrix3f(m) { Normalize(); }
		
		Rotation3(const Vector3f& dirFrom, const Vector3f& dirTo) 
		{
			float c = Vector3f::Cos(dirFrom, dirTo);
			if (abs(c) > .999999f)
				SetIdentity();
			else
			{
				Point3D axis = (dirFrom.Cross(dirTo)).Normalize();
				set_rodrigues_rotation(axis.x, axis.y, axis.z, c, sqrt(1.0f-c*c), &v[0]);
			}
		}
		
		// axis assumed normalized, angle in radian
		//*
		Rotation3(const Vector3f& axis, float angle) {	SetAxisAngle(axis, angle);	}
		void SetAxisAngle(const Vector3f& axis, float angle)
		{
			set_rodrigues_rotation(axis.x, axis.y, axis.z, cos(angle), sin(angle), &v[0]);
		}

		void GetAxisAngle(Vector3f& axis, float& angle, bool* degenerate = 0) const
		{
			axis.x = (e21() - e12());
			axis.y = (e02() - e20());
			axis.z = (e10() - e01());
			if (axis.L2Norm() > 1e-3)
			{
				float c = (Trace() - 1) / 2;
				if (c > 1) c = 1;
				if (c < -1) c = -1;
				angle = acos(c);
				axis.Normalize();

				if (degenerate)
					*degenerate = false;
			}
			else
			{
				axis = Vector3f(0, 0, 1);
				angle = 0;

				// TODO: it is possible that axis/angle cannot represent some degenerate rotation, need to deal with here
				if (degenerate)
					*degenerate = true;
			}
		}
		//*/
		
		void SetQuaternion(const Quaternion& q)
		{
			quaternion_to_rotation(q.W(), q.X(), q.Y(), q.Z(), &v[0]);
		}

		//*
		// http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/index.htm
		Quaternion GetQuaternion() const
		{
			float w, x, y, z;

			// either method is fine
			rotation_to_quaternion(&v[0], w, x, y, z);
			
			/*
			float trace = Trace();
			if (trace > 0)
			{
				float s = 0.5f / sqrtf(trace+1.0f);
				w = 0.25f / s;
				x = (e21()-e12()) * s;
				y = (e02()-e20()) * s;
				z = (e10()-e01()) * s;
			}
			else 
			{
				if ( (e00() > e11()) && (e00() > e22()) ) 
				{
					float s = 2.0f * sqrtf( 1.0f + e00() - e11() - e22());
					w = (e21() - e12()) / s;
					x = 0.25f * s;
					y = (e01() + e10()) / s;
					z = (e02() + e20()) / s;
				}
				else if (e11() > e22()) 
				{
					float s = 2.0f * sqrtf( 1.0f + e11() - e00() - e22());
					w = (e02() - e20()) / s;
					x = (e01() + e10()) / s;
					y = 0.25f * s;
					z = (e12() + e21()) / s;
				} 
				else 
				{
					float s = 2.0f * sqrtf( 1.0f + e22() - e00() - e11());
					w = (e10() - e01()) / s;
					x = (e02() + e20()) / s;
					y = (e12() + e21()) / s;
					z = 0.25f * s;
				}
			}
			//*/

			return Quaternion(w, x, y, z, false);
		}
		//*/

		// y dir does not need to be orthogonal to x dir, as long as orientation is correct
		inline void SetByXYAxis(Vector3f x_dir, Vector3f y_dir)
		{
			Vector3f z_dir = x_dir.Cross(y_dir).Normalize();
			x_dir.Normalize();
			y_dir = z_dir.Cross(x_dir).Normalize();
			SetCol(x_dir, y_dir, z_dir);
		}

		inline void SetByLookAt(Vector3f z_dir, Vector3f y_dir)
		{
			z_dir.Normalize();
			Vector3f x_dir = y_dir.Cross(z_dir).Normalize();
			y_dir = z_dir.Cross(x_dir);

			v[0] = x_dir.x; v[1] = x_dir.y; v[2] = x_dir.z;
			v[3] = y_dir.x; v[4] = y_dir.y; v[5] = y_dir.z;
			v[6] = z_dir.x; v[7] = z_dir.y; v[8] = z_dir.z;
		}

		Vector3f X() const { return Vector3f(v[0], v[3], v[6]); }
		Vector3f Y() const { return Vector3f(v[1], v[4], v[7]); }
		Vector3f Z() const { return Vector3f(v[2], v[5], v[8]); }

		// return 4 by 4 matrix used in OpenGL
		inline void GetGLMatrix(float* mat) const
		{
			mat[0] = v[0];	mat[1] = v[3];	mat[2] = v[6];	mat[3] = 0;
			mat[4] = v[1];	mat[5] = v[4];	mat[6] = v[7];	mat[7] = 0;
			mat[8] = v[2];	mat[9] = v[5];	mat[10] = v[8];	mat[11] = 0;
			mat[12] = 0;	mat[13] = 0;	mat[14] = 0;	mat[15] = 1;
		}

		// common rotation operations
	public:
		Rotation3& Invert()	
		{ 
			Transpose();	// this is fast but very sensitive to numerical error!
			//Matrix3f::Invert();	// this is quite robust
			return *this; 
		}

		// remove numerical unstability
		Rotation3& Normalize()
		{ 
			SetQuaternion(GetQuaternion());
			return *this; 
		}

		Rotation3& operator*= (const Rotation3& r) 
		{ 
			// both methods are fine
			SetQuaternion(GetQuaternion() * r.GetQuaternion());
			//Matrix3f::operator*=(r);	Normalize();
			return *this;
		}

		friend Rotation3 operator*(const Rotation3& r1, const Rotation3& r2) { return Rotation3(r1) *= r2; }

		friend Rotation3 Interpolate(const Rotation3& r1, const Rotation3& r2, float w)
		{
			return Rotation3(Quaternion::Slerp(w, r1.GetQuaternion(), r2.GetQuaternion()));
		}
	};
}