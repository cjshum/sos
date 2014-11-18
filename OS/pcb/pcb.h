#include "..\global.h"

class pcb
{
private:
	int a;
	int p[6];
	bool isInCore;
public:
	static pcb *head;
	static pcb *tail;

	pcb();
	void insertProcess(int a, int p[]);
	void setInCore(bool inCoreVal);
	bool isDoingIO();
};