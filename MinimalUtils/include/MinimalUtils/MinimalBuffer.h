#pragma once

#include "MinimalUtils/MinimalStream.h"

namespace minimal::buffer
{
    using MemoryPointer = void*;
    using MemorySize = unsigned long long;

    class MinimalMemoryBuffer
    {
    public:
        MinimalMemoryBuffer();
        MinimalMemoryBuffer(MemorySize BufferSize);
        MinimalMemoryBuffer(MemorySize BufferSize, MemorySize BufferCount);
        virtual ~MinimalMemoryBuffer();

        bool AllocateBuffer(MemorySize BufferSize);
        bool AllocateBuffer(MemorySize BufferSize, MemorySize BufferCount);

        bool ReleaseBuffer();
        bool Reset();

        inline void* GetBuffer() { return bufferPointer; }
        template<typename T> inline T* GetBufferAsType() { return reinterpret_cast<T*>(bufferPointer); }
        inline MemorySize GetBufferSize() { return bufferSize; }
        inline MemorySize GetArrayCount() { return arrayCount; }

        inline bool IsArray() { return flags.bits.isBufferAnArray; }
        inline bool IsAllocated() { return flags.bits.hasBufferBeenAllocated; }

        inline stream::MinimalStream OpenReadOnly() { 
            stream::MinimalStream stream = stream::MinimalStream();
            stream.OpenReadOnly(this->bufferPointer, this->bufferSize);
            return stream;
        }

        inline stream::MinimalStream OpenWriteOnly() { 
            stream::MinimalStream stream = stream::MinimalStream();
            stream.OpenWriteOnly(this->bufferPointer, this->bufferSize);
            return stream;
        }
    private:
        void* bufferPointer;

        // Size of the original data type.
        MemorySize bufferSize;
        // Always 1 when initialized without an array.
        MemorySize arrayCount;

        union
        {
            struct
            {
                bool hasBufferBeenAllocated : 1;
                bool hasBufferBeenSet : 1;
                bool isBufferAnArray : 1;
            } bits;
            unsigned char value;
        } flags;
    };

}