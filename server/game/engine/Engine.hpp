#ifndef __SERVER_GAME_ENGINE_ENGINE_HPP__
#define __SERVER_GAME_ENGINE_ENGINE_HPP__

namespace Tools { namespace Database {
    class IConnection;
}}

namespace Tools { namespace Lua {
    class Interpreter;
}}

namespace Server { namespace Game { namespace Map {
    class Map;
}}}

namespace Server { namespace Game { namespace Engine {

    class CallManager;
    class EntityManager;
    class CallbackManager;

    class Engine :
        private boost::noncopyable
    {
    private:
        Map::Map& _map;
        Tools::Lua::Interpreter* _interpreter;
        CallManager* _callManager;
        EntityManager* _entityManager;
        CallbackManager* _callbackManager;
        Uint64 _currentTime;

    public:
        Engine(Map::Map& map);
        ~Engine();
        void Tick(Uint64 currentTime);
        void Save(Tools::Database::IConnection& conn);
        CallManager& GetCallManager() { return *this->_callManager; }
        EntityManager& GetEntityManager() { return *this->_entityManager; }
        CallbackManager& GetCallbackManager() { return *this->_callbackManager; }
        Tools::Lua::Interpreter& GetInterpreter() { return *this->_interpreter; }
        Uint64 GetCurrentTime() const { return this->_currentTime; }
        Map::Map& GetMap() { return this->_map; }
    };

}}}

#endif
