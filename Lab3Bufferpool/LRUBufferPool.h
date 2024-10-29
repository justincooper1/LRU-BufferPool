#pragma once
#include "BufferPoolADT.h"
#include "BufferBlock.h"
#include <vector>
#include <algorithm>
#include <fstream>
#include <iostream>

class LRUBufferPool : public BufferPoolADT
{
private:
    vector<BufferBlock*> bufferPool;
    string filename;
    int poolSize;
    int blockSize;

public:
    // Constructor
    LRUBufferPool(string filename, int poolSize = 5, int blockSize = 4096) : filename(filename), poolSize(poolSize), blockSize(blockSize)
    {
        bufferPool.reserve(poolSize);

        // Initialize buffer pool
        for (int i = 0; i < poolSize; ++i)
        {
            bufferPool.push_back(new BufferBlock(blockSize));
        }

        // Fill the buffer blocks with data from the file
        ifstream file(filename, ios::binary);
        if (!file.is_open())
        {
            cerr << "Error: Unable to open file " << filename << endl;
            return;
        }

        char* data = new char[blockSize];
        for (int i = 0; i < poolSize; ++i)
        {
            if (!file.read(data, blockSize))
            {
                cerr << "Error: Unable to read data from file " << filename << endl;
                break;
            }
            bufferPool[i]->setBlock(data);
            bufferPool[i]->setID(i);
        }
        delete[] data;
        file.close();
    }

    // Destructor
    ~LRUBufferPool()
    {
        for (BufferBlock* block : bufferPool)
        {
            delete block;
        }
    }

    // Copy "sz" bytes from position "pos" of the buffered
    //   storage to "space".
    void getBytes(char* space, int sz, int pos) override
    {
        ifstream file(filename, ios::binary | ios::ate); // Open file in binary and sets pointer to EOF
        int fileSize = file.tellg(); // Current position in file
        file.close();

        // Checks if position is in the file size
        // Throws error if not
        if (pos < 0 || pos + sz > fileSize)
        {
            cerr << "Error: Invalid position requested. Position: " << pos << ", Size: " << sz << ", File Size: " << fileSize << endl;
            return;
        }

        // Find relative position and block index
        int relativePos = pos % blockSize;
        int blockIndex = pos / blockSize;

        BufferBlock* block = nullptr;
        bool blockFound = false;

        // Check if block is in buffer pool
        for (auto& bufBlock : bufferPool) // Range based 
        {
            if (bufBlock->getID() == blockIndex)
            {
                block = bufBlock;
                blockFound = true;
                break;
            }
        }

        // Load block if not in buffer pool
        if (!blockFound)
        {
            // Read block
            ifstream file(filename, ios::binary);
            file.seekg(blockIndex * blockSize);
            char* data = new char[blockSize];
            if (!file.read(data, blockSize))
            {
                cerr << "Error: Unable to read data from file for block index " << blockIndex << endl;
                delete[] data;
                return;
            }

            // Replace the LRU
            BufferBlock* leastUsedBlock = bufferPool.back();
            bufferPool.pop_back();
            delete leastUsedBlock;

            block = new BufferBlock(blockSize);
            block->setBlock(data);
            block->setID(blockIndex);
            bufferPool.insert(bufferPool.begin(), block);
            delete[] data;
            file.close();
        }
        else
        {
            // Move block to front
            bufferPool.erase(remove(bufferPool.begin(), bufferPool.end(), block), bufferPool.end());
            bufferPool.insert(bufferPool.begin(), block);
        }

        // Read data from buffer block
        block->getData(relativePos, sz, space);
    }

    // Print the order of the buffer blocks using the block id
    //	 numbers.
    void printBufferBlockOrder() override
    {
        cout << "Buffer block order from most recently used to LRU:" << endl;
        for (BufferBlock* block : bufferPool)
        {
            cout << block->getID() << ", ";
        }
        cout << endl;
    }

    // Get the block id number of the least recently used 
    //	 buffer block.
    int getLRUBlockID() override
    {
        return bufferPool.back()->getID();
    }
};
