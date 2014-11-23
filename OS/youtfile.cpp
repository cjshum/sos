#include "global.h"
#include "process\process.h"

#include "stdio.h"
#include <iostream>
#include <queue>
using namespace std;

// OS Prototype Functions
void siodisk(int jobnum);
void siodrum(int jobnum, int jobsize, int coreaddress, int direction);
void ontrace();
void offtrace();

// Our Prototype Functions
void Crint(int &a, int p[]);
void Dskint(int &a, int p[]);
void Drmint(int &a, int p[]);
void Tro(int &a, int p[]);
void Svc(int &a, int p[]);

//user defined functions
static void verbose(char* interruptType, int &a, int p[]);
//user defined variables
queue<process> *readyQueue;
queue<process> *ioQueue;
queue<process> *waitingQueue;
// global variables for calculating how much cpu time a job used
int maxCpuTime;
int timer;

void startup()
{	system("cls"); // Clears output from previous run
	//ontrace();

	// initialize queues
	readyQueue = new queue<process>();
	ioQueue = new queue<process>();
	waitingQueue = new queue<process>();
}

void Crint(int &a, int p[])
{	verbose("Crint", a, p); //let's say i move to starting address 10
	readyQueue->push(process(a, p)); // save the job information
	siodrum(p[JOB_NUM], p[JOB_SIZE], 0, TO_CORE); // move job into memory
	timer = 0; // initalizes the timer
	maxCpuTime = p[JOB_MAX_TIME]; // save the max time allowed information
}

void Dskint(int &a, int p[])
{	verbose("Dskint", a, p);
	// pop the job that finshed i/o out of the i/o queue
	if (!ioQueue->empty())
	{	ioQueue->pop();
		readyQueue->push(process(a, p));
	}

	// if there are more jobs wanting to do i/o,
	// let them do io
	if (ioQueue->size())
	{	siodisk(ioQueue->front().p[JOB_NUM]);
	}
}

void Drmint(int &a, int p[])
{
	verbose("Drmint", a, p);
	// if a job is moved to memory
	// if there is a job on the ready queue run it
	if (readyQueue->size() == 1)
	{	a = CPU_RUN;
		p[JOB_ADDR] = 0;
		p[JOB_MAX_TIME] = 1;
		
		readyQueue->front().setNewValues(a, p);
		readyQueue->front().isInCore = true;
	}
	// otherwise if the program is not in memory
	// and it is on the ready queue instead
	else if (waitingQueue->front().p[JOB_NUM] = p[JOB_NUM])
	{
		// move the job into memory
		siodrum(p[JOB_NUM], p[JOB_SIZE], 0, TO_CORE);
		readyQueue->push(process(a, p));
		waitingQueue->pop();
		a = CPU_IDLE;
	}
}

void Tro(int &a, int p[])
{	verbose("Tro", a, p);
	// usually run the program
	a = CPU_RUN;
	timer++;
	printf("Current CPU runtime: %i\n", timer);
	printf("Max CPU runtime: %i\n\n", maxCpuTime);

	// but if the job is trying to go over the requested time
	// terminate it
	if (timer == maxCpuTime)
	{
		a = REQ_TERM;
		Svc(a, p);
	}
}

void Svc(int &a, int p[])
{	verbose("Svc", a, p);
	switch (a)
	{
	// if a job request termination
	case REQ_TERM:
		if (!readyQueue->empty())
			readyQueue->pop();
		a = CPU_IDLE;
		system("pause");
		break;

	//if a job requests io
	case REQ_IO:
		if (!readyQueue->empty())
		{	if (readyQueue->front().p[JOB_NUM] == p[JOB_NUM])
			{	readyQueue->pop();
			}
		}
		ioQueue->push(process(a, p));
		if (ioQueue->size() > 0)
		{	siodisk(p[JOB_NUM]);
			a = CPU_RUN;
		}
		break;

	// if a job requests blocking
	case REQ_BLOCK:
		if (ioQueue->empty())
		{	if (readyQueue->front().p[JOB_NUM] == p[JOB_NUM])
			{	siodrum(p[JOB_NUM], p[JOB_SIZE], 0, TO_DRUM);
				waitingQueue->push(process(a, p));
				readyQueue->pop();
			}
		}
		a = CPU_IDLE;
		break;
	}
}

static void verbose(char* interruptType, int &a, int p[])
{
	printf("\nJob no[%i] arrived to %s\n", p[JOB_NUM], interruptType);
	printf("Status: %s\n", aValString[a]);
	printf("Priority: %i\n", p[JOB_ADDR]);
	printf("Job size: %i\n", p[JOB_SIZE]);
	printf("Max CPU time: %i\n", p[JOB_MAX_TIME]);
	printf("Current time: %i\n", p[JOB_CURR_TIME]);
	printf("Items in Ready Queue: %i\n", readyQueue->size());
	printf("Items in IO Queue: %i\n", ioQueue->size());
	printf("Items in Waiting Queue: %i\n", waitingQueue->size());
	//printf("\n");
	//system("pause");
}