#pragma once

enum { BASE_ADDRESS, SIZE };

// represents what a free space block entry
class SpaceBlock
{	typedef int type_spaceblock;

public:
	type_spaceblock fsb[2];

	SpaceBlock() {}

	// default constructor
	SpaceBlock(int size, int baseAddress=0)
	{	fsb[SIZE] = size;
		fsb[BASE_ADDRESS] = baseAddress;
	}
};