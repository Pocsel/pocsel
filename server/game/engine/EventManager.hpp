#ifndef __SERVER_GAME_ENGINE_EVENTMANAGER_HPP__
#define __SERVER_GAME_ENGINE_EVENTMANAGER_HPP__

#include "tools/lua/Ref.hpp"

//namespace Tools { namespace Lua {
//    class CallHelper;
//}}

namespace Server { namespace Game { namespace Engine {

    class Engine;

    class EventManager :
        private boost::noncopyable
    {
    private:
        struct Event
        {
            Event(int targetId, std::string function, Tools::Lua::Ref const& copiedArgs, int callerId) :
                targetId(targetId), function(function), copiedArgs(copiedArgs), callerId(callerId)
            {
            }
            int targetId;
            std::string function;
            Tools::Lua::Ref copiedArgs;
            int callerId;
        };

    private:
        Engine& _engine;
        std::map<Uint64, std::list<Event*>> _events;

    public:
        EventManager(Engine& engine);
        ~EventManager();
        void DispatchEvents();
    private:
        void _ApiCallLater(Tools::Lua::CallHelper& helper);
    };

}}}

#endif

