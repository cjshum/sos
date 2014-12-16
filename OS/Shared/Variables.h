#pragma once
#include "..\Job\Job.h"
#include "Constants.h"
#include "..\MemMgr\WorstFit.cpp" // change when fixed
#include <list>
using namespace std;

extern bool debug; // do we need this?
extern WorstFit MemMgr;
extern list<Job> *jobTable;
extern list<int> *drumQueue;
extern list<int> *readyQueue;
extern list<int> *diskQueue;
extern list<int> *waitingQueue;
extern list<int> *terminationQueue;

extern int jobInDrum;
extern int jobInCpu;
extern int jobInDisk;