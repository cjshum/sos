#pragma once
#include "..\Shared\Constants.h"
#include <stdio.h>

// define enums for possible job class values
namespace jx
{	enum
	{	JOB_NUM,
		JOB_PRIORITY,
		JOB_MEM_ADDR,
		JOB_SIZE,
		JOB_TIME_ALLOW,
		JOB_TIME_ARRIVAL,
		JOB_TIME_USED,
		JOB_TIME_REMAIN,
		JOB_TIME_LAST,
		JOB_SWAP_DIR,
		JOB_IO_REQ
	};
}

class Job
{
public:
	int j[12];

	// default constructor initializing various values
	Job(int p[])
	{	j[jx::JOB_NUM]			= p[px::JOB_NUM];
		j[jx::JOB_PRIORITY]		= p[px::JOB_PRIORITY];
		j[jx::JOB_MEM_ADDR]		= UNDEFINED;
		j[jx::JOB_SIZE]			= p[px::JOB_SIZE];
		j[jx::JOB_TIME_ALLOW]	= p[px::JOB_TIME_ALLOW];
		j[jx::JOB_TIME_ARRIVAL] = p[px::JOB_TIME_CURR];
		j[jx::JOB_TIME_USED]	= 0;
		j[jx::JOB_TIME_REMAIN]	= p[px::JOB_TIME_ALLOW];
		j[jx::JOB_TIME_LAST]	= p[px::JOB_TIME_CURR];
		j[jx::JOB_SWAP_DIR]		= UNDEFINED;
		j[jx::JOB_IO_REQ]		= 0;
	}

	// prints the values of the job for debuggin purposes
	void printJobInfo()
	{	printf("\nJob number: %i\n", j[jx::JOB_NUM]);
		printf("Job priority: %i\n", j[jx::JOB_PRIORITY]);
		printf("Job memory address: %i\n", j[jx::JOB_MEM_ADDR]);
		printf("Job size: %i\n", j[jx::JOB_SIZE]);
		printf("Job arrival time: %i\n", j[jx::JOB_TIME_ARRIVAL]);
		printf("Job max cpu time: %i\n", j[jx::JOB_TIME_ALLOW]);
		printf("Job time remaining: %i\n", j[jx::JOB_TIME_REMAIN]);
		printf("Job time used: %i\n", j[jx::JOB_TIME_USED]);
		printf("Job time last accessed: %i\n", j[jx::JOB_TIME_LAST]);
		printf("Job swap direction: %i\n", j[jx::JOB_SWAP_DIR]);
		printf("Job requested IO: %i\n", j[jx::JOB_IO_REQ]);
	}
};