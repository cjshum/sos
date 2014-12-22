#include "Variables.h"

WorstFit MemMgr;
list<Job> *jobTable;
list<int> *drumQueue;
list<int> *readyQueue;
list<int> *diskQueue;
list<int> *waitingQueue;
list<int> *termWaitList;

int jobInDrum;
int jobInCpu;
int jobInDisk;
int jobWaitTerm;
int swapAfterIO;
bool debug;