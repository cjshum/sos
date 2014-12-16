#pragma once

// single valued constants
const int UNDEFINED = -1;
const int MEM_SIZE = 100;
const int TIME_QUANTUM = 1;

// define enums for drum directions
enum
{	TO_CORE = 0,
	TO_DRUM = 1
};

// define enums for possible p-register values
namespace px
{
	enum
	{
		JOB_NUM = 1,
		JOB_PRIORITY = 2,
		JOB_MEM_ADDR = 2,
		JOB_SIZE = 3,
		JOB_TIME_ALLOW = 4,
		JOB_TIME_SLICE = 4,
		JOB_TIME_CURR = 5
	};
}

// define enums and string description for possible a-values
enum
{	CPU_IDLE = 1,
	CPU_RUN = 2,
	REQ_TERM = 5,
	REQ_IO = 6,
	REQ_BLOCK = 7
};

static const char *aValString[] =
{
	"",
	"Idle CPU",				// a=1
	"User Mode",			// a=2
	"",
	"",
	"Termination Request",	// a=5
	"I/O Request",			// a=6
	"Block Request"			// a=7
};