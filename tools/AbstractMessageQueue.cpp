#include "tools/AbstractMessageQueue.hpp"
#include "tools/MessageQueueManager.hpp"

namespace Tools {

    AbstractMessageQueue::AbstractMessageQueue(unsigned int nbThread)
    {
        MessageQueueManager::Register(this, nbThread);
    }

    AbstractMessageQueue::~AbstractMessageQueue()
    {
        MessageQueueManager::Unregister(this);
    }

    bool AbstractMessageQueue::_HasMessage()
    {
        boost::lock_guard<boost::mutex> lock(this->_mutex);
        return this->_messages.size() > 0;
    }

    AbstractMessageQueue::Message AbstractMessageQueue::_PopMessage()
    {
        boost::lock_guard<boost::mutex> lock(this->_mutex);
        Message m = this->_messages.front();
        this->_messages.pop();
        return m;
    }

    void AbstractMessageQueue::_PushMessage(AbstractMessageQueue::Message message)
    {
        {
            boost::lock_guard<boost::mutex> lock(this->_mutex);
            this->_messages.push(message);
        }
        MessageQueueManager::NotifyOne();
    }

}
