#include "time_ex.h"

#include <time.h>

#include <vector>
using namespace std;

vector<clock_t> start;

void tic()	{	start.push_back(clock());	}

string toc(const char* tip, bool over)
{	
	char msg[512];
	int mins;
	double secs;
	if (toc(mins, secs, over))
		if (mins == 0) sprintf_s(msg, 512, "%s %5.2f secs", tip, secs);
		else sprintf_s(msg, 512, "%s %d min(s) %5.2f secs", tip, mins, secs);
	else
		sprintf_s(msg, 512, "%s : unknown time");
	return msg;
}

bool toc(int& mins, double& seconds, bool over)
{	
	if (start.empty())
	{
		//printf("cannot call toc() without calling tic()\n");
		return false;
	}
	double duration = (double)(clock() - start.back()) / CLOCKS_PER_SEC;
	if (over) start.pop_back();
	mins = int(duration / 60);
	seconds = duration  -  60 * mins;
	return true;
}

bool toc(double& seconds, bool over)
{
	if (start.empty())
	{
		//printf("cannot call toc() without calling tic()\n");
		return false;
	}
	seconds = (double)(clock() - start.back()) / CLOCKS_PER_SEC;
	if (over) start.pop_back();
	return true;
}