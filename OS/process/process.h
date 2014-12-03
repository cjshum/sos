#include "..\Global.h"
#include <iostream>

#pragma once

enum {READY, DOING_IO, SWAP_TO_CORE, SWAP_TO_DRUM, BLOCKED};

using namespace std;

class process
{
public:
	int a;
	int p[6];
	int maxCpuTime;
	int cpuTimeUsed;
	int currentStatus;
	bool isDoingIO;

	process();
	process(int a, int p[]);
	void setNewValues(int a, int p[]);
};