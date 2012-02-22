#ifndef __TOOLS_SIMPLEMESSAGEQUEUE_HPP__
#define __TOOLS_SIMPLEMESSAGEQUEUE_HPP__

namespace Tools {

    class SimpleMessageQueue
    {
    public:
        typedef std::function<void(void)> Message;
//    private:
//        boost::mutex _messagesMutex;
//        std::list<Message> _messages;
//        boost::mutex _conditionMutex;
//        boost::condition_variable _condition;


        boost::asio::io_service _ioService;

        volatile bool _isRunning;
        std::vector<boost::thread*> _threads;
        unsigned int _nbThreads;

    public:
        SimpleMessageQueue(unsigned int nbThreads) :
            _isRunning(false),
            _nbThreads(nbThreads)
        {
        }
        ~SimpleMessageQueue()
        {
        }
        void PushMessage(Message& message)
        {
            this->_ioService.dispatch(message);
//            {
//                boost::lock_guard<boost::mutex> lock(this->_messagesMutex);
//                this->_messages.push_back(message);
//            }
//            this->_condition.notify_one();
        }
        void Start()
        {
            assert(!this->_isRunning && "SimpleMessageQueue is already running");
            Tools::debug << "Starting SimpleMessageQueue with " << this->_nbThreads <<
                " thread" << (this->_nbThreads > 1 ? "s" : "") << ". (" << this << ")\n";
            _threads.resize(this->_nbThreads);
            _isRunning = true;
            for (unsigned int i = 0; i < this->_nbThreads; ++i)
                _threads[i] = new boost::thread(boost::bind(&SimpleMessageQueue::_Run, this));
        }
        void Stop()
        {
            if (!this->_isRunning)
                return;

            this->_isRunning = false;

            Tools::debug << "Stopping SimpleMessageQueue (" << this << ").\n";

            this->_ioService.stop();

//            this->_condition.notify_all();

            for (unsigned int i = 1; i < this->_threads.size() + 1; ++i)
            {
                if (this->_threads[i - 1] != 0)
                {
                    for (unsigned int j = 0 ; j < this->_threads.size(); ++j)
                    {
                        if (this->_threads[j] != 0 &&
                            this->_threads[j]->timed_join(boost::posix_time::milliseconds(50)))
                        {
                            Tools::Delete(this->_threads[j]);
                            this->_threads[j] = 0;
                        }
                    }
//                    this->_condition.notify_all();
                    i = 0;
                }
            }
            this->_threads.clear();
            Tools::debug << "SimpleMessageQueue stopped (" << this << ").\n";
        }

    private:
        void _Run()
        {
            Tools::debug << "1 thread runnin'\n";
            boost::asio::io_service::work work(this->_ioService);
            this->_ioService.run();
//            while (this->_isRunning)
//            {
//                Message message(0);
//
//                {
//                    boost::lock_guard<boost::mutex> lock(this->_messagesMutex);
//                    if (this->_messages.size() > 0)
//                    {
//                        message = this->_messages.front();
//                        this->_messages.pop_front();
//                    }
//                }
//
//                if (!message)
//                {
//                    boost::unique_lock<boost::mutex> conditionLock(this->_conditionMutex);
//                    this->_condition.wait(conditionLock);
//                }
//                else
//                    message();
//            }
            Tools::debug << "1 thread over\n";
        }
    };

}

#endif
