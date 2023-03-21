#include "MinimalUtils/MinimalMessage.h"
#include "MinimalUtils/MinimalHash.h"
#include "MinimalUtils/MinimalCrypt.h"

#include <iostream>

namespace minimal::message
{
    static constexpr buffer::MemorySize smMessageLimit = 0x1FF;
    static constexpr MessageId smShuffleChallenge = 0x7368666C;

    class MinimalMessageHeader : public stream::MinimalStreamObject
    {
    public:
        MinimalMessageHeader():
            messageSize(0),
            messageDataId(0),
            messageFieldShuffle(0),
            messageCertificateSeed(0)
        {}

        MinimalMessageHeader* GenerateSeeds()
        {
            messageCertificateSeed = hash::MinimalPrng64(hash::MinimalRandom());
            messageFieldShuffle = hash::MinimalPrng64(hash::MinimalRandom());
            
            return this;
        }

        IMPLEMENT_MINIMAL_STREAM_OBJECT("MinimalMessageHeader", 1);
    
        buffer::MemorySize messageSize;
        MessageId messageDataId;
        MessageId messageCertificateSeed;
        MessageId messageFieldShuffle;
        hash::MinimalHash messageDecryptedHash;
    };

    // MinimalMessageHeader

    buffer::MemorySize MinimalMessageHeader::GetExportSize()
    {
        return  sizeof(MinimalMessageHeader::messageSize) +
                sizeof(MinimalMessageHeader::messageDataId) +
                sizeof(MinimalMessageHeader::messageFieldShuffle) +
                sizeof(MinimalMessageHeader::messageFieldShuffle) + // shuffle challenge
                sizeof(MinimalMessageHeader::messageCertificateSeed) +
                sizeof(MinimalMessageHeader::messageDecryptedHash);
    }

    bool MinimalMessageHeader::ExportObject(stream::MinimalStream* exportStream)
    {
        exportStream->Write(messageFieldShuffle);
        exportStream->Write(smShuffleChallenge ^ messageFieldShuffle);
        exportStream->Write(messageSize ^ messageFieldShuffle);
        exportStream->Write(messageDataId ^ messageFieldShuffle);
        exportStream->Write(messageCertificateSeed ^ messageFieldShuffle);
        exportStream->Write(messageDecryptedHash ^ messageFieldShuffle);

        return true;
    }

    bool MinimalMessageHeader::ImportObject(stream::MinimalStream* importStream)
    {
        messageFieldShuffle = importStream->Read<decltype(messageFieldShuffle)>();
        
        if((importStream->Read<unsigned long long>() ^ messageFieldShuffle) != smShuffleChallenge)
        {
            std::cerr << "Suffle challenge failed." << std::endl;
            std::exit(-1);
        }


        messageSize = importStream->Read<decltype(messageSize)>() ^ messageFieldShuffle;
        messageDataId = importStream->Read<decltype(messageDataId)>() ^ messageFieldShuffle;
        messageCertificateSeed = importStream->Read<decltype(messageCertificateSeed)>() ^ messageFieldShuffle;
        messageDecryptedHash = importStream->Read<decltype(messageDecryptedHash)>() ^ messageFieldShuffle;

        messageSize &= messageSize;
        
        return true;
    }

    // MinimalMessage

    static void ExportStreamObjectAndEncrypt(MinimalMessageHeader* messageHeader, stream::MinimalStreamObject* messageObject, stream::MinimalStream* messageStream, minimal::hash::MinimalHash colliderHash)
    {
        
        crypt::CryptCertificate certificate = crypt::CryptCertificate(messageHeader->messageCertificateSeed);
        crypt::CryptCertificate colliderCertificate = crypt::CryptCertificate(colliderHash);

        certificate.Merge(colliderCertificate);

        messageHeader->messageDataId = messageObject->GetIdentifierId();
        messageHeader->messageSize = messageObject->GetExportSize();

        buffer::MinimalMemoryBuffer* memoryBuffer = new buffer::MinimalMemoryBuffer(messageObject->GetExportSize());
        stream::MinimalStream memoryStream = stream::MinimalStream();

        // Object Export & Encryption
        messageObject->ExportObject(memoryStream.OpenWriteOnly(memoryBuffer->GetBuffer(), memoryBuffer->GetBufferSize()));
        hash::MinimalHash decryptedHash = hash::MinimalDataHash64(memoryBuffer->GetBuffer(), memoryBuffer->GetBufferSize());
        certificate.Update(memoryBuffer->GetBuffer(), memoryBuffer->GetBufferSize());

        // Append Hash into headers

        messageHeader->messageDecryptedHash = decryptedHash;
        
        // Header & Object Export into actual stream
        messageHeader->ExportObject(messageStream);
        messageStream->WriteRawBuffer(memoryBuffer->GetBuffer(), memoryBuffer->GetBufferSize());
        
        memoryBuffer->Reset();
        delete memoryBuffer;
    } 

