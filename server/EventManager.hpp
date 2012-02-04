#ifndef __EVENTMANAGER_HPP__
#define __EVENTMANAGER_HPP__

#include <boost/asio/io_service.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

namespace Server {

    class EventManager : private boost::noncopyable
    {
    public:
        typedef boost::function<void(void)> Callback;

    private:
        boost::asio::io_service _ioService;
        std::unique_ptr<boost::asio::io_service::work> _work;

    public:
        EventManager();
        ~EventManager();
        void AddEvent(long expiryTime, Callback f);
        void Run();
        void Stop();
    };

}

#endif
