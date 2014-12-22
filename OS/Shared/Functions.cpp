#include "Functions.h"

// initialize the shared variables
void initVariables()
{	jobTable = new list<Job>;
	MemMgr = WorstFit(MEM_SIZE);
	drumQueue = new list<int>();
	readyQueue = new list<int>();
	diskQueue = new list<int>();
	waitingQueue = new list<int>();
	termWaitList = new list<int>();

	jobInDrum = UNDEFINED;
	jobInCpu = UNDEFINED;
	jobInDisk = UNDEFINED;
	swapAfterIO = UNDEFINED;
}

// insert a job on the drum queue
void setupDrum(int jonNumber, int swapDir)
{	// queue the job for drum operation
	sortedInsert(jonNumber, drumQueue, jx::JOB_TIME_REMAIN);

	// save direction of the swap
	list<Job>::iterator jobPointer = searchJob(jonNumber);
	if (jobPointer != jobTable->end())
		jobPointer->j[jx::JOB_SWAP_DIR] = swapDir;
}

// tries swapping a big job out for a smaller one to come in later on
bool trySwapping(list<int>::iterator readyPtr, list<Job>::iterator drumJobPtr)
{	list<Job>::iterator readyJobPtr = searchJob(*readyPtr);

	// if the job qualifies for swapping
	if ((readyJobPtr->j[jx::JOB_TIME_REMAIN] > 200) &&
		(readyJobPtr->j[jx::JOB_SIZE] >= drumJobPtr->j[jx::JOB_SIZE]) &&
		(readyJobPtr->j[jx::JOB_NUM] != jobInDisk))
	{	// prioritize the job since it qualifies for swapping
		drumQueue->push_front(*readyPtr);
		readyJobPtr->j[jx::JOB_SWAP_DIR] = TO_DRUM;

		// remove the job from the various queues
		list<int>::iterator diskPtr = searchQueue(*readyPtr, diskQueue);
		if (diskPtr != diskQueue->end())
			diskQueue->erase(diskPtr);
		if (*readyPtr == jobInCpu)
			jobInCpu = UNDEFINED;
		readyQueue->erase(readyPtr);
		swapAfterIO = UNDEFINED;

		// return the memory and start swapping
		MemMgr.ReturnMemory(readyJobPtr->j[jx::JOB_MEM_ADDR], readyJobPtr->j[jx::JOB_SIZE]);
		runDrum();
		return true;
	}
	return false;
}

// tries to find large jobs in memory and then tries to swap them out
bool swapForSmallJob()
{	//if there are reasonable number of jobs that we might want to swap
	if (drumQueue->size() < 3) return false;

	list<int>::iterator drumPtr = drumQueue->begin();	
	while (drumPtr != drumQueue->end())
	{	list<Job>::iterator drumJobPtr = searchJob(*drumPtr);

		// if the job is small enough for higher throuput
		if (drumJobPtr->j[jx::JOB_TIME_REMAIN] <= 200)
		{	// if there any objects in the ready queue
			if (readyQueue->size() > 0)
			{	list<int>::iterator readyPtr = prev(readyQueue->end());
				list<Job>::iterator readyJobPtr;
				
				// start looking at the large jobs
				while (readyPtr != readyQueue->begin())
				{	// if the large job is doing io, mark it for posible swapping
					// after it finishes doing io
					readyJobPtr = searchJob(*readyPtr);
					if ((readyJobPtr->j[jx::JOB_NUM] == jobInDisk) &&
						(searchQueue(*readyPtr, diskQueue) != diskQueue->end()))
					{	swapAfterIO = jobInDisk;
						continue;
					}

					// otherwise try swapping the job
					if (trySwapping(readyPtr, drumJobPtr))
						return true;
					advance(readyPtr, -1);
				}
				// trys swapping the last job since we are going in reverse
				if (trySwapping(readyPtr, drumJobPtr))
					return true;
			}
		}
		else
			return false;
		advance(drumPtr, 1);
	}
	return false;
}

