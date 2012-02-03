#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bind.hpp>

#include "tools/atomic.hpp"

#include "server/EventManager.hpp"

namespace {

    struct Event
    {
        boost::asio::deadline_timer timer;
        Server::EventManager::Callback func;
        static std::atomic<int> count;
        Event(boost::asio::io_service& io, long t, Server::EventManager::Callback& func) :
            timer(io, boost::posix_time::milliseconds(t)),
            func(func)
        {
            ++count;
        }

        ~Event()
        {
            --count;
        }

        void Execute(const boost::system::error_code& e)
        {
            if (!e)
                func();
            else
                std::cerr << "Timed Event error: " << e << std::endl;
        }
    };
    std::atomic<int> Event::count;

}

namespace Server {

    EventManager::EventManager() :
        _ioService(),
        _work(new boost::asio::io_service::work(_ioService))
    {
    }

    EventManager::~EventManager() {}

    void EventManager::AddEvent(long expiryTime, Callback f)
    {
        boost::shared_ptr<Event> ptr(new Event(this->_ioService, expiryTime, f));
        ptr->timer.async_wait(
            boost::bind(&Event::Execute, ptr, boost::asio::placeholders::error)
        );
    }

    void EventManager::Run()
    {
        this->_ioService.run();
        std::cout << "Timer io_service is down" << std::endl;
        std::cout << "Remaining Events: " << Event::count << std::endl;
    }

    void EventManager::Stop()
    {
        this->_work.reset();
        std::cout << "Stopping Timer io_service" << std::endl;
        this->_ioService.stop();
    }

}
