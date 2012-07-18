#ifndef __SERVER_GAME_ENGINE_ENTITYMANAGER_HPP__
#define __SERVER_GAME_ENGINE_ENTITYMANAGER_HPP__

#include "tools/lua/Ref.hpp"
#include "tools/lua/AResource.hpp"
#include "tools/lua/ResourceManager.hpp"
#include "server/game/engine/CallbackManager.hpp"
#include "common/Position.hpp"

namespace Tools { namespace Database {
    class IConnection;
}}
namespace Server { namespace Rcon {
    class EntityManager;
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
            SpawnEvent(Uint32 pluginId, std::string const& entityName, Tools::Lua::Ref const& arg, Uint32 spawnerId, Uint32 notificationCallbackId, bool hasPosition = false, Common::Position const& pos = Common::Position()) :
                pluginId(pluginId), entityName(entityName), arg(arg), spawnerId(spawnerId), notificationCallbackId(notificationCallbackId), hasPosition(hasPosition), pos(pos)
            {
            }
            Uint32 pluginId;
            std::string entityName;
            Tools::Lua::Ref arg;
            Uint32 spawnerId;
            Uint32 notificationCallbackId;
            bool hasPosition;
            Common::Position pos;
        };
        struct KillEvent
        {
            KillEvent(Uint32 entityId, Tools::Lua::Ref const& arg, Uint32 killerId, Uint32 notificationCallbackId) :
                entityId(entityId), arg(arg), killerId(killerId), notificationCallbackId(notificationCallbackId)
            {
            }
            Uint32 entityId;
            Tools::Lua::Ref arg;
            Uint32 killerId;
            Uint32 notificationCallbackId;
        };
    public:
        struct LuaResource : public Tools::Lua::AResource<EntityManager>
        {
            LuaResource() : entityId(0), disabled(true) {}
            LuaResource(Uint32 entityId) : entityId(entityId), disabled(false) {}
            virtual bool IsValid() const { return this->entityId && !this->disabled; }
            virtual void Invalidate() { this->entityId = 0; this->disabled = true; }
            virtual void Index(EntityManager& entityManager, Tools::Lua::CallHelper& helper);
            virtual void NewIndex(EntityManager& entityManager, Tools::Lua::CallHelper& helper);
            Uint32 entityId;
            bool disabled;
        };

    private:
        Engine& _engine;
        std::map<Uint32 /* pluginId */, std::map<std::string /* entityName */, EntityType*>> _entityTypes;
        std::map<Uint32 /* entityId */, Entity*> _entities; // pointeur nul quand une entité est désactivée
        std::map<Uint32 /* entityId */, PositionalEntity*> _positionalEntities; // pointeur nul quand une entité est désactivée
        std::map<Uint32 /* entityId */, PositionalEntity*> _disabledEntities;
        Uint32 _nextEntityId;
        std::list<SpawnEvent*> _spawnEvents;
        std::list<KillEvent*> _killEvents;
        Tools::Lua::ResourceManager<LuaResource, EntityManager>* _luaResourceManager;

    public:
        EntityManager(Engine& engine);
        ~EntityManager();

        // peut retourner toutes les valeurs de CallbackManager::Result sauf CallbackNotFound (evidemment)
        // seul entry point Lua avec Engine::RconExecute()
        // si Error est retourné, l'entité a été supprimée
        CallbackManager::Result CallEntityFunction(Uint32 entityId, std::string const& function, Tools::Lua::Ref const& arg, Tools::Lua::Ref const& bonusArg, Tools::Lua::Ref* ret = 0);

        void AddSpawnEvent(Uint32 pluginId, std::string const& entityName, Tools::Lua::Ref const& arg, Uint32 spawnerId, Uint32 notificationCallbackId, bool hasPosition = false, Common::Position const& pos = Common::Position());
        void AddKillEvent(Uint32 entityId, Tools::Lua::Ref const& arg, Uint32 killerId, Uint32 notificationCallbackId);
        void DispatchSpawnEvents();
        void DispatchKillEvents();
        void Save(Tools::Database::IConnection& conn);
        void Load(Tools::Database::IConnection& conn);
        void BootstrapPlugin(Uint32 pluginId, Tools::Database::IConnection& conn);
        Entity const& GetEntity(Uint32 entityId) const throw(std::runtime_error); // ne pas garder la reference, l'entité peut etre delete à tout moment
        PositionalEntity const& GetPositionalEntity(Uint32 entityId) const throw(std::runtime_error); // ne pas garder la reference, l'entité peut etre delete à tout moment
        PositionalEntity const& GetDisabledEntity(Uint32 entityId) const throw(std::runtime_error); // ne pas garder la reference, l'entité peut etre delete à tout moment
        bool IsEntityPositional(Uint32 entityId) const;
        void DisableEntity(Uint32 entityId, bool chunkUnloaded = true) throw(std::runtime_error);
        void EnableEntity(Uint32 entityId) throw(std::runtime_error);
        bool IsEntityTypePositional(Uint32 pluginId, std::string const& entityName) const;
        std::map<Uint32, PositionalEntity*> const& GetDisabledEntities() const { return this->_disabledEntities; }
        std::map<Uint32, PositionalEntity*> const& GetPositionalEntities() const { return this->_positionalEntities; }
        Tools::Lua::ResourceManager<LuaResource, EntityManager>& GetLuaResourceManager() { return *this->_luaResourceManager; }

        // rcon requests
        std::string RconGetEntities() const;
        void RconAddEntityTypes(Rcon::EntityManager& manager) const;

    private:
        Entity* _CreateEntity(Uint32 entityId, Uint32 pluginId, std::string entityName, bool hasPosition = false, Common::Position const& pos = Common::Position()) throw(std::runtime_error);
        void _DeleteEntity(Uint32 id, Entity* entity);
        void _ApiGetEntityById(Tools::Lua::CallHelper& helper);
        void _ApiSpawn(Tools::Lua::CallHelper& helper);
        void _ApiSave(Tools::Lua::CallHelper& helper);
        void _ApiLoad(Tools::Lua::CallHelper& helper);
        void _ApiKill(Tools::Lua::CallHelper& helper);
        void _ApiRegister(Tools::Lua::CallHelper& helper);

        // positionnal
        void _ApiRegisterPositional(Tools::Lua::CallHelper& helper);
        void _ApiSetPos(Tools::Lua::CallHelper& helper);
        void _ApiGetPos(Tools::Lua::CallHelper& helper);
        void _ApiSetSpeed(Tools::Lua::CallHelper& helper);
        void _ApiGetSpeed(Tools::Lua::CallHelper& helper);
        void _ApiSetAccel(Tools::Lua::CallHelper& helper);
        void _ApiGetAccel(Tools::Lua::CallHelper& helper);
    };

}}}

#endif
