#include "WorstFit.h"

// empty constructor
WorstFit::WorstFit() {};

// default constructor that initializes memory of any given size
WorstFit::WorstFit(const int memSize)
{	memTable->push_front(SpaceBlock(memSize));
	largestMemBlock = memTable->begin();
}

int WorstFit::RequestMemory(const int size)
{	// if there is no memory left
	if (largestMemBlock == memTable->end())
		return UNDEFINED;
	// if the requested memory size is greater than largest memory block left
	if (size > largestMemBlock->fsb[SIZE])
		return UNDEFINED;
	// otherwise we have sufficent memory
	else
	{	// set the return base address to the largest block base address
		int returnAddress = largestMemBlock->fsb[BASE_ADDRESS];
		
		// if the requested size is the same as the largest block
		// delete the largest block
		if (size == largestMemBlock->fsb[SIZE])
			memTable->erase(largestMemBlock);
		else
		{	// otherwise change the size of the largest block
			// and also change the base address of the largest block
			largestMemBlock->fsb[SIZE] -= size;
			largestMemBlock->fsb[BASE_ADDRESS] += size;
		}
		// set the new largest block
		SetLargestBlock();
		// return the base address of the memory the new process can use
		return returnAddress;
	}
}

// goes through the memory table to find the largest block of free memory
void WorstFit::SetLargestBlock()
{	list<SpaceBlock>::iterator iterator = memTable->begin();
	largestMemBlock = memTable->begin();
	
	if (largestMemBlock == memTable->end()) return;

	advance(iterator, 1);
	while (iterator != memTable->end())
	{	if (largestMemBlock->fsb[SIZE] < iterator->fsb[SIZE])
			largestMemBlock = iterator;
		advance(iterator, 1);
	}
}

void WorstFit::ReturnMemory(const int baseAddress, const int size)
{	// if there is no free space left, then make a new
	// free space and make the largestMemBlock and front point at it
	if (largestMemBlock == memTable->end())
	{	memTable->push_front(SpaceBlock(size, baseAddress));
		largestMemBlock = memTable->begin();
		return;
	}

	// find the position where the memory block might belong to
	list<SpaceBlock>::iterator iterator = memTable->begin();
	while (iterator != memTable->end())
	{	if (baseAddress < iterator->fsb[BASE_ADDRESS])
			break;
		advance(iterator, 1);
	}

	// if the block belongs somewhere at the begining
	if (iterator == memTable->begin())
	{	// if the block cannot merge and belongs at the begining
		if (baseAddress + size < iterator->fsb[BASE_ADDRESS])
		{	memTable->push_front(SpaceBlock(size, baseAddress));
			SetLargestBlock();
		}
		// if the block belongs at the begining and can merge with the next block
		else if (baseAddress + size == iterator->fsb[BASE_ADDRESS])
		{	iterator->fsb[BASE_ADDRESS] -= size;
			iterator->fsb[SIZE] += size;
			SetLargestBlock();
		}
	}
	else
	{	list<SpaceBlock>::iterator prevBlock = prev(iterator);
		// if the memory block belongs somewhere at the ned
		if (iterator == memTable->end())
		{
			// if the memory block can merge with the previous block, do so
			if (prevBlock->fsb[BASE_ADDRESS] + prevBlock->fsb[SIZE] == baseAddress)
			{	prevBlock->fsb[SIZE] += size;
				SetLargestBlock();
			}
			// otherwise a new block is insterted at the end
			else
			{	memTable->push_back(SpaceBlock(size, baseAddress));
				SetLargestBlock();
			}
		}
		else
		{	// if the block is somewhere at the middle and can merge with the previous and next blocks
			if (prevBlock->fsb[BASE_ADDRESS] + prevBlock->fsb[SIZE] + size == iterator->fsb[BASE_ADDRESS])
			{	prevBlock->fsb[SIZE] += size + iterator->fsb[SIZE];
				memTable->erase(iterator);
				SetLargestBlock();
			}
			// if the new block can only merge with the previous block
			else if (prevBlock->fsb[BASE_ADDRESS] + prevBlock->fsb[SIZE] == baseAddress)
			{	prevBlock->fsb[SIZE] += size;
				SetLargestBlock();
			}
			// if the new block can only merge with the next block
			else if (baseAddress + size == iterator->fsb[BASE_ADDRESS])
			{	iterator->fsb[BASE_ADDRESS] -= size;
				iterator->fsb[SIZE] += size;
				SetLargestBlock();
			}
			// otherwise, it cant merge so insert it in between
			else
			{	memTable->insert(iterator, SpaceBlock(size, baseAddress));
				SetLargestBlock();
			}
		}
	}
}

// prints the memory blocks sequentially in order
void WorstFit::PrintMemoryBlocks()
{	list<SpaceBlock>::iterator iterator = memTable->begin();
	while (iterator != memTable->end())
	{	printf("Address: %i\n", iterator->fsb[BASE_ADDRESS]);
		printf("Size: %i\n", iterator->fsb[SIZE]);
		advance(iterator, 1);
	}
	printf("\n");
}