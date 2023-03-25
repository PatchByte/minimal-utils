#pragma once
#include "MinimalHash.h"
#include "MinimalStreamer.h"
#include <iostream>

namespace minimal::structs
{

    class MinimalString : public stream::MinimalStreamObject
    {
    public:
        MinimalString();
        MinimalString(std::string Content);
        MinimalString(const char* Content);

        IMPLEMENT_MINIMAL_STREAM_OBJECT("MinimalString", 0xe1);

        std::string GetContent() { return content; }

        void operator= (std::string Str)
        {
            content = Str;
        }

        void operator= (const char* Str)
        {
            content = Str;
        }

        void operator= (char* Str)
        {
            content = Str;
        }

        buffer::MemorySize GetContentLimit() { return contentLimit; }

    private:
        MinimalString* GenerateContentFlipSeed();

        unsigned char contentFlipSeed;
        buffer::MemorySize contentLimit;
        std::string content;
    };

}