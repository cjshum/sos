#pragma once

#include "Constants.h"
#include "Variables.h"
using namespace std;

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
void saveCurrentJob(int currentTime);
void setupCpu(int jobNumber);
void runCpu(int &a, int p[]);
void setupDisk(int jobNumber);
void runDisk();
bool swapForSmallJob();
bool trySwapping(list<int>::iterator readyPtr, list<Job>::iterator drumJobPtr);

// helper and debug functions
list<Job>::iterator searchJob(int jobNumber);
list<int>::iterator searchQueue(int jobNumber, list<int> *queueObject);
void sortedInsert(int jobNumber, list<int> *queueObject, int sortBy);
void printQueueElements(list<int> *queueObject);
void printQueueContents(list<int> *queueObject);
void verbose(char* interruptType, int &a, int p[]);

// interupt handlers
void handleDrumInt();
void handleTroInt(int &a, int p[], int jobTimedOut);
void handleDiskInt(int &a, int p[]);
void handleTermReq(int jobReqSvc);
void handleIoReq(int jobReqSvc);
void handleBlockReq(int jobReqSvc);