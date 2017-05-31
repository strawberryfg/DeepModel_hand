#pragma once

#include <string>
using namespace std;

////////////////////////////////////////////////////////////
// Matlab like timing functions
// example usage
/*
tic();
for(...)
{
do_something();
double seconds;
toc(seconds, false);
printf("seconds : %f", seconds);
}
toc("total time : ");
*/

void tic();
string toc(const char* tip = "", bool over = true);
bool toc(int& mins, double& seconds, bool over = true);
bool toc(double& seconds, bool over = true);


