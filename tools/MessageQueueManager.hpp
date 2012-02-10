#ifndef __TOOLS_MESSAGEQUEUEMANAGER_HPP__
#define __TOOLS_MESSAGEQUEUEMANAGER_HPP__

#include "tools/AbstractMessageQueue.hpp"
#include "tools/atomic.hpp"

namespace Tools {

    class MessageQueueManager
    {
    public:
        typedef std::function<void(void)> Message;

    private:
        struct QueueWorker;

    private:
        static std::vector<QueueWorker*> _queues;
        static boost::mutex _mutex;
        static boost::condition_variable _condition;
        static volatile bool _isRunning;
        static std::vector<boost::thread*> _threads;

    public:
        static void Register(AbstractMessageQueue* queue, unsigned int nbThread);
        static void Unregister(AbstractMessageQueue* queue);
        static void Start(unsigned int nbThread);
        static void Stop();
        static void NotifyOne();

    private:
        MessageQueueManager() {}
        static void _Run();
    };

}

#endif
