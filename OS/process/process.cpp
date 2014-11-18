#include "process.h"

process::process(int a, int p[])
{
	this->a = a;
	for (int i = 1; i <= 5; ++i)
	{
		this->p[i] = p[i];
	}
	isInCore = false;
}

bool process::isDoingIO()
{
	return (a == IO_REQ);
}