#pragma once

#include <windows.h>
#include <gdiplus.h>
using namespace Gdiplus;

#include <iostream>
using namespace std;

inline ostream& operator<<(ostream& out, const Rect& rc)
{
	return out << rc.X << " " << rc.Y << " " << rc.Width << " " << rc.Height;
}

inline istream& operator>>(istream& in, Rect& rc)
{
	return in >> rc.X >> rc.Y >> rc.Width >> rc.Height;
}