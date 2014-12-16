#pragma once

#include "Constants.h"
#include "Variables.h"

// function prototypes from SOS
extern void siodisk(int jobnum);
extern void siodrum(int jobnum, int jobsize, int coreaddress, int direction);
extern void Crint(int &a, int p[]);
extern void Drmint(int &a, int p[]);
extern void Tro(int &a, int p[]);
extern void Svc(int &a, int p[]);
extern void Dskint(int &a, int p[]);

// function prototypes to be used in OS
void initVariables();
void setupDrum(int jobNumber, int swapDir);
void runDrum();
void saveCurrentJob();
void setupCpu(int jobNumber);
void runCpu(int &a, int p[]);
void setupDisk(int jobNumber);
void runDisk();
void verbose(char* interruptType, int &a, int p[]);
void quickPrint(int value);

// things that might go away
list<Job>::iterator searchJob(int jobNumber);
list<int>::iterator searchQueue(int jobNumber, list<int> *queueObject);