#pragma once

#include <geometry\Rotation3.h>

namespace geometry
{
	// 3D Euclidian transformation consists of a 3D rotation followed by a 3D translation
	class Euclidian3D
	{
	public:
		inline friend ostream & operator<< (ostream & out, const Euclidian3D& e)
		{ return out << e.r << " " << e.t;	}

		inline friend istream & operator>> (istream & in, Euclidian3D& e)
		{ return in >> e.r >> e.t;	}

	public:
		Rotation3 r;
		Vector3f t;

		Euclidian3D() {} // default as identity
		Euclidian3D(const Rotation3& _r, const Vector3f& _t) : r(_r), t(_t) {}
		
		Vector3f Apply(const Vector3f& x) const	{ return r * x + t;	}
		Vector3f ApplyInverse(const Vector3f& x) const	{	return (x - t) * r;	}

		// rotation around the center is equivalent to the transformation
		// R(x-c) = Rx+t => t=-R*c => c'=-t'*R
		Vector3f GetCenter() const { return -t * r; }

		void SetIdentity()	{	r.SetIdentity(); t = Vector3f(0, 0, 0);	}
		Euclidian3D& Invert() {	r.Invert();	t = -(r * t); 	return *this; 	}

		// |R1 t1| |R2 t2| = |R1R2 R1t2+t1|
		// | 0  1| | 0  1|   |  0     1   |		
		Euclidian3D& operator*= (const Euclidian3D& e)	{	t += r * e.t;	r *= e.r;	return *this;	}
		Euclidian3D& operator*= (const Rotation3& r1)	{	r *= r1;	return *this;	}
	};

	inline Euclidian3D operator*(const Euclidian3D& e1, const Euclidian3D& e2)
	{ Euclidian3D e(e1); return e *= e2; }

	inline Euclidian3D operator*(const Rotation3& r, const Euclidian3D& e)
	{ return Euclidian3D(r * e.r, r * e.t); 	}

	// 3D similarity transformation consists of a 3D scaling followed by a 3D Euclidian transformation
	class Similarity3D : public Euclidian3D
	{
	public:
		inline friend ostream & operator<< (ostream & out, const Similarity3D& s)
		{ return out << (Euclidian3D)s << " " << s.s << endl;	}

		inline friend istream & operator>> (istream & in, Similarity3D& s)
		{ return in >> (Euclidian3D)s >> s.s;	}

	public:
		Vector3f s;

		Similarity3D() : s(1) {} // default as identity
		Similarity3D(const Rotation3& _r, const Vector3f& _t, const Vector3f& _s) : Euclidian3D(_r, _t), s(_s) {}

		Vector3f Apply(const Vector3f& x) const	{	return Euclidian3D::Apply(s * x);	}
		Vector3f ApplyInverse(const Vector3f& x) const	{	return Euclidian3D::ApplyInverse(x) / s;	}

		void SetIdentity()	{	Euclidian3D::SetIdentity();	s = Vector3f(1); }
	};
}