#include "Variables.h"

bool debug; // do we need this?
WorstFit MemMgr;
list<Job> *jobTable;
list<int> *drumQueue;
list<int> *readyQueue;
list<int> *diskQueue;
list<int> *waitingQueue;

int jobInDrum;
int jobInCpu;
int jobInDisk;