#pragma once

#include <windows.h>
#include <unknwn.h>  // TODO: why MSVC automatically define WIN32_LEAN_AND_MEAN before entering <windows.h>?
#include <Gdiplus.h>
using namespace Gdiplus;

#include <string>
using namespace std;

#include <memory>

string GetPixelFormatStr(PixelFormat format);

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

// format values : L"image/jpeg", L"image/bmp"
bool SaveBitmap(Bitmap& bitmap, const WCHAR* format, const WCHAR* filename);
bool SaveBitmap(Bitmap& bitmap, const WCHAR* format, const string& filename);

bool SaveRGBImage(const unsigned char* buffer, unsigned int width, unsigned int height, unsigned int stride
	, const WCHAR* format, const string& filename);

// a wrapper for easier usage without the stride parameter, so caller does not need to care about whether 3*width is multiplies of 4
// if 3*width is multiplies, it calls the original version
// otherwise it creates a new buffer with appropriate stride
bool SaveRGBImage(const unsigned char* buffer, unsigned int width, unsigned int height
	, const WCHAR* format, const string& filename);

// if succeed, return a newed unique pointer, which can be safely used the caller without delete
// otherwise return NULL
unique_ptr<Bitmap> LoadBitmap(const string& filename);

// visualize float buffer as rgb and save
#include <gadget\color_convert.h>

template<class FloatType, class IsValidPredicate>
bool SaveFloatAsRGBImage(const FloatType* buffer, unsigned int width, unsigned int height, bool is_ascending
	, const WCHAR* format, const string& filename, IsValidPredicate isValueValid = is_always_true<FloatType>())
{
	vector<unsigned char> rgb(3*width*height, 0);

	if (!NormalizeToRGB(buffer, width*height, is_ascending, &rgb[0], isValueValid))
	{
		cout << "fail to normalize to RGB" << endl;
		return false;
	}

	if (!SaveRGBImage(&rgb[0], width, height, format, filename))
	{
		cout << "can't save distance transform result " << endl;
		return false;
	}

	return true;
}