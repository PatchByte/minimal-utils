#include "MinimalUtils/MinimalStream.h"

namespace minimal::stream
{

    MinimalStream::MinimalStream():
        streamMode(MinimalStreamMode::INVALID),
        streamData(nullptr),
        streamPosition(0),
        streamSpace(0)
    {}

    MinimalStream* MinimalStream::OpenWriteOnly(void* Buffer, unsigned long long BufferSize)
    {
        this->streamMode = MinimalStreamMode::WRITE_ONLY;
        this->streamData = (unsigned char*)Buffer;
        this->streamPosition = 0;
        this->streamSpace = BufferSize;
        this->streamError = false;
        return this;
    }

    MinimalStream* MinimalStream::OpenReadOnly(void* Buffer, unsigned long long BufferSize)
    {
        this->streamMode = MinimalStreamMode::READ_ONLY;
        this->streamData = (unsigned char*)Buffer;
        this->streamPosition = 0;
        this->streamSpace = BufferSize;
        this->streamError = false;
        return this;
    }

}