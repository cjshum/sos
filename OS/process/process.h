#include "..\global.h"
#include <iostream>

using namespace std;

struct process
{
	int a;
	int p[6];
	bool isInCore;
	int cpuTimeUsed;

	process();
	process(int a, int p[]);
	bool isDoingIO();
	void setNewValues(int a, int p[]);
};