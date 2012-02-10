#ifndef __TOOLS_GUI_EVENTMANAGER_HPP__
#define __TOOLS_GUI_EVENTMANAGER_HPP__

#include "tools/gui/Event.hpp"

namespace Tools { namespace Gui {

    class EventManagerImplem;
    class Window;

    class EventManager : private boost::noncopyable
    {
    public:
        typedef unsigned int HandlerId;
        typedef std::function<void(Event const&)> EventCallback;
    private:
        typedef std::unordered_map<HandlerId, EventCallback> HandlersMap;
        typedef std::unordered_map<std::string, std::list<HandlerId>> SignalsMap;
        typedef std::list<std::pair<std::string, Event*>> EventsList;

    private:
        volatile HandlerId      _currentId;
        volatile bool           _isRunning;
        EventManagerImplem*     _implem;
        HandlersMap             _handlers;
        SignalsMap              _signals;
        EventsList              _toFire;
        boost::mutex            _handlersMutex;
        boost::mutex            _signalsMutex;
        boost::mutex            _toFireMutex;

    public:
        EventManager(Window& window);
        ~EventManager();
        HandlerId Connect(std::string const& signal_id, EventCallback cb);
        void Disconnect(HandlerId handler);
        void Fire(std::string const& signal_id, Event const& ev) { this->Fire(signal_id.c_str(), ev); }
        void Fire(char const* signal_id, Event const& ev);
        void Run();
        void Stop();

        void Notify(std::string const& signal_id, std::unique_ptr<Event> ev);
        template<typename EventType>
            void Notify(std::string const& signal_id)
            { this->_Notify(signal_id, new EventType()); }

        template<typename EventType, typename P1>
            void Notify(std::string const& id, P1 p1)
            { this->_Notify(id, new EventType(p1)); }

        template<typename EventType, typename P1, typename P2>
            void Notify(std::string const& id, P1 p1, P2 p2)
            { this->_Notify(id, new EventType(p1, p2)); }

        template<typename EventType, typename P1, typename P2, typename P3>
            void Notify(std::string const& id, P1 p1, P2 p2, P3 p3)
            { this->_Notify(id, new EventType(p1, p2, p3)); }

        template<typename EventType, typename P1, typename P2, typename P3, typename P4>
            void Notify(std::string const& id, P1 p1, P2 p2, P3 p3, P4 p4)
            { this->_Notify(id, new EventType(p1, p2, p3, p4)); }

        template<typename EventType, typename P1, typename P2, typename P3, typename P4, typename P5>
            void Notify(std::string const& id, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
            { this->_Notify(id, new EventType(p1, p2, p3, p4, p5)); }

    private:
        void _Notify(std::string const& signal_id, Event* ev);
        size_t _Flush();
        void _ExecuteSignal(std::string const& signal_id, Event const& ev);
    };

}}

#endif
