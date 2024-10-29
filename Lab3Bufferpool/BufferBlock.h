#pragma once

#include "BufferBlockADT.h"
#include <cstring>

class BufferBlock : public BufferBlockADT
{
private:
	int blockID;
	char* block;
	int blockSize;

public:
	BufferBlock(int blockSize = 4096) : blockSize(blockSize)
	{
		block = new char[blockSize];
		blockID = -1;
	}
	~BufferBlock()
	{
		delete[] block;
	}

    //read the block from pos to pos + sz-1 (or to the end of the block)
	void getData(int pos, int sz, char* data) override
	{
		if (pos + sz > blockSize)
		{
			sz = blockSize - pos;
		}
		memcpy(data, block + pos, sz);
	}

    //setID
	void setID(int id) override
	{
		blockID = id;
	}

    //getID
	int getID() const override
	{
		return blockID;
	}

    //getBlocksize
	int getBlockSize() const override
	{
		return blockSize;
	}

    //return the block
	char* getBlock() const override
	{
		return block;
	}

    //set the block
	void setBlock(char* blk) override
	{
		memcpy(block, blk, blockSize);
	}
};