#ifndef __SERVER_GAME_ENGINE_ENGINE_HPP__
#define __SERVER_GAME_ENGINE_ENGINE_HPP__

namespace Tools { namespace Database {
    class IConnectionPool;
}}

namespace Tools { namespace Lua {
    class Interpreter;
}}

namespace Server { namespace Game { namespace Engine {

    class EventManager;
    class EntityManager;

    class Engine :
        private boost::noncopyable
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
        void Save(Tools::Database::IConnectionPool& connectionPool);
        void Load(Tools::Database::IConnectionPool& connectionPool);
        EventManager& GetEventManager() { return *this->_eventManager; }
        EntityManager& GetEntityManager() { return *this->_entityManager; }
        Tools::Lua::Interpreter& GetInterpreter() { return *this->_interpreter; }
        Uint64 GetCurrentTime() const { return this->_currentTime; }
    };

}}}

#endif