// runs a job on the drum queue
void runDrum()
{	// if there are no jobs using the drum
	if (jobInDrum == UNDEFINED)
	{	// if there are jobs on the drum queue
		if (drumQueue->size() > 0)
		{	list<int>::iterator drumPtr = drumQueue->begin();
			list<Job>::iterator drumJobPtr = searchJob(*drumPtr);
			
			// if the job wants to be in ram memory
			if (drumJobPtr->j[jx::JOB_SWAP_DIR] == TO_CORE)
			{	while (drumPtr != drumQueue->end())
				{	drumJobPtr->j[jx::JOB_MEM_ADDR] = MemMgr.RequestMemory(drumJobPtr->j[jx::JOB_SIZE]);
					if (drumJobPtr->j[jx::JOB_MEM_ADDR] != INSUFFICENT_MEM)
					{	siodrum
						(	drumJobPtr->j[jx::JOB_NUM],
							drumJobPtr->j[jx::JOB_SIZE],
							drumJobPtr->j[jx::JOB_MEM_ADDR],
							drumJobPtr->j[jx::JOB_SWAP_DIR]
						);
						// set this job to be using the drum, and remove from drum queue
						jobInDrum = drumJobPtr->j[jx::JOB_NUM];
						drumQueue->erase(drumPtr);
						break;
					}
					// otherwise tries to swap out big jobs for small jobs
					else
						if (swapForSmallJob()) return;
					advance(drumPtr, 1);
				}
			}

			// if the job wants to be swapped out of memory
			else if (drumJobPtr->j[jx::JOB_SWAP_DIR] == TO_DRUM)
			{	siodrum
				(	drumJobPtr->j[jx::JOB_NUM],
					drumJobPtr->j[jx::JOB_SIZE],
					drumJobPtr->j[jx::JOB_MEM_ADDR],
					drumJobPtr->j[jx::JOB_SWAP_DIR]
				);
				// set this job to be using the drum, and remove from drum queue
				jobInDrum = drumJobPtr->j[jx::JOB_NUM];
				drumQueue->erase(drumPtr);
			}
		}
	}
}

// saves the current job
void saveCurrentJob(int currentTime)
{	if (jobInCpu != UNDEFINED)
	{	list<Job>::iterator jobPointer = searchJob(jobInCpu);
		
		int timeDiff = currentTime - jobPointer->j[jx::JOB_TIME_LAST];
		jobPointer->j[jx::JOB_TIME_USED] += timeDiff;
		jobPointer->j[jx::JOB_TIME_REMAIN] -= timeDiff;
		jobPointer->j[jx::JOB_TIME_LAST] = currentTime;

		readyQueue->push_front(jobInCpu);
		jobInCpu = UNDEFINED;
	}
}

// inserts a job on the ready queue
void setupCpu(int jobNumber)
{	// queue the job for cpu operation
	sortedInsert(jobNumber, readyQueue, jx::JOB_TIME_ALLOW);
}

// runs a job on the ready queue
void runCpu(int &a, int p[])
{	// if there are no jobs using the cpu
	if (jobInCpu == UNDEFINED)
	{	// if there are jobs on the ready queue
		if (readyQueue->size() > 0)
		{	list<int>::iterator jobReadyPtr = readyQueue->begin();
			list<Job>::iterator jobPointer = searchJob(*jobReadyPtr);

			p[px::JOB_NUM]		= jobPointer->j[jx::JOB_NUM];
			p[px::JOB_MEM_ADDR]	= jobPointer->j[jx::JOB_MEM_ADDR];
			p[px::JOB_SIZE]		= jobPointer->j[jx::JOB_SIZE];
			p[px::JOB_TIME_SLICE] = jobPointer->j[jx::JOB_TIME_REMAIN];
			jobPointer->j[jx::JOB_TIME_LAST] = p[px::JOB_TIME_CURR];

			jobInCpu = jobPointer->j[jx::JOB_NUM];
			readyQueue->erase(jobReadyPtr);
			a = CPU_RUN;
		}

		// no more jobs on the ready queue
		else
		{	jobInCpu = UNDEFINED;
			a = CPU_IDLE;
		}
	}
}

// inserts a job on the disk queue
void setupDisk(int jobNumber)
{	list<Job>::iterator jobPointer = searchJob(jobNumber);
	jobPointer->j[jx::JOB_IO_REQ] = 1;
	diskQueue->push_back(jobNumber);
}

