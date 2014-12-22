#include "Shared\Functions.h"

void ontrace();
void offtrace();

void startup()
{	system("cls");
//	ontrace();
	debug = false;
	initVariables();
}

void Crint(int &a, int p[])
{	// save important values
	saveCurrentJob(p[px::JOB_TIME_CURR]);
	jobTable->push_back(Job(p));

	setupDrum(p[px::JOB_NUM], TO_CORE);
	runDrum();
	runCpu(a, p);

}

void Drmint(int &a, int p[])
{	saveCurrentJob(p[px::JOB_TIME_CURR]);
	handleDrumInt();
	runDrum();
	runCpu(a, p);
}

void Tro(int &a, int p[])
{	// save important values
	int jobTimedOut = jobInCpu;
	saveCurrentJob(p[px::JOB_TIME_CURR]);

	handleTroInt(a, p, jobTimedOut);
	runCpu(a, p);
}

void Svc(int &a, int p[])
{	// save important values
	int jobReqSvc = jobInCpu;
	saveCurrentJob(p[px::JOB_TIME_CURR]);

	switch (a)
	{
	case REQ_TERM:
		handleTermReq(jobReqSvc);
		break;

	case REQ_IO:
		handleIoReq(jobReqSvc);
		break;

	case REQ_BLOCK:
		handleBlockReq(jobReqSvc);
		break;
	}

	runDrum();
	runCpu(a, p);
}

void Dskint(int &a, int p[])
{	saveCurrentJob(p[px::JOB_TIME_CURR]);
	handleDiskInt(a, p);
	runDisk();
	runCpu(a, p);
}
