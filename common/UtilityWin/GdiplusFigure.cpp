#include "GdiplusFigure.h"

#include <assert.h>
#include <iostream>
#include <algorithm>

//#include <atlstr.h>		// for CStringW

/*
	inline void Float2String(float value, int digit, CStringW& str)
	{
		if (digit == 0) str.Format(L"%d", int(value+0.5));
		else if (digit == 1) str.Format(L"%.1f", value);
		else if (digit == 2) str.Format(L"%.2f", value);
		else if (digit == 3) str.Format(L"%.3f", value);
		else str.Format(L"%.4f", value);
	}

	inline CStringW Float2String(float value, int digit)
	{
		CStringW s;
		Float2String(value, digit, s);
		return s;
	}
*/

namespace GdiplusEx
{
	void plot(Graphics* g, Pen* pen, float font_size, Color color, const vector<float>& X, const vector<float>& Y, const RectF& boundingBox, bool draw_bounding_box)
	{
		Figure figure(g, font_size);
		figure.AddData(X, Y, color);
		figure.SetBoundingBox(boundingBox);
		figure.Draw(pen, draw_bounding_box);
	}

#pragma push_macro("min")
#pragma push_macro("max")
	void plot(Graphics* g, Pen* pen, float font_size, const vector<float>& Y, Color color, const RectF& boundingBox, bool draw_bounding_box)
	{
		float max_value = *max_element(Y.begin(), Y.end());
		float min_value = *min_element(Y.begin(), Y.end());

		if (max_value == min_value)
		{
			max_value += 1;
			min_value -= 1;
		}

		// apply transformation
		const float DATA_WIDTH = Y.size();
		const float DATA_HEIGHT = max_value - min_value;
		const float sx = boundingBox.Width / DATA_WIDTH;
		const float sy = boundingBox.Height / DATA_HEIGHT;
		g->TranslateTransform(boundingBox.X, boundingBox.Y);
		g->ScaleTransform(sx, sy);
		g->TranslateTransform(0, max_value);
		g->ScaleTransform(1.0, -1.0);
		const float old_width = pen->GetWidth();
		pen->SetWidth(old_width / max(sx, sy));

		// draw data
		vector<PointF> points(Y.size());	
		for(unsigned int n = 0; n < Y.size(); n++)
		{
			points[n].X = n;
			points[n].Y = Y[n];
		}
		g->DrawLines(pen, &points[0], points.size());

		// restore transformation
		pen->SetWidth(old_width);
		g->ScaleTransform(1.0, -1.0);
		g->TranslateTransform(0, -max_value);
		g->ScaleTransform(1.0f/sx, 1.0f/sy);
		g->TranslateTransform(-boundingBox.X, -boundingBox.Y);

		if (draw_bounding_box) g->DrawRectangle(&Pen(Color::Black), boundingBox);
	}
#pragma pop_macro("min")
#pragma pop_macro("max")

	Figure::Axis::Axis(const WCHAR* l, int d) : limits(0.0f, 0.0f)
		,label(l), digit(d), first_set_limits(true)
	{
	}

	void Figure::Axis::SetLimits(float min_value, float max_value)
	{
		assert(min_value < max_value);
		if (first_set_limits)
		{
			first_set_limits = false;
			limits.first = min_value;
			limits.second = max_value;
		}
		else
		{
			limits.first = min(limits.first, min_value);
			limits.second = max(limits.second, max_value);
		}	
	}

	void Figure::Axis::SetLimits(const vector<float>& data)
	{
		if (first_set_limits)
		{
			first_set_limits = false;
			limits.first = *min_element(data.begin(), data.end());
			limits.second = *max_element(data.begin(), data.end());
		}
		else
		{
			limits.first = min(limits.first, *min_element(data.begin(), data.end()));
			limits.second = max(limits.second, *max_element(data.begin(), data.end()));
		}

		if (limits.first == limits.second)
		{
			limits.first -= 1;
			limits.second += 1;
		}
	}

	void Figure::Axis::Draw(const RectF& boundingBox, Graphics* g, float font_size, Color color)
	{

	}

	Figure::XAxis::XAxis(const WCHAR* l, int d) : Axis(l, d)
	{
	}

	void Figure::XAxis::Draw(const RectF& boundingBox, Graphics* g, float font_size, Color color)
	{
		RectF rc;
		float tick_width, tick_height;

		// draw min value
		// CStringW str; Float2String(limits.first, digit, str);
		
		WCHAR str[255]; // TODO: not done
		rc = MeasureString(g, str, 0, 0, font_size);
		DrawString(g, str, boundingBox.X-rc.Width/2, boundingBox.GetBottom(), font_size, color);

		// draw max value
		//Float2String(limits.second, digit, str);

		rc = MeasureString(g, str, 0, 0, font_size);
		tick_width = rc.Width;
		tick_height = rc.Height;	
		DrawString(g, str, boundingBox.GetRight()-rc.Width/2, boundingBox.GetBottom(), font_size, color);

		// draw ticks

		// draw label
		rc = MeasureString(g, label.c_str(), 0, 0, font_size);

		DrawString(g, label.c_str(), boundingBox.X+boundingBox.Width/2-rc.Width/2, boundingBox.GetBottom()+tick_height, font_size, color);
	}

