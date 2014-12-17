#include <stdio.h>
#include "..\Node\Node.cpp"
#include "SpaceBlock.cpp"

#pragma once

enum WorstFitErr { INSUFFICENT_MEM=-1 };

class WorstFit
{
private:
	Node<SpaceBlock>* largestMemBlock;
	Node<SpaceBlock>* front;

	// goes through each free space block and sets largestMemBlock pointer to the
	// largest available free space block
	void SetLargestBlock()
	{
		Node<SpaceBlock>* iterator = front;
		largestMemBlock = front;
		while (iterator->next != NULL)
		{
			if (largestMemBlock->data->column[SIZE] < iterator->next->data->column[SIZE])
				largestMemBlock = iterator->next;
			iterator = iterator->next;
		}
	}

	void DeleteBlock(Node<SpaceBlock>* deletingBlock)
	{
		Node<SpaceBlock>* prevNode = deletingBlock->prev;
		Node<SpaceBlock>* nextNode = deletingBlock->next;

		// the block to delete is the last one
		if (front->next == NULL)
		{
			largestMemBlock = NULL;
			front = NULL;
		}
		// re-link the previous and next block together
		else
		{
			if (prevNode != NULL)
				prevNode->next = nextNode;
			if (nextNode != NULL)
				nextNode->prev = prevNode;
		}
		delete deletingBlock->data;
		delete deletingBlock;
	}

public:
	WorstFit() {}

	WorstFit(const int memSize)
	{
		largestMemBlock = new Node<SpaceBlock>(SpaceBlock(memSize));
		front = largestMemBlock;
	}

	int RequestMemory(const int size)
	{
		// if there is no memory left
		if (largestMemBlock == NULL)
			return INSUFFICENT_MEM;
		// if the requested memory size is greater than largest memory block left
		if (size > largestMemBlock->data->column[SIZE])
			return INSUFFICENT_MEM;
		
		// otherwise we have sufficent memory
		else
		{
			// set the return base address to the largest block base address
			int returnAddress = largestMemBlock->data->column[BASE_ADDRESS];
			
			// if the requested size is the same as the largest block
			// delete the largest block
			if (size == largestMemBlock->data->column[SIZE])
			{
				DeleteBlock(largestMemBlock);
			}
			else
			{
				// otherwise change the size of the largest block
				// and also change the base address of the largest block
				largestMemBlock->data->column[SIZE] -= size;
				largestMemBlock->data->column[BASE_ADDRESS] += size;
			}
			// set the new largest block
			SetLargestBlock();
			// return the base address of the memory the new process can use
			return returnAddress;
		}
	}


	void ReturnMemory(const int baseAddress, const int size)
	{
		// if there is no free space left, then make a new
		// free space and make the largestMemBlock and front
		// point at it
		if (largestMemBlock == NULL)
		{
			largestMemBlock = new Node<SpaceBlock>(SpaceBlock(size, baseAddress));
			front = largestMemBlock;
		}

		// otherwise, there is some other free space
		// blocks that we might need to merge with
		else
		{
			// a pointer that will loop through the blocks
			// to se where to put the free space memory block
			Node<SpaceBlock>* iterator = front;
			Node<SpaceBlock>* prevNode;
			// while i haven't gone through the entire linked list
			while (iterator != NULL)
			{
				// if the returning block is after the current pointing block
				// move the iterator to the next block
				if (baseAddress + size  > iterator->data->column[BASE_ADDRESS])
					iterator = iterator->next;
				else
				{
					prevNode = iterator->prev;
					// if i am pointing to the first block
					if (prevNode == NULL)
					{
						// if i can merge with the next block, don't have to create a new
						// node, just adjust the base address and size of the next block
						if (baseAddress + size == iterator->data->column[BASE_ADDRESS])
						{
							iterator->data->column[BASE_ADDRESS] -= size;
							iterator->data->column[SIZE] += size;
							SetLargestBlock();
							return;
						}
						// create a new free space block and set is as the front block
						else
						{
							Node<SpaceBlock>* newBlock = new Node<SpaceBlock>(SpaceBlock(size, baseAddress));
							newBlock->next = iterator;
							iterator->prev = newBlock;
							front = newBlock;
							SetLargestBlock();
							return;
						}
					}
					// otherwise, our block belongs somewhere in the middle

					// if the new space block can merge with the previous and next block
					else if (prevNode->data->column[BASE_ADDRESS] + prevNode->data->column[SIZE] + size == iterator->data->column[BASE_ADDRESS])
					{
						prevNode->data->column[SIZE] += size + iterator->data->column[SIZE];
						prevNode->next = iterator->next;
						DeleteBlock(iterator);
						SetLargestBlock();
						return;
					}
					// or if the new space block can only merge with the previous block
					else if (prevNode->data->column[BASE_ADDRESS] + prevNode->data->column[SIZE] == baseAddress)
					{
						prevNode->data->column[SIZE] += size;
						SetLargestBlock();
						return;
					}
					// or if the block can merge simply insert the block in between
					else
					{
						Node<SpaceBlock>* newBlock = new Node<SpaceBlock>(SpaceBlock(size, baseAddress));
						prevNode->next = newBlock;
						iterator->prev = newBlock;
						newBlock->prev = prevNode;
						newBlock->next = iterator;
						SetLargestBlock();
						return;
					}
				}
			}
			// otherwise there is no other blocks to merge with, this new block belongs towards the end
			Node<SpaceBlock>* newBlock = new Node<SpaceBlock>(SpaceBlock(size, baseAddress));
			prevNode = newBlock;
			newBlock->prev = prevNode;
			SetLargestBlock();
		}
	}

	// goes through the each block of free space memory and print its base address and size
	void PrintMemoryBlocks()
	{
		Node<SpaceBlock>* iterator = front;
		while (iterator != NULL)
		{
			printf("Address: %i\n", iterator->data->column[BASE_ADDRESS]);
			printf("Size: %i\n", iterator->data->column[SIZE]);
			iterator = iterator->next;
		}
	}
};