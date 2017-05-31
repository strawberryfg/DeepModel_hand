#include <iostream>
using namespace std;

#include "BitmapHelper.h"

void BitmapAndGdiplusSample()
{
	const WCHAR* filename = L"yichenw.bmp";
	Bitmap bmp(filename);

	if (bmp.GetLastStatus() != Ok)
		wcout << "can't read image from " << filename << endl;

	// to access to raw pixel data, use BitmapData
	BitmapData bmpData;
	bmp.LockBits(&Rect(0, 0, bmp.GetWidth(), bmp.GetHeight()), ImageLockModeRead, bmp.GetPixelFormat(), &bmpData);
	// access raw image buffer at bmpData.Scan0
	bmp.UnlockBits(&bmpData);	// need to unlock it, otherwise bmp is readonly

	// using Graphics to draw graphics on an image, for some visualization
	Graphics g(&bmp);

	Pen pen(Color(255,0,0), 2);

	Rect rc(0, 0, bmp.GetWidth(), bmp.GetHeight());
	rc.Inflate(-rc.Width/4, -rc.Height/4);
	g.DrawRectangle(&pen, rc);

	const WCHAR* result_file = L"result.jpg";
	if (SaveBitmap(bmp, L"image/jpeg", result_file))
		wcout << "result is saved to " << result_file << endl;
	else
		wcerr << "can't save result to " << result_file << endl;
}