#pragma once

#include "ProjectiveCamera.h"

namespace geometry
{
	// an easy to use projective camera model, which does not consider skew and image distortion
	// alpha_u and alpha_v are assumed identical in constructor
	// using auxiliary data to upspeed various camera projection operations
	class ProjectiveCameraFast : public ProjectiveCamera
	{
	private:
		// auxiliary and redundant data for fast computation
		void ComputeAuxiliaryData();

		Matrix3f M, invM;		// P = [M; p4] = [K*R; K*t]
		Vector3f p4;
		Vector3f principle_axis;// [0, 0, 1] in world space, and second row of R in camera space

	public:
		ProjectiveCameraFast();
		explicit ProjectiveCameraFast(float c[12]);
		ProjectiveCameraFast(float f, float u, float v, float ext_para[12]);
		ProjectiveCameraFast(float f, float u, float v, const Euclidian3D& ext);

		void SetCenter(const Vector3f& cen);
		void SetExtPara(const Euclidian3D& _ext);
		void SetExtPara(const Rotation3& rot, const Vector3f& cen);
		void SetImageScale(float scale);	// modify the current camera matrix such that it can apply to the new image scaled from the old image

		// Concatenate a rotation R1 around center c1(both in camera coordinate frame) before current camera external transformation
		void Rotate(const Rotation3& r1, const Vector3f& c1);

		// Concatenate a translation t(in camera coordinate frame) before current camera external transformation
		void Translate(const Vector3f& t);

	public:
		const Vector3f& principleAxis() const {	return principle_axis;	}	// already normalized
		const Vector3f& KT() const	{	return p4;	}
		const Matrix3f& KR() const	{	return M;	}
		const Matrix3f& invKR() const	{	return invM;	}

		// use pre-computed auxiliary data for common camera operations
		Point2D Project(const Point3D& point) const
		{
			Point3D p = M * point + p4;
			return Point2D(p.x / p.z, p.y / p.z);
		}

		Vector3f ProjectLine(const Point3D& p1, const Point3D& p2) const
		{
			Point2D proj1 = Project(p1);
			Point2D proj2 = Project(p2);
			return Vector3f(proj1.x, proj1.y, 1).Cross(Vector3f(proj2.x, proj2.y, 1));
		}

		Plane3D BackProjectLine(const Vector3f& line) const
		{
			Matrix3f Mt = M;
			Mt.Transpose();
			Vector3f p = Mt * line;	
			return Plane3D(p, p4.Dot(line));
		}
		
		// a plane is parameterized as n * x + w = 0
		template<class C>
		Point3D BackProjectToPlane(const C x, const C y, const Point3D& n, float w) const
		{
			Vector3f invM_p = invM * Vector3f(x, y, 1);
			float u = -(center.Dot(n) + w) / invM_p.Dot(n);
			return u * invM_p + center;
		}

		template<class C>
		Point3D BackProjectToPlane(const Vector2<C>& p, const Point3D& n, float w) const
		{
			return BackProjectToPlane(p.x, p.y, n, w);
		}

		template<class C>
		Point3D BackProjectToDepth(const C x, const C y, float z) const
		{	
			return BackProjectToPlane(p, principle_axis, -z - center.Dot(principle_axis));	
		}

		template<class C>
		Point3D BackProjectToDepth(const Vector2<C>& p, float z) const
		{
			return BackProjectToDepth(p.x, p.y, z);
		}
		
		// TODO: it seems to be identical to BackProjectToDepth?
		template<class C>
		Point3D BackProjectToZPlane(const C x, const C y, float z) const
		{
			Vector3f invM_p = invM * Vector3f(x, y, 1);
			float u = (z - center.z) / invM_p.z;
			return u * invM_p + center;
		}

		template<class C>
		Point3D BackProjectToInfinity(const C x, const C y) const {	return invM * Vector3f(x, y, 1);	}
		

	public:
		// IO
		inline friend ostream & operator<< (ostream & out, const ProjectiveCameraFast& c)
		{	
			out << c.io_flag << endl;
			if (c.io_flag == ProjectiveCameraFast::MATRIX_3X4)
			{
				out << c.M[0] << " " << c.M[1] << " " << c.M[2] << " " << c.p4.x << endl;
				out << c.M[3] << " " << c.M[4] << " " << c.M[5] << " " << c.p4.y << endl;
				out << c.M[6] << " " << c.M[7] << " " << c.M[8] << " " << c.p4.z << endl;
			}
			else if (c.io_flag == ProjectiveCameraFast::INT_EXT)
			{		
				out << c.alpha_u << " " << c.u0 << " " << c.v0 << endl;
				out << c.ext << endl;
			}
			return out;
		}

		inline friend istream & operator>> (istream & in, ProjectiveCameraFast& c)
		{
			unsigned int io_flag;
			in >> io_flag;
			if (io_flag == ProjectiveCameraFast::MATRIX_3X4)
			{
				float m[12];
				for(int n = 0; n < 12; n++) in >> m[n];
				c = ProjectiveCameraFast(m);		
			}
			else if (c.io_flag == ProjectiveCameraFast::INT_EXT)
			{		
				float u, v, f;
				Euclidian3D ext;
				in >> f >> u >> v >> ext;
				c = ProjectiveCameraFast(f, u, v, ext);
			}
			c.io_flag = io_flag;
			return in;
		}

		// flags for camera parameters IO, corresponding to constructors
		enum
		{
			MATRIX_3X4	= 0
			,INT_EXT
		};

		unsigned int io_flag;
	};
}