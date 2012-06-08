#include "tools/MessageQueueManager.hpp"


namespace Tools {

    std::vector<MessageQueueManager::QueueWorker*>  MessageQueueManager::_queues;
    boost::mutex                                    MessageQueueManager::_mutex;
    boost::condition_variable                       MessageQueueManager::_condition;
    volatile bool                                   MessageQueueManager::_isRunning = false;
    std::vector<boost::thread*>                     MessageQueueManager::_threads;

    struct MessageQueueManager::QueueWorker
    {
        AbstractMessageQueue* queue;
        std::atomic<unsigned int> nbThreads;
        std::atomic<unsigned int> workingThreads;
        boost::thread::id threadId;

        QueueWorker(AbstractMessageQueue* queue, unsigned int nbThreads) :
            queue(queue),
            nbThreads(nbThreads),
            workingThreads(0)
        {}

        void Run(Message message)
        {
            assert(message != 0);
            threadId = boost::this_thread::get_id();
            try
            {
                message();
            }
            catch (std::exception& err)
            {
                std::cerr << "Message queue exception catched: " << err.what() << ".\n";
            }
            --this->workingThreads;
        }

    };

    void MessageQueueManager::Register(AbstractMessageQueue* queue, unsigned int nbThread)
    {
        boost::lock_guard<boost::mutex> lock(_mutex);
#ifdef DEBUG
        for (auto it = _queues.begin(), end = _queues.end(); it != end; ++it)
            assert((*it)->queue != queue && "Queue already registered");
#endif
        _queues.push_back(new QueueWorker(queue, nbThread));
    }

    void MessageQueueManager::Unregister(AbstractMessageQueue* queue)
    {
        assert(queue != 0 && "Unregister NULL ptr");
        std::cout << "Unregister " << queue << "\n";
        QueueWorker* worker = 0;
        {
            boost::lock_guard<boost::mutex> lock(_mutex);
            for (auto it = _queues.begin(), end = _queues.end(); it != end; ++it)
                if ((*it)->queue == queue)
                {
                    worker = *it;
                    if (*it == _queues.back())
                    {
                        *it = 0;
                    }
                    else //if (_queues.size() > 1)
                    {
                        *it = _queues.back();
                    }
                    _queues.resize(_queues.size() - 1); // do not use `it' and `end' anymore
                    break;
                }
        }

        if (worker != 0)
        {
            worker->nbThreads = 0;
            // write_done pour eviter le spam et comprendre ce qu'il se passe
            int write_done = 0;
            while (worker->workingThreads > 0)
            {
                if (write_done < 10)
                {
                    std::cout << "LOOP " << boost::this_thread::get_id() << " vs " <<
                        worker->threadId << "\n";
                    std::cout << __FILE__ << ':' << __LINE__ << ": waiting " << worker->workingThreads << " tasks to end.\n";
                    ++write_done;
                }
            }

            delete worker;
        }
    }

    void MessageQueueManager::Start(unsigned int nbThread)
    {
        assert(!MessageQueueManager::_isRunning && "MessageQueueManager is already running");
        std::cout << "Starting MessageQueue with " << nbThread << " threads.\n";
        _threads.resize(nbThread);
        _isRunning = true;
        for (unsigned int i = 0; i < nbThread; ++i)
            _threads[i] = new boost::thread(&MessageQueueManager::_Run);
    }


    void MessageQueueManager::Stop()
    {
        if (!MessageQueueManager::_isRunning)
            return;

        MessageQueueManager::_isRunning = false;
        MessageQueueManager::_condition.notify_all();

        for (unsigned int i = 0; i < _threads.size(); ++i)
        {
            _threads[i]->join();
            delete _threads[i];
        }
        _threads.clear();
        //while (MessageQueueManager::_queues.size())
        //{
        //    std::cout << "Delete " << MessageQueueManager::_queues.front()->queue << "\n";
        //    MessageQueueManager::Unregister(MessageQueueManager::_queues.front()->queue);
        //}
    }

    void MessageQueueManager::NotifyOne()
    {
        _condition.notify_one();
    }

    void MessageQueueManager::_Run()
    {
        static boost::mutex conditionMutex;

        while (_isRunning)
        {
            QueueWorker* worker(0);
            Message message(nullptr);

            { // pick next task
                boost::lock_guard<boost::mutex> lock(_mutex);
                static unsigned int idx = 0;
                size_t count = 0,
                       size = MessageQueueManager::_queues.size();
                if (size != 0)
                    idx = idx % size;
                while (count < size)
                {
                    count += 1;
                    worker = MessageQueueManager::_queues[idx];
                    idx = (idx + 1) % size;

                    assert(worker && "found null worker");

                    if (worker->workingThreads < worker->nbThreads && worker->queue->_HasMessage())
                    {
                        ++worker->workingThreads;
                        message = worker->queue->_PopMessage();
                        break;
                    }
                }
            }

            if (!message) // going into sleep mode
            {
                boost::unique_lock<boost::mutex> conditionLock(conditionMutex);
                _condition.wait(conditionLock);
            }
            else
            {
                assert(worker != 0);
                worker->Run(message);
            }
        }
    }

}
