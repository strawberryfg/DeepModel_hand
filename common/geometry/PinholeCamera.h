#pragma once

#include "ProjectiveCamera.h"

#include "radial_distortion.h"

namespace geometry
{
	// the basic projective camera is augmented with skew and image distortion model
	// because the distortion step is nonlinear, the projection operations in ProjectiveCamera(Fast)
	// are no longer valid and overriden here

	// check the explanation of the skew and distortion parameters at
	// http://www.vision.caltech.edu/bouguetj/calib_doc/htmls/parameters.html

	class PinholeCamera : public ProjectiveCamera
	{
		float skew;
		PlumbBobModel model; // it could be templated as long as it provides distort/undistort interface

	public:
		PinholeCamera() : skew(0) {}
		PinholeCamera(float fu, float fv, float s, float u, float v, const Euclidian3D& ext, const PlumbBobModel& _model)
			: ProjectiveCamera(fu, fv, u, v, ext), skew(s), model(_model) {}

		// override of projection functions
	public:
		Point2D Project(const Point3D& point) const
		{
			Point3D p_cam = ext.Apply(point);

			float x, y;
			model.distort(p_cam.x / p_cam.z, p_cam.y / p_cam.z, x, y);

			// to take into account that image X-Y is opposite to world X-Y, but can be done outside
			//model.distort(-p_cam.x / p_cam.z, -p_cam.y / p_cam.z, x, y);
			
			return Point2D(alpha_u*(x + skew*y) + u0, alpha_v*y + v0); // PCam2PImg
		}

		int Undistort(float x, float y, float& dx, float& dy) const
		{
			// PImg2PCam
			float yd = (y-v0)/alpha_v;
			//float xd = (x-u0)/alpha_u;
			float xd = (x-u0)/alpha_u - skew*yd;
			return model.undistort(xd, yd, dx, dy);
		}

		// return the direction of back projected line
		template<class C>
		Point3D BackProject(const C x, const C y) const
		{
			C dx, dy;
			Undistort(x, y, dx, dy);
			Point3D pt_cam(dx, dy, 1);
			// to take into account that image X-Y is opposite to world X-Y, but can be done outside
			//Point3D pt_cam(-dx, -dy, 1);
			return (ext.ApplyInverse(pt_cam) - Center()).Normalize();
		}

		template<class C>
		Point3D BackProjectToDepth(const C x, const C y, float z) const
		{	
			C dx, dy;
			Undistort(x, y, dx, dy);
			Point3D pt_cam(z*dx, z*dy, z);
			// to take into account that image X-Y is opposite to world X-Y, but can be done outside
			//Point3D pt_cam(-z*dx, -z*dy, z);

			return ext.ApplyInverse(pt_cam);
		}

		template<class C>
		Point3D BackProjectToDepth(const Vector2<C>& p, float z) const
		{	
			return BackProjectToDepth(p.x, p.y, z);
		}
	};
}