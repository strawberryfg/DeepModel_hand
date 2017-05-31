#pragma once

#include <iostream>
using namespace std;

#include <geometry\LinearPrimitive.h>
#include <geometry\Transformation.h>

namespace geometry
{
	// a basic projective camera is fully described by a 3x4 matrix P = K * [R; t]
	// K is 3x3 intrinsic matrix, [R; t] is external camera pose
	// skew and image distortion is NOT considered here, and addressed in PinholeCamera
	class ProjectiveCamera
	{
	protected:
		// kernel data member
		float u0, v0, alpha_u, alpha_v;	// internal parameters: K
		
		Euclidian3D ext;					// external transformation [R; t]
		Vector3f center;					// TODO: camera center, redundant as to ext, only used in back projection

		void Decompose(float c[12]);		// decompose camera matrix into internal and external parameters

		// construction
	public:
		// default camera focal length is 1000, image dimension is 640x480
		ProjectiveCamera() : u0(320), v0(240), alpha_u(1000), alpha_v(1000) {}

		explicit ProjectiveCamera(float c[12]);
		ProjectiveCamera(float fu, float fv, float u, float v, float ext_para[12]);
		ProjectiveCamera(float fu, float fv, float u, float v, const Euclidian3D& ext);

		// data access
	public:
		float U0() const {	return u0;	}
		float V0() const {	return v0;	}
		float focalLen() const	{	return alpha_u;	}
		float Aspect() const	{	return u0 / v0;	}
		float heightAngle() const {	return 2*atan(v0 / alpha_v);	}

		Vector3f viewingDir() const	{	return Vector3f(Rot()[6], Rot()[7], Rot()[8]);	}	// the same as principleAxis()
		Vector3f upDir() const 		{	return -Vector3f(Rot()[3], Rot()[4], Rot()[5]);	}
		Vector3f rightDir() const 	{	return Vector3f(Rot()[0], Rot()[1], Rot()[2]);	}
		const Vector3f& Center() const	{	return center;	}
		const Rotation3& Rot()	const {	return ext.r;	}
		const Euclidian3D& Ext() const	{	return ext;	}
		float DepthOf(const Point3D& point) const	{	return ext.Apply(point).z;	}
		Point3D WorldPt2CameraPt(const Point3D& point) const	{	return ext.Apply(point);	}
		
	public:
		// [x_cam, y_cam] = [x_world / z_world, y_world / z_world]
		// convert world dimension to pixel dimension
		void PCam2PImg(float x_cam, float y_cam, float& x_img, float& y_img) const
		{
			x_img = alpha_u*x_cam + u0;
			y_img = alpha_v*y_cam + v0;
		}

		// return the direction of back projected line
		template<class C>
		Point3D BackProject(const C x, const C y) const
		{
			Point3D pt_cam((x - u0) / alpha_u, (y - v0) / alpha_v, 1);
			return (ext.ApplyInverse(pt_cam) - center).Normalize();
		}
	};

	template<class Camera, class C>
	Point3D BackProject(const Camera& c, const Vector2<C>& p)
	{
		return c.BackProject(p.x, p.y);
	}

	// back project the image pixel to a 3D line, originated from camera center
	template<class Camera, class C>
	Line3D BackProjectToLine(const Camera& c, const C x, const C y)
	{
		return Line3D(c.Center(), c.BackProject(x, y));
	}

	template<class Camera, class C>
	Line3D BackProjectToLine(const Camera& c, const Vector2<C>& p)
	{
		return BackProjectToLine(c, p.x, p.y);
	}
}