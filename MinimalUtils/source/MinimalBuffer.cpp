#include "MinimalUtils/MinimalBuffer.h"

#include <malloc.h>
#include <cstring>

namespace minimal::buffer
{

    MinimalMemoryBuffer::MinimalMemoryBuffer():
        flags({ 0 }),
        arrayCount(0),
        bufferSize(0),
        bufferPointer(nullptr)
    {
        Reset();
    }

    MinimalMemoryBuffer::MinimalMemoryBuffer(MemorySize BufferSize)
    {
        Reset();
        AllocateBuffer(BufferSize);
    }

    MinimalMemoryBuffer::MinimalMemoryBuffer(MemorySize BufferSize, MemorySize BufferCount)
    {
        Reset();
        AllocateBuffer(BufferSize, BufferCount);
    }

    MinimalMemoryBuffer::~MinimalMemoryBuffer()
    {
        Reset();
    }

    bool MinimalMemoryBuffer::AllocateBuffer(MemorySize BufferSize) 
    {
        Reset();
        
        void* allocatedBuffer = malloc(BufferSize);
        memset(allocatedBuffer, 0, BufferSize);
        if(allocatedBuffer == nullptr)
        {
            return false;
        }

        this->bufferPointer = allocatedBuffer;
        this->bufferSize = BufferSize;
        this->arrayCount = 1;
        this->flags.value = 0;
        this->flags.bits.hasBufferBeenAllocated = true;
        this->flags.bits.hasBufferBeenSet = true;

        return true;
    }

    bool MinimalMemoryBuffer::AllocateBuffer(MemorySize BufferSize, MemorySize BufferCount)
    {
        if(BufferCount <= 0)
        {
            return false;
        }

        bool result = AllocateBuffer(BufferSize * BufferCount);

        if(result == false)
        {
            return result;
        }

        this->flags.bits.isBufferAnArray = 1;
        this->arrayCount = BufferCount;

        return true;
    }

    bool MinimalMemoryBuffer::ReleaseBuffer()
    {
        if(this->flags.bits.hasBufferBeenAllocated && this->flags.bits.hasBufferBeenSet)
        {
            this->flags.bits.hasBufferBeenAllocated = 0;
            this->flags.bits.hasBufferBeenSet = 0;

            void* memoryBuffer = this->bufferPointer;
            this->bufferPointer = nullptr; 

            free(memoryBuffer);

            return true;
        }

        return false;
    }

    bool MinimalMemoryBuffer::Reset()
    {
        ReleaseBuffer();
        
        this->bufferPointer = nullptr;
        this->bufferSize = 0;
        this->arrayCount = 0;
        this->flags.value = 0;

        return true;
    }

}