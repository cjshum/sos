#pragma once
#include "..\Shared\Constants.h"

// define enums for possible job class values
namespace jx
{
	enum
	{
		JOB_NUM,
		JOB_PRIORITY,
		JOB_MEM_ADDR,
		JOB_SIZE,
		JOB_TIME_ALLOWED,
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
		j[jx::JOB_TIME_ALLOWED]	= p[px::JOB_TIME_ALLOWED];
		j[jx::JOB_TIME_ARRIVAL]	= p[px::JOB_TIME_CURR];

		j[jx::JOB_TIME_USED]	= 0;
		j[jx::JOB_SWAP_DIR]		= UNDEFINED;
		
	}
};