#pragma once
#include "..\Job\Job.h"
#include "Constants.h"
#include "..\MemMgr\WorstFit.h"
#include <list>
using namespace std;

extern WorstFit MemMgr;
extern list<Job> *jobTable;
extern list<int> *drumQueue;
extern list<int> *readyQueue;
extern list<int> *diskQueue;
extern list<int> *waitingQueue;
extern list<int> *termWaitList;

extern int jobInDrum;
extern int jobInCpu;
extern int jobInDisk;
extern int swapAfterIO;
extern bool debug;