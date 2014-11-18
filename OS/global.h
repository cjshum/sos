enum pValues { NOT_USED0, JOB_NUM, JOB_PRI, JOB_SIZE, MAX_CPU_TIME, CURR_TIME };
enum drumDir { TO_CORE, TO_DRUM };
static const char *aValString[] = { "", "Idle CPU", "User Mode", "", "", "Termination Request", "I/O Request", "Block Request" };
enum aValues { NOT_USED1, IDLE_CPU, USER_MODE, NOT_USED2, NOT_USED3, TERM_REQ, IO_REQ, BLOCK_REQ };