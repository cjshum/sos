#include "pcb.h"
#include <iostream>

pcb::pcb()
{
	head = NULL;
	tail = NULL;
}

void pcb::insertProcess(int a, int p[])
{
	pcb *newProcess = new pcb();
	newProcess->a = a;
	for (int i = 1; i < 6; ++i)
	{
		newProcess->p[i] = p[i];
	}
	newProcess->isInCore = false;

	if (head == NULL)
	{
		head = newProcess;
	}
	tail = newProcess;
}

void pcb::setInCore(bool inCoreVal)
{
	this->isInCore = inCoreVal;
}

bool pcb::isDoingIO()
{
	return (this->a == IO_REQ);
}