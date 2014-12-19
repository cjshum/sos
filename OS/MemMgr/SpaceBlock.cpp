#pragma once

enum { BASE_ADDRESS, SIZE };

class SpaceBlock
{
	typedef int type_spaceblock;
public:
	type_spaceblock fsp[2];

	SpaceBlock() {}

	SpaceBlock(int size, int baseAddress=0)
	{
		fsp[SIZE] = size;
		fsp[BASE_ADDRESS] = baseAddress;
	}
};