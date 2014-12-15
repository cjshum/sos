#pragma once
enum FEILD_VALUES { BASE_ADDRESS, SIZE };

class SpaceBlock
{
	typedef int type_column;
public:
	type_column column[2];

	SpaceBlock() {}

	SpaceBlock(int size, int baseAddress=0)
	{
		column[SIZE] = size;
		column[BASE_ADDRESS] = baseAddress;
	}
};