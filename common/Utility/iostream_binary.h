#pragma once 

// iostream based binary I/O support, in a similar manner as using operator << and >>
#include <iostream>
using namespace std;

class ostream_binary
{
public:
	ostream& os;
	explicit ostream_binary(ostream& _os) : os(_os) {}

#define DECLARE_OUTPUT_TYPE(TYPE)\
	friend ostream_binary& operator<<(ostream_binary& out, const TYPE& v)\
	{\
		out.os.write((char*)(&v), sizeof(TYPE));\
		return out;\
	}

	DECLARE_OUTPUT_TYPE(char)
	DECLARE_OUTPUT_TYPE(unsigned char)
	DECLARE_OUTPUT_TYPE(short)
	DECLARE_OUTPUT_TYPE(unsigned short)
	DECLARE_OUTPUT_TYPE(int)
	DECLARE_OUTPUT_TYPE(unsigned int)
	DECLARE_OUTPUT_TYPE(float)
	DECLARE_OUTPUT_TYPE(double)

	template<class Type>
	ostream_binary& write(const Type* pV, const int len)
	{
		os.write((char*)pV, sizeof(Type)*len);
		return *this;
	}
};

class istream_binary
{
public:
	istream& is;
	explicit istream_binary(istream& _is) : is(_is) {}

	bool operator!() const	
	{
		return is.fail();
	}

#define DECLARE_INPUT_TYPE(TYPE)\
	friend istream_binary& operator>>(istream_binary& in, TYPE& v)\
	{\
		in.is.read((char*)(&v), sizeof(TYPE));\
		return in;\
	}

	DECLARE_INPUT_TYPE(char)
	DECLARE_INPUT_TYPE(unsigned char)
	DECLARE_INPUT_TYPE(short)
	DECLARE_INPUT_TYPE(unsigned short)
	DECLARE_INPUT_TYPE(int)
	DECLARE_INPUT_TYPE(unsigned int)
	DECLARE_INPUT_TYPE(float)
	DECLARE_INPUT_TYPE(double)

	template<class Type>
	istream_binary& read(Type* pV, const int len)
	{
		is.read((char*)pV, sizeof(Type)*len);
		return *this;
	}
};

void test_binary_iostream();