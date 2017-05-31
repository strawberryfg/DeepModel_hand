#pragma once

#include <numeric\vector2.h>
#include <numeric\vector3.h>
using namespace numeric;

#include <vector>
#include <iostream>

namespace geometry
{
	template<class V>
	class Box
	{
	protected:
		V min_vtx, max_vtx;

	public:
		typedef V point_type;
		typedef typename V::value_type value_type;

		Box() {}
		Box(const V& _min_vtx, const V& _max_vtx) : min_vtx(_min_vtx), max_vtx(_max_vtx){}

		Box(const std::vector<V>& pts)
		{
			if (pts.empty()) return;

			min_vtx = pts[0];
			max_vtx = pts[0];

			for(unsigned int n = 1; n < pts.size(); n++)
			{
				min_vtx = V::Min(min_vtx, pts[n]);
				max_vtx = V::Max(max_vtx, pts[n]);
			}
		}
		
		const V& Min() const {	return min_vtx;	}
		const V& Max() const {	return max_vtx;	}
		V Center() const	{	return (min_vtx + max_vtx) / 2;}
		V Size() const	{	return max_vtx - min_vtx;	}

		value_type Width() const	{	return max_vtx.x - min_vtx.x;	}
		value_type Height() const	{	return max_vtx.y - min_vtx.y;	}
		value_type Depth() const	{	return max_vtx.z - min_vtx.z;	}

		bool isEmpty() const	{	return min_vtx == max_vtx;	}
		bool isPtInside(const V& pt) const	{	return (pt >= min_vtx) && (pt <= max_vtx);	}
		bool isContainedIn(const Box& b) const	{	return (min_vtx >= b.min_vtx) && (max_vtx <= b.max_vtx);	}
		
		friend std::ostream & operator<< (std::ostream & out, const Box & b) {	return out << b.min_vtx << " " << b.max_vtx;}
		friend std::istream & operator>> (std::istream & in, Box & b) {	return in >> b.min_vtx >> b.max_vtx;	}
	};

	template<class B>
	inline B Union(const B& b1, const B& b2, bool ignore_empty)
	{
		if (b1.isEmpty() && ignore_empty) return b2;
		if (b2.isEmpty() && ignore_empty) return b1;
		return B(B::point_type::Min(b1.Min(), b2.Min()), B::point_type::Max(b1.Max(), b2.Max()));
	}

	/////////////////////////////////////////////////////////////////////////////
	typedef Box<Vector3<float>> Box3f;
	/*
	template <class C> class Box2;
	typedef Box2<float> Box2f;
	typedef Box2<int>	Box2i;
	typedef Box2<short> Box2s;

	template <class C>
	class Box2 : public Box<Vector2<C>>
	{
	public:		
	};
	
	template <class C> class Box3;
	typedef Box3<float> Box3f;
	typedef Box3<int>	Box3i;
	typedef Box3<short>	Box3s;

	template <class C>
	class Box3 : public Box<Vector3<C>>
	{
	public:		
	};
	*/
}