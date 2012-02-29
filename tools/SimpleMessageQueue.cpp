#include "tools/SimpleMessageQueue.hpp"

namespace Tools {

    SimpleMessageQueue::SimpleMessageQueue(unsigned int nbThreads) :
        _isRunning(false),
        _nbThreads(nbThreads)
    {
    }

    SimpleMessageQueue::~SimpleMessageQueue()
    {
        std::for_each(this->_timers.begin(), this->_timers.end(), [](SmqTimer* t) { Tools::Delete(t); });
    }

    void SimpleMessageQueue::SetLoopTimer(Uint64 us, TimerLoopMessage& message)
    {
        Tools::debug << "Adding new timer: " << us << "us.\n";

        this->_timers.push_back(new SmqTimer(
                    this->_ioService, us, message
                    ));

        std::function<void(boost::system::error_code const& e)>
            runLoop(std::bind(&SimpleMessageQueue::_ExecLoopTimer, this, this->_timers.size() - 1, std::placeholders::_1));
        this->_timers.back()->timer.async_wait(runLoop);
    }

    void SimpleMessageQueue::PushMessage(Message& message)
    {
        this->_ioService.dispatch(message);
    }

    void SimpleMessageQueue::PushTimedMessage(Uint64 us, Message& message)
    {
        boost::asio::deadline_timer *t = new boost::asio::deadline_timer(this->_ioService, boost::posix_time::microseconds(us));

        std::function<void(boost::system::error_code const& e)>
            function(std::bind(&SimpleMessageQueue::_ExecTimedMessage,
                               this,
                               message,
                               std::shared_ptr<boost::asio::deadline_timer>(t),
                               std::placeholders::_1));
        t->async_wait(function);
    }

    void SimpleMessageQueue::Start()
    {
        assert(!this->_isRunning && "SimpleMessageQueue is already running");
        Tools::debug << "Starting SimpleMessageQueue with " << this->_nbThreads <<
            " thread" << (this->_nbThreads > 1 ? "s" : "") << ". (" << this << ")\n";
        _threads.resize(this->_nbThreads);
        _isRunning = true;
        for (unsigned int i = 0; i < this->_nbThreads; ++i)
            _threads[i] = new boost::thread(boost::bind(&SimpleMessageQueue::_Run, this));
    }

    void SimpleMessageQueue::Stop()
    {
        if (!this->_isRunning)
            return;
        this->_isRunning = false;

        Tools::debug << "Stopping SimpleMessageQueue (" << this << ").\n";

        this->_ioService.stop();

        for (unsigned int i = 0; i < this->_threads.size(); ++i)
        {
            this->_threads[i]->join();
            Tools::Delete(this->_threads[i]);
        }
        this->_threads.clear();

        Tools::debug << "SimpleMessageQueue stopped (" << this << ").\n";
    }

    void SimpleMessageQueue::_ExecLoopTimer(size_t index, boost::system::error_code const& error)
    {
        if (error == boost::asio::error::operation_aborted)
        {
            Tools::debug << "Timer loop aborted;";
            Tools::Delete(this->_timers[index]);
            this->_timers[index] = 0;
            return;
        }

        SmqTimer& t = *this->_timers[index];

        Uint64 elapsedTime = t.chrono.GetPreciseElapsedTime();
        t.message(elapsedTime);

        while (t.lastTime + t.us <= elapsedTime)
            t.lastTime += t.us;

        Uint64 waitTime = (t.lastTime + t.us) - elapsedTime;

        std::function<void(boost::system::error_code const& e)>
            runLoop(std::bind(&SimpleMessageQueue::_ExecLoopTimer, this, index, std::placeholders::_1));

        t.timer.expires_from_now(
                boost::posix_time::microseconds(waitTime)
                );
        t.timer.async_wait(runLoop);
    }

    void SimpleMessageQueue::_ExecTimedMessage(Message& message,
                                               std::shared_ptr<boost::asio::deadline_timer>,
                                               boost::system::error_code const& error)
    {
        if (error == boost::asio::error::operation_aborted)
        {
            Tools::debug << "Timer aborted;";
            return;
        }

        message();
    }

    void SimpleMessageQueue::_Run()
    {
        Tools::debug << "Thread started.\n";
        boost::asio::io_service::work work(this->_ioService);
        this->_ioService.run();
        Tools::debug << "Thread over.\n";
    }

}
