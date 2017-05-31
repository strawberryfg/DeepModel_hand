#pragma once

#include <numeric\vector3.h>
using namespace numeric;

#include <math.h>
#include <assert.h>

namespace geometry
{	
	// TODO: Line3D seems able to be templated to Line<PointType>

	// a line is represented by a reference point P and unit direction D
	// a point on the line is parameterized by the length t from reference point as : P + t * D
	class Line3D
	{
	private:
		Point3D ref_pt, dir;

	public:
		typedef Point3D PointType;
		Line3D() : dir(1.0f, 0.0f, 0.0f){}
		Line3D(const PointType& p0, const PointType& d) : ref_pt(p0), dir(d)	{	dir.Normalize();	}

		const PointType& RefPoint() const {	return ref_pt;	}
		const PointType& Dir() const	{	return dir;	}
		const PointType At(float len) const { return ref_pt + len * dir; }

		PointType OrthogonalProjection(const PointType& pt, float* len = NULL) const
		{
			// solve t from (ref_pt + t*dir - pt) DOT dir = 0
			// assume that ||dir||_2 = 1, it is the signed length of projection of (pt-ref_pt) on the line
			float t = dir.Dot(pt - ref_pt);
			if (len != NULL) *len = t;
			return ref_pt + t * dir;
		}

		float Distance(const PointType& pt) const
		{	return pt.Distance(OrthogonalProjection(pt));	}

		float Distance2Segment(const PointType& pt, const float segment_len, PointType&foot = PointType(), int* id = 0) const
		{		
			int _id = 0;
			float len;
			foot = OrthogonalProjection(pt, &len);
			if (len < 0)
			{
				_id = 1;
				foot = ref_pt;
			}
			else if (len > segment_len)
			{
				_id = 2;
				foot = ref_pt + segment_len*dir;
			}
			if (id)
				*id = _id;
			return pt.Distance(foot);
		}

		// find points on the line and a given sphere, return number of such points
		int FindPointOnSphere(const Point3D& center, float radius, bool is_line_directed, Point3D& pt_near, Point3D& pt_far) const
		{
			float len;
			Point3D ptPerpendFoot = OrthogonalProjection(center, &len);
			float dist_to_line = center.Distance(ptPerpendFoot);

			if (dist_to_line > radius)	return 0;
			if (radius - dist_to_line < 1e-6)
			{
				if (is_line_directed && (len < 0))
					return 0;
				else
				{
					pt_near = pt_far = ptPerpendFoot;
					return 1;
				}
			}

			float dist_on_line = sqrt(radius*radius - dist_to_line*dist_to_line);

			float t1 = len - dist_on_line, t2 = len + dist_on_line;

			if (is_line_directed)	// only valid for t1(2) >= 0
			{
				if (t2 < 0) return 0;
				if (t1 < 0)	// only t2 is valid
				{
					pt_near = ref_pt + dir*t2;
					return 1;
				}
				else	// both t1(2) are valid
				{
					pt_near = ref_pt + dir*t1;
					pt_far = ref_pt + dir*t2;
					return (dist_on_line < 1e-6) ? 1 : 2;
				}
			}
			else
			{
				pt_near = ref_pt + dir*t1;
				pt_far = ref_pt + dir*t2;
				return (dist_on_line < 1e-6) ? 1 : 2;
			}
		}
	};

	inline float Det2_2(float a, float b, float c, float d) {	return a*d-b*c;	}

	// a 3D plane represented by unit normal N and factor d
	// a 3D point p on the plane is parameterized as p * N + d = 0
	class Plane3D
	{
	private:
		Point3D normal;
		float d;

	public:
		Plane3D() : normal(1.0f, 0.0f, 0.0f), d(0.0f)	{}
		Plane3D(const Point3D& p, const Point3D& n) : normal(n), d(-(p.Dot(normal.Normalize()))) {}
		Plane3D(const Point3D& p0, const Point3D& p1, const Point3D& p2) : normal((p1 - p0).Cross(p2 - p0)), d(-(normal.Normalize().Dot(p0))) {}
		Plane3D(const Point3D& n, float _d) : normal(n), d(_d)
		{
			float norm = normal.L2Norm(); normal /= norm; d /= norm;
		}

