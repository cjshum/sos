class process
{
private:
	int a;
	int p[6];
	bool isInCore;

public:
	process(int a, int p[]);
	void setInCoreVal(bool inCoreVal);
	bool isInCore();
};