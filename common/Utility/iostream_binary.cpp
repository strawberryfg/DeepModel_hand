#include "iostream_binary.h"

#include <sstream>

void test_binary_iostream()
{
	ostringstream oss;
	ostream_binary out_bin(oss);

	char char_value0 = -5, char_value1;
	unsigned char uchar_value0 = 112, uchar_value1;

	short short_value0 = -3567, short_value1;
	unsigned short ushort_value0 = 8765, ushort_value1;

	int int_value0 = -8917320, int_value1;
	unsigned int uint_value0 = 82345145, uint_value1;

	float float_value0 = 90874.193, float_value1;
	double double_value0 = 10759183.20485901, double_value1;

	out_bin << char_value0 << uchar_value0 
		<< short_value0 << ushort_value0 
		<< int_value0 << uint_value0
		<< float_value0 << double_value0;

	istringstream iss(oss.str());
	istream_binary in_bin(iss);

	in_bin >> char_value1;
	if (char_value0 != char_value1) cout << "binary stream fail in char";

	in_bin >> uchar_value1;
	if (uchar_value0 != uchar_value1) cout << "binary stream fail in unsigned char";

	in_bin >> short_value1;
	if (short_value0 != short_value1) cout << "binary stream fail in short";

	in_bin >> ushort_value1;
	if (ushort_value0 != ushort_value1) cout << "binary stream fail in unsigned short";

	in_bin >> int_value1;
	if (int_value0 != int_value1) cout << "binary stream fail in int";

	in_bin >> uint_value1;
	if (uint_value0 != uint_value1) cout << "binary stream fail in unsigned int";

	in_bin >> float_value1;
	if (float_value0 != float_value1) cout << "binary stream fail in float";

	in_bin >> double_value1;
	if (double_value0 != double_value1) cout << "binary stream fail in double";

	cout << "binary iostream test succeeds!" << endl;
}