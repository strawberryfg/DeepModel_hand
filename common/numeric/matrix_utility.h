#pragma once

template<int N, class real_type>
void matrix_make_identity(real_type a[N][N])
{
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
			a[i][j] = (i==j);
}

template<int N, class real_type>
void matrix_transpose(const real_type a[N][N], real_type at[N][N])
{
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
			at[i][j] = a[j][i];
}

template<int nRows, int nCols, class real_type>
void matrix_minus(const real_type a[nRows][nCols], const real_type b[nRows][nCols], real_type c[nRows][nCols])
{
	for (int i = 0; i < nRows; i++)
		for (int j = 0; j < nCols; j++)
			c[i][j] = a[i][j] - b[i][j];
}

// TODO: this depends on platform files, undesirable, but may be more efficient for large matrices
/*
template<class real_type>
void matrix_copy(int m, int n, const real_type* src, real_type* dst)
{
	memcpy(dst, src, m * n * sizeof(real_type));
}
*/

// should be sufficient for small static matrices
template<int nRows, int nCols, class real_type>
void matrix_copy(const real_type src[nRows][nCols], real_type dst[nRows][nCols])
{
	for (int i = 0; i < nRows; i++)
		for (int j = 0; j < nCols; j++)
			dst[i][j] = src[i][j];
}

// extract a (nRowCount, nColCount) sub-matrix from the location (r, c)
// this function is very inefficient
/*
template<int nRows, int nCols, int nRowCount, int nColCount, class real_type>
void matrix_copy_sub(const real_type src[nRows][nCols], real_type dst[nRowCount][nColCount], const int r, const int c)
{
	for (int i = 0; i < nRowCount; i++)
	{
		int u = i + r;

		if (u>=0 && u<nRows)
		{
			int v = c;
			for (int j = 0; j < nColCount; j++, v++)
				dst[i][j] = (v>=0 && v<nCols) ? src[u][v] : 0;
		}
		else
		{
			for (int j = 0; j < nColCount; j++)
				dst[i][j] = 0;
		}
	}
}
*/

template<int nRows, int nCols, int nCopyRows, class real_type>
void matrix_copy_rows_out(const real_type src[nRows][nCols], real_type dst[nCopyRows][nCols], const int rows[nCopyRows])
{
	// static_assert(nCopyRows <= nRows);
	for (int i = 0; i < nCopyRows; i++)
	{
		const int r = rows[i];
		// assert(r >= 0);
		// assert(r < nRows);
		for (int j = 0; j < nCols; j++)
		{
			dst[i][j] = src[r][j];
		}
	}
}

template<int nRows, int nCols, int nCopyRows, class real_type>
void matrix_copy_rows_in(real_type src[nRows][nCols], const real_type dst[nCopyRows][nCols], const int rows[nCopyRows])
{
	// static_assert(nCopyRows <= nRows);
	for (int i = 0; i < nCopyRows; i++)
	{
		const int r = rows[i];
		// assert(r >= 0);
		// assert(r < nRows);
		for (int j = 0; j < nCols; j++)
			src[r][j] = dst[i][j];
	}
}

template<int nRows, int nCols, int nCopyCols, class real_type>
void matrix_copy_cols_out(const real_type src[nRows][nCols], real_type dst[nRows][nCopyCols], const int cols[nCopyCols])
{
	// static_assert(nCopyCols <= nCols);
	for (int j = 0; j < nCopyCols; j++)
	{
		const int c = cols[j];
		// assert(j >= 0);
		// assert(j < nCols);
		for (int i = 0; i < nRows; i++)
		{
			dst[i][j] = src[i][c];
		}
	}
}

template<int nRows, int nCols, int nCopyCols, class real_type>
void matrix_copy_cols_in(real_type src[nRows][nCols], const real_type dst[nRows][nCopyCols], const int cols[nCopyCols])
{
	// static_assert(nCopyCols <= nCols);
	for (int j = 0; j < nCopyCols; j++)
	{
		const int c = cols[j];
		// assert(j >= 0);
		// assert(j < nCols);
		for (int i = 0; i < nRows; i++)
		{
			src[i][c] = dst[i][j];
		}
	}
}

template <int nRows, int nCols, class real_type>
void matrix_set_col(real_type a[nRows][nCols], int c, const real_type v[nRows])
{
	for (int i = 0; i < nRows; i++) a[i][c] = v[i];
}

template<int M, int N, int L, class real_type1, class real_type2, class real_type3>
void matrix_multiply(const real_type1 a[M][N], const real_type2 b[N][L], real_type3 c[M][L])
{
	for (int i = 0; i < M; i++)
		for (int j = 0; j < L; j++)
		{
			c[i][j] = a[i][0] * b[0][j];
			for (int k = 1; k < N; k++)
				c[i][j] += a[i][k] * b[k][j];
		}
}

// TODO: not tested yet, as it can't be defined as specialization of template function matrix_multiply()
// , which is not supported by compiler
template<int M, class real_type>
void matrix_multiply_n22(const real_type a[M][2], const real_type b[2][2], real_type c[M][2])
{
	for (int i = 0; i < M; i++)
	{
		c[i][0] = a[i][0] * b[0][0] + a[i][1] * b[1][0];
		c[i][1] = a[i][0] * b[0][1] + a[i][1] * b[1][1];
	}
}

/*
QR-Decomposition for a 2x2 Matrix, A = Q * R
http://www.youtube.com/watch?v=51MRHjKSbtk
[a b] = [c -s] * [x z]
[c d]   [s  c]   [0 y]
(1) if A is invertible, without loss of generality, let x > 0, we have
x = sqrt(a^2+c^2)
z = (ab+cd) / x
y = (ad-bc) / x
c = a / x
s = c / x

(2) if A is not invertible
2.1 a = c = 0, the decomposition is not unique
2.2 (a,c) = k(b,d), for any k, the same solution as in (1), unique
*/
template<class real_type>
void qr_solve_2x2(const real_type A[2][2], real_type Q[2][2], real_type R[2][2])
{
	//assert((a != 0) || (c != 0));
	R[0][0] = sqrt(A[0][0]*A[0][0]+A[1][0]*A[1][0]);
	R[0][1] = (A[0][0]*A[0][1]+A[1][0]*A[1][1]) / R[0][0];
	R[1][0] = 0;
	R[1][1] = (A[0][0]*A[1][1]-A[0][1]*A[1][0]) / R[0][0];

	Q[0][0] = Q[1][1] = A[0][0] / R[0][0];
	Q[1][0] = A[1][0] / R[0][0];
	Q[0][1] = -Q[1][0];
}

///////////////////////////////////////////////////////////////////////////
/////////////// auxiliary functions
template<int nRows, int nCols, class ostream_type, class real_type>
void matrix_output(const real_type a[nRows][nCols], ostream_type& out)
{
	for (int i = 0; i < nRows; i++)
	{
		for (int j = 0; j < nCols; j++)
			out << a[i][j] << " ";
		out << endl;
	}
}

template<class real_type, class ostream_type>
void vector_output(const real_type* a, int N, ostream_type& out)
{
	for (int i = 0; i < N; i++)
		out << a[i] << " ";
	out << endl;
}