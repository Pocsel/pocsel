#ifndef __SERVER_GAME_ENGINE_ENGINE_HPP__
#define __SERVER_GAME_ENGINE_ENGINE_HPP__

#include "server/game/engine/EntityManager.hpp"

namespace Tools { namespace Database {
    class IConnection;
}}
namespace Tools { namespace Lua {
    class Interpreter;
}}
namespace Server {
    namespace Game {
        namespace Map {
            class Map;
        }
        class World;
    }
    namespace Network {
        class UdpPacket;
    }
}
namespace Common {
    class Packet;
}

namespace Server { namespace Game { namespace Engine {

    class MessageManager;
    class CallbackManager;
    class DoodadManager;
    class BodyManager;
    class PhysicsManager;

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
        PhysicsManager* _physicsManager;
        BodyManager* _bodyManager;
        Uint64 _currentTime;
        std::map<Uint32 /* pluginId */, std::map<std::string /* server_file name */, Tools::Lua::Ref /* module */>> _modules; // uniquement en debug
        Uint32 _currentPluginRegistering;

    public:
        Engine(Map::Map& map, World& world);
        ~Engine();
        void Tick(Uint64 currentTime);
        void Load(Tools::Database::IConnection& conn);
        void Save(Tools::Database::IConnection& conn);
        MessageManager& GetMessageManager() { return *this->_messageManager; }
        MessageManager const& GetMessageManager() const { return *this->_messageManager; }
        EntityManager& GetEntityManager() { return *this->_entityManager; }
        EntityManager const& GetEntityManager() const { return *this->_entityManager; }
        CallbackManager& GetCallbackManager() { return *this->_callbackManager; }
        CallbackManager const& GetCallbackManager() const { return *this->_callbackManager; }
        DoodadManager& GetDoodadManager() { return *this->_doodadManager; }
        DoodadManager const& GetDoodadManager() const { return *this->_doodadManager; }
        BodyManager& GetBodyManager() { return *this->_bodyManager; }
        BodyManager const& GetBodyManager() const { return *this->_bodyManager; }
        PhysicsManager& GetPhysicsManager() { return *this->_physicsManager; }
        PhysicsManager const& GetPhysicsManager() const { return *this->_physicsManager; }
        Tools::Lua::Interpreter& GetInterpreter() { return *this->_interpreter; }
        Tools::Lua::Interpreter const& GetInterpreter() const { return *this->_interpreter; }
        Uint64 GetCurrentTime() const { return this->_currentTime; }
        Map::Map& GetMap() { return this->_map; }
        World& GetWorld() { return this->_world; }
        void SetCurrentPluginRegistering(Uint32 pluginId) { this->_currentPluginRegistering = pluginId; }
        Uint32 GetCurrentPluginRegistering() const { return this->_currentPluginRegistering; }

        void SendPacket(Uint32 playerId, std::unique_ptr<Common::Packet>& packet);
        void SendUdpPacket(Uint32 playerId, std::unique_ptr<Network::UdpPacket>& packet);

        // appelés uniquement en debug
        void SetModules(std::map<Uint32 /* pluginId */, std::map<std::string /* server_file name */, std::pair<bool /* loading in progress */, Tools::Lua::Ref /* module */>>> const& modules);
        void RegisterRequire();

        // rcon requests
        std::string RconExecute(Uint32 pluginId, std::string const& lua);

    private:
        void _ApiRequire(Tools::Lua::CallHelper& helper);
        void _ApiPrint(Tools::Lua::CallHelper& helper);
    };

}}}

#endif
