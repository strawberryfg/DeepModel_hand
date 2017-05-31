#include "BitmapHelper.h"

std::string GetPixelFormatStr(PixelFormat format)
{
	switch (format)
	{
	case PixelFormat1bppIndexed: return "PixelFormat1bppIndexed";
	case PixelFormat4bppIndexed: return "PixelFormat4bppIndexed";
	case PixelFormat8bppIndexed: return "PixelFormat8bppIndexed";
	case PixelFormat16bppGrayScale: return "PixelFormat16bppGrayScale";
	case PixelFormat16bppRGB555: return "PixelFormat16bppRGB555";
	case PixelFormat16bppRGB565: return "PixelFormat16bppRGB565";
	case PixelFormat16bppARGB1555: return "PixelFormat16bppARGB1555";
	case PixelFormat24bppRGB: return "PixelFormat24bppRGB";
	case PixelFormat32bppRGB: return "PixelFormat32bppRGB";
	case PixelFormat32bppARGB: return "PixelFormat32bppARGB";
	case PixelFormat32bppPARGB: return "PixelFormat32bppPARGB";
	case PixelFormat48bppRGB: return "PixelFormat48bppRGB";
	case PixelFormat64bppARGB: return "PixelFormat64bppARGB";
	case PixelFormat64bppPARGB: return "PixelFormat64bppPARGB";
	case PixelFormat32bppCMYK: return "PixelFormat32bppCMYK";

	default: return "Unknown";
	};
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	using Gdiplus::ImageCodecInfo;
	using Gdiplus::GetImageEncodersSize;
	using Gdiplus::GetImageEncoders;

	UINT  num = 0;          /// number of image encoders
	UINT  size = 0;         /// size of the image encoder array in bytes

	/// get the number of codecs in list
	GetImageEncodersSize(&num, &size);
	if (size == 0) return -1;  /// Failure
	if (size <= num * sizeof(ImageCodecInfo)) return -1;

	/// Allocate the memory, which maybe larger than ImageCodeInfo array
	BYTE* pBuffer = new BYTE[size];
	if (pBuffer == NULL) return -1;  /// Failure

	/// Bind the head of buffer to arrray of codec infos
	ImageCodecInfo* pImageCodecInfo = (ImageCodecInfo*) (void*) pBuffer;

	/// read the codec info
	GetImageEncoders(num, size, pImageCodecInfo);

	/// enumerate and check the name
	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			delete[] pBuffer;
			return j;  /// Success
		}    
	}

	delete[] pBuffer;
	return -1;  /// Failure
}

bool SaveBitmap(Bitmap& bitmap, const WCHAR* format, const WCHAR* filename)
{
	CLSID  encoderClsid;
	if (GetEncoderClsid(format, &encoderClsid) < 0)
		return false;
	Status s = bitmap.Save(filename, &encoderClsid);
	return s == Ok;
}

#include <vector>

bool SaveBitmap(Bitmap& bitmap, const WCHAR* format, const string& filename)
{
	vector<WCHAR> wstr(filename.size() + 1);
	size_t num;
	mbstowcs_s(&num, &wstr[0], wstr.size(), filename.c_str(), filename.size());

	return SaveBitmap(bitmap, format, &wstr[0]);
}

bool SaveRGBImage(const unsigned char* buffer, unsigned int width, unsigned int height, unsigned int stride
	, const WCHAR* format, const string& filename)
{
	Bitmap bmp(width, height, stride, PixelFormat24bppRGB, const_cast<BYTE*>(buffer));
	return SaveBitmap(bmp, format, filename);
}

bool SaveRGBImage(const unsigned char* buffer, unsigned int width, unsigned int height
	, const WCHAR* format, const string& filename)
{
	if (3*width % 4 == 0)
		return SaveRGBImage(buffer, width, height, 3*width, format, filename);

	// TODO: NOT tested yet!
	unsigned int stride = (3*width / 4 + 1) * 4;
	vector<unsigned char> aug_buffer(stride*height);
	for(unsigned int y = 0; y < height; y++)
	{
		memcpy(&aug_buffer[y*stride], buffer+y*3*width, 3*width);
	}
	return SaveRGBImage(&aug_buffer[0], width, height, stride, format, filename);
}

unique_ptr<Bitmap> LoadBitmap(const string& filename)
{
	vector<WCHAR> wstr(filename.size() + 1);
	WCHAR* ws = &wstr[0];
	size_t num;
	mbstowcs_s(&num, ws, wstr.size(), filename.c_str(), filename.size());

	unique_ptr<Bitmap> pBmp(new Bitmap(ws));
	//Bitmap* pBmp = new Bitmap(ws);

	if (pBmp->GetLastStatus() != Ok)
	{
		//delete pBmp;
		pBmp.reset();
		return NULL;
	}
	else return pBmp;
}