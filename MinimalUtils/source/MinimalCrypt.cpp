#include "MinimalUtils/MinimalCrypt.h"

#include <cstring>
#include <cstdio>

namespace minimal::crypt
{

    CryptCertificate::CryptCertificate()
    {
        Reset();
    }

    CryptCertificate::CryptCertificate(minimal::hash::MinimalHash seed)
    {
        Reset();
        GenerateFromSeed(seed);
    }

    CryptCertificate::~CryptCertificate()
    {}

    hash::MinimalHash CryptCertificate::GetValue(hash::MinimalHash Seed)
    {
        hash::MinimalHash seed = hash::MinimalHash64(Seed * hash::MinimalHash64(this->binder[0x1f]));
        seed ^= hash::MinimalHash64(this->key[0x7f]);

        int index = ((int)((seed & 0xFFFFFFFF) ^ ((seed >> 32) & 0xFFFFFFFF)) & (~(1 << 31))) % smKeySize;
        index = this->binder[index] & 0xFF;

        return hash::MinimalHash64(
                this->key[index]
        );
    }

    void CryptCertificate::GenerateFromSeed(minimal::hash::MinimalHash seed)
    {
        Reset();
        
        hash::MinimalHash nextSeed = hash::MinimalPrng64(seed);

        for(size_t i = 0; i < smKeySize; i++)
        {
            this->binder[i] = hash::MinimalPrng64(nextSeed) & 0xFF;
            this->key[i] = (hash::MinimalPrng64(nextSeed) >> 1) & 0xFF;
            nextSeed = hash::MinimalPrng64(nextSeed);
        }
    }
    
    void CryptCertificate::Reset()
    {
        memset(this->binder, 0, sizeof(this->binder));
        memset(this->key, 0, sizeof(this->key));
    }

    void CryptCertificate::Update(unsigned char* dataBuffer, unsigned long long dataSize)
    {
        for(size_t i = 0; i < dataSize; i++)
        {
            dataBuffer[i] ^= this->GetValue(i);
        }
    }

    CryptCertificate* CryptCertificate::Merge(CryptCertificate& mergeWith)
    {
        for(size_t i = 0; i < smKeySize; i++)
        {
            this->binder[i] ^= mergeWith.binder[i];
            this->key[i] ^= mergeWith.key[i];
        }
        
        return this;
    }

}