// runs a job on the disk queue
void runDisk()
{	if (jobInDisk == UNDEFINED)
	{	if (diskQueue->size() > 0)
		{	int nextJob = diskQueue->front();
			// release disk for other jobs
			siodisk(nextJob);
			jobInDisk = nextJob;
			diskQueue->erase(searchQueue(jobInDisk, diskQueue));
		}
	}
}

// search a job object using the job number
list<Job>::iterator searchJob(int jobNumber)
{	list<Job>::iterator searchIterator = jobTable->begin();
	while (searchIterator != jobTable->end())
	{	if (searchIterator->j[jx::JOB_NUM] == jobNumber)
			return searchIterator;
		advance(searchIterator, 1);
	}
	return searchIterator;
}

// search a job inside any queue
list<int>::iterator searchQueue(int jobNumber, list<int> *queueObject)
{	list<int>::iterator searchIterator = queueObject->begin();
	while (searchIterator != queueObject->end())
	{	if (*searchIterator == jobNumber)
			return searchIterator;
		advance(searchIterator, 1);
	}
	return searchIterator;
}

// prints the items inside the queue
void printQueueElements(list<int> *queueObject)
{	list<int>::iterator searchIterator = queueObject->begin();
	while (searchIterator != queueObject->end())
	{	printf("%i ", *searchIterator);
		advance(searchIterator, 1);
	}
	printf("\n");
}

// prints the job information for each element of the queue
void printQueueContents(list<int> *queueObject)
{	list<Job>::iterator searcher;
	list<int>::iterator iterator = queueObject->begin();
	while (iterator != queueObject->end())
	{	searcher = searchJob(*iterator);
		searcher->printJobInfo();
		printf("\n");
		advance(iterator, 1);
	}
}

// insert elements into a queue in a sorted fashion
void sortedInsert(int jobNumber, list<int> *queueObject, int sortBy)
{	list<Job>::iterator searcher;
	list<Job>::iterator currentJob = searchJob(jobNumber);
	list<int>::iterator iterator = queueObject->begin();
	while (iterator != queueObject->end())
	{	searcher = searchJob(*iterator);
		if (currentJob->j[sortBy] > searcher->j[sortBy])
			advance(iterator, 1);
		else
			break;
	}
	queueObject->insert(iterator, jobNumber);
}

// handle the different cases of Drmint
void handleDrumInt()
{	list<Job>::iterator jobPointer = searchJob(jobInDrum);
	// if the job finished moving to core
	if (jobPointer->j[jx::JOB_SWAP_DIR] == TO_CORE)
	{	setupCpu(jobPointer->j[jx::JOB_NUM]);
		if (jobPointer->j[jx::JOB_IO_REQ] == 1)
			setupDisk(jobInDrum);
		jobPointer->j[jx::JOB_SWAP_DIR] = UNDEFINED;
	}
	else if (jobPointer->j[jx::JOB_SWAP_DIR] == TO_DRUM)
		setupDrum(jobPointer->j[jx::JOB_NUM], TO_CORE);

	// reset using drum status from flag and job
	jobInDrum = UNDEFINED;
}

// handle the different cases of Tro
void handleTroInt(int &a, int p[], int jobTimedOut)
{	list<Job>::iterator jobPointer = searchJob(jobTimedOut);
	// if the job exceeds the maximum allowed time, terminate it
	if (jobPointer->j[jx::JOB_TIME_USED] == jobPointer->j[jx::JOB_TIME_ALLOW])
	{	list<int>::iterator jobTerm = searchQueue(jobTimedOut, readyQueue);
		readyQueue->erase(jobTerm);
		jobInCpu = jobTimedOut;
		a = REQ_TERM;
		Svc(a, p);
	}
}

