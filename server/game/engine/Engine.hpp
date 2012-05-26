#ifndef __SERVER_GAME_ENGINE_ENGINE_HPP__
#define __SERVER_GAME_ENGINE_ENGINE_HPP__

#include "server/game/engine/EntityManager.hpp"

namespace Tools { namespace Database {
    class IConnection;
}}

namespace Tools { namespace Lua {
    class Interpreter;
}}

namespace Server { namespace Game {
    namespace Map {
        class Map;
    }
    class World;
}}

namespace Server { namespace Game { namespace Engine {

    class MessageManager;
    class CallbackManager;
    class DoodadManager;

    class Engine :
        private boost::noncopyable
    {
    private:
        Map::Map& _map;
        World& _world;
        Tools::Lua::Interpreter* _interpreter;
        MessageManager* _messageManager;
        EntityManager* _entityManager;
        CallbackManager* _callbackManager;
        DoodadManager* _doodadManager;
        Uint64 _currentTime;
        Uint32 _overriddenPluginId;
        Uint32 _overriddenEntityId;

    public:
        Engine(Map::Map& map, World& world);
        ~Engine();
        void Tick(Uint64 currentTime);
        void Load(Tools::Database::IConnection& conn);
        void Save(Tools::Database::IConnection& conn);
        MessageManager& GetMessageManager() { return *this->_messageManager; }
        EntityManager& GetEntityManager() { return *this->_entityManager; }
        CallbackManager& GetCallbackManager() { return *this->_callbackManager; }
        DoodadManager& GetDoodadManager() { return *this->_doodadManager; }
        Tools::Lua::Interpreter& GetInterpreter() { return *this->_interpreter; }
        Uint64 GetCurrentTime() const { return this->_currentTime; }
        Map::Map& GetMap() { return this->_map; }
        World& GetWorld() { return this->_world; }
        Uint32 GetRunningPluginId() { return this->_overriddenPluginId ? this->_overriddenPluginId : this->_entityManager->GetRunningPluginId(); }
        void OverrideRunningPluginId(Uint32 pluginId) { this->_overriddenPluginId = pluginId; }
        Uint32 GetRunningEntityId() { return this->_overriddenEntityId ? this->_overriddenEntityId : this->_entityManager->GetRunningEntityId(); }
        void OverrideRunningEntityId(Uint32 entityId) { this->_overriddenEntityId = entityId; }

        // rcon requests
        std::string RconExecute(Uint32 pluginId, std::string const& lua);

    private:
        void _ApiPrint(Tools::Lua::CallHelper& helper);
    };

}}}

#endif
