#include "process.h"

process::process(int a, int p[])
{
	this->a = a;
	for (int i = 1; i <= 5; ++i)
	{
		this->p[i] = p[i];
	}
	this->isInCore = false;
}

void process::setInCoreVal(bool inCoreVal)
{
	this->isInCore = inCoreVal;
}

bool process::isInCore()
{
	return this->isInCore;
}