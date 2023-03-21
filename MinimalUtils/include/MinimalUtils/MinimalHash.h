#pragma once

#include <iostream>

namespace minimal::hash
{
    using MinimalHash = unsigned long long;

    MinimalHash MinimalRandom();
    MinimalHash MinimalPrng64(MinimalHash Value);
    MinimalHash MinimalHash64(MinimalHash Value);
    MinimalHash MinimalDataHash64(void* DataBuffer, MinimalHash DataSize);
    inline MinimalHash MinimalStringHash64(std::string Value) { return MinimalPrng64(MinimalDataHash64((void*)(Value.c_str()), Value.length())); }

}