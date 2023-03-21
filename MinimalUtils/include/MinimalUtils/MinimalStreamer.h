#pragma once

#include "MinimalUtils/MinimalStream.h"
#include "MinimalUtils/MinimalBuffer.h"

#define IMPLEMENT_MINIMAL_STREAM_OBJECT(name, identifier)   minimal::stream::MinimalStreamObjectId GetIdentifierId() { return identifier ; } \
                                                            static constexpr minimal::stream::MinimalStreamObjectId GetStaticIdentifierId() { return identifier; } \
                                                            const char* GetIdentifierName() { return name ; } \
                                                            minimal::buffer::MemorySize GetExportSize(); \
                                                            bool ImportObject(minimal::stream::MinimalStream* ImportStream); \
                                                            bool ExportObject(minimal::stream::MinimalStream* ExportStream);


namespace minimal::stream
{

    using MinimalStreamObjectId = unsigned long long;

    class MinimalStreamObject
    {
    public:
        virtual ~MinimalStreamObject() = default;
        virtual MinimalStreamObjectId GetIdentifierId() { return -1; }
        virtual const char* GetIdentifierName() { return "MinimalStreamObject base class."; }
        virtual buffer::MemorySize GetExportSize() { return 0; }
        virtual bool ImportObject(stream::MinimalStream* ImportStream) { return false; }
        virtual bool ExportObject(stream::MinimalStream* ExportStream) { return false; }
    };

}