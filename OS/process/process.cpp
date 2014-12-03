#include "process.h"

process::process() {}

process::process(int a, int p[])
{
	this->a = a;
	for (int i = 1; i <= 5; ++i)
	{
		this->p[i] = p[i];
	}
}

void process::setNewValues(int a, int p[])
{
	this->a = a;
	for (int i = 1; i <= 5; ++i)
	{
		this->p[i] = p[i];
	}
}