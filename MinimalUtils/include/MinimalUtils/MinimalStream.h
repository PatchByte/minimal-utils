#pragma once

#if _DEBUG
#include <iostream>
#endif

namespace minimal::stream
{

    enum class MinimalStreamMode
    {
        INVALID,
        READ_ONLY,
        WRITE_ONLY
    };
    
    class MinimalStream
    {
    public:
        MinimalStream();

        MinimalStream* OpenWriteOnly(void* Buffer, unsigned long long BufferSize);
        MinimalStream* OpenReadOnly(void* Buffer, unsigned long long BufferSize);

        unsigned long long GetCursorPosition() { return streamPosition; }

        bool IsEOF() { return streamPosition >= streamSpace; }

        bool HasError() { return streamError; }

        bool IsReadyOnly() { return this->streamMode == MinimalStreamMode::READ_ONLY; }
        bool IsWriteOnly() { return this->streamMode == MinimalStreamMode::WRITE_ONLY; }

        template<typename T>
        T Read()
        {
            T value = { 0 };
            Read(&value);
            return value;
        }

        template<typename T>
        T* Read(T* _buffer)
        {

            if(!IsReadyOnly()) 
            { 
                #if _DEBUG
                std::cerr << "!IsReadOnly() L" << __LINE__ << std::endl; 
                #endif

                streamError = true;
                return nullptr; 
            }

            unsigned char *buffer = (unsigned char*)_buffer; 

            if(IsEOF())
            { 
                #if _DEBUG
                std::cerr << "Read IsEOF fail. " << sizeof(T) << std::endl;
                #endif

                streamError = true;
                return nullptr;
            }

            if((streamPosition + sizeof(T)) > streamSpace)
            {
                return nullptr;
            }

            for(unsigned long long i = 0; i < sizeof(T); i++)
            {
                buffer[i] = streamData[streamPosition+i];
            }

            streamPosition += sizeof(T);

            return _buffer;
        }

        template<typename T>
        void Write(T buffer)
        {
            Write(&buffer);
        }

        template<typename T>
        T* Write(T* _buffer)
        {
            if(!IsWriteOnly()) { return nullptr; }

            unsigned char *buffer = (unsigned char*)_buffer; 

            if(IsEOF())
            {
                #if _DEBUG
                std::cerr << "IsEOF Write check failed. " << sizeof(T) << std::endl;
                #endif

                streamError = true;
                return nullptr;
            }

            if((streamPosition + sizeof(T)) > streamSpace)
            {
                streamError = true;
                return nullptr;
            }

            for(unsigned long long i = 0; i < sizeof(T); i++)
            {
                streamData[streamPosition+i] = buffer[i];
            }

            streamPosition += sizeof(T);

            return _buffer;
        }

        void* ReadRawBuffer(void* _buffer, unsigned long long len)
        {
            if(!IsReadyOnly()) { return nullptr; }

            unsigned char *buffer = (unsigned char*)_buffer; 

            if(IsEOF())
            {
                streamError = true;
                return nullptr;
            }

            if((streamPosition + len) > streamSpace)
            {
                streamError = true;
                return nullptr;
            }

            for(unsigned long long i = 0; i < len; i++)
            {
                buffer[i] = streamData[streamPosition+i];
            }

            streamPosition += len;

            return _buffer;
        }

        void* WriteRawBuffer(void* _buffer, unsigned long long len)
        {
            if(!IsWriteOnly()) { return nullptr; }

            unsigned char *buffer = (unsigned char*)_buffer; 

            if(IsEOF())
            {
                streamError = true;
                return nullptr;
            }

            if((streamPosition + len) > streamSpace)
            {
                streamError = true;
                return nullptr;
            }

            for(unsigned long long i = 0; i < len; i++)
            {
                streamData[streamPosition+i] = buffer[i];
            }

            streamPosition += len;

            return _buffer;
        }
    private:
        MinimalStreamMode streamMode;
        unsigned char* streamData;
        unsigned long long streamPosition;
        unsigned long long streamSpace;
        bool streamError;
    };

}