		const Point3D& Normal() const	{	return normal;	}
		float D() const { return d; }

		Point3D Intersection(const Line3D& line) const
		{
			float v = line.Dir().Dot(normal);
			//assert(fabs(v) > 1e-6);
			// solve t from (ref_pt + t*d) * N + d = 0;
			float t = -(line.RefPoint().Dot(normal) + d) / v;
			return line.RefPoint() + t * line.Dir();
		}

		// return the absolute angle (in radian) between the direction and plane
		float AngleBetween(const Point3D& dir) const
		{
			float fDot = dir.Dot(normal) / dir.L2Norm();
			// fDot is the cosine value of angle between plane normal and given dir
			if (fDot < 0) fDot = -fDot;
			return asin(fDot);
		}

		Point3D OrthogonalProjection(const Point3D& pt) const
		{	return Intersection(Line3D(pt, normal));	}

		float DistanceTo(const Point3D& pt) const
		{	return pt.Distance(OrthogonalProjection(pt));	}

		float SignedDistanceTo(const Point3D& pt) const
		{
			Point3D proj = OrthogonalProjection(pt);
			if (normal.Dot(pt - proj) > 0) return pt.Distance(proj);
			else return -pt.Distance(proj);
		}

		//*
		static Line3D Intersection(const Plane3D& p1, const Plane3D& p2)
		{
			const Point3D& n1 = p1.Normal();
			const Point3D& n2 = p2.Normal();

			Point3D line_dir = n1.Cross(n2);
			assert(line_dir.L2Norm() > 1e-6);
			line_dir.Normalize();

			Point3D ptOnLine;	// find one point on the line

			// TODO: verify and add some comment
			if( fabs(line_dir.x)>0.577f ) //0.577 = sqrt(1/3)
			{
				ptOnLine.x = 1.0f;
				float fDet = Det2_2(n1.y, n1.z, n2.y, n2.z);
				float r1 = -p1.D() - n1.x;
				float r2 = -p2.D() - n2.x;
				ptOnLine.y = Det2_2(r1, n1.z, r2, n2.z) / fDet;
				ptOnLine.z = Det2_2(n1.y, r1, n2.y, r2) / fDet;
			}
			else if( fabs(line_dir.y)>0.577f )
			{
				ptOnLine.y = 1.0f;
				float fDet = Det2_2(n1.x, n1.z, n2.x, n2.z);
				float r1, r2;
				r1 = -p1.D()-n1.y;
				r2 = -p2.D()-n2.y;
				ptOnLine.z = Det2_2(n1.x, r1, n2.x, r2) / fDet;
				ptOnLine.x = Det2_2(r1, n1.z, r2, n2.z) / fDet;
			}
			else
			{
				ptOnLine.z = 1.0f;
				float fDet = Det2_2(n1.x, n1.y, n2.x, n2.y);
				float r1, r2;
				r1 = -p1.D()-n1.z;
				r2 = -p2.D()-n2.z;
				ptOnLine.y = Det2_2(n1.x, r1, n2.x, r2) / fDet;
				ptOnLine.x = Det2_2(r1, n1.y, r2, n2.y) / fDet;
			}

			return Line3D(ptOnLine, line_dir);
		}
		//*/

		static Point3D IntersectionDirectionAverage(int NUM_PLANES, const Plane3D* planes, const Point3D& ref_dir)
		{
			Point3D dir_sum;
			int count = 0;
			for(int i = 0; i < NUM_PLANES; i++)
				for(int j = i+1; j < NUM_PLANES; j++)
				{
					Line3D l = Intersection(planes[i], planes[j]);
					count++;
					dir_sum += (l.Dir().Dot(ref_dir) > 0.0f) ? l.Dir() : -l.Dir();
				}

			return (dir_sum / (float)count).Normalize();
		}
	};

	template<class PointType>
	PointType ReflectAroundNormal(const PointType& p, PointType n)
	{
		n.Normalize();
		return 2*p.Dot(n)*n-p;
	}
}