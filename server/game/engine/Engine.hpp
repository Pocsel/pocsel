#ifndef __SERVER_GAME_ENGINE_ENGINE_HPP__
#define __SERVER_GAME_ENGINE_ENGINE_HPP__

namespace Tools { namespace Lua {
    class Interpreter;
}}

namespace Server { namespace Game { namespace Engine {

    class EventManager;
    class EntityManager;

    class Engine
    {
    private:
        Tools::Lua::Interpreter* _interpreter;
        EventManager* _eventManager;
        EntityManager* _entityManager;
        Uint64 _currentTime;

    public:
        Engine();
        ~Engine();
        void Tick(Uint64 currentTime);
        EventManager& GetEventManager() { return *this->_eventManager; }
        EntityManager& GetEntityManager() { return *this->_entityManager; }
        Tools::Lua::Interpreter& GetInterpreter() { return *this->_interpreter; }
        Uint64 GetCurrentTime() const { return this->_currentTime; }
    };

}}}

#endif
