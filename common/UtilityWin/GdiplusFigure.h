#pragma once

#include "windows.h"
#include <gdiplus.h>
using namespace Gdiplus;

#include <vector>
using namespace std;

namespace GdiplusEx
{
	void plot(Graphics* g, Pen* pen, float font_size, Color color, const vector<float>& Y, const RectF& boundingBox, bool draw_bounding_box = false);
	void plot(Graphics* g, Pen* pen, float font_size, Color color, const vector<float>& X, const vector<float>& Y, const RectF& boundingBox, bool draw_bounding_box = false);

	// a simulation of Matlab Figure
	class Figure
	{
		class Axis
		{
		public:
			Axis(const WCHAR* l, int d);

			// SetLimits() could be called multiple times to expand the limits
			void SetLimits(float min_value, float max_value);
			void SetLimits(const vector<float>& data);

			virtual void Draw(const RectF& boundingBox, Graphics* g, float font_size, Color color = Color::Black);
		
			pair<float, float> limits;
			wstring label;
			unsigned int digit;

		private:
			bool first_set_limits;	// true when SetLimits() has not been called yet
		};

		class XAxis : public Axis
		{
		public:
			XAxis(const WCHAR* l, int d);
			virtual void Draw(const RectF& boundingBox, Graphics* g, float font_size, Color color = Color::Black);
		};

		class YAxis : public Axis
		{
		public:
			YAxis(const WCHAR* l, int d, bool _on_right);
			virtual void Draw(const RectF& boundingBox, Graphics* g, float font_size, Color color = Color::Black);

			bool on_right;	// if true, draw the axis on right
		};

		struct Data
		{
			Data(const vector<float>* _X, const vector<float>* _Y, Color _color, const WCHAR* l) 
				: X(_X), Y(_Y), color(_color), label(l){}
			const vector<float> *X, *Y;
			Color color;
			wstring label;
		};

	public:
		static void SetDefaultLabelDigits(const int x_digit, const int y_digit);
	private:
		static int s_nDigitX, s_nDigitY;

	public:
		Figure(Graphics* _g, float _font_size = 10.0f, bool y_on_right = false);

		void SetLabelDigits(const int x_digit, const int y_digit);
		void SetLabelText(const WCHAR* x_text, const WCHAR* y_text);		
		void SetXLimits(const float min_value, const float max_value);
		void SetYLimits(const float min_value, const float max_value);
		//void SetYAxisOnRight(bool b);
		void SetLabelLineWidth(float l);

		// data are added into Figure object as reference, therefore their lifetime should be no shorter than the Figure object
		void AddData(const vector<float>& Y, Color color = Color::Blue, const WCHAR* label = L"");
		bool AddData(const vector<float>& X, const vector<float>& Y, Color color = Color::Blue, const WCHAR* label = L"");

		// call SetBoundingBox() after all data is added
		// once bounding box is set, the transformation can be determined
		void SetBoundingBox(const RectF& boundingBox);

		// transform a logical coordinate (in data) to physical coordinate (in figure)
		void TransformPoint(PointF& pt);

		// draw all stored data
		void Draw(Pen* pen, bool draw_bounding_box = false);

		// draw single element in logical coordinate, apply transformation manually
		void DrawPoint(Pen* pen, PointF& pt, bool on_right = true, float size = 3.0f);
		void DrawPointValue(Pen* pen, PointF& pt, float value, int digit, bool on_right = true, float size = 3.0f);
		
		void DrawLine(Pen* pen, PointF& pt0, PointF& pt1);

		// draw batch elements in logical coordinate, apply transformation automatically
		void DrawLines(Pen* pen, const vector< pair<PointF, PointF> >& lines);

	private:
		void ApplyTransformation();
		void ApplyInvTransformation();

		void DrawData(Pen* pen);

		void DrawCrossAndStr(Pen* pen, PointF& pt, const WCHAR* str, bool on_right = true, float size = 3.0f);

		Graphics* g;
		RectF bound_box;
		float font_size;
		float label_line_length;
		XAxis x_axis;
		YAxis y_axis;
		vector<Data> data;
		float sx, sy;			// scale transformation parameters
	};
}