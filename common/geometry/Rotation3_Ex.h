#pragma once
#include "Rotation3.h"

namespace geometry
{
	// cr -sr  0     1   0   0     cy  0   sy
	// sr  cr  0  *  0   cp -sp *  0   1   0
	// 0   0   1     0   sp  cp   -sy  0   cy

	//cr*cy - sr*sp*sy   -sr*cp   cr*sy + cy*sr*sp
	//sr*cy + cr*sp*sy    cr*cp   sr*sy - cr*sp*cy
	//   -cp*sy            sp         cp*cy
	struct RotAngle
	{
		RotAngle():roll(0.0f), pitch(0.0f), yaw(0.0f){}
		RotAngle(float r, float p, float y):roll(r), pitch(p), yaw(y){}
		// The following angles are in radian
		union
		{
			float angles[3];
			struct {
				float  roll, pitch, yaw;
			};
		};
	};
	inline Rotation3 RotAngle2RotMat(const RotAngle& angle)
	{
		return Rotation3(Point3D(0, 0, 1), angle.roll) 
			* Rotation3(Point3D(1, 0, 0), angle.pitch) 
			* Rotation3(Point3D(0, 1, 0), angle.yaw);
	}
	inline RotAngle RotMat2RotAngle(const Rotation3& rot)
	{	
		float sin_pitch = rot.v[7];
		float cos_pitch = sqrtf(1.0f - sin_pitch * sin_pitch); // always positive
		float p = sin_pitch < 0 ? -acos(cos_pitch) : acos(cos_pitch);
	
		float sin_yaw = - rot.v[6] / cos_pitch;
		float cos_yaw = rot.v[8] / cos_pitch;
		cos_yaw /= sqrtf(sin_yaw * sin_yaw + cos_yaw * cos_yaw);
		float y = sin_yaw < 0 ? -acos(cos_yaw) : acos(cos_yaw);
	
		float sin_roll = - rot.v[1] / cos_pitch;
		float cos_roll = rot.v[4] / cos_pitch;
		cos_roll /= sqrtf(sin_roll * sin_roll + cos_roll * cos_roll);
		float r = sin_roll < 0 ? -acos(cos_roll) : acos(cos_roll);

		// verify formula, compare these value with corresponding rotation matrix value.
		//float r0 = cos_roll * cos_yaw - sin_roll * sin_pitch * sin_yaw;
		//float r2 = cos_roll * sin_yaw + cos_yaw * sin_roll * sin_pitch;
		//float r3 = sin_roll * cos_yaw + cos_roll * sin_pitch * sin_yaw;
		//float r5 = sin_roll * sin_yaw - cos_roll * sin_pitch * cos_yaw;
	
		// degree
		//float p_a = p / PI * 180.0f;
		//float r_a = r / PI * 180.0f;
		//float y_a = y / PI * 180.0f;
		return RotAngle(r, p, y);
	}
}