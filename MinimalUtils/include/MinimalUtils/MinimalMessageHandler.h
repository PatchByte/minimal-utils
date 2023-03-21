#pragma once

#include "MinimalUtils/MinimalMessage.h"
#include <map>
#include <functional>

namespace minimal::message
{
    using MinimalMessageHandler = std::function<void(class MinimalMessageHandlerParameters*)>;

    class MinimalMessageHandlerParameters
    {
    public:
        MinimalMessageHandlerParameters(): messageBufferData(nullptr), messageParameter(nullptr) {}
        ~MinimalMessageHandlerParameters() = default;

        inline minimal::buffer::MinimalMemoryBuffer* GetMemoryBuffer() { return messageBufferData; }
        template<typename T> inline T GetMessageParameter() { return reinterpret_cast<T>(messageParameter); } 
    private:
        friend class MinimalMessageHandlerPool;

        // Warning this isn't handled by the MinimalMessageHandlerParameteres class
        minimal::buffer::MinimalMemoryBuffer* messageBufferData;
        void* messageParameter;
    };

    class MinimalMessageHandlerPool
    {
    public:
        MinimalMessageHandlerPool() {}
        ~MinimalMessageHandlerPool() = default;

        bool RegisterHandler(MessageId MessageDataId, MinimalMessageHandler MessageHandler);
        bool Handle(buffer::MinimalMemoryBuffer* MessageData, void* MessageParameter);
    private:
        std::map<MessageId, MinimalMessageHandler> registeredMessageHandlers;
    };

}