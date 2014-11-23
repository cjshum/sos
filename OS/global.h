#pragma once

// define enums for drum directions
static enum drumDir { TO_CORE, TO_DRUM };

// define enums for possible p-register values
static enum pValues { NOT_USED0, JOB_NUM, JOB_ADDR, JOB_SIZE, JOB_MAX_TIME, JOB_CURR_TIME };

// define enums and string description for possible a-values
static enum aValues { NOT_USED1, CPU_IDLE, CPU_RUN, NOT_USED2, NOT_USED3, REQ_TERM, REQ_IO, REQ_BLOCK };
static const char *aValString[] = { "", "Idle CPU", "User Mode", "", "", "Termination Request", "I/O Request", "Block Request" };