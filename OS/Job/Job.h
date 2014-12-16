#pragma once
#include "..\Shared\Constants.h"
#include <stdio.h>

// define enums for possible job class values
namespace jx
{
	enum
	{
		JOB_NUM,
		JOB_PRIORITY,
		JOB_MEM_ADDR,
		JOB_SIZE,
		JOB_TIME_ALLOW,
		JOB_TIME_ARRIVAL,
		JOB_TIME_USED,
		JOB_SWAP_DIR,
		// debate the need
		JOB_TIME_SLICE
	};
}

class Job
{
public:
	int j[20];

	Job(int p[])
	{
		j[jx::JOB_NUM]			= p[px::JOB_NUM];
		j[jx::JOB_PRIORITY]		= p[px::JOB_PRIORITY];
		j[jx::JOB_SIZE]			= p[px::JOB_SIZE];
		j[jx::JOB_TIME_ALLOW]	= p[px::JOB_TIME_ALLOW];
		j[jx::JOB_TIME_ARRIVAL] = p[px::JOB_TIME_CURR];

		j[jx::JOB_TIME_USED]	= 0;
		j[jx::JOB_SWAP_DIR]		= UNDEFINED;
		
	}

	void printJobInfo()
	{
		printf("Job number: %i\n", j[jx::JOB_NUM]);
		printf("Job priority: %i\n", j[jx::JOB_PRIORITY]);
		printf("Job memory address: %i\n", j[jx::JOB_MEM_ADDR]);
		printf("Job size: %i\n", j[jx::JOB_SIZE]);
		printf("Job max cpu time: %i\n", j[jx::JOB_TIME_ALLOW]);
		printf("Job arrival time: %i\n", j[jx::JOB_TIME_ARRIVAL]);
		printf("Job time used: %i\n", j[jx::JOB_TIME_USED]);
		printf("Job time slice: %i\n", j[jx::JOB_TIME_SLICE]);
		printf("Job swap direction: %i\n", j[jx::JOB_SWAP_DIR]);
	}
};