#include "MinimalUtils/MinimalString.h"
#include "MinimalUtils/MinimalHash.h"
#include <string>

namespace minimal::structs
{
    static constexpr buffer::MemorySize smStringLimit = 0x110;

    MinimalString::MinimalString():
        contentLimit(smStringLimit)
    {
    }

    MinimalString::MinimalString(std::string Content):
        content(Content),
        contentLimit(smStringLimit)
    {
    }

    MinimalString::MinimalString(const char* Content):
        content(Content),
        contentLimit(smStringLimit)
    {
    }

    MinimalString* MinimalString::GenerateContentFlipSeed()
    {
        this->contentFlipSeed = hash::MinimalPrng64(hash::MinimalRandom()) & 0xFF;
        return this;
    }

    // Stream Object function

    buffer::MemorySize MinimalString::GetExportSize()
    {
        return sizeof(contentFlipSeed) +
               sizeof(content.length()) +
               content.length() + 
               sizeof(hash::MinimalHash);
    }

    bool MinimalString::ExportObject(stream::MinimalStream* ExportStream)
    {
        GenerateContentFlipSeed();

        ExportStream->Write(contentFlipSeed);
        ExportStream->Write(content.length());

        hash::MinimalHash contentHash = 0;

        for(char c : content)
        {
            char cd = c ^ contentFlipSeed;
            contentHash ^= hash::MinimalHash64(c);
            ExportStream->Write(cd);
        }

        ExportStream->Write(contentHash);

        return true;
    }

    bool MinimalString::ImportObject(stream::MinimalStream* ImportStream)
    {
        size_t contentLength = 0;
        hash::MinimalHash contentHash = 0;
        hash::MinimalHash expectedHash = 0;

        ImportStream->Read(&contentFlipSeed);
        ImportStream->Read(&contentLength);

        if(ImportStream->HasError())
        {
            return false;
        }

        //contentLength &= contentLimit;
        content = "";

        for(size_t _ = 0; _ < contentLength; _++)
        {
            char c = ImportStream->Read<char>();
            c ^= contentFlipSeed;

            content.append(sizeof(c), c);

            contentHash ^= hash::MinimalPrng64(c);
        }

        ImportStream->Read<hash::MinimalHash>(&expectedHash);

        if(contentHash != expectedHash)
        {
            #if _DEBUG
            std::cerr << __FUNCTION__ << " Hash missmatch. " << std::hex << contentHash << " != " << expectedHash << std::endl;
            #endif

            content = "[[ INVALID IMPORT FROM MINIMAL LIB ]]";

            return false;
        }

        return true;

    }

}