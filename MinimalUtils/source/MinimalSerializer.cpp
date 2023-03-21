#include "MinimalUtils/MinimalSerializer.h"

namespace minimal::serializer
{

    static constexpr char smSerialiserKey = 'A';
    static constexpr char smMaxSerialiserKey = 'A' + 0xF;

    std::string MinimalSerializer::SerializeData(void* DataBuffer, buffer::MemorySize DataSize)
    {
        std::string textString = std::string(DataSize * 2, ' ');

        uint8_t* input = static_cast<uint8_t*>(DataBuffer);
        char* output = (char*) textString.c_str();

        for(uint64_t i = 0; i < DataSize; i++)
        {
            *output = smSerialiserKey + ((input[i] >> 0) & 0xF);
            output++;
            *output = smSerialiserKey + ((input[i] >> 4) & 0xF);
            output++;
        }

        return textString;
    }

    void* MinimalSerializer::DeserializeData(std::string Text, void* DataBuffer)
    {
        uint8_t* output = static_cast<uint8_t*>(DataBuffer);
        char* input = (char*)Text.c_str();
        buffer::MemorySize length = Text.length() / 2;

        for(size_t i = 0; i < length; i++)
        {
            uint8_t val = 0;

            val |= (*input - smSerialiserKey) << 0;
            input++;
            val |= (*input - smSerialiserKey) << 4;
            input++;

            output[i] = val;
        }


        return DataBuffer;
    }

    buffer::MinimalMemoryBuffer* MinimalSerializer::FastDeserializeData(std::string Text)
    {
        if(Text.length() < 2)
        {
            return nullptr;
        }

        buffer::MinimalMemoryBuffer* buffer = new buffer::MinimalMemoryBuffer(Text.length() / 2);
        DeserializeData(Text, buffer->GetBuffer());

        return buffer;
    }

}