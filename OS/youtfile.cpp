#include "global.h"
//#include "process\process.h"

#include "stdio.h"
#include <iostream>
#include <queue>
using namespace std;

void siodisk(int jobnum);
void siodrum(int jobnum, int jobsize, int coreaddress, int direction);

// Channel commands siodisk and siodrum are made available to you by the simulator.
// siodisk has one argument: job number, of type int and passed by value.
// siodrum has four arguments, all of type int and passed by value:
// first argument is job number;
// second argument is job size;
// third argument is starting core address;
// fourth argument is interpreted as follows:
// 1 => move from core (memory) to drum
// 0 => move from drum to core (memory)

void ontrace(); // called without arguments
void offtrace(); // called without arguments

// The 2 trace procedures allow you to turn the tracing mechanism on and off.
// The default value is off. WARNING: ontrace() produces a blow-by-blow description
// of each event and results in an extremely large amount of output.
// It should be used only as an aid in debugging.
// Even with the trace off, performance statistics are
// generated at regular intervals and a diagnostic message appears in case of a crash.
// In either case, your OS need not print anything.

// INTERRUPT HANDLERS
// The following 5 functions are the interrupt handlers. The arguments
// passed from the environment are detailed with each function below.
// See RUNNING A JOB, below, for additional information

// prototypes for implementing functions
void Crint(int &a, int p[]);
void Dskint(int &a, int p[]);
void Drmint(int &a, int p[]);
void Tro(int &a, int p[]);
void Svc(int &a, int p[]);

//user defined functions
static void verbose(char* interruptType, int &a, int p[]);

//user defined variables
bool isDoingIO;
//queue<process> *processControlBlock;

void startup()
{
	// Allows initialization of (static) system variables declared above.
	// Called once at start of the simulation.
	system("cls"); // Clears output from previous run
	ontrace();
	//processControlBlock = new queue<process>();
}

void Crint(int &a, int p[])
{
	// Indicates the arrival of a new job on the drum.
	// At call : p [1] = job number
	// p [2] = priority
	// p [3] = job size, K bytes
	// p [4] = max CPU time allowed for job
	// p [5] = current time
	verbose("Crint", a, p); //let's say i move to starting address 10
	siodrum(p[JOB_NUM], p[JOB_SIZE], 0, TO_CORE);
	isDoingIO = false;
}


void Dskint(int &a, int p[])
{
	// Disk interrupt.
	// At call : p [5] = current time
	verbose("Dskint", a, p);
	isDoingIO = false; // wrong place
}

void Drmint(int &a, int p[])
{
	// Drum interrupt.
	// At call : p [5] = current time
	verbose("Drmint", a, p);
	
	a = USER_MODE;
	p[2] = 0; // p [2] = base address of job to be run
	p[MAX_CPU_TIME] = 1;
}

void Tro(int &a, int p[])
{
	// Timer-Run-Out. 
	// At call : p [5] = current time
	//verbose("Tro", a, p);
	a = USER_MODE;
}

void Svc(int &a, int p[])
{
	// Supervisor call from user program.
	// At call : p [5] = current time
	// a = 5 => job has terminated
	// a = 6 => job requests disk i/o
	// a = 7 => job wants to be blocked until all its pending
	// I/O requests are completed
	verbose("Svc", a, p);
	switch (a)
	{
	case TERM_REQ:
		a = IDLE_CPU;
		break;
	case IO_REQ:
		siodisk(p[JOB_NUM]);
		a = USER_MODE;
		p[MAX_CPU_TIME] = 1;
		isDoingIO = true;
		break;
	case BLOCK_REQ:
		if (!isDoingIO) // if it is not doing io
		{
			siodrum(p[JOB_NUM], p[JOB_SIZE], 0, TO_DRUM); //maybe
			a = IDLE_CPU;
		}
		else
		{
			a = IDLE_CPU;
		}
		
		break;
	}
}

// Before leaving each interrupt handler (with the return statement)
// you must set the a and p[] arguments as follows:

// a = 1 CPU is idle, p is ignored
// a = 2 CPU is in user mode,
// p [0], p [1], and p [5] are ignored
// p [2] = base address of job to be run
// p [3] = size (in K) of job to be run
// p [4] = time quantum

static void verbose(char* interruptType, int &a, int p[])
{
	printf("\nJob no[%i] arrived to %s\n", p[1], interruptType);
	printf("Status: %s\n", aValString[a]);
	printf("Priority: %i\n", p[2]);
	printf("Job size: %i\n", p[3]);
	printf("Max CPU time: %i\n", p[4]);
	printf("Current time: %i\n", p[5]);
	printf("\n");
	//system("pause");
}