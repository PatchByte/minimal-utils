#include "MinimalUtils/MinimalMessageHandler.h"

namespace minimal::message
{

    bool MinimalMessageHandlerPool::RegisterHandler(MessageId MessageDataId, MinimalMessageHandler MessageHandler)
    {
        if(registeredMessageHandlers.count(MessageDataId) > 0)
        {
            return false;
        }

        registeredMessageHandlers.emplace(MessageDataId, MessageHandler);

        return true;
    }
    
    bool MinimalMessageHandlerPool::Handle(buffer::MinimalMemoryBuffer* MessageData, void* MessageParameter)
    {
        MessageId peekId = MinimalMessage::PeekMessageDataId(MessageData);

        if(registeredMessageHandlers.count(peekId) <= 0)
        {
            return false;
        }

        MinimalMessageHandler messageHandler = registeredMessageHandlers.at(peekId);
        MinimalMessageHandlerParameters* messageParameters = new MinimalMessageHandlerParameters();

        messageParameters->messageBufferData = MessageData;
        messageParameters->messageParameter = MessageParameter;

        messageHandler(messageParameters);

        delete messageParameters;

        return true;
    }

}