// handle the different cases of Dskint
void handleDiskInt(int &a, int p[])
{	// clears job io flag
	list<Job>::iterator jobPointer = searchJob(jobInDisk);
	jobPointer->j[jx::JOB_IO_REQ] = 0;

	// if the job that finished i/o was blocked
	list<int>::iterator jobWaitPtr = searchQueue(jobInDisk, waitingQueue);
	if (jobWaitPtr != waitingQueue->end())
	{	setupCpu(*jobWaitPtr);
		waitingQueue->erase(jobWaitPtr);
	}

	// if the job that finished io was waiting for termination
	list<int>::iterator termPointer = searchQueue(jobInDisk, termWaitList);
	if (termPointer != termWaitList->end())
	{	jobInCpu = jobInDisk;
		jobInDisk = UNDEFINED;

		a = REQ_TERM;
		Svc(a, p);
	}

	// if the job is marked to be swapped after io
	if (swapAfterIO != UNDEFINED)
	{	printf("swap job %i\n", swapAfterIO);
		list<Job>::iterator jobPointer = searchJob(swapAfterIO);
		MemMgr.ReturnMemory(jobPointer->j[jx::JOB_MEM_ADDR], jobPointer->j[jx::JOB_SIZE]);
		drumQueue->push_front(swapAfterIO);
		jobPointer->j[jx::JOB_SWAP_DIR] = TO_DRUM;
		if (swapAfterIO == jobInCpu)
			jobInCpu = UNDEFINED;
		list<int>::iterator readyPtr = searchQueue(swapAfterIO, readyQueue);
		if (readyPtr != readyQueue->end())
			readyQueue->erase(readyPtr);
		swapAfterIO = UNDEFINED;
		runDrum();
	}
	jobInDisk = UNDEFINED;
}

// handles termination requests
void handleTermReq(int jobReqSvc)
{	list<Job>::iterator jobPointer = searchJob(jobReqSvc);
	list<int>::iterator jobInDiskQueue = searchQueue(jobReqSvc, diskQueue);
	list<int>::iterator termPointer = searchQueue(jobReqSvc, termWaitList);

	// if the job has pending i/o
	if ((jobReqSvc == jobInDisk) || (jobInDiskQueue != diskQueue->end()))
	{	termWaitList->push_back(jobReqSvc);
		// if the job is in the disk queue, priotirize it
		if (jobInDiskQueue != diskQueue->end())
		{	diskQueue->erase(jobInDiskQueue);
			diskQueue->push_front(jobReqSvc);
		}
	}
	// or if the job was held off termination due to io
	else if (termPointer != termWaitList->end())
	{	termWaitList->erase(termPointer);
		MemMgr.ReturnMemory(jobPointer->j[jx::JOB_MEM_ADDR], jobPointer->j[jx::JOB_SIZE]);
		jobTable->erase(searchJob(jobReqSvc));
	}
	// otherwise can terminate
	else
	{	MemMgr.ReturnMemory(jobPointer->j[jx::JOB_MEM_ADDR], jobPointer->j[jx::JOB_SIZE]);
		jobTable->erase(searchJob(jobReqSvc));
	}
	readyQueue->erase(searchQueue(jobReqSvc, readyQueue));
}


// handles io requests
void handleIoReq(int jobReqSvc)
{	setupDisk(jobReqSvc);
	runDisk();
}

// handles block requests
void handleBlockReq(int jobReqSvc)
{	list<int>::iterator jobInDiskQueue = searchQueue(jobReqSvc, diskQueue);

	// if the job disk i/o is not finished, block the job
	if ((jobReqSvc == jobInDisk) || (jobInDiskQueue != diskQueue->end()))
	{	waitingQueue->push_back(jobReqSvc);
		readyQueue->erase(searchQueue(jobReqSvc, readyQueue));
	}
}

// this prints the values of a and p for debug purposes
void verbose(char *interruptType, int &a, int p[])
{	if (!debug)	return;

	printf("\nJob no[%i] %s\n", p[px::JOB_NUM], interruptType);
	printf("Status: %s\n", aValString[a]);
	printf("Base Address: %i\n", p[px::JOB_MEM_ADDR]);
	printf("Job size: %i\n", p[px::JOB_SIZE]);
	printf("Max CPU time: %i\n", p[px::JOB_TIME_ALLOW]);
	printf("Current time: %i\n", p[px::JOB_TIME_CURR]);
	printf("Job using Drum: %i\n", jobInDrum);
	printf("Job using CPU: %i\n", jobInCpu);
	printf("Job using Disk: %i\n", jobInDisk);
	printf("Items in Ready Queue: %i\n", readyQueue->size());
	printf("Items in Drum Queue: %i\n", drumQueue->size());
	printf("Items in Disk Queue: %i\n", diskQueue->size());
	printf("Items in Waiting Queue: %i\n", waitingQueue->size());
	printf("Number of Jobs waiting for Termination: %i\n", termWaitList->size());

	//printf("\n");
	//system("pause");
}