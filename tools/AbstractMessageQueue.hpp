#ifndef __TOOLS_ABSTRACTMESSAGEQUEUE_HPP__
#define __TOOLS_ABSTRACTMESSAGEQUEUE_HPP__

#include <functional>
#include <queue>

#include <boost/thread.hpp>

namespace Tools {

    class MessageQueueManager;

    class AbstractMessageQueue
    {
    public:
        typedef std::function<void(void)> Message;

    private:
        std::queue<Message> _messages;
        boost::mutex _mutex;

    protected:
        AbstractMessageQueue(unsigned int nbThread);
        virtual ~AbstractMessageQueue();
        void _PushMessage(Message message);
    private:
        bool _HasMessage();
        Message _PopMessage();

        friend class MessageQueueManager;
    };

}

#endif
