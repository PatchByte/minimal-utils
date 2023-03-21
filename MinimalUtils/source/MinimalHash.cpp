#include "MinimalUtils/MinimalHash.h"

#include <stdint.h>
#include <random>

namespace minimal::hash
{
    MinimalHash MinimalRandom()
    {
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> dist(0,INT32_MAX);

        MinimalHash buildValue = 0;

        buildValue |= (dist(rng) << 0) & UINT32_MAX;
        buildValue |= (dist(rng) << 31) & UINT32_MAX;

        return buildValue;
    }

    MinimalHash MinimalPrng64(MinimalHash Value)
    {
        unsigned short part1 = (short)((Value >> 0) & 0xFFFF);
        unsigned short part2 = (short)((Value >> 16) & 0xFFFF);
        unsigned short part3 = (short)((Value >> 24) & 0xFFFF);
        unsigned short part4 = (short)((Value >> 32) & 0xFFFF);

        part1 ^= Value;
        part1 ^= (unsigned short)(Value >> 5) & 0xFFFF;

        part2 ^= Value >> 3;

        MinimalHash valPrng = ~Value >> 4;
        MinimalHash valPrng2 = (part1 << 0) | (part2 << 16) | ((MinimalHash) part3 << 32) | ((MinimalHash) part4 << 48);

        for(int i = 0; i < 8; i++)
        {
            valPrng ^= (valPrng2 ^ (Value >> i)) << (i * 8);
            valPrng ^= ((Value * 9) ^ ((Value >> 43) & 0xFFFF)) << (i * 8);
        }

        return valPrng;
    }

    MinimalHash MinimalHash64(MinimalHash Value)
    {
        return MinimalPrng64(   // pre shuffle
            //MinimalPrng64(      // safe shuffle
                Value
            //)
        );
    }

    MinimalHash MinimalDataHash64(void* DataBuffer, MinimalHash DataSize)
    {
        MinimalHash currentHash = 1;
        uint8_t* dataBuffer = (uint8_t*)DataBuffer;
        MinimalHash leftOverSize = DataSize;

        for(MinimalHash i = 0; i < DataSize; i++)
        {
            currentHash ^= MinimalHash64(dataBuffer[i]);
            currentHash *= MinimalHash64(dataBuffer[i]); 
        }
        
        return currentHash;
    }

}