#include "..\global.h"
#include <iostream>

using namespace std;

struct process
{
	int a;
	int p[6];
	int isInCore;

	process(int a, int p[]);
	bool isDoingIO();
};