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

	// save the running job back to the ready queue
	if (jobInCpu != UNDEFINED)
	{
		readyQueue->push_front(jobInCpu);
		jobInCpu = UNDEFINED;
	}

	// add new job into job-table
	jobTable->push_back(Job(p));

	// schedule drum operations
	setupDrum(p, TO_CORE);
	runDrum();

	// run availabe jobs
	runCpu(a, p);

	verbose("leaving Crint", a, p);
}

void Drmint(int &a, int p[])
{
	verbose("arrived Drmint", a, p);

	// need to find a generic fix for jobs that finish swapping
	// and no job is using the cpu. job 7 is one of such case
	if (jobInDrum == 7)
	{
		list<Job>::iterator jobPointer = searchJob(jobInDrum);
		if (jobPointer->j[jx::JOB_SWAP_DIR] == TO_CORE)
		{
			setupCpu(jobPointer->j[jx::JOB_NUM]);
		}
		else
		{
			waitingQueue->push_back(jobPointer->j[jx::JOB_NUM]);
		}
		jobInDrum = UNDEFINED;
		jobPointer->j[jx::JOB_SWAP_DIR] = UNDEFINED;
	}

	if (jobInCpu != UNDEFINED)
	{
		readyQueue->push_front(jobInCpu);
		jobInCpu = UNDEFINED;
	}

	// if the job no longer wants to be blocked
	if (waitingQueue->size() > 0)
	{
		list<int>::iterator jobWaitPtr = searchQueue(p[px::JOB_NUM], waitingQueue);
		if (jobWaitPtr != waitingQueue->end())
		{
			waitingQueue->erase(searchQueue(p[px::JOB_NUM], waitingQueue));
			setupDrum(p, TO_CORE);
		}
		jobInDrum = UNDEFINED;
	}

	// otherwise job came from drum operations
	if (jobInDrum != UNDEFINED)
	{
		list<Job>::iterator jobPointer = searchJob(jobInDrum);

		// if the job finished moving to core
		if (jobPointer->j[jx::JOB_SWAP_DIR] == TO_CORE)
		{
			setupCpu(jobPointer->j[jx::JOB_NUM]);
		}

		// if the job finished moving to drum
		else
		{
			waitingQueue->push_back(jobPointer->j[jx::JOB_NUM]);
		}
		// reset using drum status from flag and job
		jobInDrum = UNDEFINED;
		jobPointer->j[jx::JOB_SWAP_DIR] = UNDEFINED;
	}

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

	// otherwise put it back to the ready queue
	else
	{
//******* push front for first come first serve
		readyQueue->push_front(jobInCpu);
	}

	jobInCpu = UNDEFINED;
	runCpu(a, p);

//	verbose("leaving Tro", a, p);
}

void Svc(int &a, int p[])
{
	verbose("arrived Svc", a, p);

	// save the running job back to the ready queue
	if (jobInCpu != UNDEFINED)
	{
		readyQueue->push_front(jobInCpu);
		jobInCpu = UNDEFINED;
	}
	
	switch (a)
	{
	// if a job request termination
	case REQ_TERM:
		// make sure that the job is not doing io
		if (terminationQueue->size() > 0)
		{
			list<int>::iterator jobTermPtr = searchQueue(p[px::JOB_NUM], terminationQueue);
			if (jobTermPtr != terminationQueue->end())
			{
				MemMgr.ReturnMemory(p[px::JOB_MEM_ADDR], p[px::JOB_SIZE]);
				terminationQueue->erase(jobTermPtr);
				jobTable->erase(searchJob(p[px::JOB_NUM]));
			}
		}
		else if (jobInDisk != p[px::JOB_NUM])
		{
			MemMgr.ReturnMemory(p[px::JOB_MEM_ADDR], p[px::JOB_SIZE]);
			readyQueue->erase(searchQueue(p[px::JOB_NUM], readyQueue));
			jobTable->erase(searchJob(p[px::JOB_NUM]));
		}
		// if it is doing io put it in the termination queue
		else
		{
			jobTable->erase(searchJob(p[px::JOB_NUM]));
			terminationQueue->push_back(p[px::JOB_NUM]);
			readyQueue->erase(searchQueue(p[px::JOB_NUM], readyQueue));
		}
		break;

	//if a job requests io
	case REQ_IO:
		setupDisk(p[px::JOB_NUM]);
		runDisk();
		break;

	// if a job requests blocking
	case REQ_BLOCK:
		// move the job to the waiting queue
		waitingQueue->push_back(p[px::JOB_NUM]);
		readyQueue->erase(searchQueue(p[px::JOB_NUM], readyQueue));
		
		// if the job is not doing io, schedule it to drum
		if (jobInDisk != p[px::JOB_NUM])
		{
			list<Job>::iterator jobPointer = searchJob(p[px::JOB_NUM]);
			MemMgr.ReturnMemory(jobPointer->j[jx::JOB_MEM_ADDR], jobPointer->j[jx::JOB_SIZE]);
			//setupDrum(p, TO_DRUM);
/*--------------------------swap out before sawping in---------------------------*/
			drumQueue->push_front(p[px::JOB_NUM]);
			jobPointer->j[jx::JOB_SWAP_DIR] = TO_DRUM;
/*--------------------------swap out before sawping in---------------------------*/
			runDrum();
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

	jobInDisk = UNDEFINED;
	runDisk();
	runCpu(a, p);
	verbose("leaving Dskit", a, p);
}
