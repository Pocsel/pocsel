#ifndef __TOOLS_SIMPLEMESSAGEQUEUE_HPP__
#define __TOOLS_SIMPLEMESSAGEQUEUE_HPP__

#include "tools/Timer.hpp"

namespace Tools {

    class SimpleMessageQueue
    {
    public:
        typedef std::function<void(void)> Message;
        typedef std::function<void(Uint32)> TimerLoopMessage;

    private:
        struct SmqTimer
        {
            boost::asio::deadline_timer timer;
            Uint32 ms;
            TimerLoopMessage message;
            Timer chrono;
            Uint32 lastTime;
            SmqTimer(boost::asio::io_service& io, Uint32 ms, TimerLoopMessage& m) :
                timer(io, boost::posix_time::milliseconds(ms)),
                ms(ms),
                message(m),
                chrono(),
                lastTime(0)
            {
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
        void SetLoopTimer(Uint32 ms, TimerLoopMessage& message);
        void PushMessage(Message& message);
        void PushTimedMessage(Uint32 ms, Message& message);
        void Start();
        void Stop();

    private:
        void _SetLoopTimer(Uint32 ms, TimerLoopMessage& message);
        void _ExecLoopTimer(Uint32 index, boost::system::error_code const& error);
        void _ExecTimedMessage(TimerLoopMessage& message,
                               std::shared_ptr<boost::asio::deadline_timer>,
                               boost::system::error_code const& error);
        void _Run();
    };

}

#endif
