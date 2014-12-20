#pragma once

#include "SpaceBlock.cpp"
#include "..\Shared\Constants.h"
#include <list>
using namespace std;

const int INSUFFICENT_MEM = -1;

class WorstFit
{
private:
	list<SpaceBlock> *memTable = new list<SpaceBlock>();
	list<SpaceBlock>::iterator largestMemBlock;

public:
	WorstFit();
	WorstFit(const int memSize);
	int RequestMemory(const int size);
	void SetLargestBlock();
	void ReturnMemory(const int baseAddress, const int size);
	void PrintMemoryBlocks();
};