#include "Functions.h"

// initialize the shared variables
void initVariables()
{
	jobTable = new list<Job>;
	MemMgr = WorstFit(MEM_SIZE);
	drumQueue = new list<int>();
	readyQueue = new list<int>();
	diskQueue = new list<int>();
	waitingQueue = new list<int>();

	jobInDrum = UNDEFINED;
	jobInCpu = UNDEFINED;
	jobInDisk = UNDEFINED;
}

// insert a job on the drum queue
void setupDrum(int p[], int swapDir)
{
	// queue the job for drum operation
	drumQueue->push_back(p[px::JOB_NUM]);

	// save directoin of the swap
	list<Job>::iterator jobPointer = searchJob(p[px::JOB_NUM]);
	jobPointer->j[jx::JOB_SWAP_DIR] = swapDir;
}

// runs a job on the drum queue
void runDrum()
{
	// if there are no jobs using the drum
	if (jobInDrum == UNDEFINED)
	{
		// if there are jobs on the drum queue
		if (drumQueue->size() > 0)
		{
			list<int>::iterator jobDrumPtr = drumQueue->begin();
			list<Job>::iterator jobPointer = searchJob(*jobDrumPtr);
			
			// if the job wants to be in ram memory
			if (jobPointer->j[jx::JOB_SWAP_DIR] == TO_CORE)
			{
				jobPointer->j[jx::JOB_MEM_ADDR] = MemMgr.RequestMemory(jobPointer->j[jx::JOB_SIZE]);
				if (jobPointer->j[jx::JOB_MEM_ADDR] != INSUFFICENT_MEM)
				{
					siodrum
					(
						jobPointer->j[jx::JOB_NUM],
						jobPointer->j[jx::JOB_SIZE],
						jobPointer->j[jx::JOB_MEM_ADDR],
						jobPointer->j[jx::JOB_SWAP_DIR]
					);
				}
			}

			// if the job wants to be swapped out of memory
			else
			{
				siodrum
				(
					jobPointer->j[jx::JOB_NUM],
					jobPointer->j[jx::JOB_SIZE],
					jobPointer->j[jx::JOB_MEM_ADDR],
					jobPointer->j[jx::JOB_SWAP_DIR]
				);
			}

			// remove the job from the drum queue
			drumQueue->erase(jobDrumPtr);
			// set this job to be using the drum
			jobInDrum = jobPointer->j[jx::JOB_NUM];
		}
		
	}
}

// inserts a job on the ready queue
void setupCpu(int jobNumber)
{
	// queue the job for cpu operation
	readyQueue->push_back(jobNumber);
}

// runs a job on the ready queue
void runCpu(int &a, int p[])
{
	// if there are no jobs using the cpu
	if (jobInCpu == UNDEFINED)
	{
		// if there are jobs on the ready queue
		if (readyQueue->size() > 0)
		{
			list<int>::iterator jobReadyPtr = readyQueue->begin();
			list<Job>::iterator jobPointer = searchJob(*jobReadyPtr);

			p[px::JOB_NUM]		= jobPointer->j[jx::JOB_NUM];
			p[px::JOB_MEM_ADDR]	= jobPointer->j[jx::JOB_MEM_ADDR];
			p[px::JOB_SIZE]		= jobPointer->j[jx::JOB_SIZE];
			p[px::JOB_TIME_SLICE] = TIME_QUANTUM;

			jobInCpu = jobPointer->j[jx::JOB_NUM];
			readyQueue->erase(jobReadyPtr);
			a = CPU_RUN;
		}

		// no more jobs on the ready queue
		else
		{
			jobInCpu = UNDEFINED;
			a = CPU_IDLE;
		}
	}
}

// inserts a job on the disk queue
void setupDisk(int jobNumber)
{
	diskQueue->push_back(jobNumber);
}

// runs a job on the disk queue
void runDisk()
{
	if (jobInDisk == UNDEFINED)
	{
		if (diskQueue->size() > 0)
		{
			int nextJob = diskQueue->front();
			if (diskQueue->size() > 0)
			{
				siodisk(nextJob);
				jobInDisk = nextJob;
			}
		}
	}
}

// this prints the values of a and p for debug purposes
void verbose(char* interruptType, int &a, int p[])
{
	if (!debug)	return;

	printf("\nJob no[%i] %s\n", p[px::JOB_NUM], interruptType);
	printf("Status: %s\n", aValString[a]);
	printf("Base Address: %i\n", p[px::JOB_MEM_ADDR]);
	printf("Job size: %i\n", p[px::JOB_SIZE]);
	printf("Max CPU time: %i\n", p[px::JOB_TIME_ALLOWED]);
	printf("Current time: %i\n", p[px::JOB_TIME_CURR]);
	printf("Job using Drum: %i\n", jobInDrum);
	printf("Job using CPU: %i\n", jobInCpu);
	printf("Job using Disk: %i\n", jobInDisk);
	printf("Items in Waiting Queue: %i\n", waitingQueue->size());
	//printf("\n");
	//system("pause");
}

void quickPrint(int value)
{
	printf("%i\n", value);
	system("pause");
}

list<Job>::iterator searchJob(int jobNumber)
{
	list<Job>::iterator searchIterator = jobTable->begin();
	while (searchIterator != jobTable->end())
	{
		if (searchIterator->j[jx::JOB_NUM] == jobNumber)
			return searchIterator;
		advance(searchIterator, 1);
	}
	return searchIterator;
}

list<int>::iterator searchQueue(int jobNumber, list<int> *queueObject)
{
	list<int>::iterator searchIterator = queueObject->begin();
	while (searchIterator != queueObject->end())
	{
		if (*searchIterator == jobNumber)
			return searchIterator;
		advance(searchIterator, 1);
	}
	return searchIterator;
}

/************************ COME BACK AND VIST******************************************/
template <typename listType, typename valueType>
typename list<listType>::iterator searchList
	(list<listType> *listObject, bool(*compareFunc)(listType, valueType), valueType compareObject)
{
	list<listType>::iterator searchIterator = listObject->begin();
	while (searchIterator != listObject->end())
	{
		if (compareFunc(*searchIterator, compareObject))
			return searchIterator;
		advance(searchIterator, 1);
	}
	return searchIterator;
}

template <typename Type>
class objRef
{
public:
	Type *objPtr;

	objRef(Type &obj)
	{
		this->objPtr = &obj;
	}
};