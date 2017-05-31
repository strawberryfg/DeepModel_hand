#pragma once

#include <geometry\Rotation3.h>

namespace geometry
{
	// a 3D ball is centered on the screen, mouse move rotates the ball around its center
	class CArcBall
	{
	protected:
		int cx, cy;		// screen center, in pixel
		int	radius;		// ball radius, in pixel
		
		Point3D	prevBallPoint;	// normalized point on unit sphere

	public:
		CArcBall() : radius(100), cx(100), cy(100), prevBallPoint(0, 0, 1)	{}
		CArcBall(int x, int y, int r) : cx(x), cy(y), radius(r), prevBallPoint(0, 0, 1) {}
		void SetArcBall(int x, int y, int r) {	cx = x;	cy = y;	radius = r;	}

		void StartDrag(int x, int y) {	prevBallPoint = ScreenPt2BallPt(x, y);	}

		// the rotation from last ball point to current ball point
		Rotation3 GetMotion(int x, int y)
		{
			Point3D currBallPoint = ScreenPt2BallPt(x, y);
			Point3D oldPt = prevBallPoint;
			prevBallPoint = currBallPoint;
			//set rotation directly from two directions
			return Rotation3(oldPt, currBallPoint);
			
			/*
			Vector3f axis = prevBallPoint.Cross(currBallPoint);
			prevBallPoint = currBallPoint;
			float norm = axis.L2Norm();
			if (norm < 1e-6)
				return Rotation3(Vector3f(0, 0, 1), 0);	
			else
				return Rotation3(axis.Normalize(), asin(norm));
			//*/
		}

	private:
		Point3D ScreenPt2BallPt(int x, int y)
		{	
			float fx = (float)(x - cx) / radius;
			float fy = (float)(y - cy) / radius;
			float n2 = fx*fx + fy*fy;
			if (n2 > 1.0)
			{
				float n = sqrt(n2);
				return Point3D(fx/n, fy/n, 0);
			}
			else
				return Point3D(fx, fy, sqrt(1-n2));
		}
	};
}