    static bool ImportStreamObjectAndDecrypt(MinimalMessageHeader* messageHeader, stream::MinimalStreamObject* messageObject, stream::MinimalStream* messageStream, minimal::hash::MinimalHash colliderHash)
    {
        messageHeader->ImportObject(messageStream);

        crypt::CryptCertificate certificate = crypt::CryptCertificate(messageHeader->messageCertificateSeed);
        crypt::CryptCertificate colliderCertificate = crypt::CryptCertificate(colliderHash);

        certificate.Merge(colliderCertificate);

        buffer::MinimalMemoryBuffer* memoryBuffer = new buffer::MinimalMemoryBuffer(messageHeader->messageSize);
        stream::MinimalStream memoryStream = stream::MinimalStream();

        messageStream->ReadRawBuffer(memoryBuffer->GetBuffer(), memoryBuffer->GetBufferSize());
        certificate.Update(memoryBuffer->GetBuffer(), memoryBuffer->GetBufferSize());
        hash::MinimalHash decryptedHash = hash::MinimalDataHash64(memoryBuffer->GetBuffer(), memoryBuffer->GetBufferSize());
        messageObject->ImportObject(memoryStream.OpenReadOnly(memoryBuffer->GetBuffer(), memoryBuffer->GetBufferSize()));
        memoryBuffer->Reset();

        bool result = true;
        if(decryptedHash != messageHeader->messageDecryptedHash)
        {
            result = false;
            std::cerr << "if(hash::MinimalDataHash64(memoryBuffer->GetBuffer(), memoryBuffer->GetBufferSize()) != messageHeader->messageDecryptedHash)" << " [" << std::hex << decryptedHash << " != " << messageHeader->messageDecryptedHash << " ] " << std::endl;
        }
        
        if(messageStream->HasError())
        {
            result = false;
        }

        delete memoryBuffer;

        return result;
    }

    buffer::MemorySize MinimalMessage::GetMessageMaxSize()
    {
        return smMessageLimit;
    }

    buffer::MinimalMemoryBuffer* MinimalMessage::Export(stream::MinimalStreamObject* StreamObject, minimal::hash::MinimalHash ColliderHash)
    {
        MinimalMessageHeader* messageHeader = (new MinimalMessageHeader())->GenerateSeeds();
        buffer::MinimalMemoryBuffer* memoryBuffer = new buffer::MinimalMemoryBuffer(StreamObject->GetExportSize() + messageHeader->GetExportSize());
        stream::MinimalStream memoryStream = memoryBuffer->OpenWriteOnly();

        ExportStreamObjectAndEncrypt(messageHeader, StreamObject, &memoryStream, ColliderHash);

        delete messageHeader;
        return memoryBuffer;
    }

    stream::MinimalStreamObject* MinimalMessage::Import(buffer::MinimalMemoryBuffer* StreamBuffer, stream::MinimalStreamObject* StreamObject, minimal::hash::MinimalHash ColliderHash)
    {
        MinimalMessageHeader* messageHeader = new MinimalMessageHeader();
        stream::MinimalStream memoryStream = stream::MinimalStream();
        
        memoryStream.OpenReadOnly(StreamBuffer->GetBuffer(), StreamBuffer->GetBufferSize());

        if(!ImportStreamObjectAndDecrypt(messageHeader, StreamObject, &memoryStream, ColliderHash))
        {
            return nullptr;
        }
        
        if(StreamObject->GetIdentifierId() != messageHeader->messageDataId)
        {
            std::cerr << "StreamObject->GetIdentifierId() != messageHeader->messageDataId L" << __LINE__ << " (" << messageHeader->messageDataId << " != " << StreamObject->GetIdentifierId() << ")" << std::endl;
            return nullptr;
        }

        if(memoryStream.HasError())
        {
            std::cerr << "memoryStream.HasError()" << std::endl;
            return nullptr;
        }
        
        return StreamObject;
    }

    MessageId MinimalMessage::PeekMessageDataId(buffer::MinimalMemoryBuffer* StreamObject)
    {
        MinimalMessageHeader* messageHeader = new MinimalMessageHeader();
        stream::MinimalStream memoryStream = stream::MinimalStream();
        
        memoryStream.OpenReadOnly(StreamObject->GetBuffer(), StreamObject->GetBufferSize());
        messageHeader->ImportObject(&memoryStream);

        MessageId messageId = messageHeader->messageDataId;
        
        delete messageHeader;
        
        return messageId;
    }

}