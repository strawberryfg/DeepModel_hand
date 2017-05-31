double innerproduct(double xx1, double yy1, double xx2, double yy2)
{
	return xx1 * xx2 + yy1 * yy2;
}

double crossproduct(double xx1, double yy1, double xx2, double yy2)
{
	return xx1 * yy2 - xx2 * yy1;
}

double l2norm(double xx1, double yy1)
{
	return sqrt(pow(xx1, 2) + pow(yy1, 2));
}