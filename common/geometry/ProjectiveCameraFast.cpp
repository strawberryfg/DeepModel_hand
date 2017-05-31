#include "ProjectiveCameraFast.h"

using namespace geometry;

ProjectiveCameraFast::ProjectiveCameraFast() : io_flag(INT_EXT)
{
	ComputeAuxiliaryData();
}

ProjectiveCameraFast::ProjectiveCameraFast(float c[12]) : io_flag(INT_EXT)
{
	Decompose(c);

	// compute invM
	float temp[] = 
	{
		c[0], c[1], c[2],
		c[4], c[5], c[6],
		c[8], c[9], c[10]
	};

	invM = M = temp;
	p4 = Vector3f(c[3], c[7], c[11]);
	invM.Invert();
	principle_axis = M.GetRow(2);
	if (M.Det() < 0) principle_axis = -principle_axis;
	principle_axis.Normalize();
}

ProjectiveCameraFast::ProjectiveCameraFast(float f, float u, float v, float ext_para[12]) : 
    ProjectiveCamera(f, f, u, v, ext_para), io_flag(INT_EXT)
{
	ComputeAuxiliaryData();
}

ProjectiveCameraFast::ProjectiveCameraFast(float f, float u, float v, const Euclidian3D& ext) :
    ProjectiveCamera(f, f, u, v, ext), io_flag(INT_EXT)
{
	ComputeAuxiliaryData();
}

void ProjectiveCameraFast::ComputeAuxiliaryData()
{	
	float K[] = {
		alpha_u, 0, u0,
		0, alpha_v, v0,
		0,	0,	1
	};
	M = Matrix3f(K);
	M *= ext.r;
	p4 = Matrix3f(K) * ext.t;
	invM = M;
	invM.Invert();

	principle_axis = ext.r.GetRow(2);
	if (M.Det() < 0) principle_axis = -principle_axis;
}

void ProjectiveCameraFast::SetExtPara(const Euclidian3D& _ext)
{
	ext = _ext;
	center = ext.GetCenter();
	ComputeAuxiliaryData();
}	

void ProjectiveCameraFast::SetExtPara(const Rotation3& rot, const Vector3f& cen)
{
	ext.r = rot;
	center = cen;
	ext.t = -(rot * center);
	ComputeAuxiliaryData();
}

void ProjectiveCameraFast::SetCenter(const Vector3f& cen)
{
	center = cen;
	ext.t = -(ext.r * center);
	ComputeAuxiliaryData();
}

void ProjectiveCameraFast::SetImageScale(float scale)
{
	u0 *= scale;
	v0 *= scale;
	alpha_u *= scale;
	alpha_v *= scale;

	ComputeAuxiliaryData();
}

/* Concatenate a rotation R1 around center c1(both in camera coordinate frame) before current camera external transformation [R, -Rc]

|R1 -R1c1+c1| * |R -Rc| = |R1R -R1Rc-R1c1+c1|
| 0     1   |   | 0  1|   |  0      1    |	|
*/
void ProjectiveCameraFast::Rotate(const Rotation3& r1, const Vector3f& c1)
{	
	Rotation3 rot = r1 * Rot();
	Vector3f t = c1 - rot * Center() - r1 * c1;
	SetExtPara(rot, -t * rot);	// inv(R)t = R't = (t'* R)'
}

/* Concatenate a translation t(in camera coordinate frame) before current camera external transformation

|I t| |R -Rc| = |R -Rc+t| = |R -R(c+inv(R)t)|
|0 1| |0   1|   |0   1  |   |0       1      |
*/
void ProjectiveCameraFast::Translate(const Vector3f& t)
{	
	SetCenter(Center() + t * Rot()); // inv(R)t = R't = (t'* R)'
}