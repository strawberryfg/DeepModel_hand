#pragma once

// simple extensions for Gdiplus

#include "windows.h"
#include <gdiplus.h>
using namespace Gdiplus;

#include <iostream>
using namespace std;

namespace GdiplusEx
{
	inline ostream& operator<< (ostream& out, const PointF& p)
	{
		return out << p.X << " " << p.Y;
	}

	inline istream& operator>> (istream& in, PointF& p)
	{
		return in >> p.X >> p.Y;
	}

	inline ostream& operator<< (ostream& out, const Point& p)
	{
		return out << p.X << " " << p.Y;
	}

	inline istream& operator>> (istream& in, Point& p)
	{
		return in >> p.X >> p.Y;
	}

	inline void DrawCross(Graphics* g, Pen* pen, const PointF& pt, const float halfLength)
	{
		g->DrawLine(pen, pt.X-halfLength, pt.Y, pt.X+halfLength, pt.Y);
		g->DrawLine(pen, pt.X, pt.Y-halfLength, pt.X, pt.Y+halfLength);
	}

	inline void DrawCross(Graphics* g, Pen* pen, const Point& pt, const int halfLength)
	{
		g->DrawLine(pen, pt.X-halfLength, pt.Y, pt.X+halfLength, pt.Y);
		g->DrawLine(pen, pt.X, pt.Y-halfLength, pt.X, pt.Y+halfLength);
	}	

	inline void DrawString(Graphics* pGraphics
		,const WCHAR	*string
		,const float X, const float Y
		,const float font_size = 12.0f
		,Color color = Color::Red)
	{
		pGraphics->DrawString(string, -1, &Gdiplus::Font(L"Arial", font_size), PointF(X, Y), &SolidBrush(color));
	}

	inline RectF MeasureString(Graphics* pGraphics
		,const WCHAR	*string
		,const float X, const float Y
		,const float font_size = 12.0f)
	{
		RectF boundingBox;
		pGraphics->MeasureString(string, -1, &Gdiplus::Font(L"Arial", font_size), PointF(X, Y), &boundingBox);
		return boundingBox;
	}
}

#include <Utility\gdipluscolor_clean.h>

inline Color ToColor(const GdiplusColor& c)
{
	return Color(c.GetA(), c.GetR(), c.GetG(), c.GetB());
}