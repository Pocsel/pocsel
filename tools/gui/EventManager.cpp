#include "tools/Timer.hpp"

#include "tools/gui/EventManager.hpp"
#include "tools/gui/EventManagerImplem.hpp"
#include "tools/gui/Window.hpp"

#include "tools/gui/events/all.hpp"

namespace Tools { namespace Gui {

    EventManager::EventManager(Window& window) :
        _currentId(0),
        _isRunning(false),
        _implem(0)
    {
        this->_implem = new EventManagerImplem(window);
    }

    EventManager::~EventManager()
    {
        if (this->_isRunning)
        {
            std::cerr << "Warning: EventManager destroyed but not stopped" << std::endl;
        }
        delete this->_implem;
        auto it = this->_toFire.begin(),
             end = this->_toFire.end();
        for (; it != end; ++it)
            delete it->second;
    }

    EventManager::HandlerId EventManager::Connect(std::string const& id, EventCallback cb)
    {
        // TODO atomic increment
        EventManager::HandlerId handlerId = ++EventManager::_currentId;

        {
            boost::unique_lock<boost::mutex> lock(this->_handlersMutex);
            this->_handlers[handlerId] = cb;
        }

        {
            boost::unique_lock<boost::mutex> lock(this->_signalsMutex);
            this->_signals[id].push_back(handlerId);
        }
        return handlerId;
    }


    void EventManager::Disconnect(EventManager::HandlerId handler)
    {
        boost::unique_lock<boost::mutex> lock(this->_handlersMutex);
        this->_handlers.erase(handler);
    }

    void EventManager::Notify(std::string const& id, std::unique_ptr<Event> ev)
    {
        this->_Notify(id, ev.release());
    }

    void EventManager::_Notify(std::string const& id, Event* ev)
    {
        boost::unique_lock<boost::mutex> lock(this->_toFireMutex);
        this->_toFire.push_back(std::pair<std::string, Event*>(id, ev));
    }


    size_t EventManager::_Flush()
    {
        std::list<std::pair<std::string, Event*>> copy;

        {
            boost::unique_lock<boost::mutex> lock(this->_toFireMutex);
            copy = this->_toFire;
            this->_toFire.clear();
        }

        auto it = copy.begin(), end = copy.end();
        for (; it != end; ++it)
        {
            std::pair<std::string, Event*> p = *it;
            this->Fire(p.first, *p.second);
            delete p.second;
        }
        return copy.size();
    }

    void EventManager::Fire(char const* id, Event const& ev)
    {
        //std::cout << "EventManager::Fire(" << id << ")\n";
        std::list<HandlerId> handlersIds;
        std::set<HandlerId> toRemove;

        {
            boost::unique_lock<boost::mutex> lock(this->_signalsMutex);
            handlersIds = this->_signals[id];
        }

        auto handlerIt = handlersIds.begin(),
             lastHandler = handlersIds.end();
        for (; handlerIt != lastHandler; ++handlerIt)
        {
            HandlerId handlerId = *handlerIt;
            HandlersMap::iterator cb;
            bool notFound;

            {
                boost::unique_lock<boost::mutex> lock(this->_handlersMutex);
                cb = this->_handlers.find(handlerId);
                notFound = (cb == this->_handlers.end());
            }

            if (notFound)
                toRemove.insert(handlerId);
            else
                try
                {
                    //std::cout << " ==> EventManager::Fire(" << id << ") : "
                    //          << handlerId << std::endl;
                    cb->second(ev);
                }
            catch (std::exception const& err)
            {
                std::cerr << "Callback " << handlerId << " failed with: " << err.what() << std::endl;
            }
        }

        if (toRemove.size() > 0)
        {
            std::list<HandlerId> newlist;
            auto toRemoveEnd = toRemove.end();

            boost::unique_lock<boost::mutex> lock(this->_signalsMutex);
            std::list<HandlerId>& ids = this->_signals[id];
            auto it = ids.begin(), end = ids.end();
            for (; it != end; ++it)
            {
                if (toRemove.find(*it) == toRemoveEnd)
                    newlist.push_back(*it);
            }
            ids = newlist;
        }
    }

    void EventManager::Run()
    {
        Tools::Timer t;
        this->_isRunning = true;
        Events::Frame frameEvent(0);
        while (this->_isRunning)
        {
            t.Reset();
            this->Fire("on-frame", frameEvent);
            size_t pcount = this->_implem->Poll();
            if (!this->_isRunning)
                break;
            size_t fcount = this->_Flush();
            if (!this->_isRunning)
                break;
            if (pcount + fcount == 0)
                this->Fire("on-idle", Events::Idle());
            frameEvent.frameTime = t.GetElapsedTime();
        }
    }

    void EventManager::Stop()
    {
        std::cout << "EventManager::Stop()\n";
        this->_isRunning = false;
        this->_implem->Stop();
    }

}}
