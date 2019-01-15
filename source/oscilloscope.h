#pragma once
#include <string>
using namespace std;

struct numExp;

double time2sec(int hour, int min, int sec, double msec,
	int HOUR, int MIN, int SEC, double MSEC);
numExp e2num(string numberStr);
string fixedfloat(float x, int precision);
void oscDualFormatter(string filename, string savefilename);
void oscSingleFormatter(string filename, string savefilename);
void oscilloscopeFormatter();
