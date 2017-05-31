#include "ProjectiveCamera.h"

using namespace geometry;

ProjectiveCamera::ProjectiveCamera(float c[12])
{
	Decompose(c);
}

ProjectiveCamera::ProjectiveCamera(float fu, float fv, float u, float v, float ext_para[12]) 
: u0(u), v0(v), alpha_u(fu), alpha_v(fv)
{	
	float temp[] = {
		ext_para[0], ext_para[1], ext_para[2],
		ext_para[4], ext_para[5], ext_para[6],
		ext_para[8], ext_para[9], ext_para[10]
	};

	ext.r = Rotation3(Matrix3f(temp));
	ext.t = Vector3f(ext_para[3], ext_para[7], ext_para[11]);
	center = ext.GetCenter();
}

ProjectiveCamera::ProjectiveCamera(float fu, float fv, float u, float v, const Euclidian3D& _ext)
: u0(u), v0(v), alpha_u(fu), alpha_v(fv), ext(_ext)
{
	center = ext.GetCenter();
}

void ProjectiveCamera::Decompose(float c[12])
{
	// computer internal and external parameters
	Vector3f q1(c[0], c[1], c[2]);
	Vector3f q2(c[4], c[5], c[6]);
	Vector3f q3(c[8], c[9], c[10]);

	float p = 1/(q3.L2Norm());	
	u0 = p * p * (q1.Dot(q3));
	v0 = p * p * (q2.Dot(q3));
	alpha_u = sqrt(p*p*(q1.Dot(q1)) - u0*u0);
	alpha_v = sqrt(p*p*(q2.Dot(q2)) - v0*v0);	

	Matrix3f m(p * (q1 - (u0*q3)) / alpha_u
		,p * (q2 - (v0*q3)) / alpha_v
		,p * q3);
	m.Transpose();
	
	ext.r = Rotation3(m);
	// R and m may have significant difference sometimes, e.g., redflower data 50 views. It might be that the assumption skew = 0 does not hold during the bundle adjustment.
	//float error = m.Distance(R);
	//cout << "enforcing rotation matrix causing error : " << error << endl;

	ext.t = Vector3f(p * (c[3] - u0*c[11]) / alpha_u, p * (c[7] - v0*c[11]) / alpha_v, p * c[11]);
	center = ext.GetCenter();
}

// change the imaging coordinate frame to OpenGL coordinate frame
// camera center and rotation are changed
// geometry corresponding to the camera should also be changed
/*
Imaging coordinate(IC) assumes X-axis points rightward, Y-axis points downward,
Z-axis points outwards, orthogonal to the image plane. While, OpenGL coordinate(OC)
assumes	different Y and Z axis.
In both cases, eye is positioned at origin and looks inwards the image(screen).

z                 y
/                   |
/                    |
/______ x             |________ x
|                    / 
|                   /
|                  /
y                 z
Image Coordinate    OpenGL Coordinate

In order for a 3D point X to have the same projection positions in IC and OC,
X=(x,y,z) in IC is to be (x, -y, -z) in OC, that is, a rotation T applied to X in IC.
Correspondingly, external parameters in camera matrix need to be changed.

The original external camera transformation is
R (X - C)
//It is obvious that, by change R to R*inv(T), and C to TC, the external transformation
//after the coordinate changing is the same
//    R*inv(T)*(TX - TC) = R*(X - C)
Should apply T to R(X-C), that is, change R to TR is enough.
|1  0  0|
where T = inv(T) = |0 -1  0|
|0  0 -1|
*/
/*
void ProjectiveCamera::ChangeCoordFrame()
{
R[3] = -R[3];
R[4] = -R[4];
R[5] = -R[5];
R[6] = -R[6];
R[7] = -R[7];
R[8] = -R[8];	
}
*/

/*
void ProjectiveCamera::Dump(ostream& out, unsigned int flag) const
{	
if(flag & CAMERA_MATRIX)
{		
out << M[0] << " " << M[1] << " " << M[2] << " " << p4.x << endl;
out << M[3] << " " << M[4] << " " << M[5] << " " << p4.y << endl;
out << M[6] << " " << M[7] << " " << M[8] << " " << p4.z << endl;
out << endl;
}	

if(flag & INTERNAL_PARA)
{
out << alpha_u << " " << 0.0 << " " << u0 << endl;
out << 0.0 << " " << alpha_v << " " << v0 << endl;
out << 0.0 << " " << 0.0 << " " << 1.0 << endl;
out << endl;
}

if(flag & CENTER)
out << " center = ( " << center.x << ", " << center.y << ", " << center.z << ")" << endl << endl;

if(flag & PRINCIPLE_AXIS)
out << " principle axis = ( " << principle_axis.x << ", " << principle_axis.y << ", " << principle_axis.z << ")" << endl << endl;

if(flag & ROTATION)
{	
const Rotation& R = ext.GetRotation();
out << "R = " << endl;
out << R.GetRow(0).x << " " << R.GetRow(0).y << " " << R.GetRow(0).z << endl;
out << R.GetRow(1).x << " " << R.GetRow(1).y << " " << R.GetRow(1).z << endl;
out << R.GetRow(2).x << " " << R.GetRow(2).y << " " << R.GetRow(2).z << endl;		
}
}
*/