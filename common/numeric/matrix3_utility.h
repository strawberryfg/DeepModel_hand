#pragma once

template<class real_type>
void matrix3_make_identity(const real_type* a)
{
	a[0] = 1; a[1] = 0; a[2] = 0;
	a[3] = 0; a[4] = 1; a[5] = 0;
	a[6] = 0; a[7] = 0; a[8] = 1;
}

template<class real_type>
void matrix3_plus(const real_type* a, const real_type* b, real_type* c)
{
	for(int i = 0; i < 9; i++) c[i] = a[i] + b[i];
}

template<class real_type>
void matrix3_minus(const real_type* a, const real_type* b, real_type* c)
{
	for(int i = 0; i < 9; i++) c[i] = a[i] - b[i];
}

template<class real_type>
void matrix3_multiply(const real_type* a, const real_type* b, real_type* c)
{
	c[0] = a[0]*b[0] + a[1]*b[3] + a[2]*b[6];
	c[1] = a[0]*b[1] + a[1]*b[4] + a[2]*b[7];
	c[2] = a[0]*b[2] + a[1]*b[5] + a[2]*b[8];
	c[3] = a[3]*b[0] + a[4]*b[3] + a[5]*b[6];
	c[4] = a[3]*b[1] + a[4]*b[4] + a[5]*b[7];
	c[5] = a[3]*b[2] + a[4]*b[5] + a[5]*b[8];
	c[6] = a[6]*b[0] + a[7]*b[3] + a[8]*b[6];
	c[7] = a[6]*b[1] + a[7]*b[4] + a[8]*b[7];
	c[8] = a[6]*b[2] + a[7]*b[5] + a[8]*b[8];
}

template<class real_type>
real_type matrix3_det(const real_type* a)
{
	return a[0]*a[4]*a[8] + a[1]*a[5]*a[6] + a[2]*a[3]*a[7]
		- a[2]*a[4]*a[6] - a[0]*a[5]*a[7] - a[1]*a[3]*a[8];
}

template<class real_type>
bool matrix3_invert(const real_type* a, real_type* a_inv)
{
	real_type d = matrix3_det(a);

	if ((d < 1e-6) && (d > -1e-6)) // close to 0
		return false;

	real_type d_inv = 1 / d;

	a_inv[0] = (a[4]*a[8] - a[5]*a[7]) * d_inv;
	a_inv[1] = (a[7]*a[2] - a[1]*a[8]) * d_inv;
	a_inv[2] = (a[1]*a[5] - a[4]*a[2]) * d_inv;
	a_inv[3] = (a[5]*a[6] - a[3]*a[8]) * d_inv;
	a_inv[4] = (a[0]*a[8] - a[6]*a[2]) * d_inv;
	a_inv[5] = (a[3]*a[2] - a[0]*a[5]) * d_inv;
	a_inv[6] = (a[3]*a[7] - a[4]*a[6]) * d_inv;
	a_inv[7] = (a[6]*a[1] - a[0]*a[7]) * d_inv;
	a_inv[8] = (a[4]*a[0] - a[3]*a[1]) * d_inv;

	return true;
}