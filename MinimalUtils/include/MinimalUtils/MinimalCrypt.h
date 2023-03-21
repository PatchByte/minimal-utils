#pragma once

#include "MinimalUtils/MinimalHash.h"

namespace minimal::crypt
{
    
    #pragma pack(push)
    #pragma pack(1)

    class CryptCertificate
    {
    public:
        CryptCertificate();
        CryptCertificate(minimal::hash::MinimalHash seed);
        ~CryptCertificate();

        hash::MinimalHash GetValue(minimal::hash::MinimalHash seed);
        void GenerateFromSeed(minimal::hash::MinimalHash seed);
        void Reset();

        inline void Update(void* dataBuffer, unsigned long long dataSize)
        {
            Update((unsigned char*)dataBuffer, dataSize);
        }

        void Update(unsigned char* dataBuffer, unsigned long long dataSize);

        CryptCertificate* Merge(CryptCertificate& mergeWith);
        
        static constexpr unsigned long long smKeySize = 0x100;
    private:
        unsigned char key[smKeySize];
        unsigned char binder[smKeySize];
   };

    #pragma pack(pop)

}