#pragma once

////////////////////////////////////////////////////////////
// windows.h based timing functions
// example usage
/*
Timer timer;
step1();
float flDeltaLast, flDeltaStart;
timer.Stamp(flDeltaLast, flDeltaStart);
printf("time from start : %fs, time in step 1: %fs", flDeltaLast, flDeltaStart);

step2();
timer.Stamp(flDeltaLast, flDeltaStart);
printf("time from start : %fs, time in step 2: %fs", flDeltaLast, flDeltaStart);
*/
#include <Windows.h>
class Timer
{
public:
	Timer()
	{
		QueryPerformanceFrequency(&m_iFrequency);
		QueryPerformanceCounter(&m_iLast);
		m_iStart = m_iLast;
	}

	// return time measured in seconds
	float Stamp()
	{
		LARGE_INTEGER iNow = {0};
		QueryPerformanceCounter(&iNow);
		return float(iNow.QuadPart - m_iStart.QuadPart) / m_iFrequency.QuadPart;
	}

	float StampFromLast()
	{
		LARGE_INTEGER iNow = {0};
		QueryPerformanceCounter(&iNow);
		float flDeltaLast = float(iNow.QuadPart - m_iLast.QuadPart) / m_iFrequency.QuadPart;
		m_iLast = iNow;
		return flDeltaLast;
	}

	void Stamp(float& flDeltaLast, float& flDeltaStart)
	{
		LARGE_INTEGER iNow = {0};
		QueryPerformanceCounter(&iNow);
		flDeltaLast = float(iNow.QuadPart - m_iLast.QuadPart) / m_iFrequency.QuadPart;
		flDeltaStart = float(iNow.QuadPart - m_iStart.QuadPart) / m_iFrequency.QuadPart;
		m_iLast = iNow;
	}

protected:
	LARGE_INTEGER m_iStart;
	LARGE_INTEGER m_iLast;
	LARGE_INTEGER m_iFrequency;
};

#include <string>
#include <iostream>
using namespace std;

class OneShotTimer
{
	Timer t;
	string tip;

public:
	OneShotTimer(const char* _tip) : tip(_tip) {}
	~OneShotTimer() 
	{
		cout << tip << ": " << t.Stamp() * 1000 << " ms" << endl;
	}
};