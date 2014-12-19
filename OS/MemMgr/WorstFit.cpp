#include "SpaceBlock.cpp"
#include "..\Shared\Constants.h"
#include <list>
using namespace std;

const int INSUFFICENT_MEM = -1;

class WorstFit
{
public:
	list<SpaceBlock> *memTable = new list<SpaceBlock>();
	list<SpaceBlock>::iterator largestMemBlock;

	// empty constructor
	WorstFit() {};

	// default constructor that initializes memory of any given size
	WorstFit(const int memSize)
	{
		memTable->push_front(SpaceBlock(memSize));
		largestMemBlock = memTable->begin();
	}

	// 
	int RequestMemory(const int size)
	{
		// if there is no memory left
		if (largestMemBlock == memTable->end())
			return UNDEFINED;
		// if the requested memory size is greater than largest memory block left
		if (size > largestMemBlock->fsp[SIZE])
			return UNDEFINED;
		// otherwise we have sufficent memory
		else
		{
			// set the return base address to the largest block base address
			int returnAddress = largestMemBlock->fsp[BASE_ADDRESS];
			// if the requested size is the same as the largest block
			// delete the largest block
			if (size == largestMemBlock->fsp[SIZE])
			{
				memTable->erase(largestMemBlock);
			}
			else
			{
				// otherwise change the size of the largest block
				// and also change the base address of the largest block
				largestMemBlock->fsp[SIZE] -= size;
				largestMemBlock->fsp[BASE_ADDRESS] += size;
			}
			// set the new largest block
			SetLargestBlock();
			// return the base address of the memory the new process can use
			return returnAddress;
		}
	}

	// goes through the memory table to find the largest block of free memory
	void SetLargestBlock()
	{
		list<SpaceBlock>::iterator iterator = memTable->begin();
		largestMemBlock = memTable->begin();
		advance(iterator, 1);
		while (iterator != memTable->end())
		{
			if (largestMemBlock->fsp[SIZE] < iterator->fsp[SIZE])
				largestMemBlock = iterator;
			advance(iterator, 1);
		}
	}

	void ReturnMemory(const int baseAddress, const int size)
	{
		// if there is no free space left, then make a new
		// free space and make the largestMemBlock and front point at it
		if (largestMemBlock == memTable->end())
		{
			memTable->push_front(SpaceBlock(size, baseAddress));
			largestMemBlock = memTable->begin();
			return;
		}

		// find the position where the memory block might belong to
		list<SpaceBlock>::iterator iterator = memTable->begin();
		while (iterator != memTable->end())
		{
			if (baseAddress < iterator->fsp[BASE_ADDRESS])
				break;
			advance(iterator, 1);
		}

		// if the block belongs somewhere at the begining
		if (iterator == memTable->begin())
		{
			// if the block cannot merge and belongs at the begining
			if (baseAddress + size < iterator->fsp[BASE_ADDRESS])
			{
				memTable->push_front(SpaceBlock(size, baseAddress));
				SetLargestBlock();
			}

			// if the block belongs at the begining and can merge with the next block
			else if (baseAddress + size == iterator->fsp[BASE_ADDRESS])
			{
				iterator->fsp[BASE_ADDRESS] -= size;
				iterator->fsp[SIZE] += size;
				SetLargestBlock();
			}
		}
		else
		{
			list<SpaceBlock>::iterator prevBlock = prev(iterator);
			// if the memory block belongs somewhere at the ned
			if (iterator == memTable->end())
			{
				// if the memory block can merge with the previous block, do so
				if (prevBlock->fsp[BASE_ADDRESS] + prevBlock->fsp[SIZE] == baseAddress)
				{
					prevBlock->fsp[SIZE] += size;
					SetLargestBlock();
				}
				// otherwise a new block is insterted at the end
				else
				{
					memTable->push_back(SpaceBlock(size, baseAddress));
					SetLargestBlock();
				}
			}
			else
			{
				// if the block is somewhere at the middle and can merge with the previous and next blocks
				if (prevBlock->fsp[BASE_ADDRESS] + prevBlock->fsp[SIZE] + size == iterator->fsp[BASE_ADDRESS])
				{
					prevBlock->fsp[SIZE] += size + iterator->fsp[SIZE];
					memTable->erase(iterator);
					SetLargestBlock();
				}
				// if the new block can only merge with the previous block
				else if (prevBlock->fsp[BASE_ADDRESS] + prevBlock->fsp[SIZE] == baseAddress)
				{
					prevBlock->fsp[SIZE] += size;
					SetLargestBlock();
				}
				// if the new block can only merge with the next block
				else if (baseAddress + size == iterator->fsp[BASE_ADDRESS])
				{
					iterator->fsp[BASE_ADDRESS] -= size;
					iterator->fsp[SIZE] += size;
					SetLargestBlock();
				}
				// otherwise, it cant merge so insert it in between
				else
				{
					memTable->insert(iterator, SpaceBlock(size, baseAddress));
					SetLargestBlock();
				}
			}
		}
	}

	// prints the memory blocks sequentially in order
	void PrintMemoryBlocks()
	{
		list<SpaceBlock>::iterator iterator = memTable->begin();
		while (iterator != memTable->end())
		{
			printf("Address: %i\n", iterator->fsp[BASE_ADDRESS]);
			printf("Size: %i\n", iterator->fsp[SIZE]);
			advance(iterator, 1);
		}
		printf("\n");
	}
};