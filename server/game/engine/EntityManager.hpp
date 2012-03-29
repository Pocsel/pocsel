#ifndef __SERVER_GAME_ENGINE_ENTITYMANAGER_HPP__
#define __SERVER_GAME_ENGINE_ENTITYMANAGER_HPP__

#include "tools/lua/Ref.hpp"
#include "server/game/engine/CallbackManager.hpp"

namespace Tools { namespace Database {
    class IConnection;
}}

namespace Server { namespace Game { namespace Engine {

    class Engine;
    class Entity;
    class PositionalEntity;
    class EntityType;

    class EntityManager :
        private boost::noncopyable
    {
    private:
        struct SpawnEvent
        {
            SpawnEvent(Uint32 pluginId, std::string const& entityName, Tools::Lua::Ref const& arg, Uint32 spawnerId, Uint32 notificationCallbackId) :
                pluginId(pluginId), entityName(entityName), arg(arg), spawnerId(spawnerId), notificationCallbackId(notificationCallbackId)
            {
            }
            Uint32 pluginId;
            std::string entityName;
            Tools::Lua::Ref arg;
            Uint32 spawnerId;
            Uint32 notificationCallbackId;
        };
        struct KillEvent
        {
            KillEvent(Uint32 targetId, Tools::Lua::Ref const& arg, Uint32 killerId, Uint32 notificationCallbackId) :
                targetId(targetId), arg(arg), killerId(killerId), notificationCallbackId(notificationCallbackId)
            {
            }
            Uint32 targetId;
            Tools::Lua::Ref arg;
            Uint32 killerId;
            Uint32 notificationCallbackId;
        };

    private:
        Engine& _engine;
        std::map<Uint32 /* pluginId */, std::map<std::string /* entityName */, EntityType*>> _entityTypes;
        std::map<Uint32 /* entityId */, Entity*> _entities;
        std::map<Uint32 /* entityId */, PositionalEntity*> _positionalEntities;
        Uint32 _pluginIdForRegistering;
        std::string _pluginNameForRegistering;
        Uint32 _nextEntityId;
        Uint32 _runningEntityId;
        std::queue<SpawnEvent*> _spawnEvents;
        std::queue<KillEvent*> _killEvents;

    public:
        EntityManager(Engine& engine);
        ~EntityManager();

        // entry point unique lua (peut retourner toutes les valeurs de CallbackManager::Result sauf CallbackNotFound (evidemment)
        CallbackManager::Result LuaFunctionCall(Uint32 targetId, std::string const& function, Tools::Lua::Ref const& arg, Tools::Lua::Ref const& bonusArg);

        Uint32 GetRunningEntityId() const; // retourne 0 si aucune entité n'est en cours d'éxécution
        void AddSpawnEvent(Uint32 pluginId, std::string const& entityName, Tools::Lua::Ref const& arg, Uint32 spawnerId, Uint32 notificationCallbackId);
        void AddKillEvent(Uint32 targetId, Tools::Lua::Ref const& arg, Uint32 killerId, Uint32 notificationCallbackId);
        void DispatchSpawnEvents();
        void DispatchKillEvents();
        void Save(Tools::Database::IConnection& conn);
        void BeginPluginRegistering(Uint32 pluginId, std::string const& pluginName);
        void EndPluginRegistering();
        void BootstrapPlugin(Uint32 pluginId);
    private:
        Uint32 _CreateEntity(Uint32 pluginId, std::string entityName) throw(std::runtime_error);
        void _ApiSpawn(Tools::Lua::CallHelper& helper);
        void _ApiSpawnFromPlugin(Tools::Lua::CallHelper& helper);
        void _ApiSpawnPositional(Tools::Lua::CallHelper& helper);
        void _ApiSpawnPositionalFromPlugin(Tools::Lua::CallHelper& helper);
        void _ApiKill(Tools::Lua::CallHelper& helper);
        void _ApiRegister(Tools::Lua::CallHelper& helper);
    };

}}}

#endif
