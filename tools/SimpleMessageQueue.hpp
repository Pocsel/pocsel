#ifndef __TOOLS_SIMPLEMESSAGEQUEUE_HPP__
#define __TOOLS_SIMPLEMESSAGEQUEUE_HPP__

#include "tools/Timer.hpp"

namespace Tools {

    class SimpleMessageQueue
    {
    public:
        typedef std::function<void(void)> Message;
        typedef std::function<void(Uint64)> TimerLoopMessage;

    private:
        struct SmqTimer
        {
            boost::asio::deadline_timer timer;
            Uint64 us;
            TimerLoopMessage message;
            Timer chrono;
            Uint64 lastTime;
            SmqTimer(boost::asio::io_service& io, Uint64 us, TimerLoopMessage& m, Uint64 startTime = 0) :
                timer(io, boost::posix_time::microseconds(us)),
                us(us),
                message(m),
                chrono(),
                lastTime(startTime)
            {
                chrono.Set(startTime);
            }
        };

    private:
        boost::asio::io_service _ioService;

        std::vector<SmqTimer*> _timers;

        volatile bool _isRunning;
        std::vector<boost::thread*> _threads;
        unsigned int _nbThreads;

    public:
        SimpleMessageQueue(unsigned int nbThreads);
        ~SimpleMessageQueue();
        void SetLoopTimer(Uint64 us, TimerLoopMessage& message, Uint64 startTime = 0);
        void PushMessage(Message& message);
        void PushTimedMessage(Uint64 us, Message& message);
        void Start();
        void Stop();

    private:
        void _ExecLoopTimer(size_t index, boost::system::error_code const& error);
        void _ExecTimedMessage(Message& message,
                               std::shared_ptr<boost::asio::deadline_timer>,
                               boost::system::error_code const& error);
        void _Run();
    };

}

#endif
