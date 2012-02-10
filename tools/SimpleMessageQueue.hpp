#ifndef __TOOLS_SIMPLEMESSAGEQUEUE_HPP__
#define __TOOLS_SIMPLEMESSAGEQUEUE_HPP__

namespace Tools {

    class SimpleMessageQueue
    {
    private:
        typedef std::function<void(void)> Message;
        boost::mutex _messagesMutex;
        std::list<Message> _messages;
        boost::mutex _conditionMutex;
        boost::condition_variable _condition;
        volatile bool _isRunning;
        std::vector<boost::thread*> _threads;
        unsigned int _nbThreads;

    protected:
        void _PushMessage(Message message)
        {
            {
                boost::lock_guard<boost::mutex> lock(this->_messagesMutex);
                this->_messages.push_back(message);
            }
            this->_condition.notify_one();
        }
        void _Start()
        {
            assert(!this->_isRunning && "SimpleMessageQueue is already running");
            std::cout << "Starting SimpleMessageQueue with " << this->_nbThreads <<
                " thread" << (this->_nbThreads > 1 ? "s" : "") << ". (" << this << ")\n";
            _threads.resize(this->_nbThreads);
            _isRunning = true;
            for (unsigned int i = 0; i < this->_nbThreads; ++i)
                _threads[i] = new boost::thread(boost::bind(&SimpleMessageQueue::_Run, this));
        }
        void _Stop()
        {
            if (!this->_isRunning)
                return;
            std::cout << "Stopping SimpleMessageQueue (" << this << ").\n";

            this->_isRunning = false;
            this->_condition.notify_all();

            for (unsigned int i = 0; i < this->_threads.size(); ++i)
            {
                this->_threads[i]->join();
                Tools::Delete(this->_threads[i]);
            }
            this->_threads.clear();
            std::cout << "SimpleMessageQueue stopped (" << this << ").\n";
        }
        SimpleMessageQueue(unsigned int nbThreads) :
            _isRunning(false),
            _nbThreads(nbThreads)
        {
        }
        // XXX destructeur non virtual, on aura jamais de pointeur sur cette merde
        ~SimpleMessageQueue()
        {
        }

    private:
        void _Run()
        {
            while (this->_isRunning)
            {
                Message message(0);

                {
                    boost::lock_guard<boost::mutex> lock(this->_messagesMutex);
                    if (this->_messages.size() > 0)
                    {
                        message = this->_messages.front();
                        this->_messages.pop_front();
                    }
                }

                if (!message)
                {
                    boost::unique_lock<boost::mutex> conditionLock(this->_conditionMutex);
                    this->_condition.wait(conditionLock);
                }
                else
                    message();
            }
        }
    };

}

#endif
