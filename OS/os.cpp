#include "Shared\Functions.h"

void ontrace();
void offtrace();

void startup()
{
	system("cls");
	//ontrace();
	
	debug = false;
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
	verbose("arrived Tro", a, p);

	list<Job>::iterator jobPointer = searchJob(jobInCpu);
	jobPointer->j[jx::JOB_TIME_USED] += TIME_QUANTUM;

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

	verbose("leaving Tro", a, p);
}

void Svc(int &a, int p[])
{
	verbose("arrived Svc", a, p);

	int jobReqSvc = jobInCpu;
	list<Job>::iterator jobPointer = searchJob(jobReqSvc);
	list<int>::iterator jobInDiskQueue = searchQueue(jobReqSvc, diskQueue);
	list<int>::iterator termPointer = searchQueue(jobReqSvc, termWaitList);

	saveCurrentJob();

	switch (a)
	{
	// if a job request termination
	case REQ_TERM:
		// if the job has pending i/o
		if ((jobReqSvc==jobInDisk) || (jobInDiskQueue!=diskQueue->end()))
		{
			termWaitList->push_back(jobReqSvc);
			// if the job is in the disk queue, priotirize it
			if (jobInDiskQueue != diskQueue->end())
			{
				diskQueue->erase(jobInDiskQueue);
				diskQueue->push_front(jobReqSvc);
			}
		}
		// or if the job was held off termination due to io
		else if (termPointer != termWaitList->end())
		{
			termWaitList->erase(termPointer);
			MemMgr.ReturnMemory(jobPointer->j[jx::JOB_MEM_ADDR], jobPointer->j[jx::JOB_SIZE]);
			jobTable->erase(searchJob(jobReqSvc));
		}
		// otherwise can terminate
		else
		{
			MemMgr.ReturnMemory(jobPointer->j[jx::JOB_MEM_ADDR], jobPointer->j[jx::JOB_SIZE]);
			jobTable->erase(searchJob(jobReqSvc));
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
		// if the job disk i/o is not finished, block the job
		if ((jobReqSvc == jobInDisk) || (jobInDiskQueue != diskQueue->end()))
		{
			waitingQueue->push_back(jobReqSvc);
			readyQueue->erase(searchQueue(jobReqSvc, readyQueue));
		}
		break;
	}

	runCpu(a, p);
	verbose("leaving Svc", a, p);
}

void Dskint(int &a, int p[])
{
	verbose("arrived Dskit", a, p);

	saveCurrentJob();

	// if the job that finished i/o was blocked
	list<int>::iterator jobWaitPtr = searchQueue(jobInDisk, waitingQueue);
	if (jobWaitPtr != waitingQueue->end())
	{
		setupCpu(*jobWaitPtr);
		waitingQueue->erase(jobWaitPtr);
	}

	// if the job that finished io was waiting for termination
	list<int>::iterator termPointer = searchQueue(jobInDisk, termWaitList);
	if (termPointer != termWaitList->end())
	{
		jobInCpu = jobInDisk;
		jobInDisk = UNDEFINED;

		a = REQ_TERM;
		Svc(a, p);
	}

	jobInDisk = UNDEFINED;
	runDisk();
	runCpu(a, p);
	verbose("leaving Dskit", a, p);
}
