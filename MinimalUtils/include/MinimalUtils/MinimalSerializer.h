#pragma once

#include "MinimalUtils/MinimalBuffer.h"

#include <iostream>

namespace minimal::serializer
{

    class MinimalSerializer
    {
    public:
        static inline buffer::MemorySize GetFinalSerializationSize(buffer::MemorySize DataSize)
        {
            return DataSize * 2;
        }

        static inline buffer::MemorySize GetFinalDeserializationSize(buffer::MemorySize TextSize)
        {
            return TextSize / 2;
        }

        static inline std::string SerializeData(buffer::MinimalMemoryBuffer* Buffer)
        {
            return SerializeData(Buffer->GetBuffer(), Buffer->GetBufferSize());
        }

        static std::string SerializeData(void* DataBuffer, buffer::MemorySize DataSize);
        static void* DeserializeData(std::string Text, void* DataBuffer);
        static buffer::MinimalMemoryBuffer* FastDeserializeData(std::string Text);
    };

}