	Figure::YAxis::YAxis(const WCHAR* l, int d, bool _on_right) : Axis(l, d), on_right(_on_right)
	{
	}

	void Figure::YAxis::Draw(const RectF& boundingBox, Graphics* g, float font_size, Color color)
	{
		RectF rc;
		float tick_width, tick_height;

		// draw min value
		//CStringW str; Float2String(limits.first, digit, str);
		WCHAR str[255]; // TODO: not done
		rc = MeasureString(g, str, 0, 0, font_size);

		if (!on_right)
			DrawString(g, str, boundingBox.X-rc.Width, boundingBox.GetBottom()-rc.Height/2, font_size, color);
		else
			DrawString(g, str, boundingBox.GetRight(), boundingBox.GetBottom()-rc.Height/2, font_size, color);

		// draw max value
		//Float2String(limits.second, digit, str);
		rc = MeasureString(g, str, 0, 0, font_size);
		tick_width = rc.Width;
		tick_height = rc.Height;
		if (!on_right)
			DrawString(g, str, boundingBox.X-rc.Width, boundingBox.Y-rc.Height/2, font_size, color);
		else
			DrawString(g, str, boundingBox.GetRight(), boundingBox.Y-rc.Height/2, font_size, color);

		// draw ticks

		// draw label
		rc = MeasureString(g, label.c_str(), 0, 0, font_size);
		if (!on_right)
			DrawString(g, label.c_str(), boundingBox.X-rc.Width-tick_width, boundingBox.Y+boundingBox.Height/2-rc.Height/2, font_size, color);
		else
			DrawString(g, label.c_str(), boundingBox.GetRight()+tick_width, boundingBox.Y+boundingBox.Height/2-rc.Height/2, font_size, color);
	}

	int Figure::s_nDigitX = 0;
	int Figure::s_nDigitY = 1;

	void Figure::SetDefaultLabelDigits(const int x_digit, const int y_digit)
	{
		s_nDigitX = x_digit;
		s_nDigitY = y_digit;
	}

	Figure::Figure(Graphics* _g, float _font_size, bool y_on_right) : g(_g), font_size(_font_size)
		,x_axis(L"x", s_nDigitX), y_axis(L"y", s_nDigitY, y_on_right), sx(1.0f), sy(1.0f), label_line_length(10.0f)
	{	
	}

	void Figure::SetLabelDigits(const int x_digit, const int y_digit)
	{
		x_axis.digit = x_digit;
		y_axis.digit = y_digit;
	}

	void Figure::SetLabelText(const WCHAR* x_text, const WCHAR* y_text)
	{
		x_axis.label = x_text;
		y_axis.label = y_text;
	}

	void Figure::SetXLimits(const float min_value, const float max_value)
	{
		x_axis.SetLimits(min_value, max_value);
	}

	void Figure::SetYLimits(const float min_value, const float max_value)
	{
		y_axis.SetLimits(min_value, max_value);
	}

	//void Figure::SetYAxisOnRight(bool b) {	y_axis.on_right = b; }

	void Figure::SetLabelLineWidth(float l)
	{
		label_line_length = l;
	}

	void Figure::AddData(const vector<float>& Y, Color color, const WCHAR* label)
	{
		x_axis.SetLimits(0, Y.size()-1);
		y_axis.SetLimits(Y);
		data.push_back(Data(NULL, &Y, color, label));
	}

	bool Figure::AddData(const vector<float>& X, const vector<float>& Y, Color color, const WCHAR* label)
	{
		if (X.size() != Y.size())
		{
			cout << "dimension of X and Y is not equal, cannot add data" << endl;
			return false;
		}

		x_axis.SetLimits(X);
		y_axis.SetLimits(Y);
		data.push_back(Data(&X, &Y, color, label));
		return true;
	}

	void Figure::SetBoundingBox(const RectF& boundingBox)
	{
		bound_box = boundingBox;

		assert(x_axis.limits.second > x_axis.limits.first);
		assert(y_axis.limits.second > y_axis.limits.first);

		sx = boundingBox.Width / (x_axis.limits.second - x_axis.limits.first);
		sy = boundingBox.Height / (y_axis.limits.second - y_axis.limits.first);
	}

	void Figure::TransformPoint(PointF& pt)
	{
		pt.X = (pt.X - x_axis.limits.first) * sx + bound_box.X;
		pt.Y = (y_axis.limits.second - pt.Y) * sy + bound_box.Y;
	}

	void Figure::ApplyTransformation()
	{
		g->TranslateTransform(bound_box.X, bound_box.Y);
		g->ScaleTransform(sx, sy);
		g->TranslateTransform(-x_axis.limits.first, y_axis.limits.second);
		g->ScaleTransform(1.0, -1.0);
	}

