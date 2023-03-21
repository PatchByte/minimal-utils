#pragma once

#include "MinimalUtils/MinimalBuffer.h"
#include "MinimalUtils/MinimalStreamer.h"
#include "MinimalUtils/MinimalHash.h"

namespace minimal::message
{
    using MessageId = unsigned long long;

    class MinimalMessage
    {
    public:
        static buffer::MemorySize GetMessageMaxSize();
        static buffer::MinimalMemoryBuffer* Export(stream::MinimalStreamObject* StreamObject, minimal::hash::MinimalHash ColliderHash = 0);

        // If you don't know what the StreamObject is use MinimalMessage::PeekMessageDataId
        static stream::MinimalStreamObject* Import(buffer::MinimalMemoryBuffer* StreamBuffer, stream::MinimalStreamObject* StreamObject, minimal::hash::MinimalHash ColliderHash = 0);

        static MessageId PeekMessageDataId(buffer::MinimalMemoryBuffer* StreamObject);
    };

}