#include "Shared\Functions.h"

void ontrace();
void offtrace();

void startup()
{
	ontrace();
	debug = true;

	initVariables();
}

void Crint(int &a, int p[])
{
	verbose("arrived Crint", a, p);

	saveCurrentJob();

	// add new job into job-table
	jobTable->push_back(Job(p));

	// schedule drum operations
	setupDrum(p[px::JOB_NUM], TO_CORE);
	runDrum();

	// run availabe jobs
	runCpu(a, p);

	verbose("leaving Crint", a, p);
}

void Drmint(int &a, int p[])
{
	verbose("arrived Drmint", a, p);

	saveCurrentJob();

	list<Job>::iterator jobPointer = searchJob(jobInDrum);
	// if the job finished moving to core
	if (jobPointer->j[jx::JOB_SWAP_DIR] == TO_CORE)
	{
		setupCpu(jobPointer->j[jx::JOB_NUM]);
	}
	// reset using drum status from flag and job
	jobInDrum = UNDEFINED;
	jobPointer->j[jx::JOB_SWAP_DIR] = UNDEFINED;

	// run available jobs in drum and cpu queue
	runDrum();
	runCpu(a, p);
	verbose("leaving Drmint", a, p);
}

void Tro(int &a, int p[])
{
//	verbose("arrived Tro", a, p);

	list<Job>::iterator jobPointer = searchJob(jobInCpu);
	jobPointer->j[jx::JOB_TIME_USED] += TIME_QUANTUM;

	printf("CPU Time Used: %i\n", jobPointer->j[jx::JOB_TIME_USED]);
	printf("Allowed CPU Time: %i\n", jobPointer->j[jx::JOB_TIME_ALLOW]);
	
	// if the job exceeds the maximum allowed time, terminate it
	if (jobPointer->j[jx::JOB_TIME_USED] == jobPointer->j[jx::JOB_TIME_ALLOW])
	{
		a = REQ_TERM;
		Svc(a, p);
	}

//---------- put short-term scheduling algorithm here ---------
	else
	{
		readyQueue->push_front(jobInCpu);
		jobInCpu = UNDEFINED;
	}

	runCpu(a, p);

//	verbose("leaving Tro", a, p);
}

void Svc(int &a, int p[])
{
	verbose("arrived Svc", a, p);

	int jobReqSvc = jobInCpu;
	list<Job>::iterator jobPointer = searchJob(jobReqSvc);

	saveCurrentJob();
	
	switch (a)
	{
	// if a job request termination
	case REQ_TERM:
		// if the job is in the termination queue
		if (terminationQueue->size() > 0)
		{
			list<int>::iterator jobTermPtr = searchQueue(jobReqSvc, terminationQueue);
			if (jobTermPtr != terminationQueue->end())
			{
				terminationQueue->erase(jobTermPtr);
				MemMgr.ReturnMemory(jobPointer->j[jx::JOB_MEM_ADDR], jobPointer->j[jx::JOB_SIZE]);
				jobTable->erase(searchJob(jobReqSvc));
			}
		}
		// make sure that the job is not doing io
		else if (jobInDisk != jobReqSvc)
		{
			MemMgr.ReturnMemory(jobPointer->j[jx::JOB_MEM_ADDR], jobPointer->j[jx::JOB_SIZE]);
			jobTable->erase(searchJob(jobReqSvc));
		}
		// if it is doing io put it in the termination queue
		else
		{
			terminationQueue->push_back(jobReqSvc);
		}
		readyQueue->erase(searchQueue(jobReqSvc, readyQueue));

		runDrum();
		break;

	//if a job requests io
	case REQ_IO:
		setupDisk(jobReqSvc);
		runDisk();
		break;

	// if a job requests blocking
	case REQ_BLOCK:
		// move the job to the waiting queue
		waitingQueue->push_back(jobReqSvc);
		readyQueue->erase(searchQueue(jobReqSvc, readyQueue));

		// if the job is not doing io, schedule it to drum
		if (jobInDisk != jobReqSvc)
		{
			readyQueue->push_back(jobReqSvc);
			waitingQueue->erase(searchQueue(jobReqSvc, waitingQueue));
		}

		break;
	}

	runCpu(a, p);
	verbose("leaving Svc", a, p);
}

void Dskint(int &a, int p[])
{
	verbose("arrived Dskit", a, p);

	if (jobInCpu != UNDEFINED)
	{
		readyQueue->push_front(jobInCpu);
		jobInCpu = UNDEFINED;
	}

	// if the job that finished i/o was blocked
	list<int>::iterator jobWaitPtr = searchQueue(jobInDisk, waitingQueue);
	if (jobWaitPtr != waitingQueue->end())
	{
		readyQueue->push_back(*jobWaitPtr);
		waitingQueue->erase(jobWaitPtr);
	}

/*
	list<int>::iterator jobTermPtr = searchQueue(jobInDisk, terminationQueue);
	if (jobTermPtr != terminationQueue->end())
	{
		if (jobInDisk == 9)
			printf("");

		jobInCpu = jobInDisk;
		jobInDisk = UNDEFINED;
		a = REQ_TERM;
		Svc(a, p);
	}
*/

	jobInDisk = UNDEFINED;
	runDisk();
	runCpu(a, p);
	verbose("leaving Dskit", a, p);
}