	void Figure::ApplyInvTransformation()
	{
		g->ScaleTransform(1.0, -1.0);
		g->TranslateTransform(x_axis.limits.first, -y_axis.limits.second);
		g->ScaleTransform(1.0f/sx, 1.0f/sy);
		g->TranslateTransform(-bound_box.X, -bound_box.Y);
	}

	void Figure::DrawLines(Pen* pen, const vector< pair<PointF, PointF> >& lines)
	{
		// apply transformation
		ApplyTransformation();
		const float old_width = pen->GetWidth();
		pen->SetWidth(old_width / max(sx, sy));

		for(unsigned int n = 0; n < lines.size(); n++)
			g->DrawLine(pen, lines[n].first, lines[n].second);

		// restore transformation
		pen->SetWidth(old_width);
		ApplyInvTransformation();
	}

	void Figure::DrawData(Pen* pen)
	{
		// apply transformation
		ApplyTransformation();
		Color old_color;
		pen->GetColor(&old_color);
		const float old_width = pen->GetWidth();
		pen->SetWidth(old_width / max(sx, sy));

		for(int nData = 0; nData < data.size(); nData++)
		{
			const Data& a = data[nData];
			pen->SetColor(a.color);
			vector<PointF> points(a.Y->size());
			if (a.X == NULL)
				for(unsigned int n = 0; n < a.Y->size(); n++)
				{
					points[n].X = n;
					points[n].Y = (*a.Y)[n];
				}
			else 
				for(unsigned int n = 0; n < a.Y->size(); n++)			
				{
					points[n].X = (*a.X)[n];
					points[n].Y = (*a.Y)[n];
				}
				g->DrawLines(pen, &points[0], points.size());
		}

		// restore transformation
		pen->SetColor(old_color);
		pen->SetWidth(old_width);
		ApplyInvTransformation();
	}

	void Figure::Draw(Pen* pen, bool draw_bounding_box)
	{
		DrawData(pen);

		// draw data label
		float max_label_width = 0.0f, label_height = 0.0f;
		for(int nData = 0; nData < data.size(); nData++)	// measure size
		{
			RectF rc = MeasureString(g, data[nData].label.c_str(), 0, 0, font_size);
			if (rc.Width > max_label_width) max_label_width = rc.Width;
			if (rc.Height > label_height) label_height = rc.Height;
		}

		label_line_length = bound_box.Width / 20.0f;
		float text_X = y_axis.on_right ? bound_box.X + 10 + label_line_length : bound_box.GetRight() - 10 - max_label_width;
		float text_Y = bound_box.Y + 10;
		if (label_height > 0) for(int nData = 0; nData < data.size(); nData++)	// draw string
		{		
			g->DrawLine(&Pen(data[nData].color), text_X-label_line_length-5, text_Y+label_height/2, text_X-5, text_Y+label_height/2);
			DrawString(g, data[nData].label.c_str(), text_X, text_Y, font_size, data[nData].color);
			text_Y += label_height;
		}

		if (draw_bounding_box) g->DrawRectangle(&Pen(Color::Black), bound_box);

		// draw axis
		Pen black_pen(Color::Black);
		// x axis
		g->DrawLine(&black_pen, bound_box.X, bound_box.GetBottom(), bound_box.GetRight(), bound_box.GetBottom());

		// y axis
		if (!y_axis.on_right)
			g->DrawLine(&black_pen, bound_box.X, bound_box.GetBottom(), bound_box.X, bound_box.Y);
		else
			g->DrawLine(&black_pen, bound_box.GetRight(), bound_box.GetBottom(), bound_box.GetRight(), bound_box.Y);

		x_axis.Draw(bound_box, g, font_size);
		y_axis.Draw(bound_box, g, font_size);
	}

	void Figure::DrawPoint(Pen* pen, PointF& pt, bool on_right, float size)
	{
		//CStringW str; str = "(" + Float2String(pt.X, x_axis.digit) + ", " + Float2String(pt.Y, y_axis.digit) + ")";
		WCHAR str[255]; // TODO: not done
		DrawCrossAndStr(pen, pt, str, on_right, size);
	}

	void Figure::DrawPointValue(Pen* pen, PointF& pt, float value, int digit, bool on_right, float size)
	{
		//CStringW str; Float2String(value, digit, str);
		WCHAR str[255]; // TODO: not done
		DrawCrossAndStr(pen, pt, str, on_right, size);
	}

	void Figure::DrawCrossAndStr(Pen* pen, PointF& pt, const WCHAR* str, bool on_right, float size)
	{
		TransformPoint(pt);
		DrawCross(g, pen, pt, size);
		Color color;
		pen->GetColor(&color);
		RectF rc = MeasureString(g, str, 0, 0, font_size);
		if (on_right)
			DrawString(g, str, pt.X, pt.Y-rc.Height/2, font_size, color);
		else
			DrawString(g, str, pt.X-rc.Width, pt.Y-rc.Height/2, font_size, color);
	}

	void Figure::DrawLine(Pen* pen, PointF& pt0, PointF& pt1)
	{
		TransformPoint(pt0);
		TransformPoint(pt1);
		g->DrawLine(pen, pt0, pt1);
	}

}