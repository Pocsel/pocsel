#ifndef __SERVER_GAME_ENGINE_EVENTMANAGER_HPP__
#define __SERVER_GAME_ENGINE_EVENTMANAGER_HPP__

namespace Tools { namespace Lua {
    class CallHelper;
}}

namespace Server { namespace Game { namespace Engine {

    class Engine;

    class EventManager
    {
    private:
        struct Event
        {
            int entityId;
            std::string function;
            std::string args;
        };

    private:
        Engine& _engine;
        std::map<Uint64, Event*> _events;

    public:
        EventManager(Engine& engine);
        ~EventManager();
        void DispatchEvents();
    private:
        void _CallLater(Tools::Lua::CallHelper& helper);
    };

}}}

#endif

